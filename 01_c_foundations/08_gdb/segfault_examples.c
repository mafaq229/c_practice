/*
CS-6200 Preparation - Module 08: Debugging Segfaults

Segmentation faults are common in C. This file teaches you to debug them.

Compile: clang -Wall -Wextra -g segfault_examples.c -o segfault_examples

Debug with LLDB (macOS):
  lldb ./segfault_examples
  (lldb) run
  [crash]
  (lldb) thread backtrace

Debug with GDB (via Docker):

docker run -it --rm -v $(pwd):/code -w /code gios-prep \
  sh -c "gcc -Wall -Wextra -g segfault_examples.c -o segfault_examples && gdb ./segfault_examples"

Difficulty: [MEDIUM] to [HARD]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================================
SEGFAULT 1: NULL Pointer Dereference
============================================================================

The most common cause of segfaults.
*/
void segfault1_null_pointer(void) {
    printf("\n=== Segfault 1: NULL Pointer ===\n");

    int *ptr = NULL;

    /* This will crash! */
    /* *ptr = 42; */

    printf("Uncomment the crash line to test with debugger.\n");
    printf("When it crashes, use 'bt' to see the backtrace.\n");
}

/* ============================================================================
SEGFAULT 2: Use After Free
============================================================================

Accessing memory after it's been freed.
*/
void segfault2_use_after_free(void) {
    printf("\n=== Segfault 2: Use After Free ===\n");

    int *ptr = malloc(sizeof(int));
    *ptr = 42;
    printf("Before free: *ptr = %d\n", *ptr);

    free(ptr);

    /* This is undefined behavior - might work, might crash */
    /* printf("After free: *ptr = %d\n", *ptr); */

    printf("Uncomment to test (may or may not crash).\n");
}

/* ============================================================================
SEGFAULT 3: Stack Overflow
============================================================================

Infinite recursion causes stack overflow.
*/
int infinite_recursion(int n) {
    /* No base case - infinite recursion! */
    return infinite_recursion(n + 1);
}

void segfault3_stack_overflow(void) {
    printf("\n=== Segfault 3: Stack Overflow ===\n");

    /* This will crash with stack overflow! */
    /* infinite_recursion(0); */

    printf("Uncomment to test stack overflow.\n");
    printf("The backtrace will show many identical frames.\n");
}

/* ============================================================================
SEGFAULT 4: Out of Bounds Access
============================================================================

Accessing array elements outside valid range.
*/
void segfault4_out_of_bounds(void) {
    printf("\n=== Segfault 4: Out of Bounds ===\n");

    int arr[5] = {1, 2, 3, 4, 5};

    /* Accessing way out of bounds - likely to crash */
    /* printf("arr[1000000] = %d\n", arr[1000000]); */

    printf("Uncomment to test out of bounds access.\n");
}

/* ============================================================================
SEGFAULT 5: Invalid Pointer Arithmetic
============================================================================

Dereferencing an invalid pointer.
*/
void segfault5_invalid_pointer(void) {
    printf("\n=== Segfault 5: Invalid Pointer ===\n");

    int x = 42;
    int *ptr = &x;

    /* Move pointer to invalid location */
    ptr = ptr + 1000000;

    /* This will crash */
    /* printf("*ptr = %d\n", *ptr); */

    printf("Uncomment to test invalid pointer access.\n");
}

/* ============================================================================
SEGFAULT 6: Modifying String Literal
============================================================================

String literals are stored in read-only memory.
*/
void segfault6_string_literal(void) {
    printf("\n=== Segfault 6: Modifying String Literal ===\n");

    char *str = "Hello";  /* Points to read-only memory */

    /* This will crash on most systems */
    /* str[0] = 'J'; */

    printf("Uncomment to test modifying string literal.\n");
    printf("Note: char str[] = \"Hello\" would be modifiable.\n");
}

/* ============================================================================
SEGFAULT 7: Uninitialized Pointer
============================================================================

Using a pointer before initializing it.
*/
void segfault7_uninitialized(void) {
    printf("\n=== Segfault 7: Uninitialized Pointer ===\n");

    int *ptr;  /* Uninitialized - contains garbage */

    /* This will likely crash */
    /* *ptr = 42; */

    printf("Uncomment to test uninitialized pointer.\n");
    printf("ptr contains garbage address: %p\n", (void*)ptr);
}

/* ============================================================================
EXERCISE: Debug This Function
============================================================================

This function has a segfault bug. Use the debugger to find it.
*/
typedef struct {
    char *name;
    int value;
} Item;

Item *create_item(const char *name, int value) {
    Item *item = malloc(sizeof(Item));
    if (item == NULL) return NULL;

    item->name = strdup(name);
    item->value = value;

    return item;
}

void print_item(Item *item) {
    /* BUG: No NULL check! */
    printf("Item: %s = %d\n", item->name, item->value);
}

void free_item(Item *item) {
    if (item == NULL) return;
    free(item->name);
    free(item);
}

void exercise_debug_segfault(void) {
    printf("\n=== Exercise: Debug This ===\n");

    Item *item1 = create_item("apple", 100);
    print_item(item1);

    Item *item2 = NULL;  /* Simulating failed creation */
    /* print_item(item2);  // This will crash! */

    printf("Uncomment the buggy line and debug.\n");
    printf("Hint: Use breakpoint at print_item.\n");

    free_item(item1);
}

/* ============================================================================
HOW TO DEBUG SEGFAULTS
============================================================================

Step 1: Run in debugger
  lldb ./program
  (lldb) run

Step 2: When it crashes, get backtrace
  (lldb) thread backtrace
  or
  (lldb) bt

Step 3: Look at the backtrace
  - Find the first frame in YOUR code (ignore library frames)
  - That's likely where the bug is

Step 4: Examine variables
  (lldb) frame variable
  (lldb) print ptr
  (lldb) print *ptr

Step 5: Common causes to check
  - Is the pointer NULL?
  - Was the memory freed already?
  - Is the index within array bounds?
  - Was the pointer properly initialized?

Step 6: Use Address Sanitizer (via Docker)
  docker run --rm -v $(pwd):/code -w /code gios-prep \
      bash -c "gcc -g -fsanitize=address program.c -o program && ./program"

  Address Sanitizer gives detailed reports on memory errors.
*/

/* ============================================================================
MAIN FUNCTION
============================================================================
*/
int main(int argc, char *argv[]) {
    printf("\n");
    printf("================================================\n");
    printf("  Module 08: Debugging Segfaults\n");
    printf("================================================\n");

    /* Allow selecting which test to run */
    int test = 0;
    if (argc > 1) {
        test = atoi(argv[1]);
    }

    switch (test) {
        case 1:
            segfault1_null_pointer();
            break;
        case 2:
            segfault2_use_after_free();
            break;
        case 3:
            segfault3_stack_overflow();
            break;
        case 4:
            segfault4_out_of_bounds();
            break;
        case 5:
            segfault5_invalid_pointer();
            break;
        case 6:
            segfault6_string_literal();
            break;
        case 7:
            segfault7_uninitialized();
            break;
        default:
            /* Run all (safe versions) */
            segfault1_null_pointer();
            segfault2_use_after_free();
            segfault3_stack_overflow();
            segfault4_out_of_bounds();
            segfault5_invalid_pointer();
            segfault6_string_literal();
            segfault7_uninitialized();
            exercise_debug_segfault();
            break;
    }

    printf("\n================================================\n");
    printf("  To test a specific crash:\n");
    printf("  ./segfault_examples [1-7]\n");
    printf("  Then uncomment the crash line and rebuild.\n");
    printf("================================================\n\n");

    return 0;
}
