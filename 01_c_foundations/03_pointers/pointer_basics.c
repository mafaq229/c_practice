/*
CS-6200 Preparation - Module 03: Pointer Basics

Pointers are CRITICAL for systems programming and GIOS. This module builds
your understanding from the ground up.

Compile: clang -Wall -Wextra -g pointer_basics.c -o pointer_basics
Run:     ./pointer_basics

Topics covered:
  - What pointers are and why we use them
  - Address-of (&) and dereference (*) operators
  - Pointer arithmetic
  - Arrays and pointers relationship
  - Passing pointers to functions

Difficulty: [EASY] to [MEDIUM]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================================
EXERCISE 1: Pointer Basics [EASY]
============================================================================

Understand the relationship between variables, addresses, and pointers.
*/
void exercise1_pointer_basics(void) {
    printf("\n=== Exercise 1: Pointer Basics ===\n");

    int x = 42;
    int *ptr;  /* Declare a pointer to int */

    /* TODO: Make ptr point to x using the address-of operator (&) */
    ptr = &x;  /* TODO: Replace NULL with the correct expression */

    printf("Value of x: %d\n", x);
    printf("Address of x: %p\n", (void*)&x);
    printf("Value of ptr (address it holds): %p\n", (void*)ptr);

    /* TODO: Print the value ptr points to using the dereference operator (*) */
    /* printf("Value ptr points to: %d\n", ???); */
    printf("Value ptr points to: %d\n", *ptr);

    /* TODO: Change the value of x through ptr */
    /* Uncomment and fix: *ptr = 100; */
    *ptr = 100;
    printf("After modification through ptr, x = %d (expected: 100)\n", x);

    /*
     * KEY CONCEPTS:
     * - &x gives the memory address of x
     * - ptr = &x makes ptr point to x
     * - *ptr gives the value at the address ptr holds
     * - *ptr = value changes the value at that address
     */
}

/* ============================================================================
EXERCISE 2: Pointer Arithmetic [MEDIUM]
============================================================================

Pointer arithmetic is essential for working with arrays and buffers.
*/
void exercise2_pointer_arithmetic(void) {
    printf("\n=== Exercise 2: Pointer Arithmetic ===\n");

    int arr[] = {10, 20, 30, 40, 50};
    int *ptr = arr;  /* Points to first element (arr[0]) */

    printf("Array: [10, 20, 30, 40, 50]\n\n");

    /* Accessing elements using pointer arithmetic */
    printf("*ptr       = %d (first element)\n", *ptr);
    printf("*(ptr + 1) = %d (second element)\n", *(ptr + 1));
    printf("*(ptr + 2) = %d (third element)\n", *(ptr + 2));

    /* TODO: Complete these using pointer arithmetic */
    int fourth = *(ptr + 3);  /* TODO: Get fourth element using *(ptr + ?) */
    int fifth = *(ptr + 4);   /* TODO: Get fifth element using *(ptr + ?) */

    printf("Fourth element: %d (expected: 40)\n", fourth);
    printf("Fifth element: %d (expected: 50)\n", fifth);

    /* TODO: Use pointer increment to iterate through the array */
    printf("\nIterating with pointer increment:\n");
    int *p = arr;
    for (int i = 0; i < 5; i++) {
        /* TODO: Print *p, then increment p */
        /* printf("arr[%d] = %d\n", i, ???); */
        /* ??? increment p */
        printf("arr[%d] = %d\n", i, *(p));
        p++;
    }

    /*
     * KEY CONCEPTS:
     * - ptr + n moves n elements forward (not n bytes!)
     * - *(ptr + n) is equivalent to ptr[n] and arr[n]
     * - ptr++ moves to the next element
     * - ptr - another_ptr gives the number of elements between them
     */
}

/* ============================================================================
EXERCISE 3: Pointers and Arrays [MEDIUM]
============================================================================

Arrays and pointers are closely related but not identical.
*/
void exercise3_arrays_pointers(void) {
    printf("\n=== Exercise 3: Pointers and Arrays ===\n");

    int arr[5] = {1, 2, 3, 4, 5};
    int *ptr = arr;  /* Array name decays to pointer to first element */

    /* These are all equivalent ways to access arr[2] */
    printf("arr[2] = %d\n", arr[2]);
    printf("*(arr + 2) = %d\n", *(arr + 2));
    printf("ptr[2] = %d\n", ptr[2]);
    printf("*(ptr + 2) = %d\n", *(ptr + 2));

    /* TODO: Implement a function to sum array elements using a pointer */
    /* Sum using array indexing (provided) */
    int sum1 = 0;
    for (int i = 0; i < 5; i++) {
        sum1 += arr[i];
    }
    printf("\nSum using array indexing: %d\n", sum1);

    /* TODO: Sum using pointer arithmetic */
    int sum2 = 0;
    int *p = arr;
    /* TODO: Implement the loop using pointer arithmetic */
    /* HINT: Use a while loop, increment p, check if p < arr + 5 */
    while (p < arr + 5) {
        sum2 += *p;
        p++;
    }
    /*
    for (p = arr; p < arr + 5; p++) {
        sum2 += *p;
    }
    */

    printf("Sum using pointer arithmetic: %d (expected: 15)\n", sum2);

    /*
     * KEY DIFFERENCE:
     * - arr is an array - sizeof(arr) gives total array size
     * - ptr is a pointer - sizeof(ptr) gives pointer size (8 bytes on 64-bit)
     */
    printf("\nsizeof(arr) = %zu bytes (total array size)\n", sizeof(arr));
    printf("sizeof(ptr) = %zu bytes (pointer size)\n", sizeof(ptr));
}

/* ============================================================================
EXERCISE 4: Passing Pointers to Functions [MEDIUM]
============================================================================

This is how you modify variables across function calls.
*/

/*
TODO: Complete this function to set the value at the pointer.
*/
void set_value(int *ptr, int value) {
    /* TODO: Set the value at the address ptr points to */
    /* HINT: Use the dereference operator */
    *ptr = value;
}

/*
TODO: Complete this function to swap two integers using pointers.
*/
void swap_ints(int *a, int *b) {
    /* TODO: Implement swap
     * 1. Store *a in a temp variable
     * 2. Set *a = *b
     * 3. Set *b = temp
     */
    int temp = *a;
    *a = *b;
    *b = temp;
}

/*
TODO: Complete this function to find min and max in an array.
Store results in the provided pointers.
*/
void find_min_max(int arr[], int size, int *min, int *max) {
    /* TODO: Find min and max, store in *min and *max */
    /* HINT: Initialize *min and *max to arr[0], then iterate */
    *min = arr[0];
    *max = arr[0];
    for (int i = 1; i < size; i++) {
        if (*min > arr[i]) {
            *min = arr[i];
        }
        if (*max < arr[i]) {
            *max = arr[i];
        }
    }
}

void exercise4_pointer_functions(void) {
    printf("\n=== Exercise 4: Passing Pointers to Functions ===\n");

    /* Test set_value */
    int x = 0;
    set_value(&x, 42);
    printf("After set_value(&x, 42): x = %d (expected: 42)\n", x);

    /* Test swap_ints */
    int a = 10, b = 20;
    printf("Before swap: a = %d, b = %d\n", a, b);
    swap_ints(&a, &b);
    printf("After swap:  a = %d, b = %d (expected: a = 20, b = 10)\n", a, b);

    /* Test find_min_max */
    int arr[] = {3, 7, 1, 9, 4, 6, 2, 8, 5};
    int min, max;
    find_min_max(arr, 9, &min, &max);
    printf("Array: [3, 7, 1, 9, 4, 6, 2, 8, 5]\n");
    printf("Min: %d (expected: 1), Max: %d (expected: 9)\n", min, max);
}

/* ============================================================================
EXERCISE 5: Pointer Comparison and NULL [EASY]
============================================================================

Understanding NULL and pointer comparison is crucial for safe code.
*/
void exercise5_null_comparison(void) {
    printf("\n=== Exercise 5: NULL and Pointer Comparison ===\n");

    int x = 42;
    int *ptr1 = &x;
    int *ptr2 = NULL;  /* NULL pointer - points to nothing */

    /* Always check for NULL before dereferencing! */
    if (ptr1 != NULL) {
        printf("ptr1 is valid, value: %d\n", *ptr1);
    }

    if (ptr2 != NULL) {
        printf("ptr2 is valid, value: %d\n", *ptr2);
    } else {
        printf("ptr2 is NULL - cannot dereference!\n");
    }

    /* TODO: Implement a safe dereference function */
    /* This pattern is VERY common in systems programming */
}

/*
TODO: Implement a function that safely gets a value from a pointer.
If ptr is NULL, return the default_value.
Otherwise, return *ptr.
*/
int safe_get(int *ptr, int default_value) {
    /* TODO: Implement this function */
    return ptr != NULL ? *ptr : default_value;
}

/* ============================================================================
EXERCISE 6: Const Pointers [MEDIUM]
============================================================================

Understanding const with pointers - this trips up many C programmers!
*/
void exercise6_const_pointers(void) {
    printf("\n=== Exercise 6: Const Pointers ===\n");

    int x = 10, y = 20;

    /*
     * There are THREE different uses of const with pointers:
     *
     * 1. Pointer to const int - can't modify value through pointer
     *    const int *ptr1;  OR  int const *ptr1;
     *
     * 2. Const pointer to int - can't change what pointer points to
     *    int * const ptr2;
     *
     * 3. Const pointer to const int - can't do either
     *    const int * const ptr3;
     */

    /* Pointer to const int */
    const int *ptr1 = &x;
    printf("ptr1 points to: %d\n", *ptr1);
    /* *ptr1 = 100;  // ERROR: can't modify through ptr1 */
    ptr1 = &y;  /* OK: can change what ptr1 points to */
    printf("ptr1 now points to: %d\n", *ptr1);

    /* Const pointer to int */
    int * const ptr2 = &x;
    printf("ptr2 points to: %d\n", *ptr2);
    *ptr2 = 100;  /* OK: can modify the value */
    printf("After *ptr2 = 100, x = %d\n", x);
    /* ptr2 = &y;  // ERROR: can't change what ptr2 points to */

    /* TODO: Declare a const pointer to const int pointing to y */
    /* const int * const ptr3 = ???; */
    /* Neither *ptr3 = value nor ptr3 = &other is allowed */
    
    const int * const ptr3 = &y; // throws a warning since not used

    printf("\nRemember:\n");
    printf("  'const int *ptr' - value is const, pointer is not\n");
    printf("  'int * const ptr' - pointer is const, value is not\n");
    printf("  'const int * const ptr' - both are const\n");
}

/* ============================================================================
CHALLENGE: Reverse String In-Place Using Pointers [MEDIUM]
============================================================================

TODO: Implement string reversal using only pointer arithmetic.
Do not use array indexing (no str[i]).
*/
void reverse_string_ptr(char *str) {
    /* TODO: Implement using pointers only
     *
     * HINTS:
     * 1. Find the end of the string (where *end == '\0')
     * 2. Use two pointers: start and end
     * 3. Swap characters while start < end
     * 4. Move pointers toward each other
     */
    if (str == NULL || *str == '\0') {
        return;
    }

    char *start = str;
    char *end = str;

    while (*end != '\0') {
        end++;
    }
    end--;

    while (start < end) {
        char temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }

}

/* ============================================================================
MAIN FUNCTION
============================================================================
*/
int main(void) {
    printf("\n");
    printf("================================================\n");
    printf("  Module 03: Pointer Basics\n");
    printf("================================================\n");

    exercise1_pointer_basics();
    exercise2_pointer_arithmetic();
    exercise3_arrays_pointers();
    exercise4_pointer_functions();
    exercise5_null_comparison();

    /* Test safe_get */
    printf("\nTesting safe_get:\n");
    int val = 42;
    printf("safe_get(&val, 0) = %d (expected: 42)\n", safe_get(&val, 0));
    printf("safe_get(NULL, -1) = %d (expected: -1)\n", safe_get(NULL, -1));

    exercise6_const_pointers();

    /* Test reverse_string_ptr */
    printf("\n=== Challenge: Reverse String with Pointers ===\n");
    char str[] = "Hello, World!";
    printf("Before: %s\n", str);
    reverse_string_ptr(str);
    printf("After:  %s (expected: !dlroW ,olleH)\n", str);

    printf("\n================================================\n");
    printf("  Pointer Basics Complete!\n");
    printf("  Next: double_pointers.c\n");
    printf("================================================\n\n");

    return 0;
}
