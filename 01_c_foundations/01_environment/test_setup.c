/*
CS-6200 Preparation - Environment Setup Verification

This program verifies that your development environment is correctly set up.
If this compiles and runs successfully, you're ready to start the curriculum.

Compile: clang -Wall -Wextra -g test_setup.c -o test_setup
Run:     ./test_setup

Expected output: All checks should pass with green checkmarks or "PASS".
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Test 1: Basic C functionality */
int test_basic_c(void) {
    int a = 10, b = 20;
    int sum = a + b;
    return sum == 30;
}

/* Test 2: Pointer operations */
int test_pointers(void) {
    int x = 42;
    int *ptr = &x;
    return *ptr == 42;
}

/* Test 3: Dynamic memory allocation */
int test_malloc(void) {
    int *arr = malloc(10 * sizeof(int));
    if (arr == NULL) {
        return 0;
    }

    for (int i = 0; i < 10; i++) {
        arr[i] = i * i;
    }

    int result = (arr[5] == 25);
    free(arr);
    return result;
}

/* Test 4: String operations */
int test_strings(void) {
    char str1[] = "Hello";
    char str2[20];
    strcpy(str2, str1);
    strcat(str2, " World");
    return strcmp(str2, "Hello World") == 0;
}

/* Test 5: Struct operations */
typedef struct {
    int x;
    int y;
    char name[32];
} Point;

int test_structs(void) {
    Point p;
    p.x = 10;
    p.y = 20;
    strcpy(p.name, "Origin");

    Point *ptr = malloc(sizeof(Point));
    if (ptr == NULL) return 0;

    ptr->x = 5;
    ptr->y = 5;
    strcpy(ptr->name, "Center");

    int result = (ptr->x == 5 && p.x == 10);
    free(ptr);
    return result;
}

/* Test 6: Size of types (important for systems programming) */
int test_type_sizes(void) {
    printf("\n  Type sizes on your system:\n");
    printf("    char:      %zu bytes\n", sizeof(char));
    printf("    short:     %zu bytes\n", sizeof(short));
    printf("    int:       %zu bytes\n", sizeof(int));
    printf("    long:      %zu bytes\n", sizeof(long));
    printf("    long long: %zu bytes\n", sizeof(long long));
    printf("    float:     %zu bytes\n", sizeof(float));
    printf("    double:    %zu bytes\n", sizeof(double));
    printf("    void*:     %zu bytes\n", sizeof(void*));
    printf("    size_t:    %zu bytes\n", sizeof(size_t));

    /* Check for 64-bit system */
    return sizeof(void*) == 8;
}

/* Print result helper */
void print_result(const char *test_name, int passed) {
    if (passed) {
        printf("  [PASS] %s\n", test_name);
    } else {
        printf("  [FAIL] %s\n", test_name);
    }
}

int main(void) {
    printf("\n");
    printf("================================================\n");
    printf("  CS-6200 Environment Setup Verification\n");
    printf("================================================\n\n");

    int all_passed = 1;

    printf("Running tests...\n\n");

    /* Run all tests */
    int t1 = test_basic_c();
    print_result("Basic C operations", t1);
    all_passed &= t1;

    int t2 = test_pointers();
    print_result("Pointer operations", t2);
    all_passed &= t2;

    int t3 = test_malloc();
    print_result("Dynamic memory (malloc/free)", t3);
    all_passed &= t3;

    int t4 = test_strings();
    print_result("String operations", t4);
    all_passed &= t4;

    int t5 = test_structs();
    print_result("Struct operations", t5);
    all_passed &= t5;

    int t6 = test_type_sizes();
    print_result("64-bit system check", t6);
    all_passed &= t6;

    printf("\n------------------------------------------------\n");

    if (all_passed) {
        printf("\n  SUCCESS! Your environment is ready.\n");
        printf("  You can now proceed to Module 02: C Basics\n\n");
        printf("  Next step:\n");
        printf("    cd ../02_basics\n");
        printf("    # Open exercises.c and start the TODO items\n\n");
    } else {
        printf("\n  Some tests failed. Please check your setup.\n");
        printf("  Refer to SETUP.md for troubleshooting.\n\n");
    }

    printf("================================================\n\n");

    return all_passed ? 0 : 1;
}
