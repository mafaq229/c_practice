/*
Makefile Exercise - Main Program

This is the main file for the Makefile exercise.
You need to write a Makefile to compile this project.
*/

#include <stdio.h>
#include "math_utils.h"
#include "string_utils.h"

int main(void) {
    printf("=== Math Utils ===\n");
    printf("add(5, 3) = %d\n", add(5, 3));
    printf("multiply(4, 7) = %d\n", multiply(4, 7));
    printf("factorial(5) = %d\n", factorial(5));

    printf("\n=== String Utils ===\n");

    char str1[] = "hello";
    to_upper(str1);
    printf("to_upper(\"hello\") = %s\n", str1);

    printf("string_length(\"test\") = %d\n", string_length("test"));

    return 0;
}
