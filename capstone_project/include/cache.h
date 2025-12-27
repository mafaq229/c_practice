/*
 * cache.h - LRU File Cache
 *
 * This header defines the caching system used in Part C (Proxy)
 * and Part D (IPC Cache Process).
 *
 * Features:
 * - LRU (Least Recently Used) eviction policy
 * - Thread-safe operations
 * - Configurable maximum cache size
 * - Cache statistics for monitoring
 *
 * The cache stores file contents in memory to avoid repeated disk reads.
 */

#ifndef CACHE_H
#define CACHE_H

#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <time.h>

/* ============================================================================
 * Constants
 * ============================================================================ */

/* Maximum size of cache in bytes (default: 10 MB) */
#define DEFAULT_CACHE_SIZE      (10 * 1024 * 1024)

/* Maximum number of cached entries */
#define MAX_CACHE_ENTRIES       1024

/* Maximum key length */
#define MAX_KEY_LEN             512

/* ============================================================================
 * Data Structures
 * ============================================================================ */

/*
 * Cache entry - one cached file
 *
 * Entries are stored in a hash table with LRU ordering.
 */
typedef struct cache_entry {
    char key[MAX_KEY_LEN];          /* Cache key (file path) */
    char *data;                     /* File contents (malloc'd) */
    size_t size;                    /* Size of data in bytes */
    time_t last_access;             /* Timestamp of last access (for LRU) */
    time_t created;                 /* When entry was cached */

    /* Linked list pointers for LRU ordering */
    struct cache_entry *lru_prev;   /* More recently used */
    struct cache_entry *lru_next;   /* Less recently used */

    /* Hash table chaining */
    struct cache_entry *hash_next;  /* Next entry in hash bucket */

} cache_entry_t;

/*
 * Cache - the main cache structure
 *
 * Uses a hash table for O(1) lookup and a doubly-linked list for LRU ordering.
 */
typedef struct {
    /* Hash table for fast lookup */
    cache_entry_t **buckets;        /* Array of bucket pointers */
    int num_buckets;                /* Size of buckets array */

    /* LRU list (doubly linked) */
    cache_entry_t *lru_head;        /* Most recently used */
    cache_entry_t *lru_tail;        /* Least recently used (evict first) */

    /* Size tracking */
    size_t current_size;            /* Total bytes currently cached */
    size_t max_size;                /* Maximum cache size */
    int num_entries;                /* Number of cached files */

    /* Statistics */
    unsigned long hits;             /* Cache hits */
    unsigned long misses;           /* Cache misses */
    unsigned long evictions;        /* Number of evictions */

    /* Thread safety */
    pthread_rwlock_t lock;          /* Read-write lock for concurrent access */

} cache_t;

/* ============================================================================
 * Function Prototypes
 * ============================================================================ */

/*
 * cache_create - Create a new cache
 *
 * @param max_size: Maximum cache size in bytes
 * @return: Pointer to cache, or NULL on error
 *
 * Allocate and initialize all structures.
 * Use a prime number for num_buckets (e.g., 1021).
 */
cache_t *cache_create(size_t max_size);

/*
 * cache_destroy - Destroy cache and free all memory
 *
 * @param cache: Cache to destroy
 *
 * Free all entries, their data, and the cache structure.
 */
void cache_destroy(cache_t *cache);

/*
 * cache_get - Look up an entry in the cache
 *
 * @param cache: Cache
 * @param key: Cache key (file path)
 * @param data: Output - pointer to data (DO NOT FREE!)
 * @param size: Output - size of data
 * @return: true if found (cache hit), false otherwise (cache miss)
 *
 * On hit:
 * - Move entry to front of LRU list
 * - Update last_access timestamp
 * - Increment hits counter
 *
 * On miss:
 * - Increment misses counter
 *
 * IMPORTANT: The returned data pointer is only valid while
 * holding the cache lock. For thread safety, consider returning
 * a copy of the data instead.
 *
 * Thread-safe: Uses read lock.
 */
bool cache_get(cache_t *cache, const char *key, char **data, size_t *size);

/*
 * cache_get_copy - Look up and return a copy of cached data
 *
 * @param cache: Cache
 * @param key: Cache key
 * @param data: Output - newly allocated copy of data (caller must free!)
 * @param size: Output - size of data
 * @return: true if found, false otherwise
 *
 * This is safer for multithreaded use - returns a copy that
 * the caller owns and can use after releasing locks.
 */
bool cache_get_copy(cache_t *cache, const char *key, char **data, size_t *size);

/*
 * cache_put - Add an entry to the cache
 *
 * @param cache: Cache
 * @param key: Cache key (file path)
 * @param data: File contents
 * @param size: Size of data
 * @return: true on success, false on error
 *
 * Steps:
 * 1. If entry exists, update it
 * 2. If cache is full, evict LRU entries until space available
 * 3. Copy data (don't just store the pointer!)
 * 4. Add to hash table and front of LRU list
 *
 * Thread-safe: Uses write lock.
 */
bool cache_put(cache_t *cache, const char *key, const char *data, size_t size);

/*
 * cache_remove - Remove an entry from the cache
 *
 * @param cache: Cache
 * @param key: Cache key
 * @return: true if entry was found and removed, false if not found
 *
 * Thread-safe: Uses write lock.
 */
bool cache_remove(cache_t *cache, const char *key);

/*
 * cache_clear - Remove all entries from the cache
 *
 * @param cache: Cache
 *
 * Reset to empty state, but keep the cache structure.
 */
void cache_clear(cache_t *cache);

/* ============================================================================
 * Statistics Functions
 * ============================================================================ */

/*
 * Cache statistics structure
 */
typedef struct {
    unsigned long hits;
    unsigned long misses;
    unsigned long evictions;
    size_t current_size;
    size_t max_size;
    int num_entries;
    double hit_rate;            /* hits / (hits + misses) */
} cache_stats_t;

/*
 * cache_get_stats - Get cache statistics
 *
 * @param cache: Cache
 * @param stats: Output structure for statistics
 */
void cache_get_stats(cache_t *cache, cache_stats_t *stats);

/*
 * cache_reset_stats - Reset hit/miss/eviction counters
 *
 * @param cache: Cache
 */
void cache_reset_stats(cache_t *cache);

/* ============================================================================
 * Internal Functions
 * ============================================================================ */

/*
 * cache_evict_lru - Evict the least recently used entry
 *
 * @param cache: Cache (must hold write lock)
 * @return: true if an entry was evicted, false if cache was empty
 *
 * Remove the entry at LRU tail (least recently used).
 */
bool cache_evict_lru(cache_t *cache);

/*
 * cache_hash - Compute hash bucket index for a key
 *
 * @param cache: Cache
 * @param key: Cache key
 * @return: Bucket index
 */
int cache_hash(cache_t *cache, const char *key);

/*
 * cache_move_to_front - Move entry to front of LRU list
 *
 * @param cache: Cache (must hold write lock)
 * @param entry: Entry to move
 *
 * Called when an entry is accessed.
 */
void cache_move_to_front(cache_t *cache, cache_entry_t *entry);

#endif /* CACHE_H */
