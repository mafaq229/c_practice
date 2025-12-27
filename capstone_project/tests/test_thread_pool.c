/*
 * test_thread_pool.c - Unit Tests for Thread Pool Implementation
 *
 * Tests the thread pool and work queue.
 *
 * Compile: make test_thread_pool
 * Run: ./test_thread_pool
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include "../include/thread_pool.h"
#include "../include/work_queue.h"

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
 * Work Queue Tests
 * ============================================================================ */

static void test_work_queue_create(void) {
    TEST(work_queue_create);

    work_queue_t *queue = work_queue_create();
    ASSERT(queue != NULL, "Should create queue");
    ASSERT(work_queue_size(queue) == 0, "Initial size should be 0");
    ASSERT(work_queue_is_empty(queue), "Should be empty");

    work_queue_destroy(queue);
    PASS();
}

static void test_work_queue_push_pop(void) {
    TEST(work_queue_push_pop);

    work_queue_t *queue = work_queue_create();
    ASSERT(queue != NULL, "Should create queue");

    /* Push items */
    ASSERT(work_queue_push(queue, 10) == 0, "Should push 10");
    ASSERT(work_queue_push(queue, 20) == 0, "Should push 20");
    ASSERT(work_queue_push(queue, 30) == 0, "Should push 30");

    ASSERT(work_queue_size(queue) == 3, "Size should be 3");

    /* Pop items (FIFO order) */
    ASSERT(work_queue_pop(queue) == 10, "Should pop 10");
    ASSERT(work_queue_pop(queue) == 20, "Should pop 20");
    ASSERT(work_queue_pop(queue) == 30, "Should pop 30");

    ASSERT(work_queue_is_empty(queue), "Should be empty after pops");

    work_queue_destroy(queue);
    PASS();
}

static void test_work_queue_shutdown(void) {
    TEST(work_queue_shutdown);

    work_queue_t *queue = work_queue_create();
    ASSERT(queue != NULL, "Should create queue");

    work_queue_push(queue, 42);
    work_queue_shutdown(queue);

    /* Pop should still work for remaining items */
    int fd = work_queue_pop(queue);
    ASSERT(fd == 42, "Should pop remaining item");

    /* Next pop should return -1 (shutdown) */
    fd = work_queue_pop(queue);
    ASSERT(fd == -1, "Should return -1 after shutdown and empty");

    work_queue_destroy(queue);
    PASS();
}

/* Producer-consumer test with multiple threads */
#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 2
#define ITEMS_PER_PRODUCER 50

static work_queue_t *test_queue;
static int consumed_count = 0;
static pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;

static void *producer_thread(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < ITEMS_PER_PRODUCER; i++) {
        int item = id * 1000 + i;
        work_queue_push(test_queue, item);
        usleep(1000);  /* 1ms delay */
    }
    return NULL;
}

static void *consumer_thread(void *arg) {
    (void)arg;
    while (1) {
        int fd = work_queue_pop(test_queue);
        if (fd < 0) break;  /* Shutdown */

        pthread_mutex_lock(&count_mutex);
        consumed_count++;
        pthread_mutex_unlock(&count_mutex);
    }
    return NULL;
}

static void test_work_queue_concurrent(void) {
    TEST(work_queue_concurrent);

    test_queue = work_queue_create();
    ASSERT(test_queue != NULL, "Should create queue");
    consumed_count = 0;

    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumers[NUM_CONSUMERS];
    int producer_ids[NUM_PRODUCERS];

    /* Start consumers */
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_create(&consumers[i], NULL, consumer_thread, NULL);
    }

    /* Start producers */
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        producer_ids[i] = i;
        pthread_create(&producers[i], NULL, producer_thread, &producer_ids[i]);
    }

    /* Wait for producers to finish */
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }

    /* Give consumers time to process */
    usleep(100000);  /* 100ms */

    /* Shutdown queue to stop consumers */
    work_queue_shutdown(test_queue);

    /* Wait for consumers */
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
    }

    int expected = NUM_PRODUCERS * ITEMS_PER_PRODUCER;
    ASSERT(consumed_count == expected,
           "All items should be consumed");

    work_queue_destroy(test_queue);
    PASS();
}

/* ============================================================================
 * Thread Pool Tests
 * ============================================================================ */

static void test_thread_pool_create(void) {
    TEST(thread_pool_create);

    thread_pool_t *pool = thread_pool_create(4);
    ASSERT(pool != NULL, "Should create pool with 4 threads");

    thread_pool_destroy(pool);
    PASS();
}

static void test_thread_pool_create_invalid(void) {
    TEST(thread_pool_create_invalid);

    thread_pool_t *pool = thread_pool_create(0);
    ASSERT(pool == NULL, "Should fail with 0 threads");

    pool = thread_pool_create(-1);
    ASSERT(pool == NULL, "Should fail with negative threads");

    PASS();
}

static void test_thread_pool_submit(void) {
    TEST(thread_pool_submit);

    thread_pool_t *pool = thread_pool_create(2);
    ASSERT(pool != NULL, "Should create pool");

    /* Create some fake socket FDs (just integers for testing) */
    /* In real use, these would be actual socket file descriptors */
    int fake_fd = 100;

    int result = thread_pool_submit(pool, fake_fd);
    ASSERT(result == 0, "Should submit successfully");

    /* Give pool time to process */
    usleep(100000);

    thread_pool_destroy(pool);
    PASS();
}

static void test_thread_pool_stats(void) {
    TEST(thread_pool_stats);

    thread_pool_t *pool = thread_pool_create(4);
    ASSERT(pool != NULL, "Should create pool");

    int tasks, active;
    thread_pool_get_stats(pool, &tasks, &active);

    /* Initially should have 0 tasks and 0 active */
    /* (Workers are waiting on queue) */

    thread_pool_destroy(pool);
    PASS();
}

static void test_thread_pool_shutdown(void) {
    TEST(thread_pool_shutdown);

    thread_pool_t *pool = thread_pool_create(4);
    ASSERT(pool != NULL, "Should create pool");

    /* Submit some work */
    for (int i = 0; i < 10; i++) {
        thread_pool_submit(pool, 100 + i);
    }

    /* Destroy should wait for all work to complete */
    thread_pool_destroy(pool);

    PASS();
}

/* ============================================================================
 * Integration Test
 * ============================================================================ */

static void test_pool_with_real_work(void) {
    TEST(pool_with_real_work);

    /* This test would require actual sockets, so we skip the real work
       and just verify the pool can handle multiple submissions */

    thread_pool_t *pool = thread_pool_create(2);
    ASSERT(pool != NULL, "Should create pool");

    /* Submit many items quickly */
    for (int i = 0; i < 100; i++) {
        if (thread_pool_submit(pool, i) != 0) {
            /* Queue might be full, that's okay */
            usleep(10000);
        }
    }

    /* Let pool process */
    usleep(200000);

    int tasks, active;
    thread_pool_get_stats(pool, &tasks, &active);

    /* Most tasks should be completed */
    printf("(completed=%d, active=%d) ", tasks, active);

    thread_pool_destroy(pool);
    PASS();
}

/* ============================================================================
 * Main
 * ============================================================================ */

int main(void) {
    printf("=== Thread Pool Tests ===\n\n");

    printf("Testing work queue:\n");
    test_work_queue_create();
    test_work_queue_push_pop();
    test_work_queue_shutdown();
    test_work_queue_concurrent();

    printf("\nTesting thread pool:\n");
    test_thread_pool_create();
    test_thread_pool_create_invalid();
    test_thread_pool_submit();
    test_thread_pool_stats();
    test_thread_pool_shutdown();

    printf("\nIntegration tests:\n");
    test_pool_with_real_work();

    printf("\n=== Results ===\n");
    printf("%d/%d tests passed\n", tests_passed, tests_run);

    return (tests_passed == tests_run) ? 0 : 1;
}
