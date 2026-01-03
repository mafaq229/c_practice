/*
CS-6200 Preparation - Module 05: String Utilities

String handling in C is a common source of bugs. GIOS Project 1 requires
extensive string parsing for protocol headers.

Compile: clang -Wall -Wextra -g string_utils.c -o string_utils
Run:     ./string_utils

Topics covered:
  - C strings (null-terminated char arrays)
  - String library functions
  - Safe string handling
  - Common string pitfalls

Difficulty: [EASY] to [MEDIUM]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

/* ============================================================================
CONCEPT: C Strings
============================================================================

In C, strings are arrays of characters ending with '\0' (null terminator).

  char str[] = "Hello";
  // Actually: ['H', 'e', 'l', 'l', 'o', '\0']
  // Size is 6, but strlen is 5

CRITICAL: Always account for the null terminator!
*/

void demonstrate_c_strings(void) {
    printf("\n=== C String Basics ===\n");

    /* Different ways to declare strings */
    char arr1[] = "Hello";           /* Array, size determined by initializer */
    char arr2[10] = "Hello";         /* Array with explicit size */
    char *ptr = "Hello";             /* Pointer to string literal (READ-ONLY!) */

    printf("arr1: '%s', sizeof=%zu, strlen=%zu\n",
           arr1, sizeof(arr1), strlen(arr1));
    printf("arr2: '%s', sizeof=%zu, strlen=%zu\n",
           arr2, sizeof(arr2), strlen(arr2));
    printf("ptr:  '%s', sizeof=%zu, strlen=%zu\n",
           ptr, sizeof(ptr), strlen(ptr));

    /* Can modify array strings */
    arr1[0] = 'J';  /* OK - now "Jello" */
    printf("Modified arr1: '%s'\n", arr1);

    /* NEVER modify string literals! */
    /* ptr[0] = 'J';  // CRASH or undefined behavior! */
}

/* ============================================================================
EXERCISE 1: String Length [EASY]
============================================================================

TODO: Implement strlen from scratch.
*/
size_t my_strlen(const char *str) {
    /* TODO: Implement this function
     *
     * Count characters until you hit '\0'.
     * Don't count the '\0' itself.
     */
    size_t count = 0;
    while (str[count] != '\0') { // always use '' (char literal) instead of "" (string literal)
        count++;
    }
    return count;  /* TODO: Fix this */
}

void exercise1_strlen(void) {
    printf("\n=== Exercise 1: strlen ===\n");

    printf("my_strlen(\"Hello\") = %zu (expected: 5)\n", my_strlen("Hello"));
    printf("my_strlen(\"\") = %zu (expected: 0)\n", my_strlen(""));
    printf("my_strlen(\"A\") = %zu (expected: 1)\n", my_strlen("A"));
}

/* ============================================================================
EXERCISE 2: String Copy [MEDIUM]
============================================================================

TODO: Implement safe string copy.
*/

/*
UNSAFE version (like strcpy) - don't use this!
*/
char *unsafe_strcpy(char *dest, const char *src) {
    char *original = dest;
    while ((*dest++ = *src++) != '\0'); // assigns the character from src into dest, then moves both pointers forward by 1.
    return original;
}

/*
TODO: Implement safe string copy with size limit.
Copies at most n-1 characters and always null-terminates.
Returns number of characters that would have been copied (like snprintf).
*/
size_t safe_strcpy(char *dest, const char *src, size_t dest_size) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. If dest_size is 0, just return strlen(src)
     * 2. Copy up to dest_size-1 characters
     * 3. Always add null terminator
     * 4. Return total length of src (even if truncated)
     */
    if (dest_size == 0) return strlen(src);
    size_t i = 0;
    while (i + 1 < dest_size && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
    return strlen(src);
}

void exercise2_strcpy(void) {
    printf("\n=== Exercise 2: Safe String Copy ===\n");

    char small_buf[5];
    char big_buf[100];

    /* Copy that fits */
    size_t len = safe_strcpy(big_buf, "Hello", sizeof(big_buf));
    printf("safe_strcpy to big buffer: '%s' (len=%zu)\n", big_buf, len);

    /* Copy that needs truncation */
    len = safe_strcpy(small_buf, "Hello, World!", sizeof(small_buf));
    printf("safe_strcpy to small buffer: '%s' (len=%zu, needed=%zu)\n",
           small_buf, strlen(small_buf), len);
    printf("Expected: 'Hell' (truncated but safe!)\n");
}

/* ============================================================================
EXERCISE 3: String Comparison [EASY]
============================================================================

TODO: Implement strcmp from scratch.
*/
int my_strcmp(const char *s1, const char *s2) {
    /* TODO: Implement this function
     *
     * Compare character by character.
     * Return:
     *   < 0 if s1 < s2
     *   0 if s1 == s2
     *   > 0 if s1 > s2
     */
    while(*s1 != '\0' && *s2 != '\0' && *s1 == *s2) {
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

/*
TODO: Implement case-insensitive comparison.
*/
int my_strcasecmp(const char *s1, const char *s2) {
    /* TODO: Implement this function
     *
     * HINT: Use tolower() from <ctype.h>
     */
    while (*s1 != '\0' && *s2 != '\0') {
        unsigned char c1 = (unsigned char)*s1;
        unsigned char c2 = (unsigned char)*s2;
        int diff = tolower(c1) - tolower(c2);
        if (diff != 0) return diff;
        s1++;
        s2++;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

void exercise3_strcmp(void) {
    printf("\n=== Exercise 3: String Comparison ===\n");

    printf("my_strcmp(\"abc\", \"abc\") = %d (expected: 0)\n",
           my_strcmp("abc", "abc"));
    printf("my_strcmp(\"abc\", \"abd\") = %d (expected: <0)\n",
           my_strcmp("abc", "abd"));
    printf("my_strcmp(\"abd\", \"abc\") = %d (expected: >0)\n",
           my_strcmp("abd", "abc"));

    printf("my_strcasecmp(\"Hello\", \"HELLO\") = %d (expected: 0)\n",
           my_strcasecmp("Hello", "HELLO"));
    printf("my_strcasecmp(\"Hello\", \"World\") = %d (expected: !=0)\n",
           my_strcasecmp("Hello", "World"));
}

/* ============================================================================
EXERCISE 4: String Concatenation [MEDIUM]
============================================================================

TODO: Implement safe string concatenation.
*/
size_t safe_strcat(char *dest, const char *src, size_t dest_size) {
    /* TODO: Implement this function
     *
     * Append src to dest, ensuring null-termination.
     * dest_size is the total size of the dest buffer.
     *
     * Steps:
     * 1. Find end of dest string
     * 2. Calculate remaining space
     * 3. Copy as much of src as fits
     * 4. Return total length (dest + src)
     */
    size_t dest_len = strlen(dest);
    size_t src_len = strlen(src);

    if (dest_len >= dest_size) {
        return dest_len + src_len;
    }

    size_t i = 0;
    size_t space = dest_size - dest_len - 1; // -1 since last value is '\0' null terminator
    while (i < space && src[i] != '\0') {
        dest[dest_len + i] = src[i];
        i++;
    }
    dest[dest_len + i] = '\0';
    return dest_len + src_len;
}

void exercise4_strcat(void) {
    printf("\n=== Exercise 4: Safe String Concatenation ===\n");

    char buf[20] = "Hello";

    safe_strcat(buf, ", ", sizeof(buf));
    safe_strcat(buf, "World!", sizeof(buf));

    printf("Result: '%s'\n", buf);
    printf("Expected: 'Hello, World!'\n");

    /* Test truncation */
    char small[10] = "Hi";
    size_t needed = safe_strcat(small, " there, friend!", sizeof(small));
    printf("Truncated: '%s' (needed %zu chars)\n", small, needed);
}

/* ============================================================================
EXERCISE 5: String Search [MEDIUM]
============================================================================

TODO: Implement strstr (find substring).
*/
char *my_strstr(const char *haystack, const char *needle) {
    /* TODO: Implement this function
     *
     * Find first occurrence of needle in haystack.
     * Return pointer to start of match, or NULL if not found.
     *
     * HINT: For each position in haystack, check if needle matches starting there.
     */
    // Empty needle matches at the start of haystack
    if (*needle == '\0') return (char *)haystack;

    for (const char *h = haystack; *h != '\0'; h++) {
        /* Try to match needle starting at this position in haystack. */
        const char *h_it = h;
        const char *n_it = needle;
        while (*h_it != '\0' && *n_it != '\0' && *h_it == *n_it) {
            h_it++;
            n_it++;
        }
        /* If we reached the end of needle, we found a full match. */
        if (*n_it == '\0') {
            return (char *)h;
        }
    }

    return NULL;
}

void exercise5_strstr(void) {
    printf("\n=== Exercise 5: String Search ===\n");

    const char *text = "Hello, World!";

    char *result = my_strstr(text, "World");
    printf("my_strstr(\"%s\", \"World\") = \"%s\"\n",
           text, result ? result : "(null)");
    printf("Expected: \"World!\"\n");

    result = my_strstr(text, "xyz");
    printf("my_strstr(\"%s\", \"xyz\") = %s\n",
           text, result ? result : "(null)");
    printf("Expected: (null)\n");
}

/* ============================================================================
EXERCISE 6: Tokenization [MEDIUM]
============================================================================

strtok is tricky! Understand how it works.
*/
void exercise6_strtok(void) {
    printf("\n=== Exercise 6: Tokenization (strtok) ===\n");

    /* WARNING: strtok modifies the original string! */
    char str[] = "apple,banana,cherry,date";
    printf("Original: '%s'\n", str);

    char *token = strtok(str, ",");
    printf("Tokens: ");
    while (token != NULL) {
        printf("'%s' ", token);
        token = strtok(NULL, ",");  /* NULL continues with same string */
    }
    printf("\n");

    /* After strtok, original string is modified */
    printf("After strtok, str = '%s' (only first token visible!)\n", str);

    /* TODO: Use strtok_r for thread-safe tokenization */
    char str2[] = "one:two:three:four";
    char *saveptr;
    char *tok = strtok_r(str2, ":", &saveptr);
    printf("\nUsing strtok_r: ");
    while (tok != NULL) {
        printf("'%s' ", tok);
        tok = strtok_r(NULL, ":", &saveptr);
    }
    printf("\n");
}

/* ============================================================================
EXERCISE 7: String to Number Conversion [MEDIUM]
============================================================================
*/
void exercise7_conversion(void) {
    printf("\n=== Exercise 7: String to Number ===\n");

    /* atoi - simple but no error checking */
    int i1 = atoi("123");
    int i2 = atoi("abc");  /* Returns 0 on failure */
    int i3 = atoi("123abc");  /* Returns 123 (stops at non-digit) */

    printf("atoi(\"123\") = %d\n", i1);
    printf("atoi(\"abc\") = %d (no error indication!)\n", i2);
    printf("atoi(\"123abc\") = %d\n", i3);

    /* strtol - better, with error checking: 10 for decimal, 16 for hexadecimal */
    char *endptr;
    long l1 = strtol("123", &endptr, 10);
    long l2 = strtol("abc", &endptr, 10);
    long l3 = strtol("0xFF", &endptr, 16);  /* Hex */

    printf("\nstrtol(\"123\", 10) = %ld\n", l1);
    printf("strtol(\"abc\", 10) = %ld (endptr points to 'a')\n", l2);
    printf("strtol(\"0xFF\", 16) = %ld (hex)\n", l3);

    /* TODO: Write a function that safely parses an integer */
    /* It should return -1 on error and set *result on success */
}

/*
TODO: Implement safe integer parsing.
Returns 0 on success, -1 on failure.
*/
int safe_parse_int(const char *str, int *result) {
    /* TODO: Implement this function
     *
     * Use strtol with error checking:
     * 1. Check if str is NULL or empty
     * 2. Call strtol
     * 3. Check if endptr points to end of string
     * 4. Check for overflow
     */
    if (str == NULL || *str == '\0' || result == NULL) return -1;

    errno = 0;
    char *endptr = NULL;
    /* Convert string to long (base 10); endptr points to first non-digit. */
    long value = strtol(str, &endptr, 10);

    if (endptr == str || *endptr != '\0') return -1;
    if (errno == ERANGE || value < INT_MIN || value > INT_MAX) return -1;

    *result = (int)value;
    return 0;
}

/* ============================================================================
COMMON STRING PITFALLS
============================================================================
*/
void common_pitfalls(void) {
    printf("\n=== Common String Pitfalls ===\n");

    printf("1. Buffer overflow: Always use strncpy, snprintf\n");
    printf("2. Missing null terminator: strncpy doesn't always add it!\n");
    printf("3. sizeof vs strlen: sizeof includes '\\0', strlen doesn't\n");
    printf("4. Modifying string literals: char *s = \"hello\"; s[0] = 'H'; // CRASH!\n");
    printf("5. strtok modifies string: Make a copy first if needed\n");
    printf("6. Comparing strings: Use strcmp, not ==\n");

    /* Example: Wrong way to compare strings */
    char *a = "hello";
    char b[] = "hello";
    if (a == b) {
        printf("\nStrings are equal (WRONG! Comparing addresses)\n");
    } else {
        printf("\na == b is false (comparing addresses, not content)\n");
    }
    if (strcmp(a, b) == 0) {
        printf("strcmp(a, b) == 0 is true (correct way!)\n");
    }
}

/* ============================================================================
MAIN FUNCTION
============================================================================
*/
int main(void) {
    printf("\n");
    printf("================================================\n");
    printf("  Module 05: String Utilities\n");
    printf("================================================\n");

    demonstrate_c_strings();
    exercise1_strlen();
    exercise2_strcpy();
    exercise3_strcmp();
    exercise4_strcat();
    exercise5_strstr();
    exercise6_strtok();
    exercise7_conversion();
    common_pitfalls();

    printf("\n================================================\n");
    printf("  String Utilities Complete!\n");
    printf("  Next: string_parsing.c\n");
    printf("================================================\n\n");

    return 0;
}
