/*
 * CS-6200 Preparation - Module 03: Deadlocks
 *
 * Learn about deadlocks and how to avoid them.
 * Critical for writing correct concurrent code!
 *
 * Compile: clang -Wall -Wextra -g -pthread deadlock.c -o deadlock
 * Run:     ./deadlock
 *
 * Difficulty: [HARD]
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/* ============================================================================
 * CONCEPT: Deadlock
 * ============================================================================
 *
 * A DEADLOCK occurs when two or more threads wait for each other forever.
 *
 * Four conditions must hold simultaneously (Coffman conditions):
 * 1. Mutual exclusion - resources can't be shared
 * 2. Hold and wait - holding one resource while waiting for another
 * 3. No preemption - can't forcibly take a resource
 * 4. Circular wait - A waits for B waits for C waits for A
 *
 * Classic example:
 *   Thread A: lock(mutex1), lock(mutex2)
 *   Thread B: lock(mutex2), lock(mutex1)
 *
 * If A gets mutex1 and B gets mutex2, both wait forever!
 */

/* ============================================================================
 * EXERCISE 1: Classic deadlock demonstration
 * ============================================================================
 */

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

void *thread_a(void *arg) {
    (void)arg;

    printf("Thread A: Trying to lock mutex1...\n");
    pthread_mutex_lock(&mutex1);
    printf("Thread A: Got mutex1!\n");

    sleep(1);  /* Increase chance of deadlock */

    printf("Thread A: Trying to lock mutex2...\n");
    pthread_mutex_lock(&mutex2);
    printf("Thread A: Got mutex2!\n");

    /* Work with both resources */

    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex1);

    printf("Thread A: Done!\n");
    return NULL;
}

void *thread_b(void *arg) {
    (void)arg;

    printf("Thread B: Trying to lock mutex2...\n");
    pthread_mutex_lock(&mutex2);
    printf("Thread B: Got mutex2!\n");

    sleep(1);  /* Increase chance of deadlock */

    printf("Thread B: Trying to lock mutex1...\n");
    pthread_mutex_lock(&mutex1);
    printf("Thread B: Got mutex1!\n");

    /* Work with both resources */

    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);

    printf("Thread B: Done!\n");
    return NULL;
}

void exercise1_classic_deadlock(void) {
    printf("\n=== Exercise 1: Classic Deadlock Demo ===\n");
    printf("WARNING: This will likely deadlock! Press Ctrl+C to stop.\n\n");

    pthread_t ta, tb;

    pthread_create(&ta, NULL, thread_a, NULL);
    pthread_create(&tb, NULL, thread_b, NULL);

    pthread_join(ta, NULL);
    pthread_join(tb, NULL);

    printf("If you see this, we got lucky and didn't deadlock!\n");
}

/* ============================================================================
 * EXERCISE 2: Fix deadlock with consistent lock ordering
 * ============================================================================
 */

void *fixed_thread_a(void *arg) {
    (void)arg;

    /*
     * FIX: Always acquire locks in the same order!
     * Both threads: mutex1 first, then mutex2
     */

    printf("Thread A: Locking mutex1, then mutex2 (fixed order)\n");
    pthread_mutex_lock(&mutex1);
    pthread_mutex_lock(&mutex2);
    printf("Thread A: Got both locks!\n");

    sleep(1);

    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex1);

    printf("Thread A: Done!\n");
    return NULL;
}

void *fixed_thread_b(void *arg) {
    (void)arg;

    /* Same order as thread A: mutex1 first, then mutex2 */
    printf("Thread B: Locking mutex1, then mutex2 (fixed order)\n");
    pthread_mutex_lock(&mutex1);
    pthread_mutex_lock(&mutex2);
    printf("Thread B: Got both locks!\n");

    sleep(1);

    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex1);

    printf("Thread B: Done!\n");
    return NULL;
}

void exercise2_fixed_ordering(void) {
    printf("\n=== Exercise 2: Fixed with Consistent Ordering ===\n");

    pthread_t ta, tb;

    pthread_create(&ta, NULL, fixed_thread_a, NULL);
    pthread_create(&tb, NULL, fixed_thread_b, NULL);

    pthread_join(ta, NULL);
    pthread_join(tb, NULL);

    printf("No deadlock - consistent ordering works!\n");
}

/* ============================================================================
 * EXERCISE 3: Fix deadlock with trylock
 * ============================================================================
 */

void *trylock_thread_a(void *arg) {
    (void)arg;

    int attempts = 0;

    while (1) {
        attempts++;

        pthread_mutex_lock(&mutex1);

        if (pthread_mutex_trylock(&mutex2) == 0) {
            /* Got both locks! */
            printf("Thread A: Got both locks on attempt %d\n", attempts);
            break;
        }

        /* Couldn't get mutex2, release mutex1 and retry */
        pthread_mutex_unlock(&mutex1);
        usleep(rand() % 1000);  /* Random backoff */
    }

    /* Work with both resources */
    sleep(1);

    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex1);

    printf("Thread A: Done!\n");
    return NULL;
}

void *trylock_thread_b(void *arg) {
    (void)arg;

    int attempts = 0;

    while (1) {
        attempts++;

        pthread_mutex_lock(&mutex2);

        if (pthread_mutex_trylock(&mutex1) == 0) {
            printf("Thread B: Got both locks on attempt %d\n", attempts);
            break;
        }

        pthread_mutex_unlock(&mutex2);
        usleep(rand() % 1000);
    }

    sleep(1);

    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);

    printf("Thread B: Done!\n");
    return NULL;
}

void exercise3_trylock_solution(void) {
    printf("\n=== Exercise 3: Fixed with trylock ===\n");

    srand(time(NULL));

    pthread_t ta, tb;

    pthread_create(&ta, NULL, trylock_thread_a, NULL);
    pthread_create(&tb, NULL, trylock_thread_b, NULL);

    pthread_join(ta, NULL);
    pthread_join(tb, NULL);

    printf("No deadlock - trylock with backoff works!\n");
}

/* ============================================================================
 * EXERCISE 4: Deadlock with more than two resources
 * ============================================================================
 */

#define NUM_RESOURCES 5
pthread_mutex_t resources[NUM_RESOURCES];

void *philosopher(void *arg) {
    int id = *(int *)arg;

    int left = id;
    int right = (id + 1) % NUM_RESOURCES;

    printf("Philosopher %d: Thinking...\n", id);
    usleep(rand() % 100000);

    /*
     * POTENTIAL DEADLOCK:
     * All philosophers pick up left fork, then wait for right.
     *
     * FIX: Use resource ordering (lower numbered first)
     */
    int first = (left < right) ? left : right;
    int second = (left < right) ? right : left;

    printf("Philosopher %d: Trying to pick up fork %d\n", id, first);
    pthread_mutex_lock(&resources[first]);

    printf("Philosopher %d: Trying to pick up fork %d\n", id, second);
    pthread_mutex_lock(&resources[second]);

    printf("Philosopher %d: Eating!\n", id);
    usleep(rand() % 100000);

    pthread_mutex_unlock(&resources[second]);
    pthread_mutex_unlock(&resources[first]);

    printf("Philosopher %d: Done eating\n", id);

    return NULL;
}

void exercise4_dining_philosophers(void) {
    printf("\n=== Exercise 4: Dining Philosophers (Fixed) ===\n");

    /* Initialize all resource mutexes */
    for (int i = 0; i < NUM_RESOURCES; i++) {
        pthread_mutex_init(&resources[i], NULL);
    }

    pthread_t threads[NUM_RESOURCES];
    int ids[NUM_RESOURCES];

    for (int i = 0; i < NUM_RESOURCES; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, philosopher, &ids[i]);
    }

    for (int i = 0; i < NUM_RESOURCES; i++) {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < NUM_RESOURCES; i++) {
        pthread_mutex_destroy(&resources[i]);
    }

    printf("All philosophers finished - no deadlock!\n");
}

/* ============================================================================
 * EXERCISE 5: Detecting potential deadlocks with timed locks
 * ============================================================================
 */

void exercise5_timed_locks(void) {
    printf("\n=== Exercise 5: Timed Locks (Deadlock Detection) ===\n");

    /*
     * pthread_mutex_timedlock() waits up to a timeout.
     * Can be used to detect potential deadlocks.
     *
     * Note: pthread_mutex_timedlock is not available on macOS.
     * We'll use pthread_mutex_trylock in a loop as a workaround.
     */

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    /* Lock the mutex */
    pthread_mutex_lock(&mutex);
    printf("Mutex locked by main thread\n");

    printf("Trying to lock again (will demonstrate deadlock detection)...\n");

    /*
     * macOS-compatible workaround: use trylock with sleep
     * On Linux, you could use pthread_mutex_timedlock() instead.
     */
    int result = -1;
    for (int attempts = 0; attempts < 4; attempts++) {
        result = pthread_mutex_trylock(&mutex);
        if (result == 0) {
            break;  /* Got the lock */
        }
        printf("  Attempt %d: lock busy, waiting...\n", attempts + 1);
        usleep(500000);  /* Wait 0.5 second */
    }

    if (result == 0) {
        printf("Got the lock (shouldn't happen for non-recursive mutex)\n");
        pthread_mutex_unlock(&mutex);
    } else {
        printf("Gave up after 2 seconds! Potential deadlock detected.\n");
    }

    pthread_mutex_unlock(&mutex);
    pthread_mutex_destroy(&mutex);
}

/* ============================================================================
 * EXERCISE 6: Self-deadlock with non-recursive mutex
 * ============================================================================
 */

pthread_mutex_t recursive_test_mutex = PTHREAD_MUTEX_INITIALIZER;

void nested_function(void) {
    printf("Nested function: Trying to lock...\n");
    /* This would deadlock with regular mutex! */
    /* pthread_mutex_lock(&recursive_test_mutex); */
    printf("Nested function: Skipping lock (would deadlock!)\n");
}

void outer_function(void) {
    printf("Outer function: Locking...\n");
    pthread_mutex_lock(&recursive_test_mutex);
    printf("Outer function: Got lock!\n");

    nested_function();

    pthread_mutex_unlock(&recursive_test_mutex);
    printf("Outer function: Unlocked\n");
}

void exercise6_self_deadlock(void) {
    printf("\n=== Exercise 6: Self-Deadlock Warning ===\n");

    /*
     * A non-recursive (default) mutex will deadlock if the same
     * thread tries to lock it twice!
     *
     * Solutions:
     * 1. Don't call lock() twice from same thread
     * 2. Use a recursive mutex (PTHREAD_MUTEX_RECURSIVE)
     * 3. Restructure code to avoid nested locking
     */

    printf("Regular mutex: Same thread locking twice = deadlock!\n\n");

    outer_function();

    /* Demonstrate recursive mutex */
    printf("\nUsing RECURSIVE mutex:\n");

    pthread_mutex_t recursive_mutex;
    pthread_mutexattr_t attr;

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&recursive_mutex, &attr);
    pthread_mutexattr_destroy(&attr);

    printf("Locking recursive mutex first time...\n");
    pthread_mutex_lock(&recursive_mutex);

    printf("Locking recursive mutex second time...\n");
    pthread_mutex_lock(&recursive_mutex);

    printf("Got lock twice! (recursive mutex allows this)\n");

    pthread_mutex_unlock(&recursive_mutex);
    pthread_mutex_unlock(&recursive_mutex);

    printf("Unlocked twice, done!\n");

    pthread_mutex_destroy(&recursive_mutex);
}

/* ============================================================================
 * MAIN
 * ============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n================================================\n");
    printf("  Module 03: Deadlocks\n");
    printf("================================================\n");

    if (argc > 1) {
        int ex = atoi(argv[1]);
        switch (ex) {
            case 1:
                printf("\nExercise 1 will DEADLOCK! Ctrl+C to stop.\n");
                printf("Run with arg 2-6 for working examples.\n\n");
                exercise1_classic_deadlock();
                break;
            case 2: exercise2_fixed_ordering(); break;
            case 3: exercise3_trylock_solution(); break;
            case 4: exercise4_dining_philosophers(); break;
            case 5: exercise5_timed_locks(); break;
            case 6: exercise6_self_deadlock(); break;
            default: printf("Unknown exercise: %d\n", ex);
        }
    } else {
        printf("\nSkipping exercise 1 (deadlock) by default.\n");
        printf("Run './deadlock 1' to see deadlock demo.\n");

        exercise2_fixed_ordering();
        exercise3_trylock_solution();
        exercise4_dining_philosophers();
        exercise6_self_deadlock();
    }

    printf("\n================================================\n");
    printf("  Deadlock Module Complete!\n");
    printf("  Key lesson: Always acquire locks in consistent order!\n");
    printf("================================================\n\n");

    return 0;
}
