/*
test_protocol.c - Unit Tests for Protocol Implementation

Tests the GETFILE protocol parsing and creation functions.

Compile: make test_protocol
Run: ./test_protocol
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/protocol.h"

/* ============================================================================
Test Utilities
============================================================================ */

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) \
    do { \
        printf("  Testing %s... ", #name); \
        tests_run++; \
    } while(0)

#define PASS() \
    do { \
        tests_passed++; \
        printf("PASSED\n"); \
    } while(0)

#define FAIL(msg) \
    do { \
        printf("FAILED: %s\n", msg); \
    } while(0)

#define ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            FAIL(msg); \
            return; \
        } \
    } while(0)

/* ============================================================================
Tests for gf_find_header_end
============================================================================ */

static void test_find_header_end_basic(void) {
    TEST(find_header_end_basic);

    const char *request = "GETFILE GET /test\r\n\r\n";
    size_t result = gf_find_header_end(request, strlen(request));

    ASSERT(result == strlen(request), "Should find header end at correct position");
    PASS();
}

static void test_find_header_end_not_found(void) {
    TEST(find_header_end_not_found);

    const char *partial = "GETFILE GET /test\r\n";  /* Missing final \r\n */
    size_t result = gf_find_header_end(partial, strlen(partial));

    ASSERT(result == 0, "Should return 0 when delimiter not found");
    PASS();
}

static void test_find_header_end_with_content(void) {
    TEST(find_header_end_with_content);

    const char *response = "GETFILE OK 100\r\n\r\nFILE_DATA_HERE";
    size_t result = gf_find_header_end(response, strlen(response));

    ASSERT(result == 18, "Should find header end before content");
    PASS();
}

/* ============================================================================
Tests for gf_create_request
============================================================================ */

static void test_create_request_basic(void) {
    TEST(create_request_basic);

    char buffer[256];
    int len = gf_create_request(buffer, sizeof(buffer), "/test.txt");

    ASSERT(len > 0, "Should return positive length");
    ASSERT(strcmp(buffer, "GETFILE GET /test.txt\r\n\r\n") == 0,
           "Should create correct request format");
    PASS();
}

static void test_create_request_buffer_too_small(void) {
    TEST(create_request_buffer_too_small);

    char buffer[10];  /* Too small */
    int len = gf_create_request(buffer, sizeof(buffer), "/test.txt");

    ASSERT(len == -1, "Should return -1 for insufficient buffer");
    PASS();
}

static void test_create_request_null_inputs(void) {
    TEST(create_request_null_inputs);

    char buffer[256];
    int len;

    len = gf_create_request(NULL, 256, "/test");
    ASSERT(len == -1, "Should return -1 for NULL buffer");

    len = gf_create_request(buffer, sizeof(buffer), NULL);
    ASSERT(len == -1, "Should return -1 for NULL path");

    PASS();
}

/* ============================================================================
Tests for gf_parse_request
============================================================================ */

static void test_parse_request_basic(void) {
    TEST(parse_request_basic);

    const char *request = "GETFILE GET /hello/world.txt\r\n\r\n";
    gf_request_t parsed;

    int consumed = gf_parse_request(request, strlen(request), &parsed);

    ASSERT(consumed > 0, "Should return bytes consumed");
    ASSERT(parsed.valid == 1, "Should be valid request");
    ASSERT(strcmp(parsed.path, "/hello/world.txt") == 0, "Should parse path correctly");
    PASS();
}

static void test_parse_request_incomplete(void) {
    TEST(parse_request_incomplete);

    const char *partial = "GETFILE GET /test\r\n";  /* Missing \r\n */
    gf_request_t parsed;

    int consumed = gf_parse_request(partial, strlen(partial), &parsed);

    ASSERT(consumed == 0, "Should return 0 for incomplete request");
    PASS();
}

static void test_parse_request_invalid_format(void) {
    TEST(parse_request_invalid_format);

    const char *invalid = "BADFORMAT /test\r\n\r\n";
    gf_request_t parsed;

    int result = gf_parse_request(invalid, strlen(invalid), &parsed);

    ASSERT(result == -1 || parsed.valid == 0, "Should detect invalid format");
    PASS();
}

static void test_parse_request_missing_method(void) {
    TEST(parse_request_missing_method);

    const char *invalid = "GETFILE /test\r\n\r\n";  /* Missing GET */
    gf_request_t parsed;

    int result = gf_parse_request(invalid, strlen(invalid), &parsed);

    ASSERT(result == -1 || parsed.valid == 0, "Should detect missing method");
    PASS();
}

/* ============================================================================
Tests for gf_create_response_header
============================================================================ */

static void test_create_response_ok(void) {
    TEST(create_response_ok);

    char buffer[256];
    int len = gf_create_response_header(buffer, sizeof(buffer), STATUS_OK, 12345);

    ASSERT(len > 0, "Should return positive length");
    ASSERT(strcmp(buffer, "GETFILE OK 12345\r\n\r\n") == 0,
           "Should create correct OK response");
    PASS();
}

static void test_create_response_not_found(void) {
    TEST(create_response_not_found);

    char buffer[256];
    int len = gf_create_response_header(buffer, sizeof(buffer),
                                         STATUS_FILE_NOT_FOUND, 0);

    ASSERT(len > 0, "Should return positive length");
    ASSERT(strcmp(buffer, "GETFILE FILE_NOT_FOUND\r\n\r\n") == 0,
           "Should create correct NOT_FOUND response");
    PASS();
}

static void test_create_response_error(void) {
    TEST(create_response_error);

    char buffer[256];
    int len = gf_create_response_header(buffer, sizeof(buffer), STATUS_ERROR, 0);

    ASSERT(len > 0, "Should return positive length");
    ASSERT(strcmp(buffer, "GETFILE ERROR\r\n\r\n") == 0,
           "Should create correct ERROR response");
    PASS();
}

/* ============================================================================
Tests for gf_parse_response_header
============================================================================ */

static void test_parse_response_ok(void) {
    TEST(parse_response_ok);

    const char *response = "GETFILE OK 67890\r\n\r\n";
    gf_response_t parsed;

    int consumed = gf_parse_response_header(response, strlen(response), &parsed);

    ASSERT(consumed > 0, "Should return bytes consumed");
    ASSERT(parsed.header_complete == 1, "Should be complete");
    ASSERT(parsed.status == STATUS_OK, "Should parse OK status");
    ASSERT(parsed.content_length == 67890, "Should parse content length");
    PASS();
}

static void test_parse_response_not_found(void) {
    TEST(parse_response_not_found);

    const char *response = "GETFILE FILE_NOT_FOUND\r\n\r\n";
    gf_response_t parsed;

    int consumed = gf_parse_response_header(response, strlen(response), &parsed);

    ASSERT(consumed > 0, "Should return bytes consumed");
    ASSERT(parsed.status == STATUS_FILE_NOT_FOUND, "Should parse NOT_FOUND status");
    PASS();
}

static void test_parse_response_incomplete(void) {
    TEST(parse_response_incomplete);

    const char *partial = "GETFILE OK 123";  /* Missing \r\n\r\n */
    gf_response_t parsed;

    int consumed = gf_parse_response_header(partial, strlen(partial), &parsed);

    ASSERT(consumed == 0, "Should return 0 for incomplete response");
    PASS();
}

/* ============================================================================
Tests for gf_status_to_string / gf_string_to_status
============================================================================ */

static void test_status_conversion(void) {
    TEST(status_conversion);

    ASSERT(strcmp(gf_status_to_string(STATUS_OK), "OK") == 0, "OK string");
    ASSERT(strcmp(gf_status_to_string(STATUS_FILE_NOT_FOUND), "FILE_NOT_FOUND") == 0,
           "NOT_FOUND string");
    ASSERT(strcmp(gf_status_to_string(STATUS_ERROR), "ERROR") == 0, "ERROR string");

    ASSERT(gf_string_to_status("OK") == STATUS_OK, "String to OK");
    ASSERT(gf_string_to_status("FILE_NOT_FOUND") == STATUS_FILE_NOT_FOUND,
           "String to NOT_FOUND");
    ASSERT(gf_string_to_status("UNKNOWN") == STATUS_INVALID, "Unknown string");

    PASS();
}

/* ============================================================================
Main
============================================================================ */

int main(void) {
    printf("=== Protocol Tests ===\n\n");

    printf("Testing gf_find_header_end:\n");
    test_find_header_end_basic();
    test_find_header_end_not_found();
    test_find_header_end_with_content();

    printf("\nTesting gf_create_request:\n");
    test_create_request_basic();
    test_create_request_buffer_too_small();
    test_create_request_null_inputs();

    printf("\nTesting gf_parse_request:\n");
    test_parse_request_basic();
    test_parse_request_incomplete();
    test_parse_request_invalid_format();
    test_parse_request_missing_method();

    printf("\nTesting gf_create_response_header:\n");
    test_create_response_ok();
    test_create_response_not_found();
    test_create_response_error();

    printf("\nTesting gf_parse_response_header:\n");
    test_parse_response_ok();
    test_parse_response_not_found();
    test_parse_response_incomplete();

    printf("\nTesting status conversions:\n");
    test_status_conversion();

    printf("\n=== Results ===\n");
    printf("%d/%d tests passed\n", tests_passed, tests_run);

    return (tests_passed == tests_run) ? 0 : 1;
}
