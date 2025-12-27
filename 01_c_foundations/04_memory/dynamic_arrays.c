/*
 * CS-6200 Preparation - Module 04: Dynamic Arrays
 *
 * Dynamic memory allocation is fundamental to systems programming.
 * This module covers malloc, calloc, realloc, and free.
 *
 * Compile: clang -Wall -Wextra -g dynamic_arrays.c -o dynamic_arrays
 * Run:     ./dynamic_arrays
 *
 * For memory checking (use Docker on M4 Mac):
 *   docker run --rm -v $(pwd):/code -w /code gios-prep \
 *       valgrind --leak-check=full ./dynamic_arrays
 *
 * Topics covered:
 *   - Stack vs Heap memory
 *   - malloc, calloc, realloc, free
 *   - Dynamic arrays (growable arrays)
 *   - Memory management best practices
 *
 * Difficulty: [MEDIUM]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * CONCEPT: Stack vs Heap Memory
 * ============================================================================
 *
 * STACK:
 *   - Automatic allocation/deallocation
 *   - Fixed size at compile time
 *   - Fast allocation
 *   - Limited size (usually a few MB)
 *   - Variables die when function returns
 *
 * HEAP:
 *   - Manual allocation (malloc/calloc)
 *   - Manual deallocation (free)
 *   - Size determined at runtime
 *   - Slower allocation
 *   - Large size available
 *   - Lives until explicitly freed
 */

void demonstrate_stack_heap(void) {
    printf("\n=== Stack vs Heap Memory ===\n");

    /* Stack allocation - size must be known at compile time */
    int stack_array[5] = {1, 2, 3, 4, 5};
    printf("Stack array: [%d, %d, %d, %d, %d]\n",
           stack_array[0], stack_array[1], stack_array[2],
           stack_array[3], stack_array[4]);

    /* Heap allocation - size can be determined at runtime */
    int size = 5;  /* This could come from user input */
    int *heap_array = malloc(size * sizeof(int));
    if (heap_array == NULL) {
        printf("Malloc failed!\n");
        return;
    }

    for (int i = 0; i < size; i++) {
        heap_array[i] = (i + 1) * 10;
    }

    printf("Heap array:  [%d, %d, %d, %d, %d]\n",
           heap_array[0], heap_array[1], heap_array[2],
           heap_array[3], heap_array[4]);

    /* MUST free heap memory! */
    free(heap_array);
    printf("Heap array freed.\n");
}

/* ============================================================================
 * EXERCISE 1: malloc and free [EASY]
 * ============================================================================
 *
 * TODO: Complete the function to allocate and populate an array.
 */

/*
 * Allocate an array of n integers and fill with values 0 to n-1.
 * Returns NULL on failure.
 * Caller is responsible for freeing the returned array.
 */
int *create_sequence(int n) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Allocate n integers with malloc
     * 2. Check if allocation succeeded
     * 3. Fill array with values 0, 1, 2, ..., n-1
     * 4. Return the array
     */

    return NULL;  /* TODO: Fix this */
}

void exercise1_malloc_free(void) {
    printf("\n=== Exercise 1: malloc and free ===\n");

    int *arr = create_sequence(10);
    if (arr != NULL) {
        printf("Created sequence: [");
        for (int i = 0; i < 10; i++) {
            printf("%d", arr[i]);
            if (i < 9) printf(", ");
        }
        printf("]\n");
        printf("Expected: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]\n");

        free(arr);
        printf("Array freed successfully.\n");
    } else {
        printf("TODO: Implement create_sequence\n");
    }
}

/* ============================================================================
 * EXERCISE 2: calloc [EASY]
 * ============================================================================
 *
 * calloc allocates AND initializes memory to zero.
 * Syntax: calloc(count, size) allocates count * size bytes, zeroed.
 */

/*
 * TODO: Allocate a zeroed array using calloc.
 */
int *create_zeroed_array(int n) {
    /* TODO: Use calloc to allocate and return zeroed array
     *
     * HINT: calloc(n, sizeof(int)) allocates n integers, all set to 0
     */

    return NULL;  /* TODO: Fix this */
}

void exercise2_calloc(void) {
    printf("\n=== Exercise 2: calloc ===\n");

    int *arr = create_zeroed_array(5);
    if (arr != NULL) {
        printf("Zeroed array: [%d, %d, %d, %d, %d]\n",
               arr[0], arr[1], arr[2], arr[3], arr[4]);
        printf("Expected: [0, 0, 0, 0, 0]\n");

        /* Unlike malloc, calloc guarantees zero initialization */
        free(arr);
    } else {
        printf("TODO: Implement create_zeroed_array\n");
    }
}

/* ============================================================================
 * EXERCISE 3: realloc - Growing Arrays [MEDIUM]
 * ============================================================================
 *
 * realloc changes the size of allocated memory.
 * It may move the data to a new location!
 */

/*
 * TODO: Implement a function to grow an array.
 * Returns new array pointer (may be different from old).
 * Returns NULL on failure (original array unchanged).
 */
int *grow_array(int *arr, int old_size, int new_size) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Use realloc to resize the array
     * 2. If realloc fails, return NULL (original array is still valid)
     * 3. If realloc succeeds, initialize new elements to 0
     * 4. Return the new pointer
     *
     * IMPORTANT: Don't use the old pointer after realloc succeeds!
     *            realloc may have moved the data.
     */

    return NULL;  /* TODO: Fix this */
}

void exercise3_realloc(void) {
    printf("\n=== Exercise 3: realloc ===\n");

    /* Create initial array */
    int *arr = malloc(3 * sizeof(int));
    if (arr == NULL) {
        printf("Initial malloc failed\n");
        return;
    }

    arr[0] = 10;
    arr[1] = 20;
    arr[2] = 30;

    printf("Initial array (size 3): [%d, %d, %d]\n", arr[0], arr[1], arr[2]);

    /* Grow to size 5 */
    int *new_arr = grow_array(arr, 3, 5);
    if (new_arr != NULL) {
        arr = new_arr;  /* Update pointer - realloc may have moved data */

        printf("After growing to size 5: [%d, %d, %d, %d, %d]\n",
               arr[0], arr[1], arr[2], arr[3], arr[4]);
        printf("Expected: [10, 20, 30, 0, 0]\n");

        free(arr);
    } else {
        printf("TODO: Implement grow_array\n");
        free(arr);
    }
}

/* ============================================================================
 * EXERCISE 4: Dynamic Array (Vector) [HARD]
 * ============================================================================
 *
 * This is a complete implementation of a growable array (like std::vector).
 * This pattern is VERY common in systems programming.
 */

typedef struct {
    int *data;      /* Pointer to array data */
    int size;       /* Number of elements currently stored */
    int capacity;   /* Total allocated capacity */
} DynamicArray;

/*
 * TODO: Initialize a dynamic array with given initial capacity.
 * Returns 0 on success, -1 on failure.
 */
int da_init(DynamicArray *da, int initial_capacity) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Allocate 'initial_capacity' integers for data
     * 2. Set size to 0
     * 3. Set capacity to initial_capacity
     * 4. Return 0 on success, -1 on failure
     */

    return -1;  /* TODO: Fix this */
}

/*
 * TODO: Add an element to the end of the array.
 * Grows the array if necessary (double the capacity).
 * Returns 0 on success, -1 on failure.
 */
int da_append(DynamicArray *da, int value) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. If size == capacity, grow the array (double capacity)
     * 2. Add value at index 'size'
     * 3. Increment size
     * 4. Return 0 on success, -1 on failure
     *
     * HINT for growing:
     *   int new_capacity = da->capacity * 2;
     *   int *new_data = realloc(da->data, new_capacity * sizeof(int));
     *   if (new_data == NULL) return -1;
     *   da->data = new_data;
     *   da->capacity = new_capacity;
     */

    return -1;  /* TODO: Fix this */
}

/*
 * TODO: Get element at index.
 * Returns the value, or -1 if index is out of bounds.
 * (In real code, you'd use a different error mechanism)
 */
int da_get(DynamicArray *da, int index) {
    /* TODO: Implement this function */

    return -1;  /* TODO: Fix this */
}

/*
 * TODO: Free the dynamic array.
 */
void da_free(DynamicArray *da) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Free the data array
     * 2. Set data to NULL
     * 3. Set size and capacity to 0
     */
}

/*
 * Print the dynamic array (provided)
 */
void da_print(DynamicArray *da) {
    printf("[");
    for (int i = 0; i < da->size; i++) {
        printf("%d", da->data[i]);
        if (i < da->size - 1) printf(", ");
    }
    printf("] (size=%d, capacity=%d)\n", da->size, da->capacity);
}

void exercise4_dynamic_array(void) {
    printf("\n=== Exercise 4: Dynamic Array (Vector) ===\n");

    DynamicArray da;

    if (da_init(&da, 4) != 0) {
        printf("TODO: Implement da_init\n");
        return;
    }

    printf("Initial state: ");
    da_print(&da);

    /* Add elements */
    for (int i = 1; i <= 10; i++) {
        if (da_append(&da, i * 10) != 0) {
            printf("da_append failed\n");
            da_free(&da);
            return;
        }
        printf("After appending %d: ", i * 10);
        da_print(&da);
    }

    printf("\nFinal array: ");
    da_print(&da);
    printf("Expected: [10, 20, 30, 40, 50, 60, 70, 80, 90, 100]\n");
    printf("Capacity should have grown: 4 -> 8 -> 16\n");

    /* Test get */
    printf("\nda_get(&da, 5) = %d (expected: 60)\n", da_get(&da, 5));
    printf("da_get(&da, 100) = %d (expected: -1, out of bounds)\n", da_get(&da, 100));

    da_free(&da);
    printf("\nArray freed.\n");
}

/* ============================================================================
 * EXERCISE 5: Memory Management Patterns [MEDIUM]
 * ============================================================================
 *
 * Common patterns for safe memory management in C.
 */

/*
 * Pattern 1: Create/Destroy pairs
 * Always pair allocation functions with deallocation functions.
 */
typedef struct {
    char *name;
    int *values;
    int count;
} Resource;

Resource *resource_create(const char *name, int count) {
    Resource *r = malloc(sizeof(Resource));
    if (r == NULL) return NULL;

    r->name = strdup(name);  /* strdup = malloc + strcpy */
    if (r->name == NULL) {
        free(r);
        return NULL;
    }

    r->values = calloc(count, sizeof(int));
    if (r->values == NULL) {
        free(r->name);
        free(r);
        return NULL;
    }

    r->count = count;
    return r;
}

void resource_destroy(Resource *r) {
    if (r == NULL) return;
    free(r->name);
    free(r->values);
    free(r);
}

/*
 * Pattern 2: Cleanup on error using goto
 * This pattern avoids duplicating cleanup code.
 */
int process_data(int size) {
    int result = -1;

    int *buffer1 = malloc(size * sizeof(int));
    if (buffer1 == NULL) goto cleanup;

    int *buffer2 = malloc(size * sizeof(int));
    if (buffer2 == NULL) goto cleanup;

    /* Do processing... */
    for (int i = 0; i < size; i++) {
        buffer1[i] = i;
        buffer2[i] = i * 2;
    }

    result = 0;  /* Success */

cleanup:
    free(buffer2);  /* free(NULL) is safe */
    free(buffer1);
    return result;
}

void exercise5_patterns(void) {
    printf("\n=== Exercise 5: Memory Management Patterns ===\n");

    /* Pattern 1: Create/Destroy */
    Resource *r = resource_create("Test Resource", 5);
    if (r != NULL) {
        printf("Created resource: %s with %d values\n", r->name, r->count);
        r->values[0] = 100;
        printf("First value: %d\n", r->values[0]);
        resource_destroy(r);
        printf("Resource destroyed.\n");
    }

    /* Pattern 2: Goto cleanup */
    int result = process_data(100);
    printf("process_data result: %d (0 = success)\n", result);

    printf("\nKey patterns:\n");
    printf("1. Always pair create/destroy functions\n");
    printf("2. Use goto for cleanup to avoid code duplication\n");
    printf("3. Always check malloc/calloc/realloc return values\n");
    printf("4. Set pointers to NULL after freeing\n");
}

/* ============================================================================
 * MAIN FUNCTION
 * ============================================================================
 */
int main(void) {
    printf("\n");
    printf("================================================\n");
    printf("  Module 04: Dynamic Arrays\n");
    printf("================================================\n");

    demonstrate_stack_heap();
    exercise1_malloc_free();
    exercise2_calloc();
    exercise3_realloc();
    exercise4_dynamic_array();
    exercise5_patterns();

    printf("\n================================================\n");
    printf("  Dynamic Arrays Complete!\n");
    printf("  Next: memory_leaks.c\n");
    printf("================================================\n\n");

    return 0;
}
