/*
 * protocol.c - GETFILE Protocol Implementation
 *
 * This file implements the GETFILE protocol parsing and creation functions.
 * The protocol is text-based with a simple format, but parsing must be
 * robust to handle partial data, malformed requests, and edge cases.
 *
 * This is similar to what you'll implement in GIOS Project 1!
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/protocol.h"

/* ============================================================================
 * TODO: Implement these functions
 *
 * Study the protocol format carefully:
 *
 * Request:  GETFILE GET /path/to/file\r\n\r\n
 * Response: GETFILE OK 12345\r\n\r\n<file_content>
 *           GETFILE FILE_NOT_FOUND\r\n\r\n
 *           GETFILE ERROR\r\n\r\n
 *           GETFILE INVALID\r\n\r\n
 *
 * Key challenges:
 * 1. Handling partial headers (network may split data)
 * 2. Validating format strictly
 * 3. Extracting path and content length correctly
 * ============================================================================ */

/*
 * gf_find_header_end - Find the end of header delimiter
 *
 * Searches for "\r\n\r\n" in the buffer.
 * Returns position AFTER the delimiter, or 0 if not found.
 */
size_t gf_find_header_end(const char *buffer, size_t buflen) {
    /*
     * TODO: Implement this function
     *
     * Hints:
     * - Use memmem() if available (GNU extension), or implement manually
     * - Search for HEADER_DELIM ("\r\n\r\n")
     * - Return position after delimiter (offset + HEADER_DELIM_LEN)
     * - Return 0 if delimiter not found
     *
     * Example:
     *   buffer = "GETFILE GET /test\r\n\r\n"
     *   returns 21 (position after \r\n\r\n)
     */

    (void)buffer;  /* Remove when implemented */
    (void)buflen;

    /* YOUR CODE HERE */

    return 0;  /* Placeholder */
}

/*
 * gf_status_to_string - Convert status code to string
 */
const char *gf_status_to_string(gf_status_t status) {
    /*
     * TODO: Implement this function
     *
     * Return the string representation of the status code.
     * Use the STATUS_STRINGS array defined in protocol.h.
     */

    (void)status;

    /* YOUR CODE HERE */

    return "UNKNOWN";  /* Placeholder */
}

/*
 * gf_string_to_status - Convert string to status code
 */
gf_status_t gf_string_to_status(const char *str) {
    /*
     * TODO: Implement this function
     *
     * Compare str with each entry in STATUS_STRINGS.
     * Return STATUS_INVALID if no match.
     *
     * Use strcmp() for comparison.
     */

    (void)str;

    /* YOUR CODE HERE */

    return STATUS_INVALID;  /* Placeholder */
}

/*
 * gf_create_request - Build a GETFILE request string
 *
 * Format: "GETFILE GET /path\r\n\r\n"
 */
int gf_create_request(char *buffer, size_t buflen, const char *path) {
    /*
     * TODO: Implement this function
     *
     * Steps:
     * 1. Validate inputs (buffer, path not NULL, buflen > 0)
     * 2. Check path starts with '/'
     * 3. Use snprintf to build: "GETFILE GET %s\r\n\r\n"
     * 4. Check for buffer overflow (snprintf return value)
     * 5. Return number of bytes written (not including null terminator)
     *
     * Example:
     *   path = "/test.txt"
     *   buffer = "GETFILE GET /test.txt\r\n\r\n"
     *   returns 25
     */

    if (buffer == NULL || path == NULL || buflen == 0) {
        return -1;
    }

    /* YOUR CODE HERE */

    return -1;  /* Placeholder */
}

/*
 * gf_parse_request - Parse a GETFILE request
 *
 * Expected format: "GETFILE GET /path\r\n\r\n"
 */
int gf_parse_request(const char *buffer, size_t buflen, gf_request_t *request) {
    /*
     * TODO: Implement this function
     *
     * This is the most complex function - parse carefully!
     *
     * Steps:
     * 1. Initialize request->valid = 0
     * 2. Find header end (\r\n\r\n) - if not found, return 0 (incomplete)
     * 3. Verify "GETFILE " prefix (must match exactly, with space)
     * 4. Verify "GET " method (must match exactly, with space)
     * 5. Extract path (everything after "GET " until "\r\n")
     * 6. Validate path (starts with '/', reasonable length)
     * 7. Copy path to request->path
     * 8. Set request->valid = 1
     * 9. Return number of bytes consumed (header_end position)
     *
     * Error handling:
     * - If header incomplete: return 0
     * - If format invalid: set request->valid = 0, return -1
     *
     * Hints:
     * - Use strncmp() for prefix matching
     * - Use memcpy() to copy path (don't include \r\n)
     * - Null-terminate the path!
     *
     * Example:
     *   buffer = "GETFILE GET /test.txt\r\n\r\n"
     *   request->path = "/test.txt"
     *   request->valid = 1
     *   returns 25
     */

    if (buffer == NULL || request == NULL) {
        return -1;
    }

    request->valid = 0;
    request->path[0] = '\0';
    request->path_len = 0;

    /* YOUR CODE HERE */

    return 0;  /* Placeholder - incomplete */
}

/*
 * gf_create_response_header - Build a GETFILE response header
 *
 * Format: "GETFILE OK 12345\r\n\r\n" or "GETFILE FILE_NOT_FOUND\r\n\r\n"
 */
int gf_create_response_header(char *buffer, size_t buflen,
                               gf_status_t status, size_t content_length) {
    /*
     * TODO: Implement this function
     *
     * Steps:
     * 1. Validate inputs
     * 2. If status is OK or CACHED, include content_length:
     *    "GETFILE OK 12345\r\n\r\n"
     * 3. Otherwise, just status:
     *    "GETFILE FILE_NOT_FOUND\r\n\r\n"
     * 4. Check for buffer overflow
     * 5. Return bytes written
     *
     * Use gf_status_to_string() to get the status string.
     */

    if (buffer == NULL || buflen == 0) {
        return -1;
    }

    /* YOUR CODE HERE */

    return -1;  /* Placeholder */
}

/*
 * gf_parse_response_header - Parse a GETFILE response header
 *
 * Handles: "GETFILE OK 12345\r\n\r\n" and "GETFILE FILE_NOT_FOUND\r\n\r\n"
 */
int gf_parse_response_header(const char *buffer, size_t buflen,
                              gf_response_t *response) {
    /*
     * TODO: Implement this function
     *
     * Steps:
     * 1. Initialize response->header_complete = 0
     * 2. Find header end - if not found, return 0 (incomplete)
     * 3. Verify "GETFILE " prefix
     * 4. Extract status string (next word)
     * 5. Convert to status code using gf_string_to_status()
     * 6. If status is OK/CACHED, extract content_length (number after status)
     * 7. Set response->header_complete = 1
     * 8. Return header size (bytes consumed)
     *
     * Parsing the content length:
     * - For "GETFILE OK 12345\r\n\r\n"
     * - Skip "GETFILE OK "
     * - Read number until "\r\n"
     * - Use strtoul() or atol()
     *
     * Hints:
     * - Be careful with string manipulation
     * - Handle missing content_length gracefully
     */

    if (buffer == NULL || response == NULL) {
        return -1;
    }

    response->header_complete = 0;
    response->status = STATUS_INVALID;
    response->content_length = 0;

    /* YOUR CODE HERE */

    return 0;  /* Placeholder - incomplete */
}

/* ============================================================================
 * Testing helper (optional)
 * ============================================================================ */

#ifdef PROTOCOL_TEST_MAIN
/*
 * Simple test program for protocol functions.
 * Compile: gcc -DPROTOCOL_TEST_MAIN -g protocol.c -o test_protocol_main
 */
int main(void) {
    char buffer[256];
    gf_request_t request;
    gf_response_t response;
    int n;

    printf("Testing protocol functions...\n\n");

    /* Test request creation */
    printf("=== Testing gf_create_request ===\n");
    n = gf_create_request(buffer, sizeof(buffer), "/test.txt");
    if (n > 0) {
        printf("Request: [%s] (%d bytes)\n", buffer, n);
    } else {
        printf("FAILED to create request\n");
    }

    /* Test request parsing */
    printf("\n=== Testing gf_parse_request ===\n");
    strcpy(buffer, "GETFILE GET /hello/world.txt\r\n\r\n");
    n = gf_parse_request(buffer, strlen(buffer), &request);
    if (n > 0 && request.valid) {
        printf("Parsed path: [%s] (%zu bytes consumed)\n", request.path, (size_t)n);
    } else {
        printf("FAILED to parse request (n=%d, valid=%d)\n", n, request.valid);
    }

    /* Test response header creation */
    printf("\n=== Testing gf_create_response_header ===\n");
    n = gf_create_response_header(buffer, sizeof(buffer), STATUS_OK, 12345);
    if (n > 0) {
        printf("Response header: [%s] (%d bytes)\n", buffer, n);
    } else {
        printf("FAILED to create response header\n");
    }

    /* Test response parsing */
    printf("\n=== Testing gf_parse_response_header ===\n");
    strcpy(buffer, "GETFILE OK 67890\r\n\r\n");
    n = gf_parse_response_header(buffer, strlen(buffer), &response);
    if (n > 0 && response.header_complete) {
        printf("Parsed: status=%s, length=%zu (%d bytes consumed)\n",
               gf_status_to_string(response.status), response.content_length, n);
    } else {
        printf("FAILED to parse response (n=%d, complete=%d)\n",
               n, response.header_complete);
    }

    printf("\nDone!\n");
    return 0;
}
#endif /* PROTOCOL_TEST_MAIN */
