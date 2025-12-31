/*
CS-6200 Preparation - Module 02: Thread Creation

Learn POSIX threads (pthreads) basics.
This is critical for GIOS Project 1 Part 2!

Compile: clang -Wall -Wextra -g -pthread thread_create.c -o thread_create
Run:     ./thread_create

Difficulty: [HARD]
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/* ============================================================================
CONCEPT: Threads vs Processes
============================================================================

Processes:
- Separate address space (memory not shared)
- Heavy to create (fork copies page tables)
- Communication via IPC (pipes, sockets, shared memory)

Threads:
- Shared address space (same memory)
- Lighter to create
- Communication via shared variables (but needs synchronization!)

pthreads API:
- pthread_create(): create a new thread
- pthread_join(): wait for thread to finish
- pthread_exit(): exit from current thread
- pthread_self(): get current thread ID
*/

/* ============================================================================
EXERCISE 1: Basic thread creation
============================================================================
*/

/*
Thread function signature: void *func(void *arg)
- Takes void* argument (can pass anything via casting)
- Returns void* (can return anything via casting)
*/
void *thread_function(void *arg) {
    (void)arg;  /* Unused in this example */

    printf("Hello from thread! Thread ID: %lu\n", (unsigned long)pthread_self());
    printf("Thread: Doing some work...\n");
    sleep(1);
    printf("Thread: Done!\n");

    return NULL;
}

void exercise1_basic_thread(void) {
    printf("\n=== Exercise 1: Basic Thread Creation ===\n");

    pthread_t thread_id;

    /*
     * TODO: Create a thread using pthread_create
     *
     * int pthread_create(pthread_t *thread,
     *                    const pthread_attr_t *attr,
     *                    void *(*start_routine)(void *),
     *                    void *arg);
     *
     * - thread: pointer to store thread ID
     * - attr: thread attributes (NULL for defaults)
     * - start_routine: function to run
     * - arg: argument to pass to function
     */

    printf("Main: Creating thread...\n");

    int ret = pthread_create(&thread_id, NULL, thread_function, NULL);
    if (ret != 0) {
        fprintf(stderr, "pthread_create failed: %d\n", ret);
        return;
    }

    printf("Main: Thread created with ID: %lu\n", (unsigned long)thread_id);
    printf("Main: Doing other work while thread runs...\n");

    /*
     * TODO: Wait for thread to complete using pthread_join
     *
     * int pthread_join(pthread_t thread, void **retval);
     *
     * - thread: thread ID to wait for
     * - retval: pointer to store thread's return value (NULL if not needed)
     */

    ret = pthread_join(thread_id, NULL);
    if (ret != 0) {
        fprintf(stderr, "pthread_join failed: %d\n", ret);
        return;
    }

    printf("Main: Thread completed!\n");
}

/* ============================================================================
EXERCISE 2: Multiple threads
============================================================================
*/

#define NUM_THREADS 5

void *numbered_thread(void *arg) {
    int thread_num = *(int *)arg;

    printf("Thread %d: Starting (pthread_self = %lu)\n",
           thread_num, (unsigned long)pthread_self());

    /* Simulate work */
    usleep(100000 * (thread_num + 1));

    printf("Thread %d: Finishing\n", thread_num);

    return NULL;
}

void exercise2_multiple_threads(void) {
    printf("\n=== Exercise 2: Multiple Threads ===\n");

    pthread_t threads[NUM_THREADS];
    int thread_nums[NUM_THREADS];

    /*
     * TODO: Create NUM_THREADS threads.
     *
     * IMPORTANT: Notice we use a separate thread_nums array.
     * If we passed &i directly, all threads might see the same value!
     * (because i changes in the loop before threads read it)
     */

    printf("Main: Creating %d threads...\n", NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_nums[i] = i;

        int ret = pthread_create(&threads[i], NULL, numbered_thread, &thread_nums[i]);
        if (ret != 0) {
            fprintf(stderr, "Failed to create thread %d\n", i);
            exit(1);
        }
    }

    printf("Main: All threads created, waiting for completion...\n");

    /* TODO: Join all threads */
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Main: All threads completed!\n");
}

/* ============================================================================
EXERCISE 3: Demonstrating shared memory
============================================================================
*/

int shared_counter = 0;  /* Global variable shared by all threads */

void *increment_thread(void *arg) {
    int increments = *(int *)arg;

    for (int i = 0; i < increments; i++) {
        shared_counter++;  /* WARNING: This is a race condition! */
    }

    return NULL;
}

void exercise3_shared_memory(void) {
    printf("\n=== Exercise 3: Shared Memory (Race Condition Demo) ===\n");

    /*
     * This demonstrates that threads share memory.
     * WARNING: This has a race condition! We'll fix it in Module 3.
     */

    shared_counter = 0;
    int increments = 100000;

    pthread_t t1, t2;

    printf("Expected final count: %d\n", increments * 2);
    printf("Creating two threads, each incrementing %d times...\n", increments);

    pthread_create(&t1, NULL, increment_thread, &increments);
    pthread_create(&t2, NULL, increment_thread, &increments);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Actual final count: %d\n", shared_counter);
    printf("Difference: %d (due to race condition!)\n",
           increments * 2 - shared_counter);

    printf("\nNote: We'll fix this with mutexes in Module 3!\n");
}

/* ============================================================================
EXERCISE 4: Thread vs main execution
============================================================================
*/

void *long_running_thread(void *arg) {
    int seconds = *(int *)arg;

    printf("Thread: Will run for %d seconds\n", seconds);

    for (int i = 0; i < seconds; i++) {
        printf("Thread: Second %d\n", i + 1);
        sleep(1);
    }

    printf("Thread: Done!\n");
    return NULL;
}

void exercise4_thread_lifetime(void) {
    printf("\n=== Exercise 4: Thread Lifetime ===\n");

    /*
     * What happens if main() exits before a thread finishes?
     * The entire process terminates!
     *
     * Options:
     * 1. pthread_join() - wait for thread (most common)
     * 2. pthread_detach() - let thread run independently
     * 3. pthread_exit() in main - exit main but keep threads running
     */

    pthread_t thread;
    int seconds = 3;

    pthread_create(&thread, NULL, long_running_thread, &seconds);

    printf("Main: Thread created\n");
    printf("Main: If we don't join, thread might not finish!\n");

    /* Try commenting out this join to see what happens */
    pthread_join(thread, NULL);

    printf("Main: Exiting\n");
}

/* ============================================================================
EXERCISE 5: Detached threads
============================================================================
*/

void *detached_thread_func(void *arg) {
    (void)arg;

    printf("Detached thread: Running independently!\n");
    sleep(1);
    printf("Detached thread: Done (no one waiting for me)\n");

    return NULL;
}

void exercise5_detached(void) {
    printf("\n=== Exercise 5: Detached Threads ===\n");

    /*
     * A detached thread runs independently.
     * You cannot join a detached thread.
     * Resources are automatically freed when it exits.
     */

    pthread_t thread;
    pthread_attr_t attr;

    /* Initialize attributes */
    pthread_attr_init(&attr);

    /* Set detach state */
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(&thread, &attr, detached_thread_func, NULL);

    pthread_attr_destroy(&attr);

    printf("Main: Created detached thread\n");
    printf("Main: Cannot join detached thread, just waiting...\n");

    /* pthread_join(thread, NULL);  // This would fail for detached thread */

    sleep(2);  /* Wait long enough for detached thread to finish */

    printf("Main: Done\n");
}

/* ============================================================================
EXERCISE 6: Getting thread self
============================================================================
*/

void *self_aware_thread(void *arg) {
    int num = *(int *)arg;

    pthread_t self = pthread_self();

    printf("Thread %d: My pthread_t is %lu\n", num, (unsigned long)self);

    return NULL;
}

void exercise6_thread_self(void) {
    printf("\n=== Exercise 6: Thread Self Identification ===\n");

    pthread_t threads[3];
    int nums[] = {0, 1, 2};

    printf("Main thread: %lu\n", (unsigned long)pthread_self());

    for (int i = 0; i < 3; i++) {
        pthread_create(&threads[i], NULL, self_aware_thread, &nums[i]);
        printf("Main: Created thread with ID %lu\n", (unsigned long)threads[i]);
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }
}

/* ============================================================================
MAIN
============================================================================
*/

int main(int argc, char *argv[]) {
    printf("\n================================================\n");
    printf("  Module 02: Thread Creation\n");
    printf("================================================\n");

    if (argc > 1) {
        int ex = atoi(argv[1]);
        switch (ex) {
            case 1: exercise1_basic_thread(); break;
            case 2: exercise2_multiple_threads(); break;
            case 3: exercise3_shared_memory(); break;
            case 4: exercise4_thread_lifetime(); break;
            case 5: exercise5_detached(); break;
            case 6: exercise6_thread_self(); break;
            default: printf("Unknown exercise: %d\n", ex);
        }
    } else {
        exercise1_basic_thread();
        exercise2_multiple_threads();
        exercise3_shared_memory();
        exercise6_thread_self();
    }

    printf("\n================================================\n");
    printf("  Thread Creation Complete!\n");
    printf("================================================\n\n");

    return 0;
}
