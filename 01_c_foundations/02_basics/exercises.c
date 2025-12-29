/*
 * CS-6200 Preparation - Module 02: C Basics & Compilation
 *
 * This module covers fundamental C concepts. Complete all TODO items.
 *
 * Compile: clang -Wall -Wextra -g exercises.c -o exercises
 * Run:     ./exercises
 *
 * Topics covered:
 *   - Variables and data types
 *   - Operators and expressions
 *   - Control flow (if/else, loops, switch)
 *   - Functions
 *   - Arrays
 *   - Header files and multi-file programs
 *
 * Difficulty: [EASY] to [MEDIUM]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* ============================================================================
 * EXERCISE 1: Variables and Data Types [EASY]
 * ============================================================================
 *
 * TODO: Complete the function to demonstrate understanding of C data types.
 *       Fill in the correct format specifiers for printf.
 */
void exercise1_data_types(void) {
    printf("\n=== Exercise 1: Data Types ===\n");

    /* Declare variables of different types */
    int integer_var = 42;
    float float_var = 3.14159f;
    double double_var = 2.71828182845;
    char char_var = 'A';

    /* TODO: Fix the format specifiers (replace ??? with correct specifier)
     * HINT: %d for int, %f for float/double, %c for char, %s for string
     */
    printf("Integer: %d\n", integer_var);           /* TODO: Replace ??? */
    printf("Float: %f\n", float_var);               /* TODO: Replace ??? */
    printf("Double: %lf\n", double_var);            /* TODO: Replace ??? */
    printf("Char: %c\n", char_var);                 /* TODO: Replace ??? */

    /* TODO: Declare a string (character array) containing "Hello, C!"
     * and print it using printf */
    char string_var[] = "heyyyy!";  /* TODO: Fix this */
    printf("String: %s\n", string_var);

    /* TODO: Print the size of each type using sizeof()
     * HINT: sizeof() returns size_t, use %zu format specifier */
    printf("\nSizes:\n");
    printf("  sizeof(int) = %zu bytes\n", sizeof(int));       /* TODO: Complete */
    printf("  sizeof(float) = %zu bytes\n", sizeof(float));   /* TODO: Fix this line */
    printf("  sizeof(double) = %zu bytes\n", sizeof(double));  /* TODO: Fix this line */
    printf("  sizeof(char) = %zu bytes\n", sizeof(char));    /* TODO: Fix this line */
}

/* ============================================================================
 * EXERCISE 2: Operators and Expressions [EASY]
 * ============================================================================
 *
 * TODO: Complete the arithmetic and logical operations.
 */
void exercise2_operators(void) {
    printf("\n=== Exercise 2: Operators ===\n");

    int a = 15, b = 4;

    /* TODO: Calculate and print the results of these operations */
    int sum = a + b;        /* TODO: a + b */
    int diff = a - b;       /* TODO: a - b */
    int product = a * b;    /* TODO: a * b */
    int quotient = a / b;   /* TODO: a / b (integer division) */
    int remainder = a % b;  /* TODO: a % b (modulo) */

    printf("a = %d, b = %d\n", a, b);
    printf("Sum: %d\n", sum);
    printf("Difference: %d\n", diff);
    printf("Product: %d\n", product);
    printf("Quotient: %d\n", quotient);
    printf("Remainder: %d\n", remainder);

    /* TODO: Implement these bitwise operations */
    int and_result = a & b;  /* TODO: a & b (bitwise AND) */
    int or_result = a | b;   /* TODO: a | b (bitwise OR) */
    int xor_result = a ^ b;  /* TODO: a ^ b (bitwise XOR) */
    int left_shift = a << 2;  /* TODO: a << 2 (left shift by 2) */
    int right_shift = a >> 2; /* TODO: a >> 2 (right shift by 2) */

    printf("\nBitwise operations:\n");
    printf("a & b = %d\n", and_result);
    printf("a | b = %d\n", or_result);
    printf("a ^ b = %d\n", xor_result);
    printf("a << 2 = %d\n", left_shift);
    printf("a >> 2 = %d\n", right_shift);
}

/* ============================================================================
 * EXERCISE 3: Control Flow [EASY]
 * ============================================================================
 *
 * TODO: Implement control flow structures.
 */

/*
 * TODO: Implement a function that returns the absolute value of an integer.
 * If n is negative, return -n. Otherwise, return n.
 */
int absolute_value(int n) {
    /* TODO: Implement using if/else */
    if (n < 0) {
        return -n;
    }
    else {
        return n;
    }
    return 0;  /* TODO: Fix this */
}

/*
 * TODO: Implement a function that returns the maximum of three integers.
 */
int max_of_three(int a, int b, int c) {
    /* TODO: Implement this function */
    int max = a;
    if (a < b) {
        max = b;
        if (b < c) {
            max = c;
        }
    }
    else if (a < c) {
        max = c;
    }
    return max;  /* TODO: Fix this */
}

/*
 * TODO: Implement a function that categorizes age.
 * Use a switch statement or if/else chain.
 *
 * Returns:
 *   "child" if age < 13
 *   "teenager" if age >= 13 and age < 20
 *   "adult" if age >= 20 and age < 65
 *   "senior" if age >= 65
 */
const char* categorize_age(int age) {
    /* TODO: Implement this function */
    if (age < 13) {
        return "child";
    }

    switch (age / 10) {
        case 1:
            return "teenager";
        case 2:
        case 3:
        case 4:
        case 5:
            return "adult";
        case 6:
            return (age < 65) ? "adult" : "senior";
        default:
            return "senior";
    }
    return "unknown";  /* TODO: Fix this */
}

void exercise3_control_flow(void) {
    printf("\n=== Exercise 3: Control Flow ===\n");

    /* Test absolute_value */
    printf("absolute_value(-5) = %d (expected: 5)\n", absolute_value(-5));
    printf("absolute_value(3) = %d (expected: 3)\n", absolute_value(3));
    printf("absolute_value(0) = %d (expected: 0)\n", absolute_value(0));

    /* Test max_of_three */
    printf("max_of_three(1, 2, 3) = %d (expected: 3)\n", max_of_three(1, 2, 3));
    printf("max_of_three(5, 2, 3) = %d (expected: 5)\n", max_of_three(5, 2, 3));
    printf("max_of_three(1, 7, 3) = %d (expected: 7)\n", max_of_three(1, 7, 3));

    /* Test categorize_age */
    printf("categorize_age(5) = %s (expected: child)\n", categorize_age(5));
    printf("categorize_age(16) = %s (expected: teenager)\n", categorize_age(16));
    printf("categorize_age(30) = %s (expected: adult)\n", categorize_age(30));
    printf("categorize_age(70) = %s (expected: senior)\n", categorize_age(70));
}

/* ============================================================================
 * EXERCISE 4: Loops [EASY]
 * ============================================================================
 *
 * TODO: Implement functions using different loop types.
 */

/*
 * TODO: Calculate the sum of all integers from 1 to n (inclusive).
 * Use a for loop.
 */
int sum_to_n(int n) {
    /* TODO: Implement using a for loop */
    int sum = 0;
    for (;n != 0; n--) {
        sum += n;
    }
    return sum;  /* TODO: Fix this */
}

/*
 * TODO: Calculate factorial of n (n!).
 * factorial(0) = 1, factorial(5) = 120
 * Use a while loop.
 */
long factorial(int n) {
    /* TODO: Implement using a while loop */
    int factorial = 1;
    while( n > 1) {
        factorial *= n;
        n--;
    }
    return factorial;  /* TODO: Fix this */
}

/*
 * TODO: Count the number of digits in a positive integer.
 * count_digits(12345) = 5, count_digits(7) = 1
 * Use a do-while loop.
 */
int count_digits(int n) {
    /* TODO: Implement using a do-while loop */
    if (n < 0) n = -n;
    int count = 0;
    do {
        count++;
        n /= 10;
    } while (n > 0);
    return count;  /* TODO: Fix this */
}

void exercise4_loops(void) {
    printf("\n=== Exercise 4: Loops ===\n");

    printf("sum_to_n(10) = %d (expected: 55)\n", sum_to_n(10));
    printf("sum_to_n(100) = %d (expected: 5050)\n", sum_to_n(100));

    printf("factorial(5) = %ld (expected: 120)\n", factorial(5));
    printf("factorial(10) = %ld (expected: 3628800)\n", factorial(10));

    printf("count_digits(12345) = %d (expected: 5)\n", count_digits(12345));
    printf("count_digits(7) = %d (expected: 1)\n", count_digits(7));
    printf("count_digits(1000000) = %d (expected: 7)\n", count_digits(1000000));
}

/* ============================================================================
 * EXERCISE 5: Arrays [MEDIUM]
 * ============================================================================
 *
 * TODO: Implement array manipulation functions.
 */

/*
 * TODO: Reverse an array in-place.
 * Example: [1, 2, 3, 4, 5] becomes [5, 4, 3, 2, 1]
 */
void reverse_array(int arr[], int size) {
    /* TODO: Implement this function
     * HINT: Swap elements from both ends, moving towards the center
     */
    int left = 0;
    int right = size - 1;
    int temp = 0;
    while (left < right) {
        temp = arr[left];
        arr[left] = arr[right];
        arr[right] = temp;
        left++;
        right--;
    }
}

/*
 * TODO: Find the minimum element in an array.
 * Return the minimum value.
 */
int find_min(int arr[], int size) {
    /* TODO: Implement this function */
    if (size == 0) return 0;
    int min = arr[0];
    for (int i = 1; i < size; i++) {
        if (arr[i] < min) {
            min = arr[i];
        }
    }
    return min;  /* TODO: Fix this */
}

/*
 * TODO: Check if an array is sorted in ascending order.
 * Returns 1 (true) if sorted, 0 (false) otherwise.
 */
int is_sorted(int arr[], int size) {
    /* TODO: Implement this function */
    for (int i = 0; i < size - 1; i++) {
        if (arr[i] > arr[i+1]) {
            return 0;
        }
    }
    return 1;  /* TODO: Fix this */
}

void print_array(int arr[], int size) {
    printf("[");
    for (int i = 0; i < size; i++) {
        printf("%d", arr[i]);
        if (i < size - 1) printf(", ");
    }
    printf("]");
}

void exercise5_arrays(void) {
    printf("\n=== Exercise 5: Arrays ===\n");

    /* Test reverse_array */
    int arr1[] = {1, 2, 3, 4, 5};
    printf("Before reverse: ");
    print_array(arr1, 5);
    reverse_array(arr1, 5);
    printf("\nAfter reverse:  ");
    print_array(arr1, 5);
    printf(" (expected: [5, 4, 3, 2, 1])\n");

    /* Test find_min */
    int arr2[] = {3, 1, 4, 1, 5, 9, 2, 6};
    printf("find_min([3,1,4,1,5,9,2,6]) = %d (expected: 1)\n", find_min(arr2, 8));

    /* Test is_sorted */
    int arr3[] = {1, 2, 3, 4, 5};
    int arr4[] = {1, 3, 2, 4, 5};
    printf("is_sorted([1,2,3,4,5]) = %d (expected: 1)\n", is_sorted(arr3, 5));
    printf("is_sorted([1,3,2,4,5]) = %d (expected: 0)\n", is_sorted(arr4, 5));
}

/* ============================================================================
 * EXERCISE 6: Functions and Scope [MEDIUM]
 * ============================================================================
 *
 * TODO: Understand function parameters and scope.
 */

/*
 * TODO: Implement swap using pointers.
 * This function should swap the values of two integers.
 *
 * HINT: To swap values, you need to use pointers (pass by reference).
 * Without pointers, changes to parameters don't affect the original variables.
 */
void swap(int *a, int *b) {
    /* TODO: Implement swap using pointers
     * HINT: Use a temporary variable
     */
    int temp = *a;
    *a = *b;
    *b = temp;
}

/*
 * TODO: Implement a function that modifies all elements of an array.
 * Multiply each element by the given factor.
 */
void multiply_array(int arr[], int size, int factor) {
    /* TODO: Implement this function */
    for (int i = 0; i < size; i++) {
        arr[i] *= factor;
    }
}

void exercise6_functions(void) {
    printf("\n=== Exercise 6: Functions and Scope ===\n");

    /* Test swap */
    int x = 10, y = 20;
    printf("Before swap: x = %d, y = %d\n", x, y);
    swap(&x, &y);
    printf("After swap:  x = %d, y = %d (expected: x = 20, y = 10)\n", x, y);

    /* Test multiply_array */
    int arr[] = {1, 2, 3, 4, 5};
    printf("Before multiply: ");
    print_array(arr, 5);
    multiply_array(arr, 5, 3);
    printf("\nAfter multiply by 3: ");
    print_array(arr, 5);
    printf(" (expected: [3, 6, 9, 12, 15])\n");
}

/* ============================================================================
 * CHALLENGE: FizzBuzz [EASY]
 * ============================================================================
 *
 * TODO: Implement the classic FizzBuzz problem.
 * Print numbers from 1 to n, but:
 *   - For multiples of 3, print "Fizz"
 *   - For multiples of 5, print "Buzz"
 *   - For multiples of both 3 and 5, print "FizzBuzz"
 */
void fizzbuzz(int n) {
    /* TODO: Implement FizzBuzz */
    printf("\n=== Challenge: FizzBuzz (1 to %d) ===\n", n);

    for (int i = 1; i <= n; i++) {
        /* TODO: Implement the logic here */
        if (i % 3 == 0 && i % 5 == 0) {
            printf("FizzBuzz: %d\n", i);
        }
        else if (i % 3 == 0) {
            printf("Fizz: %d\n", i);
        }
        else if (i % 5 == 0) {
            printf("Buzz: %d\n", i);
        }
        else printf("%d ", i);  /* TODO: Replace with correct output */
    }
    printf("\n");
}

/* ============================================================================
 * MAIN FUNCTION
 * ============================================================================
 */
int main(void) {
    printf("\n");
    printf("================================================\n");
    printf("  Module 02: C Basics & Compilation\n");
    printf("================================================\n");
    printf("\nComplete all TODO items, then compile and run.\n");
    printf("Compare your output with the expected values.\n");

    /* Run all exercises */
    exercise1_data_types();
    exercise2_operators();
    exercise3_control_flow();
    exercise4_loops();
    exercise5_arrays();
    exercise6_functions();

    /* Run challenge */
    fizzbuzz(20);

    printf("\n================================================\n");
    printf("  Exercise Complete!\n");
    printf("  Check your answers against expected values.\n");
    printf("  If all match, proceed to Module 03: Pointers\n");
    printf("================================================\n\n");

    return 0;
}
