/*
 * CS-6200 Preparation - Module 02: Thread Arguments
 *
 * Learn how to properly pass arguments to threads.
 * This is a common source of bugs in threaded programs!
 *
 * Compile: clang -Wall -Wextra -g -pthread thread_args.c -o thread_args
 * Run:     ./thread_args
 *
 * Difficulty: [HARD]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

/* ============================================================================
 * CONCEPT: Passing Arguments to Threads
 * ============================================================================
 *
 * pthread_create() takes a void* argument.
 * You can pass any data by casting to void*.
 *
 * Common approaches:
 * 1. Pass a primitive by casting (int, long, etc.)
 * 2. Pass a pointer to data
 * 3. Pass a pointer to a struct (most flexible)
 *
 * DANGER: The data must remain valid while the thread uses it!
 */

/* ============================================================================
 * EXERCISE 1: The WRONG way (common bug!)
 * ============================================================================
 */

void *buggy_thread(void *arg) {
    int *value_ptr = (int *)arg;
    usleep(1000);  /* Small delay to make bug more likely */
    printf("Thread received value: %d\n", *value_ptr);
    return NULL;
}

void exercise1_wrong_way(void) {
    printf("\n=== Exercise 1: The WRONG Way (Bug Demo) ===\n");

    /*
     * BUG: All threads receive a pointer to the same variable 'i'.
     * By the time threads read it, 'i' may have changed!
     */

    pthread_t threads[5];

    printf("Creating threads (BUGGY - all might see same value)...\n");

    for (int i = 0; i < 5; i++) {
        /* BUG: Passing &i - all threads share the same memory! */
        pthread_create(&threads[i], NULL, buggy_thread, &i);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\nExpected: 0, 1, 2, 3, 4\n");
    printf("Actual: Probably not what we wanted!\n");
}

/* ============================================================================
 * EXERCISE 2: Correct way - separate storage
 * ============================================================================
 */

void *correct_thread(void *arg) {
    int *value_ptr = (int *)arg;
    usleep(1000);
    printf("Thread received value: %d\n", *value_ptr);
    return NULL;
}

void exercise2_separate_storage(void) {
    printf("\n=== Exercise 2: Correct Way - Separate Storage ===\n");

    pthread_t threads[5];
    int values[5];  /* Separate storage for each thread */

    printf("Creating threads (CORRECT - separate storage)...\n");

    for (int i = 0; i < 5; i++) {
        values[i] = i;  /* Store value in separate location */
        pthread_create(&threads[i], NULL, correct_thread, &values[i]);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\nEach thread got its own value!\n");
}

/* ============================================================================
 * EXERCISE 3: Casting integer directly
 * ============================================================================
 */

void *cast_thread(void *arg) {
    /*
     * Cast void* back to integer.
     * Works for values that fit in a pointer (int, long).
     */
    long value = (long)arg;
    printf("Thread received value: %ld\n", value);
    return NULL;
}

void exercise3_direct_cast(void) {
    printf("\n=== Exercise 3: Direct Integer Cast ===\n");

    /*
     * Another correct approach: cast the integer to void*.
     * No pointer to worry about, value is passed directly.
     *
     * Warning: Only works for values that fit in a pointer!
     */

    pthread_t threads[5];

    printf("Creating threads (CORRECT - direct cast)...\n");

    for (int i = 0; i < 5; i++) {
        pthread_create(&threads[i], NULL, cast_thread, (void *)(long)i);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }
}

/* ============================================================================
 * EXERCISE 4: Passing a struct
 * ============================================================================
 */

typedef struct {
    int id;
    char name[32];
    int iterations;
} ThreadConfig;

void *struct_thread(void *arg) {
    ThreadConfig *config = (ThreadConfig *)arg;

    printf("Thread %d ('%s'): Starting with %d iterations\n",
           config->id, config->name, config->iterations);

    for (int i = 0; i < config->iterations; i++) {
        printf("Thread %d: Iteration %d\n", config->id, i);
        usleep(100000);
    }

    printf("Thread %d: Done!\n", config->id);
    return NULL;
}

void exercise4_struct_args(void) {
    printf("\n=== Exercise 4: Struct Arguments ===\n");

    /*
     * Best approach for complex data: pass a struct pointer.
     * The struct can contain all the parameters the thread needs.
     */

    pthread_t threads[3];
    ThreadConfig configs[3] = {
        {0, "Alpha", 2},
        {1, "Beta", 3},
        {2, "Gamma", 1}
    };

    for (int i = 0; i < 3; i++) {
        pthread_create(&threads[i], NULL, struct_thread, &configs[i]);
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }
}

/* ============================================================================
 * EXERCISE 5: Dynamically allocated arguments
 * ============================================================================
 */

typedef struct {
    int thread_id;
    int value;
} DynamicArg;

void *dynamic_thread(void *arg) {
    DynamicArg *data = (DynamicArg *)arg;

    printf("Thread %d: Working with value %d\n", data->thread_id, data->value);
    sleep(1);
    printf("Thread %d: Done\n", data->thread_id);

    /* Thread frees its own argument! */
    free(data);

    return NULL;
}

void exercise5_dynamic_args(void) {
    printf("\n=== Exercise 5: Dynamically Allocated Arguments ===\n");

    /*
     * When you don't want to maintain an array of arguments,
     * you can malloc each argument and let the thread free it.
     *
     * This is useful when:
     * - You don't know how many threads you'll create
     * - The thread needs to own its data
     * - You want to create threads from within loops without arrays
     */

    pthread_t threads[3];

    for (int i = 0; i < 3; i++) {
        /* Allocate argument for each thread */
        DynamicArg *arg = malloc(sizeof(DynamicArg));
        if (arg == NULL) {
            perror("malloc");
            exit(1);
        }

        arg->thread_id = i;
        arg->value = i * 100;

        pthread_create(&threads[i], NULL, dynamic_thread, arg);
    }

    /* Note: We don't free the args here - threads do it */

    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("All threads done and freed their arguments\n");
}

/* ============================================================================
 * EXERCISE 6: Multiple arguments via struct
 * ============================================================================
 */

typedef struct {
    int start;
    int end;
    int *result;  /* Pointer to shared result location */
} RangeSum;

void *sum_range(void *arg) {
    RangeSum *range = (RangeSum *)arg;

    int sum = 0;
    for (int i = range->start; i < range->end; i++) {
        sum += i;
    }

    printf("Sum of [%d, %d): %d\n", range->start, range->end, sum);

    /* Store result (would need synchronization in real code!) */
    *(range->result) = sum;

    return NULL;
}

void exercise6_complex_args(void) {
    printf("\n=== Exercise 6: Complex Arguments ===\n");

    /*
     * TODO: Create threads to sum different ranges of numbers.
     * Each thread sums a portion and stores in a result array.
     */

    pthread_t threads[4];
    RangeSum ranges[4];
    int results[4];

    int chunk_size = 25;

    for (int i = 0; i < 4; i++) {
        ranges[i].start = i * chunk_size;
        ranges[i].end = (i + 1) * chunk_size;
        ranges[i].result = &results[i];

        pthread_create(&threads[i], NULL, sum_range, &ranges[i]);
    }

    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    /* Combine results */
    int total = 0;
    for (int i = 0; i < 4; i++) {
        total += results[i];
    }

    printf("\nTotal sum of [0, 100): %d\n", total);
    printf("Expected: %d\n", (99 * 100) / 2);
}

/* ============================================================================
 * EXERCISE 7: String arguments
 * ============================================================================
 */

void *string_thread(void *arg) {
    char *message = (char *)arg;
    printf("Thread received message: '%s'\n", message);
    return NULL;
}

void exercise7_string_args(void) {
    printf("\n=== Exercise 7: String Arguments ===\n");

    pthread_t t1, t2, t3;

    /* String literals are safe - they exist for program lifetime */
    pthread_create(&t1, NULL, string_thread, "Hello from literal");

    /* Static strings are safe */
    static char static_msg[] = "Hello from static";
    pthread_create(&t2, NULL, string_thread, static_msg);

    /* Stack strings - must ensure they exist while thread runs! */
    char stack_msg[] = "Hello from stack";
    pthread_create(&t3, NULL, string_thread, stack_msg);

    /* IMPORTANT: Join before stack_msg goes out of scope! */
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
}

/* ============================================================================
 * MAIN
 * ============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n================================================\n");
    printf("  Module 02: Thread Arguments\n");
    printf("================================================\n");

    if (argc > 1) {
        int ex = atoi(argv[1]);
        switch (ex) {
            case 1: exercise1_wrong_way(); break;
            case 2: exercise2_separate_storage(); break;
            case 3: exercise3_direct_cast(); break;
            case 4: exercise4_struct_args(); break;
            case 5: exercise5_dynamic_args(); break;
            case 6: exercise6_complex_args(); break;
            case 7: exercise7_string_args(); break;
            default: printf("Unknown exercise: %d\n", ex);
        }
    } else {
        exercise1_wrong_way();
        exercise2_separate_storage();
        exercise3_direct_cast();
        exercise4_struct_args();
        exercise6_complex_args();
    }

    printf("\n================================================\n");
    printf("  Thread Arguments Complete!\n");
    printf("================================================\n\n");

    return 0;
}
