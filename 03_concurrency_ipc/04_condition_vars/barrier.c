/*
CS-6200 Preparation - Module 04: Thread Barrier

A barrier makes threads wait until all have reached the same point.
Useful for phased computations and synchronization.

Compile: clang -Wall -Wextra -g -pthread barrier.c -o barrier
Run:     ./barrier

Difficulty: [HARD]
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/* ============================================================================
CONCEPT: Thread Barrier
============================================================================

A barrier is a synchronization point where threads must wait
until ALL participating threads have arrived.

Use cases:
- Parallel algorithms with phases (all threads finish phase 1 before phase 2)
- Parallel testing (all threads start at the same time)
- Data parallel computations

POSIX provides pthread_barrier_t, but implementing your own is educational!
*/

/* ============================================================================
CUSTOM BARRIER IMPLEMENTATION
============================================================================
*/

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int threshold;        /* Number of threads to wait for */
    int count;            /* Number of threads currently waiting */
    int generation;       /* Barrier generation (for reusability) */
} Barrier;

/*
TODO: Initialize the barrier
*/
int barrier_init(Barrier *barrier, int num_threads) {
    if (num_threads <= 0) {
        return -1;
    }

    barrier->threshold = num_threads;
    barrier->count = 0;
    barrier->generation = 0;

    if (pthread_mutex_init(&barrier->mutex, NULL) != 0) {
        return -1;
    }

    if (pthread_cond_init(&barrier->cond, NULL) != 0) {
        pthread_mutex_destroy(&barrier->mutex);
        return -1;
    }

    return 0;
}

/*
TODO: Destroy the barrier
*/
void barrier_destroy(Barrier *barrier) {
    pthread_mutex_destroy(&barrier->mutex);
    pthread_cond_destroy(&barrier->cond);
}

/*
TODO: Wait at the barrier

Returns 1 for the "last" thread (can be used to do cleanup)
Returns 0 for all other threads
*/
int barrier_wait(Barrier *barrier) {
    pthread_mutex_lock(&barrier->mutex);

    int my_generation = barrier->generation;

    barrier->count++;

    if (barrier->count == barrier->threshold) {
        /* Last thread to arrive */
        barrier->count = 0;
        barrier->generation++;  /* New generation for next use */

        pthread_cond_broadcast(&barrier->cond);
        pthread_mutex_unlock(&barrier->mutex);

        return 1;  /* Signal that this was the last thread */
    }

    /* Wait for other threads */
    while (my_generation == barrier->generation) {
        pthread_cond_wait(&barrier->cond, &barrier->mutex);
    }

    pthread_mutex_unlock(&barrier->mutex);
    return 0;
}

/* ============================================================================
EXERCISE 1: Basic barrier usage
============================================================================
*/

Barrier global_barrier;

void *barrier_thread(void *arg) {
    int id = *(int *)arg;

    printf("Thread %d: Starting phase 1\n", id);
    usleep((rand() % 1000) * 1000);  /* Random work */
    printf("Thread %d: Finished phase 1, waiting at barrier\n", id);

    int last = barrier_wait(&global_barrier);
    if (last) {
        printf("\n--- ALL THREADS REACHED BARRIER ---\n\n");
    }

    printf("Thread %d: Starting phase 2\n", id);
    usleep((rand() % 500) * 1000);
    printf("Thread %d: Finished phase 2\n", id);

    return NULL;
}

void exercise1_basic_barrier(void) {
    printf("\n=== Exercise 1: Basic Barrier Usage ===\n");

    srand(time(NULL));

    #define NUM_THREADS 4

    barrier_init(&global_barrier, NUM_THREADS);

    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, barrier_thread, &ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    barrier_destroy(&global_barrier);
    printf("\nAll threads completed!\n");
}

/* ============================================================================
EXERCISE 2: Multiple barrier phases
============================================================================
*/

#define PHASES 3

void *multi_phase_thread(void *arg) {
    int id = *(int *)arg;

    for (int phase = 1; phase <= PHASES; phase++) {
        printf("Thread %d: Working on phase %d\n", id, phase);
        usleep((rand() % 500) * 1000);
        printf("Thread %d: Phase %d done, waiting\n", id, phase);

        int last = barrier_wait(&global_barrier);
        if (last) {
            printf("\n=== PHASE %d COMPLETE ===\n\n", phase);
        }

        /* Need another barrier to ensure all threads see the message */
        barrier_wait(&global_barrier);
    }

    return NULL;
}

void exercise2_multi_phase(void) {
    printf("\n=== Exercise 2: Multiple Barrier Phases ===\n");

    srand(time(NULL));

    barrier_init(&global_barrier, NUM_THREADS);

    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, multi_phase_thread, &ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    barrier_destroy(&global_barrier);
}

/* ============================================================================
EXERCISE 3: Using POSIX barrier
============================================================================
*/

#ifdef __linux__  /* POSIX barriers not available on macOS */

pthread_barrier_t posix_barrier;

void *posix_barrier_thread(void *arg) {
    int id = *(int *)arg;

    printf("Thread %d: Before POSIX barrier\n", id);
    usleep((rand() % 500) * 1000);

    int ret = pthread_barrier_wait(&posix_barrier);

    if (ret == PTHREAD_BARRIER_SERIAL_THREAD) {
        printf("Thread %d: I'm the serial thread!\n", id);
    }

    printf("Thread %d: After POSIX barrier\n", id);

    return NULL;
}

void exercise3_posix_barrier(void) {
    printf("\n=== Exercise 3: POSIX Barrier (Linux only) ===\n");

    pthread_barrier_init(&posix_barrier, NULL, NUM_THREADS);

    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, posix_barrier_thread, &ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_barrier_destroy(&posix_barrier);
}

#else

void exercise3_posix_barrier(void) {
    printf("\n=== Exercise 3: POSIX Barrier ===\n");
    printf("Note: POSIX barriers are not available on macOS.\n");
    printf("Use the custom barrier implementation instead.\n");
    printf("This code would work on Linux:\n\n");
    printf("  pthread_barrier_t barrier;\n");
    printf("  pthread_barrier_init(&barrier, NULL, num_threads);\n");
    printf("  // In each thread:\n");
    printf("  pthread_barrier_wait(&barrier);\n");
    printf("  pthread_barrier_destroy(&barrier);\n");
}

#endif

/* ============================================================================
EXERCISE 4: Parallel computation with barrier
============================================================================
*/

#define ARRAY_SIZE 1000
#define COMPUTE_THREADS 4
#define ITERATIONS 3

double shared_array[ARRAY_SIZE];
double temp_array[ARRAY_SIZE];
Barrier compute_barrier;

void *compute_thread(void *arg) {
    int id = *(int *)arg;

    int start = id * (ARRAY_SIZE / COMPUTE_THREADS);
    int end = (id + 1) * (ARRAY_SIZE / COMPUTE_THREADS);

    for (int iter = 0; iter < ITERATIONS; iter++) {
        /* Compute new values based on neighbors (simple averaging) */
        for (int i = start; i < end; i++) {
            if (i == 0 || i == ARRAY_SIZE - 1) {
                temp_array[i] = shared_array[i];  /* Boundary */
            } else {
                temp_array[i] = (shared_array[i-1] + shared_array[i] + shared_array[i+1]) / 3.0;
            }
        }

        /* Wait for all threads to finish computing */
        barrier_wait(&compute_barrier);

        /* Copy temp back to shared */
        for (int i = start; i < end; i++) {
            shared_array[i] = temp_array[i];
        }

        /* Wait for all threads to finish copying */
        int last = barrier_wait(&compute_barrier);

        if (last) {
            printf("Iteration %d complete, sample values: %.4f %.4f %.4f\n",
                   iter + 1,
                   shared_array[0],
                   shared_array[ARRAY_SIZE/2],
                   shared_array[ARRAY_SIZE-1]);
        }
    }

    return NULL;
}

void exercise4_parallel_compute(void) {
    printf("\n=== Exercise 4: Parallel Computation with Barrier ===\n");

    /* Initialize array with a spike in the middle */
    for (int i = 0; i < ARRAY_SIZE; i++) {
        shared_array[i] = 0.0;
    }
    shared_array[ARRAY_SIZE/2] = 100.0;

    printf("Initial: spike of 100 at position %d\n", ARRAY_SIZE/2);

    barrier_init(&compute_barrier, COMPUTE_THREADS);

    pthread_t threads[COMPUTE_THREADS];
    int ids[COMPUTE_THREADS];

    for (int i = 0; i < COMPUTE_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, compute_thread, &ids[i]);
    }

    for (int i = 0; i < COMPUTE_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    barrier_destroy(&compute_barrier);

    printf("\nThis simulates diffusion - the spike spreads out over iterations.\n");
}

/* ============================================================================
EXERCISE 5: Race start barrier
============================================================================
*/

Barrier start_barrier;
Barrier finish_barrier;

void *racer_thread(void *arg) {
    int id = *(int *)arg;

    printf("Racer %d: Ready\n", id);

    /* Wait for all racers to be ready */
    int last = barrier_wait(&start_barrier);
    if (last) {
        printf("\n*** GO! ***\n\n");
    }

    /* Race! */
    int race_time = rand() % 1000;
    usleep(race_time * 1000);
    printf("Racer %d: Finished in %d ms\n", id, race_time);

    /* Wait for all to finish */
    last = barrier_wait(&finish_barrier);
    if (last) {
        printf("\n*** ALL RACERS FINISHED! ***\n");
    }

    return NULL;
}

void exercise5_race_start(void) {
    printf("\n=== Exercise 5: Race Start Barrier ===\n");

    srand(time(NULL));

    #define RACERS 5

    barrier_init(&start_barrier, RACERS);
    barrier_init(&finish_barrier, RACERS);

    pthread_t threads[RACERS];
    int ids[RACERS];

    for (int i = 0; i < RACERS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, racer_thread, &ids[i]);
        usleep(100000);  /* Stagger thread creation to show "Ready" messages */
    }

    for (int i = 0; i < RACERS; i++) {
        pthread_join(threads[i], NULL);
    }

    barrier_destroy(&start_barrier);
    barrier_destroy(&finish_barrier);
}

/* ============================================================================
MAIN
============================================================================
*/

int main(int argc, char *argv[]) {
    printf("\n================================================\n");
    printf("  Module 04: Thread Barrier\n");
    printf("================================================\n");

    if (argc > 1) {
        int ex = atoi(argv[1]);
        switch (ex) {
            case 1: exercise1_basic_barrier(); break;
            case 2: exercise2_multi_phase(); break;
            case 3: exercise3_posix_barrier(); break;
            case 4: exercise4_parallel_compute(); break;
            case 5: exercise5_race_start(); break;
            default: printf("Unknown exercise: %d\n", ex);
        }
    } else {
        exercise1_basic_barrier();
        exercise5_race_start();
        exercise4_parallel_compute();
    }

    printf("\n================================================\n");
    printf("  Thread Barrier Complete!\n");
    printf("================================================\n\n");

    return 0;
}
