/*
CS-6200 Preparation - Module 08: GDB Debugging

This program has intentional bugs for you to find using a debugger.

Compile: clang -Wall -Wextra -g buggy_program.c -o buggy_program

Debug with LLDB (macOS):
  lldb ./buggy_program
  (lldb) run
  (lldb) thread backtrace

Debug with GDB (via Docker):

docker run -it --rm -v $(pwd):/code -w /code gios-prep \
  sh -c "gcc -Wall -Wextra -g buggy_program.c -o buggy_program && gdb ./buggy_program"

Difficulty: [MEDIUM]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================================
BUG 1: Logic Error
============================================================================

This function should calculate the average of an array.
There's a bug - use the debugger to find it.

Debugging steps:
1. Set a breakpoint at the function start
2. Step through the loop
3. Watch the 'sum' variable
4. Find the bug!
*/
double calculate_average(int *arr, int size) {
    int sum = 0;

    /* BUG: There's an error in this loop */
    for (int i = 0; i <= size; i++) {  /* Off-by-one error! */
        sum += arr[i];
    }

    return (double)sum / size;
}

void test_bug1(void) {
    printf("=== Bug 1: Logic Error ===\n");

    int data[] = {10, 20, 30, 40, 50};
    double avg = calculate_average(data, 5);
    printf("Average: %.2f (expected: 30.00)\n", avg);
}

/* ============================================================================
BUG 2: Uninitialized Variable
============================================================================

This function has an uninitialized variable bug.
Use the debugger to see the garbage value.
*/
int find_max(int *arr, int size) {
    int max;  /* BUG: Uninitialized! */

    for (int i = 0; i < size; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }

    return max;
}

void test_bug2(void) {
    printf("\n=== Bug 2: Uninitialized Variable ===\n");

    int data[] = {5, 2, 8, 1, 9};
    int max = find_max(data, 5);
    printf("Max: %d (expected: 9)\n", max);
}

/* ============================================================================
BUG 3: Pointer Not Checked
============================================================================

This function crashes on NULL input.
Use the debugger to see where it crashes.
*/
int string_length(const char *str) {
    int len = 0;
    /* BUG: No NULL check! */
    while (*str++) {
        len++;
    }
    return len;
}

void test_bug3(void) {
    printf("\n=== Bug 3: NULL Pointer ===\n");

    printf("Length of 'hello': %d\n", string_length("hello"));

    /* This will crash! Uncomment to test with debugger */
    /* printf("Length of NULL: %d\n", string_length(NULL)); */
    printf("(NULL test commented out to prevent crash)\n");
}

/* ============================================================================
BUG 4: Wrong Condition
============================================================================

This function has a logic bug in its condition.
*/
int count_positive(int *arr, int size) {
    int count = 0;

    for (int i = 0; i < size; i++) {
        /* BUG: Wrong comparison operator */
        if (arr[i] > 0) {  /* Should be >= 0 or just > 0 depending on spec */
            count++;
        }
    }

    return count;
}

void test_bug4(void) {
    printf("\n=== Bug 4: Wrong Condition ===\n");

    int data[] = {-5, 0, 3, -2, 7, 0, 1};
    int count = count_positive(data, 7);
    printf("Positive count: %d\n", count);
    printf("(Check: are you counting 0 as positive?)\n");
}

/* ============================================================================
BUG 5: Memory Corruption
============================================================================

This function writes past the end of an array.
*/
void fill_array(int *arr, int size) {
    /* BUG: Loop writes one past the end */
    for (int i = 0; i <= size; i++) {  /* Should be i < size */
        arr[i] = i * 10;
    }
}

void test_bug5(void) {
    printf("\n=== Bug 5: Buffer Overflow ===\n");

    int arr[5] = {0};
    int canary = 9999;

    fill_array(arr, 5);

    printf("Array: [%d, %d, %d, %d, %d]\n",
           arr[0], arr[1], arr[2], arr[3], arr[4]);
    printf("Canary: %d (expected: 9999, might be corrupted)\n", canary);
}

/* ============================================================================
BUG 6: Infinite Loop
============================================================================

This function has an infinite loop bug.
Use the debugger to break into it and see where it's stuck.
*/
int sum_until_zero(int *arr) {
    int sum = 0;
    int i = 0;

    /* BUG: Infinite loop if no zero in array */
    while (arr[i] != 0) {
        sum += arr[i];
        /* i++;  // BUG: i is never incremented! */
    }

    return sum;
}

void test_bug6(void) {
    printf("\n=== Bug 6: Infinite Loop ===\n");

    /* This array has a terminating zero */
    int data[] = {1, 2, 3, 0};

    /* Uncomment to test - this will hang! */
    /* int data_no_zero[] = {1, 2, 3, 4, 5}; */
    /* printf("Sum: %d\n", sum_until_zero(data_no_zero)); */

    /* This version works because we have the bug where i doesn't increment,
     * so it will infinite loop even with a zero present */
    printf("(This test is commented out to prevent hang)\n");
    printf("To debug, uncomment the call and use: (lldb) process interrupt\n");
}

/* ============================================================================
DEBUGGING CHEAT SHEET
============================================================================

LLDB (macOS) Commands:
-----------------------
lldb ./program           - Start debugger
run                      - Run the program
run arg1 arg2            - Run with arguments

breakpoint set -n main   - Break at function 'main'
breakpoint set -f file.c -l 42  - Break at file.c line 42
breakpoint list          - List breakpoints
breakpoint delete 1      - Delete breakpoint 1

next (n)                 - Step over (execute line)
step (s)                 - Step into (enter function)
continue (c)             - Continue execution
finish                   - Run until function returns

print var                - Print variable value
print *ptr               - Print dereferenced pointer
print arr[0]             - Print array element
frame variable           - Print all local variables

thread backtrace (bt)    - Show call stack
frame select 2           - Go to frame 2 in stack

process interrupt        - Break into running program
quit (q)                 - Exit debugger


GDB Commands (similar but slightly different):
-----------------------------------------------
gdb ./program            - Start debugger
run                      - Run the program

break main               - Break at function 'main'
break file.c:42          - Break at line 42
info breakpoints         - List breakpoints
delete 1                 - Delete breakpoint 1

next (n)                 - Step over
step (s)                 - Step into
continue (c)             - Continue
finish                   - Run until return

print var                - Print variable
info locals              - Print all local variables

backtrace (bt)           - Show call stack
frame 2                  - Go to frame 2

Ctrl+C                   - Break into running program
quit (q)                 - Exit debugger
*/

/* ============================================================================
MAIN FUNCTION
============================================================================
*/
int main(void) {
    printf("\n");
    printf("================================================\n");
    printf("  Module 08: GDB/LLDB Debugging\n");
    printf("================================================\n");
    printf("\nThis program has intentional bugs.\n");
    printf("Use the debugger to find and fix them.\n\n");

    test_bug1();
    test_bug2();
    test_bug3();
    test_bug4();
    test_bug5();
    test_bug6();

    printf("\n================================================\n");
    printf("  Debugging exercises - use lldb or gdb!\n");
    printf("================================================\n\n");

    return 0;
}
