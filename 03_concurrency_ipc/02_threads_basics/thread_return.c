/*
CS-6200 Preparation - Module 02: Thread Return Values

Learn how threads can return results.

Compile: clang -Wall -Wextra -g -pthread thread_return.c -o thread_return
Run:     ./thread_return

Difficulty: [HARD]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

/* ============================================================================
CONCEPT: Getting Results from Threads
============================================================================

Thread functions return void*, and pthread_join() can retrieve it.

int pthread_join(pthread_t thread, void **retval);

If retval is not NULL, it receives the thread's return value.

Common patterns:
1. Return a cast integer
2. Return a pointer to allocated memory
3. Return a pointer to passed-in struct (result stored there)
4. Return NULL (no result needed)

DANGER: Don't return pointers to stack-local variables!
*/

/* ============================================================================
EXERCISE 1: Return integer (cast to void*)
============================================================================
*/

void *compute_square(void *arg) {
    long num = (long)arg;
    long result = num * num;
    return (void *)result;
}

void exercise1_return_int(void) {
    printf("\n=== Exercise 1: Return Integer ===\n");

    pthread_t threads[5];
    long inputs[] = {2, 3, 4, 5, 6};

    /* Create threads */
    for (int i = 0; i < 5; i++) {
        pthread_create(&threads[i], NULL, compute_square, (void *)inputs[i]);
    }

    /* Get results */
    printf("Squares:\n");
    for (int i = 0; i < 5; i++) {
        void *result;
        pthread_join(threads[i], &result);
        printf("  %ld^2 = %ld\n", inputs[i], (long)result);
    }
}

/* ============================================================================
EXERCISE 2: Return allocated struct
============================================================================
*/

typedef struct {
    int input;
    int square;
    int cube;
} ComputeResult;

void *compute_powers(void *arg) {
    int num = *(int *)arg;

    /* Allocate result struct */
    ComputeResult *result = malloc(sizeof(ComputeResult));
    if (result == NULL) {
        return NULL;
    }

    result->input = num;
    result->square = num * num;
    result->cube = num * num * num;

    return result;  /* Caller must free! */
}

void exercise2_return_struct(void) {
    printf("\n=== Exercise 2: Return Allocated Struct ===\n");

    pthread_t thread;
    int input = 5;

    pthread_create(&thread, NULL, compute_powers, &input);

    ComputeResult *result;
    pthread_join(thread, (void **)&result);

    if (result != NULL) {
        printf("Input: %d\n", result->input);
        printf("Square: %d\n", result->square);
        printf("Cube: %d\n", result->cube);
        free(result);  /* Don't forget to free! */
    }
}

/* ============================================================================
EXERCISE 3: Return via passed-in struct (no allocation)
============================================================================
*/

typedef struct {
    /* Input */
    int start;
    int end;
    /* Output */
    int sum;
    int count;
} RangeTask;

void *process_range(void *arg) {
    RangeTask *task = (RangeTask *)arg;

    task->sum = 0;
    task->count = 0;

    for (int i = task->start; i <= task->end; i++) {
        task->sum += i;
        task->count++;
    }

    /* Return the same struct pointer (optional, could return NULL) */
    return task;
}

void exercise3_return_via_input(void) {
    printf("\n=== Exercise 3: Return via Input Struct ===\n");

    /*
     * This pattern avoids allocation - pass a struct that has
     * both input and output fields.
     */

    pthread_t threads[3];
    RangeTask tasks[3] = {
        {1, 10, 0, 0},
        {11, 20, 0, 0},
        {21, 30, 0, 0}
    };

    for (int i = 0; i < 3; i++) {
        pthread_create(&threads[i], NULL, process_range, &tasks[i]);
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
        printf("Range [%d, %d]: sum=%d, count=%d\n",
               tasks[i].start, tasks[i].end, tasks[i].sum, tasks[i].count);
    }

    /* Calculate total */
    int total_sum = 0;
    for (int i = 0; i < 3; i++) {
        total_sum += tasks[i].sum;
    }
    printf("Total sum: %d\n", total_sum);
}

/* ============================================================================
EXERCISE 4: pthread_exit() with return value
============================================================================
*/

void *early_exit_thread(void *arg) {
    int *value = (int *)arg;

    printf("Thread: Starting with value %d\n", *value);

    if (*value < 0) {
        printf("Thread: Negative value, exiting early!\n");
        pthread_exit((void *)(long)-1);  /* Exit with error code */
    }

    printf("Thread: Processing...\n");

    /* pthread_exit is equivalent to returning */
    pthread_exit((void *)(long)(*value * 2));
}

void exercise4_pthread_exit(void) {
    printf("\n=== Exercise 4: pthread_exit ===\n");

    pthread_t t1, t2;
    int v1 = 10, v2 = -5;

    pthread_create(&t1, NULL, early_exit_thread, &v1);
    pthread_create(&t2, NULL, early_exit_thread, &v2);

    void *ret1, *ret2;
    pthread_join(t1, &ret1);
    pthread_join(t2, &ret2);

    printf("Thread 1 returned: %ld\n", (long)ret1);
    printf("Thread 2 returned: %ld\n", (long)ret2);
}

/* ============================================================================
EXERCISE 5: THE WRONG WAY - returning stack pointer
============================================================================
*/

void *buggy_return(void *arg) {
    (void)arg;

    int local_result = 42;

    /*
     * BUG: Returning pointer to local variable!
     * The variable goes out of scope when thread exits.
     */
    return &local_result;  /* DANGER! */
}

void exercise5_wrong_way(void) {
    printf("\n=== Exercise 5: THE WRONG WAY (Don't do this!) ===\n");

    /*
     * This demonstrates what NOT to do.
     * The returned pointer points to deallocated stack memory.
     */

    pthread_t thread;
    pthread_create(&thread, NULL, buggy_return, NULL);

    int *result;
    pthread_join(thread, (void **)&result);

    /*
     * result now points to invalid memory!
     * Accessing it is undefined behavior.
     * It might work, crash, or return garbage.
     */
    printf("WARNING: Accessing this pointer is undefined behavior!\n");
    printf("Don't return pointers to stack variables from threads.\n");
}

/* ============================================================================
EXERCISE 6: Cancellation and return values
============================================================================
*/

void *cancellable_thread(void *arg) {
    (void)arg;

    printf("Thread: Starting long computation...\n");

    for (int i = 0; i < 10; i++) {
        printf("Thread: Iteration %d\n", i);
        sleep(1);

        /* Check if we've been cancelled */
        pthread_testcancel();
    }

    printf("Thread: Completed!\n");
    return (void *)(long)0;
}

void exercise6_cancellation(void) {
    printf("\n=== Exercise 6: Thread Cancellation ===\n");

    pthread_t thread;
    pthread_create(&thread, NULL, cancellable_thread, NULL);

    sleep(3);  /* Let it run for a bit */

    printf("Main: Cancelling thread...\n");
    pthread_cancel(thread);

    void *result;
    pthread_join(thread, &result);

    if (result == PTHREAD_CANCELED) {
        printf("Main: Thread was cancelled\n");
    } else {
        printf("Main: Thread returned %ld\n", (long)result);
    }
}

/* ============================================================================
EXERCISE 7: Collecting results from multiple threads
============================================================================
*/

#define ARRAY_SIZE 1000
#define NUM_THREADS 4

typedef struct {
    int *array;
    int start;
    int end;
} SumTask;

void *partial_sum(void *arg) {
    SumTask *task = (SumTask *)arg;

    long sum = 0;
    for (int i = task->start; i < task->end; i++) {
        sum += task->array[i];
    }

    return (void *)sum;
}

void exercise7_parallel_sum(void) {
    printf("\n=== Exercise 7: Parallel Array Sum ===\n");

    /* Create and initialize array */
    int array[ARRAY_SIZE];
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = i + 1;  /* 1, 2, 3, ..., 1000 */
    }

    /* Calculate expected sum */
    long expected = (long)ARRAY_SIZE * (ARRAY_SIZE + 1) / 2;

    pthread_t threads[NUM_THREADS];
    SumTask tasks[NUM_THREADS];

    int chunk_size = ARRAY_SIZE / NUM_THREADS;

    /* Create threads */
    for (int i = 0; i < NUM_THREADS; i++) {
        tasks[i].array = array;
        tasks[i].start = i * chunk_size;
        tasks[i].end = (i == NUM_THREADS - 1) ? ARRAY_SIZE : (i + 1) * chunk_size;

        pthread_create(&threads[i], NULL, partial_sum, &tasks[i]);
    }

    /* Collect results */
    long total = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        void *result;
        pthread_join(threads[i], &result);

        long partial = (long)result;
        printf("Thread %d (range [%d, %d)): %ld\n",
               i, tasks[i].start, tasks[i].end, partial);
        total += partial;
    }

    printf("\nTotal sum: %ld\n", total);
    printf("Expected:  %ld\n", expected);
    printf("Match: %s\n", total == expected ? "YES" : "NO");
}

/* ============================================================================
MAIN
============================================================================
*/

int main(int argc, char *argv[]) {
    printf("\n================================================\n");
    printf("  Module 02: Thread Return Values\n");
    printf("================================================\n");

    if (argc > 1) {
        int ex = atoi(argv[1]);
        switch (ex) {
            case 1: exercise1_return_int(); break;
            case 2: exercise2_return_struct(); break;
            case 3: exercise3_return_via_input(); break;
            case 4: exercise4_pthread_exit(); break;
            case 5: exercise5_wrong_way(); break;
            case 6: exercise6_cancellation(); break;
            case 7: exercise7_parallel_sum(); break;
            default: printf("Unknown exercise: %d\n", ex);
        }
    } else {
        exercise1_return_int();
        exercise2_return_struct();
        exercise3_return_via_input();
        exercise4_pthread_exit();
        exercise7_parallel_sum();
    }

    printf("\n================================================\n");
    printf("  Thread Return Values Complete!\n");
    printf("================================================\n\n");

    return 0;
}
