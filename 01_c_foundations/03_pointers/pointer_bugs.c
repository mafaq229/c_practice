/*
CS-6200 Preparation - Module 03: Pointer Bugs

This file contains intentionally buggy code for you to fix.
Learning to identify and fix pointer bugs is ESSENTIAL for GIOS.

Compile: clang -Wall -Wextra -g pointer_bugs.c -o pointer_bugs
Run:     ./pointer_bugs

For memory checking (use Docker on M4 Mac):

docker run --rm -v $(pwd):/code -w /code gios-prep \
  sh -c "gcc -Wall -Wextra -g pointer_bugs.c -o pointer_bugs && valgrind --leak-check=full ./pointer_bugs"

Topics covered:
  - Null pointer dereference
  - Use after free
  - Memory leaks
  - Buffer overflow
  - Uninitialized pointers
  - Double free

Difficulty: [MEDIUM] to [HARD]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================================
BUG 1: NULL Pointer Dereference [EASY]
============================================================================

TODO: Fix this function to handle NULL pointer.
It should return 0 if ptr is NULL.
*/
int get_value_bug1(int *ptr) {
    /* BUG: No NULL check before dereference */
    if (ptr == NULL) {
        return 0;
    }
    return *ptr;

    /* TODO: Fix this function */
}

void test_bug1(void) {
    printf("\n=== Bug 1: NULL Pointer Dereference ===\n");

    int x = 42;
    printf("get_value_bug1(&x) = %d\n", get_value_bug1(&x));

    /* This will crash without the fix! */
    /* printf("get_value_bug1(NULL) = %d\n", get_value_bug1(NULL)); */

    printf("TODO: Uncomment the NULL test after fixing\n");
}

/* ============================================================================
BUG 2: Use After Free [MEDIUM]
============================================================================

TODO: Fix this function to avoid use-after-free.
*/
void use_after_free_bug2(void) {
    printf("\n=== Bug 2: Use After Free ===\n");

    int *ptr = malloc(sizeof(int));
    if (ptr == NULL) {
        printf("Allocation failed\n");
        return;
    }

    *ptr = 42;
    printf("Before free: *ptr = %d\n", *ptr);

    free(ptr);
    ptr = NULL;
    /* BUG: Using ptr after it's been freed! */
    /* This is undefined behavior - might work, might crash, might corrupt data */
    if (ptr != NULL) {
        printf("After free: *ptr = %d (UNDEFINED BEHAVIOR!)\n", *ptr);
    } else {
        printf("After free: ptr is NULL (safe)\n");
    }

    /* TODO: Fix this bug
     * HINT: Set ptr to NULL after free, and check before use
     */
}

/* ============================================================================
BUG 3: Memory Leak [MEDIUM]
============================================================================

TODO: Fix this function to avoid memory leak.
*/
int *create_array_bug3(int size) {
    int *arr = malloc(size * sizeof(int));
    if (arr == NULL) return NULL;

    for (int i = 0; i < size; i++) {
        arr[i] = i * 2;
    }

    return arr;

    /* TODO: Fix this function
     * HINT: Either free arr1, or don't allocate it in the first place
     */
}

void test_bug3(void) {
    printf("\n=== Bug 3: Memory Leak ===\n");

    int *arr = create_array_bug3(10);
    if (arr != NULL) {
        printf("Array created: [%d, %d, %d, ...]\n", arr[0], arr[1], arr[2]);
        free(arr);
    }

    printf("No memory leak expected.\n");
}

/* ============================================================================
BUG 4: Buffer Overflow [HARD]
============================================================================

TODO: Fix this function to avoid buffer overflow.
*/
void string_copy_bug4(char *dest, const char *src, size_t dest_size) {
    /* BUG: strcpy doesn't check bounds! */
    if (dest == NULL || src == NULL || dest_size == 0) return;
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';

    /* TODO: Fix this function to use strncpy or manual bound checking
     * REMEMBER: strncpy might not null-terminate if src is too long
     */
}

void test_bug4(void) {
    printf("\n=== Bug 4: Buffer Overflow ===\n");

    char small_buffer[10];
    const char *long_string = "This is a very long string that will overflow!";

    string_copy_bug4(small_buffer, long_string, sizeof(small_buffer));
    printf("Buffer: %s\n", small_buffer);
}

/* ============================================================================
BUG 5: Uninitialized Pointer [EASY]
============================================================================

TODO: Fix this function - the pointer must be initialized.
*/
void uninitialized_bug5(void) {
    printf("\n=== Bug 5: Uninitialized Pointer ===\n");

    int *ptr = malloc(sizeof(*ptr));
    if (ptr == NULL) {
        printf("Allocation failed\n");
        return;
    }

    /* Using an uninitialized pointer is undefined behavior */
    /* It might point anywhere in memory */
    *ptr = 42;
    printf("Initialized ptr: *ptr = %d\n", *ptr);
    free(ptr);
    ptr = NULL;

    /* TODO: Fix by either:
     * 1. ptr = NULL; and then check before use
     * 2. ptr = malloc(...); and then use
     */
}

/* ============================================================================
BUG 6: Double Free [MEDIUM]
============================================================================

TODO: Fix this function to avoid double free.
*/
void double_free_bug6(void) {
    printf("\n=== Bug 6: Double Free ===\n");

    int *ptr = malloc(sizeof(int));
    if (ptr == NULL) return;

    *ptr = 42;
    printf("Value: %d\n", *ptr);

    free(ptr);
    ptr = NULL;
    /* BUG: Freeing the same memory twice! */
    /* free(ptr);  // This is undefined behavior! */

    printf("TODO: Set ptr = NULL after free to prevent double free.\n");

    /* TODO: Fix by setting ptr = NULL after first free
     * Then the second free(NULL) is safe (does nothing)
     */
}

/* ============================================================================
BUG 7: Returning Pointer to Local Variable [MEDIUM]
============================================================================

TODO: Fix this function - returning address of local variable is wrong!
*/
int *get_value_bug7(int value) {
    //int local = value;  /* Local variable on the stack */
    //return &local;  /* BUG: Returning address of local variable! */

    int *heap_value = malloc(sizeof(*heap_value));
    if (heap_value == NULL) return NULL;
    *heap_value = value;
    return heap_value;

    /* When the function returns, 'local' is destroyed,
     * so the returned pointer points to invalid memory */

    /* TODO: Fix by either:
     * 1. Allocate memory with malloc
     * 2. Use a static variable (with caveats)
     * 3. Have the caller pass in the memory
     */
}

void test_bug7(void) {
    printf("\n=== Bug 7: Returning Pointer to Local ===\n");

    int *ptr = get_value_bug7(42);
    if (ptr != NULL) {
        printf("Value: %d\n", *ptr);
        free(ptr);
    }
}

/* ============================================================================
BUG 8: Off-by-One Error [MEDIUM]
============================================================================

TODO: Fix this function - classic off-by-one in loop.
*/
void fill_array_bug8(int *arr, int size) {
    /* BUG: Loop goes one past the end of the array! */
    for (int i = 0; i < size; i++) {  /* Should be i < size */
        arr[i] = i * 10;
    }

    /* TODO: Fix the loop condition */
}

void test_bug8(void) {
    printf("\n=== Bug 8: Off-by-One Error ===\n");

    int arr[5] = {0};
    int canary = 9999;  /* This might get corrupted */

    fill_array_bug8(arr, 5);
    printf("Canary: %d\n", canary);
}

/* ============================================================================
BUG 9: Pointer Type Confusion [MEDIUM]
============================================================================

TODO: Understand why this is wrong and fix it.
*/
void type_confusion_bug9(void) {
    printf("\n=== Bug 9: Pointer Type Confusion ===\n");

    int x = 0x12345678;
    char *char_ptr = (char *)&x;  /* Treating int as char array */

    printf("int x = 0x%x\n", x);
    printf("Bytes: ");
    for (int i = 0; i < (int)sizeof(int); i++) {
        printf("0x%02x ", (unsigned char)char_ptr[i]);
    }
    printf("\n");

    /* This is actually valid and shows endianness!
     * On little-endian (most common): 78 56 34 12
     * On big-endian: 12 34 56 78
     */

    /* The bug is when you do this incorrectly: */
    double d = 3.14159;
    /* int *int_ptr = (int *)&d;  // WRONG type size! */
    /* printf("Double as int: %d\n", *int_ptr);  // Undefined behavior */

    printf("Type punning requires matching sizes and understanding.\n");
}

/* ============================================================================
BUG 10: Memory Allocation Failure Not Checked [EASY]
============================================================================

TODO: Fix this function to handle allocation failure.
*/
int *allocate_array_bug10(int size) {
    int *arr = malloc(size * sizeof(int));
    /* BUG: No check for malloc failure! */
    if (arr == NULL) return NULL;
    /* If malloc returns NULL, this will crash: */
    for (int i = 0; i < size; i++) {
        arr[i] = 0;
    }

    return arr;

    /* TODO: Add NULL check after malloc */
}

void test_bug10(void) {
    printf("\n=== Bug 10: Allocation Failure Not Checked ===\n");

    int *arr = allocate_array_bug10(10);
    if (arr != NULL) {
        printf("Array allocated successfully\n");
        free(arr);
    } else {
        printf("Allocation failed (handled correctly)\n");
    }

    printf("TODO: Add NULL check in allocate_array_bug10\n");
}

/* ============================================================================
SUMMARY: Common Pointer Bug Checklist
============================================================================

Before submitting GIOS projects, check for:

1. [ ] NULL checks after malloc/calloc/realloc
2. [ ] No use of pointers after free
3. [ ] Set pointers to NULL after freeing
4. [ ] No double frees
5. [ ] No returning pointers to local variables
6. [ ] Correct loop bounds (< size, not <= size)
7. [ ] strncpy instead of strcpy for bounded strings
8. [ ] Initialize all pointers before use
9. [ ] Free all allocated memory before function exit
10. [ ] No pointer type confusion (matching sizes)
*/

/* ============================================================================
MAIN FUNCTION
============================================================================
*/
int main(void) {
    printf("\n");
    printf("================================================\n");
    printf("  Module 03: Pointer Bugs\n");
    printf("================================================\n");
    printf("\nThis file contains intentional bugs for you to fix.\n");
    printf("Some tests are commented out to prevent crashes.\n");
    printf("Fix the bugs, then uncomment the tests.\n");

    test_bug1();
    use_after_free_bug2();
    test_bug3();
    test_bug4();
    uninitialized_bug5();
    double_free_bug6();
    test_bug7();
    test_bug8();
    type_confusion_bug9();
    test_bug10();

    printf("\n================================================\n");
    printf("  Pointer Bugs Complete!\n");
    printf("  Run with Valgrind (via Docker) to find leaks:\n");
    printf("  docker run --rm -v $(pwd):/code -w /code gios-prep \\\n");
    printf("      valgrind --leak-check=full ./pointer_bugs\n");
    printf("================================================\n\n");

    return 0;
}
