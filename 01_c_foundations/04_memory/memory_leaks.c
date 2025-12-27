/*
 * CS-6200 Preparation - Module 04: Memory Leaks
 *
 * Memory leaks are one of the most common bugs in C programs.
 * GIOS projects are checked for memory leaks using Valgrind.
 *
 * Compile: clang -Wall -Wextra -g memory_leaks.c -o memory_leaks
 * Run:     ./memory_leaks
 *
 * Check for leaks (REQUIRED - use Docker on M4 Mac):
 *   docker run --rm -v $(pwd):/code -w /code gios-prep \
 *       valgrind --leak-check=full --show-leak-kinds=all ./memory_leaks
 *
 * Difficulty: [MEDIUM] to [HARD]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * LEAK 1: Simple Leak - No free() [EASY]
 * ============================================================================
 *
 * TODO: Fix this function to free allocated memory.
 */
void leak1_simple(void) {
    printf("\n=== Leak 1: Simple Leak ===\n");

    int *data = malloc(100 * sizeof(int));
    if (data == NULL) return;

    for (int i = 0; i < 100; i++) {
        data[i] = i;
    }

    printf("Sum: %d\n", data[0] + data[99]);

    /* BUG: Memory is never freed! */
    /* TODO: Add free(data) here */
}

/* ============================================================================
 * LEAK 2: Lost Pointer [MEDIUM]
 * ============================================================================
 *
 * TODO: Fix this function - pointer is overwritten before free.
 */
void leak2_lost_pointer(void) {
    printf("\n=== Leak 2: Lost Pointer ===\n");

    char *str = malloc(50);
    if (str == NULL) return;
    strcpy(str, "Hello, World!");
    printf("First: %s\n", str);

    /* BUG: Overwriting pointer without freeing first allocation! */
    str = malloc(50);  /* First allocation is now leaked! */
    if (str == NULL) return;
    strcpy(str, "Goodbye!");
    printf("Second: %s\n", str);

    free(str);  /* Only frees second allocation */

    /* TODO: Fix by freeing first allocation before reassigning */
}

/* ============================================================================
 * LEAK 3: Early Return [MEDIUM]
 * ============================================================================
 *
 * TODO: Fix this function - early return causes leak.
 */
int leak3_early_return(int should_fail) {
    printf("\n=== Leak 3: Early Return ===\n");

    int *buffer = malloc(1000 * sizeof(int));
    if (buffer == NULL) return -1;

    if (should_fail) {
        printf("Simulating failure...\n");
        /* BUG: Early return without freeing buffer! */
        return -1;  /* TODO: Free buffer before returning */
    }

    /* Normal processing */
    buffer[0] = 42;
    printf("Processing: buffer[0] = %d\n", buffer[0]);

    free(buffer);
    return 0;
}

/* ============================================================================
 * LEAK 4: Struct with Nested Allocations [MEDIUM]
 * ============================================================================
 *
 * TODO: Fix the free function to properly free all nested allocations.
 */
typedef struct {
    char *name;
    int *scores;
    int num_scores;
} Student;

Student *student_create(const char *name, int num_scores) {
    Student *s = malloc(sizeof(Student));
    if (s == NULL) return NULL;

    s->name = strdup(name);
    if (s->name == NULL) {
        free(s);
        return NULL;
    }

    s->scores = calloc(num_scores, sizeof(int));
    if (s->scores == NULL) {
        free(s->name);
        free(s);
        return NULL;
    }

    s->num_scores = num_scores;
    return s;
}

void student_free_buggy(Student *s) {
    /* BUG: Only frees the struct, not the nested allocations! */
    free(s);

    /* TODO: This should also free s->name and s->scores */
}

void leak4_nested_allocations(void) {
    printf("\n=== Leak 4: Nested Allocations ===\n");

    Student *s = student_create("Alice", 5);
    if (s == NULL) return;

    s->scores[0] = 95;
    s->scores[1] = 87;
    printf("Student: %s, Score 1: %d\n", s->name, s->scores[0]);

    student_free_buggy(s);  /* This leaks name and scores! */

    printf("TODO: Fix student_free_buggy to free all allocations\n");
}

/* ============================================================================
 * LEAK 5: Array of Strings [HARD]
 * ============================================================================
 *
 * TODO: Implement proper cleanup for dynamically allocated string array.
 */
char **create_string_array(int count) {
    char **arr = malloc(count * sizeof(char *));
    if (arr == NULL) return NULL;

    for (int i = 0; i < count; i++) {
        arr[i] = malloc(32);
        if (arr[i] == NULL) {
            /* BUG: If allocation fails partway, we leak previous allocations! */
            /* TODO: Free all previously allocated strings before returning NULL */
            return NULL;
        }
        snprintf(arr[i], 32, "String %d", i);
    }

    return arr;
}

void free_string_array_buggy(char **arr, int count) {
    /* BUG: Only frees the array pointer, not the strings! */
    free(arr);

    /* TODO: Also free each string in the array */
    (void)count;  /* Silence unused parameter warning */
}

void leak5_string_array(void) {
    printf("\n=== Leak 5: Array of Strings ===\n");

    char **strings = create_string_array(5);
    if (strings == NULL) return;

    for (int i = 0; i < 5; i++) {
        printf("%s\n", strings[i]);
    }

    free_string_array_buggy(strings, 5);  /* Leaks all strings! */

    printf("TODO: Fix free_string_array_buggy\n");
}

/* ============================================================================
 * LEAK 6: Realloc Failure [HARD]
 * ============================================================================
 *
 * TODO: Fix this function - realloc failure causes leak.
 */
int *resize_array_buggy(int *arr, int new_size) {
    /* BUG: If realloc fails, original array is leaked! */
    arr = realloc(arr, new_size * sizeof(int));
    return arr;

    /* TODO: Use a temp pointer to preserve original on failure:
     *   int *temp = realloc(arr, new_size * sizeof(int));
     *   if (temp == NULL) return NULL;  // Original arr still valid!
     *   return temp;
     */
}

void leak6_realloc_failure(void) {
    printf("\n=== Leak 6: Realloc Failure ===\n");

    int *arr = malloc(10 * sizeof(int));
    if (arr == NULL) return;

    for (int i = 0; i < 10; i++) {
        arr[i] = i;
    }

    /* This might leak if realloc fails! */
    int *new_arr = resize_array_buggy(arr, 20);
    if (new_arr == NULL) {
        printf("Resize failed - original might be leaked!\n");
        /* arr is now invalid if realloc failed internally but returned NULL */
    } else {
        arr = new_arr;
        printf("Resize succeeded\n");
        free(arr);
    }

    printf("TODO: Fix resize_array_buggy to preserve original on failure\n");
}

/* ============================================================================
 * EXERCISE: Find and Fix All Leaks [HARD]
 * ============================================================================
 *
 * TODO: The functions below have memory leaks. Find and fix them.
 * Use Valgrind to verify your fixes:
 *
 *   docker run --rm -v $(pwd):/code -w /code gios-prep \
 *       valgrind --leak-check=full ./memory_leaks
 *
 * A clean Valgrind output shows:
 *   "All heap blocks were freed -- no leaks are possible"
 */

/*
 * TODO: Fix this function - multiple leaks possible.
 */
int *process_data_buggy(int n) {
    if (n <= 0) return NULL;

    int *input = malloc(n * sizeof(int));
    int *output = malloc(n * sizeof(int));

    if (input == NULL || output == NULL) {
        /* BUG: If one allocation fails, the other might leak! */
        return NULL;
    }

    for (int i = 0; i < n; i++) {
        input[i] = i;
        output[i] = input[i] * 2;
    }

    /* BUG: input is never freed! */
    return output;
}

/*
 * TODO: Fix this function - complex control flow causes leaks.
 */
int complex_function_buggy(int option) {
    char *buffer1 = malloc(100);
    char *buffer2 = malloc(100);
    int *numbers = malloc(50 * sizeof(int));

    if (buffer1 == NULL || buffer2 == NULL || numbers == NULL) {
        /* BUG: Partial allocations may leak! */
        return -1;
    }

    switch (option) {
        case 1:
            strcpy(buffer1, "Option 1");
            /* BUG: Early return - everything leaks! */
            return 1;

        case 2:
            strcpy(buffer2, "Option 2");
            numbers[0] = 42;
            break;

        default:
            /* BUG: Early return - everything leaks! */
            return -1;
    }

    /* Only reached for option 2 */
    printf("buffer1: %s, buffer2: %s, numbers[0]: %d\n",
           buffer1, buffer2, numbers[0]);

    free(buffer1);
    free(buffer2);
    free(numbers);

    return 0;
}

/* ============================================================================
 * VALGRIND CHEAT SHEET
 * ============================================================================
 *
 * Basic usage:
 *   valgrind --leak-check=full ./program
 *
 * Show all leak types:
 *   valgrind --leak-check=full --show-leak-kinds=all ./program
 *
 * Track origins of uninitialized values:
 *   valgrind --track-origins=yes ./program
 *
 * Interpreting output:
 *   - "definitely lost": Memory was allocated but never freed
 *   - "indirectly lost": Memory lost because pointer to it was lost
 *   - "still reachable": Memory still accessible at exit (not always a bug)
 *   - "possibly lost": Memory that might be lost
 *
 * Good output:
 *   "All heap blocks were freed -- no leaks are possible"
 *   "ERROR SUMMARY: 0 errors from 0 contexts"
 */

/* ============================================================================
 * MAIN FUNCTION
 * ============================================================================
 */
int main(void) {
    printf("\n");
    printf("================================================\n");
    printf("  Module 04: Memory Leaks\n");
    printf("================================================\n");
    printf("\nThis file contains intentional memory leaks.\n");
    printf("Run with Valgrind to see them:\n\n");
    printf("  docker run --rm -v $(pwd):/code -w /code gios-prep \\\n");
    printf("      valgrind --leak-check=full ./memory_leaks\n\n");

    leak1_simple();
    leak2_lost_pointer();
    leak3_early_return(1);  /* Trigger the leak */
    leak4_nested_allocations();
    leak5_string_array();
    leak6_realloc_failure();

    /* Exercise functions */
    printf("\n=== Exercise: Find and Fix All Leaks ===\n");
    int *result = process_data_buggy(10);
    if (result != NULL) {
        printf("process_data_buggy: result[5] = %d\n", result[5]);
        free(result);
    }

    complex_function_buggy(1);  /* Leaks! */
    complex_function_buggy(2);  /* Might work */

    printf("\n================================================\n");
    printf("  Memory Leaks Module Complete!\n");
    printf("  Fix all the leaks, then verify with Valgrind.\n");
    printf("  Goal: 'no leaks are possible'\n");
    printf("================================================\n\n");

    return 0;
}
