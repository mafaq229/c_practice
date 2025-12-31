/*
CS-6200 Preparation - Module 03: Counter Benchmark

Compare different synchronization approaches for a shared counter.
Understanding performance implications helps design better systems!

Compile: clang -Wall -Wextra -g -pthread counter_benchmark.c -o counter_benchmark
Run:     ./counter_benchmark

Difficulty: [HARD]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>

#define ITERATIONS 10000000
#define NUM_THREADS 4

/* ============================================================================
Helper: Get time in milliseconds
============================================================================
*/

double get_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

/* ============================================================================
APPROACH 1: No synchronization (BROKEN - baseline)
============================================================================
*/

volatile int counter_unsafe = 0;

void *unsafe_increment(void *arg) {
    int iters = *(int *)arg;

    for (int i = 0; i < iters; i++) {
        counter_unsafe++;
    }

    return NULL;
}

void benchmark_unsafe(void) {
    printf("\n--- Approach 1: No Synchronization (BROKEN) ---\n");

    counter_unsafe = 0;
    pthread_t threads[NUM_THREADS];
    int iters_per_thread = ITERATIONS / NUM_THREADS;

    double start = get_time_ms();

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, unsafe_increment, &iters_per_thread);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    double end = get_time_ms();

    printf("Time: %.2f ms\n", end - start);
    printf("Expected: %d\n", ITERATIONS);
    printf("Actual:   %d (lost %d)\n", counter_unsafe, ITERATIONS - counter_unsafe);
}

/* ============================================================================
APPROACH 2: Mutex (correct but slow)
============================================================================
*/

int counter_mutex = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *mutex_increment(void *arg) {
    int iters = *(int *)arg;

    for (int i = 0; i < iters; i++) {
        pthread_mutex_lock(&mutex);
        counter_mutex++;
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void benchmark_mutex(void) {
    printf("\n--- Approach 2: Mutex ---\n");

    counter_mutex = 0;
    pthread_t threads[NUM_THREADS];
    int iters_per_thread = ITERATIONS / NUM_THREADS;

    double start = get_time_ms();

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, mutex_increment, &iters_per_thread);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    double end = get_time_ms();

    printf("Time: %.2f ms\n", end - start);
    printf("Expected: %d\n", ITERATIONS);
    printf("Actual:   %d\n", counter_mutex);
}

/* ============================================================================
APPROACH 3: Spin lock (busy waiting)
NOTE: pthread_spinlock_t is Linux-only. Not available on macOS.
============================================================================
*/

#ifdef __linux__
int counter_spin = 0;
pthread_spinlock_t spinlock;

void *spin_increment(void *arg) {
    int iters = *(int *)arg;

    for (int i = 0; i < iters; i++) {
        pthread_spin_lock(&spinlock);
        counter_spin++;
        pthread_spin_unlock(&spinlock);
    }

    return NULL;
}

void benchmark_spinlock(void) {
    printf("\n--- Approach 3: Spinlock ---\n");

    counter_spin = 0;
    pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);

    pthread_t threads[NUM_THREADS];
    int iters_per_thread = ITERATIONS / NUM_THREADS;

    double start = get_time_ms();

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, spin_increment, &iters_per_thread);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    double end = get_time_ms();

    printf("Time: %.2f ms\n", end - start);
    printf("Expected: %d\n", ITERATIONS);
    printf("Actual:   %d\n", counter_spin);

    pthread_spin_destroy(&spinlock);
}
#else
void benchmark_spinlock(void) {
    printf("\n--- Approach 3: Spinlock ---\n");
    printf("NOTE: pthread_spinlock_t is not available on macOS.\n");
    printf("Run in Docker/Linux to test spinlocks.\n");
}
#endif

/* ============================================================================
APPROACH 4: Atomic operations (fast and correct)
============================================================================
*/

atomic_int counter_atomic = 0;

void *atomic_increment(void *arg) {
    int iters = *(int *)arg;

    for (int i = 0; i < iters; i++) {
        atomic_fetch_add(&counter_atomic, 1);
    }

    return NULL;
}

void benchmark_atomic(void) {
    printf("\n--- Approach 4: Atomic Operations ---\n");

    atomic_store(&counter_atomic, 0);
    pthread_t threads[NUM_THREADS];
    int iters_per_thread = ITERATIONS / NUM_THREADS;

    double start = get_time_ms();

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, atomic_increment, &iters_per_thread);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    double end = get_time_ms();

    printf("Time: %.2f ms\n", end - start);
    printf("Expected: %d\n", ITERATIONS);
    printf("Actual:   %d\n", atomic_load(&counter_atomic));
}

/* ============================================================================
APPROACH 5: Local counters + final merge (fastest)
============================================================================
*/

void *local_increment(void *arg) {
    int iters = *(int *)arg;
    long *result = malloc(sizeof(long));
    *result = 0;

    for (int i = 0; i < iters; i++) {
        (*result)++;
    }

    return result;
}

void benchmark_local(void) {
    printf("\n--- Approach 5: Local Counters + Merge ---\n");

    pthread_t threads[NUM_THREADS];
    int iters_per_thread = ITERATIONS / NUM_THREADS;

    double start = get_time_ms();

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, local_increment, &iters_per_thread);
    }

    long total = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        long *result;
        pthread_join(threads[i], (void **)&result);
        total += *result;
        free(result);
    }

    double end = get_time_ms();

    printf("Time: %.2f ms\n", end - start);
    printf("Expected: %d\n", ITERATIONS);
    printf("Actual:   %ld\n", total);
}

/* ============================================================================
APPROACH 6: Batched mutex (compromise)
============================================================================
*/

int counter_batched = 0;
pthread_mutex_t batch_mutex = PTHREAD_MUTEX_INITIALIZER;

#define BATCH_SIZE 1000

void *batched_increment(void *arg) {
    int iters = *(int *)arg;
    int local_count = 0;

    for (int i = 0; i < iters; i++) {
        local_count++;

        if (local_count == BATCH_SIZE) {
            pthread_mutex_lock(&batch_mutex);
            counter_batched += local_count;
            pthread_mutex_unlock(&batch_mutex);
            local_count = 0;
        }
    }

    /* Flush remaining */
    if (local_count > 0) {
        pthread_mutex_lock(&batch_mutex);
        counter_batched += local_count;
        pthread_mutex_unlock(&batch_mutex);
    }

    return NULL;
}

void benchmark_batched(void) {
    printf("\n--- Approach 6: Batched Mutex (batch=%d) ---\n", BATCH_SIZE);

    counter_batched = 0;
    pthread_t threads[NUM_THREADS];
    int iters_per_thread = ITERATIONS / NUM_THREADS;

    double start = get_time_ms();

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, batched_increment, &iters_per_thread);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    double end = get_time_ms();

    printf("Time: %.2f ms\n", end - start);
    printf("Expected: %d\n", ITERATIONS);
    printf("Actual:   %d\n", counter_batched);
}

/* ============================================================================
SINGLE-THREADED BASELINE
============================================================================
*/

void benchmark_single(void) {
    printf("\n--- Baseline: Single-Threaded ---\n");

    int counter = 0;

    double start = get_time_ms();

    for (int i = 0; i < ITERATIONS; i++) {
        counter++;
    }

    double end = get_time_ms();

    printf("Time: %.2f ms\n", end - start);
    printf("Count: %d\n", counter);
}

/* ============================================================================
MAIN
============================================================================
*/

int main(void) {
    printf("\n================================================\n");
    printf("  Module 03: Counter Benchmark\n");
    printf("  %d iterations with %d threads\n", ITERATIONS, NUM_THREADS);
    printf("================================================\n");

    benchmark_single();
    benchmark_unsafe();
    benchmark_mutex();
    benchmark_spinlock();
    benchmark_atomic();
    benchmark_local();
    benchmark_batched();

    printf("\n================================================\n");
    printf("  Summary\n");
    printf("================================================\n");
    printf("\n");
    printf("  CORRECTNESS:\n");
    printf("  - Unsafe:     BROKEN (loses updates)\n");
    printf("  - All others: CORRECT\n");
    printf("\n");
    printf("  PERFORMANCE (typical):\n");
    printf("  - Single:     Fastest (no sync overhead)\n");
    printf("  - Local:      Near single-threaded speed\n");
    printf("  - Batched:    Good balance\n");
    printf("  - Atomic:     Good for simple operations\n");
    printf("  - Spinlock:   Can be fast for short critical sections\n");
    printf("  - Mutex:      Slowest (context switch overhead)\n");
    printf("\n");
    printf("  RECOMMENDATION:\n");
    printf("  - For counters: atomic or local+merge\n");
    printf("  - For complex operations: mutex with batching\n");
    printf("================================================\n\n");

    return 0;
}
