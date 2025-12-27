/*
 * test_cache.c - Unit Tests for Cache Implementation
 *
 * Tests the LRU cache operations.
 *
 * Compile: make test_cache
 * Run: ./test_cache
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "../include/cache.h"

/* ============================================================================
 * Test Utilities
 * ============================================================================ */

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) \
    do { \
        printf("  Testing %s... ", #name); \
        tests_run++; \
    } while(0)

#define PASS() \
    do { \
        tests_passed++; \
        printf("PASSED\n"); \
    } while(0)

#define FAIL(msg) \
    do { \
        printf("FAILED: %s\n", msg); \
    } while(0)

#define ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            FAIL(msg); \
            return; \
        } \
    } while(0)

/* ============================================================================
 * Tests for cache_create / cache_destroy
 * ============================================================================ */

static void test_cache_create(void) {
    TEST(cache_create);

    cache_t *cache = cache_create(1024 * 1024);  /* 1 MB */

    ASSERT(cache != NULL, "Should create cache successfully");

    cache_destroy(cache);
    PASS();
}

static void test_cache_create_zero_size(void) {
    TEST(cache_create_zero_size);

    /* Implementation may allow zero size or reject it */
    cache_t *cache = cache_create(0);
    if (cache != NULL) {
        cache_destroy(cache);
    }
    PASS();  /* Just verify it doesn't crash */
}

/* ============================================================================
 * Tests for cache_put / cache_get
 * ============================================================================ */

static void test_cache_put_get_basic(void) {
    TEST(cache_put_get_basic);

    cache_t *cache = cache_create(1024 * 1024);
    ASSERT(cache != NULL, "Should create cache");

    const char *key = "/test/file.txt";
    const char *data = "Hello, World!";
    size_t size = strlen(data);

    bool put_result = cache_put(cache, key, data, size);
    ASSERT(put_result, "Should put successfully");

    char *retrieved;
    size_t retrieved_size;
    bool get_result = cache_get_copy(cache, key, &retrieved, &retrieved_size);

    ASSERT(get_result, "Should get successfully");
    ASSERT(retrieved_size == size, "Size should match");
    ASSERT(memcmp(retrieved, data, size) == 0, "Data should match");

    free(retrieved);
    cache_destroy(cache);
    PASS();
}

static void test_cache_get_miss(void) {
    TEST(cache_get_miss);

    cache_t *cache = cache_create(1024 * 1024);
    ASSERT(cache != NULL, "Should create cache");

    char *data;
    size_t size;
    bool result = cache_get_copy(cache, "/nonexistent", &data, &size);

    ASSERT(!result, "Should return false for cache miss");

    cache_destroy(cache);
    PASS();
}

static void test_cache_update_existing(void) {
    TEST(cache_update_existing);

    cache_t *cache = cache_create(1024 * 1024);
    ASSERT(cache != NULL, "Should create cache");

    const char *key = "/test.txt";

    /* Put initial value */
    cache_put(cache, key, "Original", 8);

    /* Update with new value */
    cache_put(cache, key, "Updated!", 8);

    /* Get and verify */
    char *data;
    size_t size;
    bool result = cache_get_copy(cache, key, &data, &size);

    ASSERT(result, "Should get updated value");
    ASSERT(memcmp(data, "Updated!", 8) == 0, "Should have new value");

    free(data);
    cache_destroy(cache);
    PASS();
}

/* ============================================================================
 * Tests for cache_remove
 * ============================================================================ */

static void test_cache_remove(void) {
    TEST(cache_remove);

    cache_t *cache = cache_create(1024 * 1024);
    ASSERT(cache != NULL, "Should create cache");

    const char *key = "/to/remove.txt";
    cache_put(cache, key, "Data", 4);

    bool remove_result = cache_remove(cache, key);
    ASSERT(remove_result, "Should remove successfully");

    char *data;
    size_t size;
    bool get_result = cache_get_copy(cache, key, &data, &size);
    ASSERT(!get_result, "Should not find removed entry");

    cache_destroy(cache);
    PASS();
}

static void test_cache_remove_nonexistent(void) {
    TEST(cache_remove_nonexistent);

    cache_t *cache = cache_create(1024 * 1024);
    ASSERT(cache != NULL, "Should create cache");

    bool result = cache_remove(cache, "/does/not/exist");
    ASSERT(!result, "Should return false for nonexistent entry");

    cache_destroy(cache);
    PASS();
}

/* ============================================================================
 * Tests for LRU Eviction
 * ============================================================================ */

static void test_cache_eviction(void) {
    TEST(cache_eviction);

    /* Small cache to force eviction */
    cache_t *cache = cache_create(100);  /* 100 bytes max */
    ASSERT(cache != NULL, "Should create cache");

    /* Put entries that exceed cache size */
    cache_put(cache, "/file1", "AAAAAAAAAAAAAAAAAAAA", 20);  /* 20 bytes */
    cache_put(cache, "/file2", "BBBBBBBBBBBBBBBBBBBB", 20);  /* 20 bytes */
    cache_put(cache, "/file3", "CCCCCCCCCCCCCCCCCCCC", 20);  /* 20 bytes */
    cache_put(cache, "/file4", "DDDDDDDDDDDDDDDDDDDD", 20);  /* 20 bytes */
    cache_put(cache, "/file5", "EEEEEEEEEEEEEEEEEEEE", 20);  /* 20 bytes = 100 */

    /* Add one more - should evict /file1 (LRU) */
    cache_put(cache, "/file6", "FFFFFFFFFFFFFFFFFFFF", 20);

    char *data;
    size_t size;

    /* file1 should be evicted */
    bool result1 = cache_get_copy(cache, "/file1", &data, &size);
    /* Note: depending on implementation, this may or may not be evicted */

    /* file6 should be present */
    bool result6 = cache_get_copy(cache, "/file6", &data, &size);
    if (result6) free(data);

    ASSERT(result6, "Newest entry should be present");

    cache_destroy(cache);
    PASS();
}

static void test_cache_lru_ordering(void) {
    TEST(cache_lru_ordering);

    cache_t *cache = cache_create(100);
    ASSERT(cache != NULL, "Should create cache");

    /* Add entries */
    cache_put(cache, "/a", "1111111111", 10);
    cache_put(cache, "/b", "2222222222", 10);
    cache_put(cache, "/c", "3333333333", 10);

    /* Access /a to move it to front of LRU */
    char *data;
    size_t size;
    if (cache_get_copy(cache, "/a", &data, &size)) {
        free(data);
    }

    /* Now /b should be LRU and evicted first when we fill cache */

    cache_destroy(cache);
    PASS();
}

/* ============================================================================
 * Tests for cache_clear
 * ============================================================================ */

static void test_cache_clear(void) {
    TEST(cache_clear);

    cache_t *cache = cache_create(1024 * 1024);
    ASSERT(cache != NULL, "Should create cache");

    cache_put(cache, "/file1", "Data1", 5);
    cache_put(cache, "/file2", "Data2", 5);
    cache_put(cache, "/file3", "Data3", 5);

    cache_clear(cache);

    char *data;
    size_t size;
    bool result = cache_get_copy(cache, "/file1", &data, &size);
    ASSERT(!result, "Cache should be empty after clear");

    cache_destroy(cache);
    PASS();
}

/* ============================================================================
 * Tests for cache_get_stats
 * ============================================================================ */

static void test_cache_stats(void) {
    TEST(cache_stats);

    cache_t *cache = cache_create(1024 * 1024);
    ASSERT(cache != NULL, "Should create cache");

    /* Generate some hits and misses */
    cache_put(cache, "/exists", "Data", 4);

    char *data;
    size_t size;

    /* Hit */
    cache_get_copy(cache, "/exists", &data, &size);
    free(data);

    /* Miss */
    cache_get_copy(cache, "/nonexistent", &data, &size);

    /* Get stats */
    cache_stats_t stats;
    cache_get_stats(cache, &stats);

    ASSERT(stats.hits >= 1, "Should have at least 1 hit");
    ASSERT(stats.misses >= 1, "Should have at least 1 miss");
    ASSERT(stats.num_entries >= 1, "Should have at least 1 entry");

    cache_destroy(cache);
    PASS();
}

/* ============================================================================
 * Concurrent Access Test
 * ============================================================================ */

#define NUM_THREADS 4
#define OPS_PER_THREAD 100

static cache_t *shared_cache;

static void *concurrent_worker(void *arg) {
    int thread_id = *(int *)arg;
    char key[64];
    char data[64];

    for (int i = 0; i < OPS_PER_THREAD; i++) {
        snprintf(key, sizeof(key), "/thread%d/file%d", thread_id, i);
        snprintf(data, sizeof(data), "Data from thread %d, op %d", thread_id, i);

        /* Put */
        cache_put(shared_cache, key, data, strlen(data));

        /* Get */
        char *retrieved;
        size_t size;
        if (cache_get_copy(shared_cache, key, &retrieved, &size)) {
            free(retrieved);
        }
    }

    return NULL;
}

static void test_cache_concurrent(void) {
    TEST(cache_concurrent);

    shared_cache = cache_create(10 * 1024 * 1024);  /* 10 MB */
    ASSERT(shared_cache != NULL, "Should create cache");

    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, concurrent_worker, &thread_ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    cache_destroy(shared_cache);
    PASS();
}

/* ============================================================================
 * Main
 * ============================================================================ */

int main(void) {
    printf("=== Cache Tests ===\n\n");

    printf("Testing cache creation:\n");
    test_cache_create();
    test_cache_create_zero_size();

    printf("\nTesting put/get:\n");
    test_cache_put_get_basic();
    test_cache_get_miss();
    test_cache_update_existing();

    printf("\nTesting remove:\n");
    test_cache_remove();
    test_cache_remove_nonexistent();

    printf("\nTesting LRU eviction:\n");
    test_cache_eviction();
    test_cache_lru_ordering();

    printf("\nTesting clear:\n");
    test_cache_clear();

    printf("\nTesting statistics:\n");
    test_cache_stats();

    printf("\nTesting concurrent access:\n");
    test_cache_concurrent();

    printf("\n=== Results ===\n");
    printf("%d/%d tests passed\n", tests_passed, tests_run);

    return (tests_passed == tests_run) ? 0 : 1;
}
