/*
 * CS-6200 Preparation - Module 05: Thread Pool Test
 *
 * Test harness for the thread pool implementation.
 *
 * Compile: clang -Wall -Wextra -g -pthread thread_pool.c work_queue.c pool_test.c -o pool_test
 * Run:     ./pool_test
 *
 * Difficulty: [HARD]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdatomic.h>

#include "thread_pool.h"

/* ============================================================================
 * TEST UTILITIES
 * ============================================================================
 */

/* Atomic counter for tracking completed tasks */
atomic_int completed_tasks = 0;

void reset_counter(void) {
    atomic_store(&completed_tasks, 0);
}

int get_counter(void) {
    return atomic_load(&completed_tasks);
}

/* ============================================================================
 * TEST 1: Basic functionality
 * ============================================================================
 */

void simple_task(void *arg) {
    int task_id = *(int *)arg;
    printf("  Task %d: Running on thread %lu\n", task_id, (unsigned long)pthread_self());
    usleep(100000);  /* Simulate work */
    atomic_fetch_add(&completed_tasks, 1);
    free(arg);
}

void test_basic(void) {
    printf("\n=== Test 1: Basic Functionality ===\n");

    reset_counter();

    printf("Creating thread pool with 4 workers...\n");
    ThreadPool *pool = thread_pool_create(4);

    if (pool == NULL) {
        printf("FAILED: Could not create pool\n");
        return;
    }

    printf("Submitting 10 tasks...\n");
    for (int i = 0; i < 10; i++) {
        int *task_id = malloc(sizeof(int));
        *task_id = i;
        if (thread_pool_submit(pool, simple_task, task_id) != 0) {
            printf("FAILED: Could not submit task %d\n", i);
            free(task_id);
        }
    }

    printf("Destroying pool (waiting for tasks to complete)...\n");
    thread_pool_destroy(pool);

    printf("Completed tasks: %d\n", get_counter());
    printf("Test 1: %s\n", get_counter() == 10 ? "PASSED" : "FAILED");
}

/* ============================================================================
 * TEST 2: High volume
 * ============================================================================
 */

void quick_task(void *arg) {
    (void)arg;
    atomic_fetch_add(&completed_tasks, 1);
}

void test_high_volume(void) {
    printf("\n=== Test 2: High Volume ===\n");

    reset_counter();

    int num_tasks = 10000;

    printf("Creating thread pool with 8 workers...\n");
    ThreadPool *pool = thread_pool_create(8);

    if (pool == NULL) {
        printf("FAILED: Could not create pool\n");
        return;
    }

    printf("Submitting %d tasks...\n", num_tasks);
    for (int i = 0; i < num_tasks; i++) {
        thread_pool_submit(pool, quick_task, NULL);
    }

    printf("Destroying pool...\n");
    thread_pool_destroy(pool);

    printf("Completed tasks: %d\n", get_counter());
    printf("Test 2: %s\n", get_counter() == num_tasks ? "PASSED" : "FAILED");
}

/* ============================================================================
 * TEST 3: Slow tasks (worker blocking)
 * ============================================================================
 */

void slow_task(void *arg) {
    int *id = (int *)arg;
    printf("  Slow task %d: Starting\n", *id);
    sleep(1);  /* Long operation */
    printf("  Slow task %d: Done\n", *id);
    atomic_fetch_add(&completed_tasks, 1);
    free(id);
}

void test_slow_tasks(void) {
    printf("\n=== Test 3: Slow Tasks (2 workers, 4 tasks) ===\n");

    reset_counter();

    printf("Creating thread pool with 2 workers...\n");
    ThreadPool *pool = thread_pool_create(2);

    printf("Submitting 4 slow tasks (1 second each)...\n");
    printf("With 2 workers, should take about 2 seconds.\n\n");

    time_t start = time(NULL);

    for (int i = 0; i < 4; i++) {
        int *task_id = malloc(sizeof(int));
        *task_id = i;
        thread_pool_submit(pool, slow_task, task_id);
    }

    thread_pool_destroy(pool);

    time_t elapsed = time(NULL) - start;
    printf("\nElapsed time: %ld seconds\n", elapsed);
    printf("Test 3: %s\n", get_counter() == 4 ? "PASSED" : "FAILED");
}

/* ============================================================================
 * TEST 4: Pending count
 * ============================================================================
 */

void delayed_task(void *arg) {
    (void)arg;
    usleep(500000);  /* 0.5 seconds */
    atomic_fetch_add(&completed_tasks, 1);
}

void test_pending(void) {
    printf("\n=== Test 4: Pending Count ===\n");

    reset_counter();

    ThreadPool *pool = thread_pool_create(2);

    /* Submit tasks faster than workers can process */
    for (int i = 0; i < 10; i++) {
        thread_pool_submit(pool, delayed_task, NULL);
    }

    /* Check pending count immediately */
    int pending = thread_pool_pending(pool);
    printf("Pending tasks after submit: %d\n", pending);

    /* Wait and check again */
    sleep(1);
    pending = thread_pool_pending(pool);
    printf("Pending tasks after 1 second: %d\n", pending);

    thread_pool_destroy(pool);

    printf("Test 4: %s\n", get_counter() == 10 ? "PASSED" : "FAILED");
}

/* ============================================================================
 * TEST 5: Task with complex data
 * ============================================================================
 */

typedef struct {
    int id;
    char message[64];
    int *result;
} ComplexTask;

void complex_task_func(void *arg) {
    ComplexTask *task = (ComplexTask *)arg;

    printf("  Complex task %d: %s\n", task->id, task->message);

    /* Store result */
    *(task->result) = task->id * 10;

    atomic_fetch_add(&completed_tasks, 1);
    free(task);
}

void test_complex_data(void) {
    printf("\n=== Test 5: Complex Task Data ===\n");

    reset_counter();

    ThreadPool *pool = thread_pool_create(4);

    int results[5] = {0};

    for (int i = 0; i < 5; i++) {
        ComplexTask *task = malloc(sizeof(ComplexTask));
        task->id = i;
        snprintf(task->message, sizeof(task->message), "Processing item %d", i);
        task->result = &results[i];

        thread_pool_submit(pool, complex_task_func, task);
    }

    thread_pool_destroy(pool);

    printf("Results: ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", results[i]);
    }
    printf("\n");

    int pass = 1;
    for (int i = 0; i < 5; i++) {
        if (results[i] != i * 10) pass = 0;
    }

    printf("Test 5: %s\n", pass ? "PASSED" : "FAILED");
}

/* ============================================================================
 * TEST 6: Simulated file transfer (GIOS-like)
 * ============================================================================
 */

typedef struct {
    int client_id;
    const char *filename;
    size_t filesize;
} FileTransferRequest;

void handle_file_transfer(void *arg) {
    FileTransferRequest *req = (FileTransferRequest *)arg;

    printf("  Worker: Handling client %d, file '%s' (%zu bytes)\n",
           req->client_id, req->filename, req->filesize);

    /* Simulate transfer time based on file size */
    usleep(req->filesize * 10);  /* 10us per byte */

    printf("  Worker: Client %d transfer complete\n", req->client_id);

    atomic_fetch_add(&completed_tasks, 1);
    free(req);
}

void test_file_transfer_sim(void) {
    printf("\n=== Test 6: Simulated File Transfer (GIOS Pattern) ===\n");

    reset_counter();

    printf("This simulates GIOS Project 1 Part 2:\n");
    printf("- Boss thread accepts connections and submits work\n");
    printf("- Worker threads handle file transfers\n\n");

    ThreadPool *pool = thread_pool_create(4);

    /* Simulate incoming connections */
    FileTransferRequest requests[] = {
        {0, "/small.txt", 1000},
        {1, "/medium.bin", 5000},
        {2, "/large.dat", 10000},
        {3, "/tiny.txt", 100},
        {4, "/huge.bin", 20000},
    };

    printf("Boss: Accepting 5 client connections...\n\n");

    for (int i = 0; i < 5; i++) {
        FileTransferRequest *req = malloc(sizeof(FileTransferRequest));
        *req = requests[i];

        printf("Boss: Client %d connected, requesting '%s'\n",
               req->client_id, req->filename);
        thread_pool_submit(pool, handle_file_transfer, req);
    }

    printf("\nBoss: All connections accepted, waiting for transfers...\n\n");

    thread_pool_destroy(pool);

    printf("\nAll transfers complete!\n");
    printf("Test 6: %s\n", get_counter() == 5 ? "PASSED" : "FAILED");
}

/* ============================================================================
 * TEST 7: Error handling
 * ============================================================================
 */

void test_error_handling(void) {
    printf("\n=== Test 7: Error Handling ===\n");

    int pass = 1;

    /* Test: Create with 0 threads */
    ThreadPool *pool = thread_pool_create(0);
    if (pool != NULL) {
        printf("FAILED: Should not create pool with 0 threads\n");
        thread_pool_destroy(pool);
        pass = 0;
    } else {
        printf("Correctly rejected 0 threads\n");
    }

    /* Test: Submit to NULL pool */
    if (thread_pool_submit(NULL, simple_task, NULL) != -1) {
        printf("FAILED: Should reject submit to NULL pool\n");
        pass = 0;
    } else {
        printf("Correctly rejected submit to NULL pool\n");
    }

    /* Test: Destroy NULL pool (should not crash) */
    thread_pool_destroy(NULL);
    printf("Correctly handled destroy NULL pool\n");

    printf("Test 7: %s\n", pass ? "PASSED" : "FAILED");
}

/* ============================================================================
 * MAIN
 * ============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n================================================\n");
    printf("  Module 05: Thread Pool Test\n");
    printf("================================================\n");

    if (argc > 1) {
        int test = atoi(argv[1]);
        switch (test) {
            case 1: test_basic(); break;
            case 2: test_high_volume(); break;
            case 3: test_slow_tasks(); break;
            case 4: test_pending(); break;
            case 5: test_complex_data(); break;
            case 6: test_file_transfer_sim(); break;
            case 7: test_error_handling(); break;
            default: printf("Unknown test: %d\n", test);
        }
    } else {
        test_basic();
        test_high_volume();
        test_slow_tasks();
        test_pending();
        test_complex_data();
        test_file_transfer_sim();
        test_error_handling();
    }

    printf("\n================================================\n");
    printf("  Thread Pool Tests Complete!\n");
    printf("\n");
    printf("  GIOS Connection:\n");
    printf("  - Boss thread = main() accepting connections\n");
    printf("  - Worker threads = thread pool processing transfers\n");
    printf("  - Work queue = pending file transfer requests\n");
    printf("  - This is EXACTLY the pattern for Project 1 Part 2!\n");
    printf("================================================\n\n");

    return 0;
}
