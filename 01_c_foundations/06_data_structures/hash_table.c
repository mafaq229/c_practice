/*
 * CS-6200 Preparation - Module 06: Hash Table
 *
 * Hash tables provide O(1) average lookup. Used in GIOS for
 * caching, file lookups, and many other purposes.
 *
 * Compile: clang -Wall -Wextra -g hash_table.c -o hash_table
 * Run:     ./hash_table
 *
 * Difficulty: [HARD]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * HASH TABLE WITH CHAINING
 * ============================================================================
 */

#define INITIAL_CAPACITY 16
#define LOAD_FACTOR_THRESHOLD 0.75

typedef struct HashEntry {
    char *key;
    void *value;
    struct HashEntry *next;
} HashEntry;

typedef struct {
    HashEntry **buckets;  /* Array of bucket pointers */
    size_t capacity;      /* Number of buckets */
    size_t size;          /* Number of entries */
} HashTable;

/*
 * Simple hash function for strings.
 * Uses djb2 algorithm.
 */
size_t hash_string(const char *str, size_t capacity) {
    size_t hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;  /* hash * 33 + c */
    }
    return hash % capacity;
}

/*
 * TODO: Initialize hash table.
 */
int ht_init(HashTable *ht, size_t capacity) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Allocate array of capacity HashEntry pointers
     * 2. Initialize all buckets to NULL (use calloc)
     * 3. Set capacity and size
     * 4. Return 0 on success, -1 on failure
     */

    return -1;  /* TODO: Fix this */
}

/*
 * TODO: Insert or update a key-value pair.
 * Makes a copy of the key string.
 */
int ht_put(HashTable *ht, const char *key, void *value) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Calculate hash of key
     * 2. Check if key exists in bucket chain
     * 3. If exists, update value
     * 4. If not, create new entry and add to chain
     * 5. Update size
     * 6. Return 0 on success, -1 on failure
     */

    return -1;  /* TODO: Fix this */
}

/*
 * TODO: Get value for key.
 * Returns NULL if key not found.
 */
void *ht_get(HashTable *ht, const char *key) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Calculate hash of key
     * 2. Search bucket chain for key
     * 3. Return value if found, NULL if not
     */

    return NULL;  /* TODO: Fix this */
}

/*
 * TODO: Check if key exists.
 */
int ht_contains(HashTable *ht, const char *key) {
    /* TODO: Implement this function */

    return 0;  /* TODO: Fix this */
}

/*
 * TODO: Remove key-value pair.
 * Returns 0 if removed, -1 if not found.
 */
int ht_remove(HashTable *ht, const char *key) {
    /* TODO: Implement this function
     *
     * Remember to free the key string!
     */

    return -1;  /* TODO: Fix this */
}

/*
 * TODO: Free the hash table.
 * Frees all keys but NOT values (caller's responsibility).
 */
void ht_free(HashTable *ht) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. For each bucket, free all entries in chain
     * 2. Free the buckets array
     */
}

/*
 * Print hash table for debugging.
 */
void ht_print(HashTable *ht) {
    printf("HashTable (size=%zu, capacity=%zu):\n", ht->size, ht->capacity);
    for (size_t i = 0; i < ht->capacity; i++) {
        if (ht->buckets[i] != NULL) {
            printf("  [%zu]: ", i);
            HashEntry *e = ht->buckets[i];
            while (e != NULL) {
                printf("('%s') ", e->key);
                e = e->next;
            }
            printf("\n");
        }
    }
}

/* ============================================================================
 * CHALLENGE: Resizing Hash Table [HARD]
 * ============================================================================
 */

/*
 * TODO: Resize hash table when load factor is exceeded.
 */
int ht_resize(HashTable *ht, size_t new_capacity) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Allocate new buckets array
     * 2. Rehash all entries into new buckets
     * 3. Free old buckets array
     * 4. Update capacity
     *
     * This is complex! Take your time.
     */

    return -1;  /* TODO: Fix this */
}

/*
 * Put with automatic resizing.
 */
int ht_put_resize(HashTable *ht, const char *key, void *value) {
    /* Check load factor */
    double load_factor = (double)ht->size / ht->capacity;
    if (load_factor > LOAD_FACTOR_THRESHOLD) {
        if (ht_resize(ht, ht->capacity * 2) != 0) {
            /* Resize failed, but we can still try to insert */
        }
    }
    return ht_put(ht, key, value);
}

/* ============================================================================
 * INTEGER KEY HASH TABLE [MEDIUM]
 * ============================================================================
 */

typedef struct IntHashEntry {
    int key;
    void *value;
    int occupied;  /* 1 if slot is used */
} IntHashEntry;

typedef struct {
    IntHashEntry *entries;
    size_t capacity;
    size_t size;
} IntHashTable;

/*
 * TODO: Initialize integer hash table.
 */
int iht_init(IntHashTable *ht, size_t capacity) {
    /* TODO: Implement this function */

    return -1;  /* TODO: Fix this */
}

/*
 * Hash function for integers.
 */
size_t hash_int(int key, size_t capacity) {
    /* Simple modulo hash */
    return (size_t)(key >= 0 ? key : -key) % capacity;
}

/*
 * TODO: Put with linear probing.
 */
int iht_put(IntHashTable *ht, int key, void *value) {
    /* TODO: Implement this function
     *
     * Linear probing:
     * If slot is occupied by different key, try next slot.
     * Wrap around using modulo.
     */

    return -1;  /* TODO: Fix this */
}

/*
 * TODO: Get with linear probing.
 */
void *iht_get(IntHashTable *ht, int key) {
    /* TODO: Implement this function */

    return NULL;  /* TODO: Fix this */
}

void iht_free(IntHashTable *ht) {
    free(ht->entries);
    ht->entries = NULL;
    ht->capacity = ht->size = 0;
}

/* ============================================================================
 * TEST FUNCTIONS
 * ============================================================================
 */

void test_string_hashtable(void) {
    printf("\n=== String Hash Table ===\n");

    HashTable ht;
    if (ht_init(&ht, INITIAL_CAPACITY) != 0) {
        printf("TODO: Implement ht_init\n");
        return;
    }

    /* Test insert and get */
    int val1 = 100, val2 = 200, val3 = 300;
    ht_put(&ht, "apple", &val1);
    ht_put(&ht, "banana", &val2);
    ht_put(&ht, "cherry", &val3);

    ht_print(&ht);

    int *result = ht_get(&ht, "banana");
    printf("Get 'banana': %d (expected: 200)\n", result ? *result : -1);

    result = ht_get(&ht, "missing");
    printf("Get 'missing': %s (expected: not found)\n",
           result ? "found" : "not found");

    /* Test contains and remove */
    printf("Contains 'apple': %d (expected: 1)\n", ht_contains(&ht, "apple"));
    ht_remove(&ht, "apple");
    printf("After remove, contains 'apple': %d (expected: 0)\n",
           ht_contains(&ht, "apple"));

    ht_free(&ht);
    printf("Hash table freed.\n");
}

void test_int_hashtable(void) {
    printf("\n=== Integer Hash Table ===\n");

    IntHashTable ht;
    if (iht_init(&ht, 8) != 0) {
        printf("TODO: Implement iht_init\n");
        return;
    }

    int vals[] = {10, 20, 30, 40, 50};
    iht_put(&ht, 1, &vals[0]);
    iht_put(&ht, 9, &vals[1]);  /* Will hash to same slot as 1 (if capacity 8) */
    iht_put(&ht, 5, &vals[2]);

    int *result = iht_get(&ht, 9);
    printf("Get key 9: %d (expected: 20)\n", result ? *result : -1);

    iht_free(&ht);
}

/* ============================================================================
 * MAIN FUNCTION
 * ============================================================================
 */
int main(void) {
    printf("\n");
    printf("================================================\n");
    printf("  Module 06: Hash Table\n");
    printf("================================================\n");

    test_string_hashtable();
    test_int_hashtable();

    printf("\n================================================\n");
    printf("  Hash Table Complete!\n");
    printf("  Next: dynamic_buffer.c\n");
    printf("================================================\n\n");

    return 0;
}
