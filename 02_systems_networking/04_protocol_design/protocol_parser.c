/*
CS-6200 Preparation - Module 04: Protocol Parser

This module teaches you to design and parse network protocols.
GIOS Project 1 requires parsing "GETFILE GET /path\r\n\r\n" style requests.

Compile: clang -Wall -Wextra -g protocol_parser.c -o protocol_parser
Run:     ./protocol_parser

Difficulty: [HARD]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ============================================================================
PROTOCOL SPECIFICATION
============================================================================

Request format:
  <METHOD> <PATH> <VERSION>\r\n
  <HEADER-NAME>: <HEADER-VALUE>\r\n
  ...
  \r\n
  [BODY]

Example:
  GETFILE GET /path/to/file\r\n
  Content-Length: 1234\r\n
  \r\n

Response format:
  <SCHEME> <STATUS> <MESSAGE>\r\n
  <HEADER-NAME>: <HEADER-VALUE>\r\n
  ...
  \r\n
  [BODY]

Example:
  GETFILE OK\r\n
  Content-Length: 1234\r\n
  \r\n
  <file data>
*/

#define MAX_PATH_LEN 256
#define MAX_HEADERS 16
#define MAX_HEADER_NAME 64
#define MAX_HEADER_VALUE 256

/* Request structure */
typedef struct {
    char scheme[16];      /* e.g., "GETFILE" */
    char method[16];      /* e.g., "GET" */
    char path[MAX_PATH_LEN];
    int header_count;
    struct {
        char name[MAX_HEADER_NAME];
        char value[MAX_HEADER_VALUE];
    } headers[MAX_HEADERS];
    long content_length;  /* Parsed from Content-Length header */
} Request;

/* Response structure */
typedef struct {
    char scheme[16];
    char status[16];      /* e.g., "OK", "ERROR", "FILE_NOT_FOUND" */
    int header_count;
    struct {
        char name[MAX_HEADER_NAME];
        char value[MAX_HEADER_VALUE];
    } headers[MAX_HEADERS];
    long content_length;
} Response;

/* ============================================================================
EXERCISE 1: Parse Request Line
============================================================================
*/

/*
TODO: Parse the first line of a request.
Format: "SCHEME METHOD PATH\r\n" or "SCHEME METHOD PATH"

Returns 0 on success, -1 on parse error.
*/
int parse_request_line(const char *line, Request *req) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Skip leading whitespace
     * 2. Read scheme (up to space)
     * 3. Read method (up to space)
     * 4. Read path (up to \r or \n or end)
     * 5. Validate that all fields were found
     *
     * HINT: Use sscanf or manual parsing
     */

    /* Initialize */
    memset(req, 0, sizeof(Request));

    /* Simple implementation using sscanf */
    int matched = sscanf(line, "%15s %15s %255s",
                         req->scheme, req->method, req->path);

    if (matched < 3) {
        fprintf(stderr, "Parse error: expected 3 fields, got %d\n", matched);
        return -1;
    }

    /* Remove trailing \r\n from path if present */
    char *cr = strchr(req->path, '\r');
    if (cr) *cr = '\0';
    char *nl = strchr(req->path, '\n');
    if (nl) *nl = '\0';

    return 0;
}

void test_parse_request_line(void) {
    printf("\n=== Exercise 1: Parse Request Line ===\n");

    const char *tests[] = {
        "GETFILE GET /path/to/file\r\n",
        "GETFILE GET /simple",
        "HTTP/1.1 GET /index.html",
        "INVALID",  /* Should fail */
    };

    for (int i = 0; i < 4; i++) {
        Request req;
        printf("\nInput: '%s'\n", tests[i]);

        if (parse_request_line(tests[i], &req) == 0) {
            printf("  Scheme: '%s'\n", req.scheme);
            printf("  Method: '%s'\n", req.method);
            printf("  Path: '%s'\n", req.path);
        } else {
            printf("  Parse failed (expected for invalid input)\n");
        }
    }
}

/* ============================================================================
EXERCISE 2: Parse Headers
============================================================================
*/

/*
TODO: Parse a single header line.
Format: "Header-Name: Header-Value\r\n"

Returns 0 on success, -1 on parse error, 1 if empty line (end of headers).
*/
int parse_header_line(const char *line, char *name, size_t name_size,
                      char *value, size_t value_size) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Check for empty line (\r\n or just \n) - return 1
     * 2. Find the colon
     * 3. Copy name (before colon)
     * 4. Skip colon and whitespace
     * 5. Copy value (rest of line, excluding \r\n)
     * 6. Trim whitespace from both ends
     */

    /* Check for empty line (end of headers) */
    if (line[0] == '\r' || line[0] == '\n' || line[0] == '\0') {
        return 1;  /* End of headers */
    }

    /* Find colon */
    const char *colon = strchr(line, ':');
    if (colon == NULL) {
        return -1;  /* No colon found */
    }

    /* Copy name */
    size_t name_len = colon - line;
    if (name_len >= name_size) {
        name_len = name_size - 1;
    }
    strncpy(name, line, name_len);
    name[name_len] = '\0';

    /* Skip colon and whitespace */
    const char *val_start = colon + 1;
    while (*val_start == ' ' || *val_start == '\t') {
        val_start++;
    }

    /* Copy value (excluding trailing \r\n) */
    size_t val_len = strlen(val_start);
    while (val_len > 0 && (val_start[val_len-1] == '\r' || val_start[val_len-1] == '\n')) {
        val_len--;
    }
    if (val_len >= value_size) {
        val_len = value_size - 1;
    }
    strncpy(value, val_start, val_len);
    value[val_len] = '\0';

    return 0;
}

/*
TODO: Parse all headers from a multi-line string.
*/
int parse_headers(const char *data, Request *req) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Split by lines (\r\n or \n)
     * 2. Parse each line with parse_header_line
     * 3. Stop when empty line is found
     * 4. Store headers in req->headers
     * 5. Update req->header_count
     */

    req->header_count = 0;
    req->content_length = 0;

    const char *line_start = data;
    const char *line_end;

    while ((line_end = strstr(line_start, "\n")) != NULL || *line_start != '\0') {
        /* Calculate line length */
        size_t line_len;
        if (line_end) {
            line_len = line_end - line_start + 1;
        } else {
            line_len = strlen(line_start);
            line_end = line_start + line_len;
        }

        /* Copy line to buffer */
        char line[512];
        if (line_len >= sizeof(line)) {
            line_len = sizeof(line) - 1;
        }
        strncpy(line, line_start, line_len);
        line[line_len] = '\0';

        /* Parse the line */
        char name[MAX_HEADER_NAME];
        char value[MAX_HEADER_VALUE];
        int result = parse_header_line(line, name, sizeof(name), value, sizeof(value));

        if (result == 1) {
            /* Empty line - end of headers */
            break;
        } else if (result == 0) {
            /* Valid header */
            if (req->header_count < MAX_HEADERS) {
                strcpy(req->headers[req->header_count].name, name);
                strcpy(req->headers[req->header_count].value, value);
                req->header_count++;

                /* Check for Content-Length */
                if (strcasecmp(name, "Content-Length") == 0) {
                    req->content_length = atol(value);
                }
            }
        }

        line_start = line_end + 1;
        if (*line_start == '\0') break;
    }

    return 0;
}

void test_parse_headers(void) {
    printf("\n=== Exercise 2: Parse Headers ===\n");

    const char *headers =
        "Content-Length: 12345\r\n"
        "Content-Type: application/octet-stream\r\n"
        "X-Custom-Header: some value with spaces\r\n"
        "\r\n";

    Request req = {0};
    parse_headers(headers, &req);

    printf("\nParsed %d headers:\n", req.header_count);
    for (int i = 0; i < req.header_count; i++) {
        printf("  '%s' = '%s'\n", req.headers[i].name, req.headers[i].value);
    }
    printf("Content-Length: %ld\n", req.content_length);
}

/* ============================================================================
EXERCISE 3: Parse Complete Request
============================================================================
*/

/*
TODO: Parse a complete request (request line + headers).
*/
int parse_request(const char *data, Request *req) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Find end of first line (\r\n)
     * 2. Parse request line
     * 3. Parse headers starting from after first line
     */

    /* Find end of first line */
    const char *first_line_end = strstr(data, "\r\n");
    if (first_line_end == NULL) {
        first_line_end = strstr(data, "\n");
        if (first_line_end == NULL) {
            return -1;  /* No line ending found */
        }
    }

    /* Copy first line */
    size_t first_line_len = first_line_end - data;
    char first_line[512];
    if (first_line_len >= sizeof(first_line)) {
        first_line_len = sizeof(first_line) - 1;
    }
    strncpy(first_line, data, first_line_len);
    first_line[first_line_len] = '\0';

    /* Parse request line */
    if (parse_request_line(first_line, req) != 0) {
        return -1;
    }

    /* Find start of headers */
    const char *headers_start = first_line_end + 1;
    if (*first_line_end == '\r' && *(first_line_end + 1) == '\n') {
        headers_start = first_line_end + 2;
    }

    /* Parse headers */
    return parse_headers(headers_start, req);
}

void test_parse_complete_request(void) {
    printf("\n=== Exercise 3: Parse Complete Request ===\n");

    const char *request =
        "GETFILE GET /path/to/myfile.txt\r\n"
        "Content-Length: 0\r\n"
        "Accept: */*\r\n"
        "\r\n";

    Request req;
    if (parse_request(request, &req) == 0) {
        printf("\nParsed request:\n");
        printf("  Scheme: '%s'\n", req.scheme);
        printf("  Method: '%s'\n", req.method);
        printf("  Path: '%s'\n", req.path);
        printf("  Headers (%d):\n", req.header_count);
        for (int i = 0; i < req.header_count; i++) {
            printf("    '%s': '%s'\n", req.headers[i].name, req.headers[i].value);
        }
    } else {
        printf("Parse failed\n");
    }
}

/* ============================================================================
EXERCISE 4: Build Response
============================================================================
*/

/*
TODO: Build a response string.
*/
int build_response(char *buffer, size_t buffer_size, const Response *resp) {
    /* TODO: Implement this function
     *
     * Build a response like:
     *   GETFILE OK\r\n
     *   Content-Length: 1234\r\n
     *   \r\n
     */

    int written = snprintf(buffer, buffer_size, "%s %s\r\n",
                          resp->scheme, resp->status);

    for (int i = 0; i < resp->header_count && written < (int)buffer_size; i++) {
        written += snprintf(buffer + written, buffer_size - written,
                           "%s: %s\r\n",
                           resp->headers[i].name, resp->headers[i].value);
    }

    if (written < (int)buffer_size) {
        written += snprintf(buffer + written, buffer_size - written, "\r\n");
    }

    return written;
}

void test_build_response(void) {
    printf("\n=== Exercise 4: Build Response ===\n");

    Response resp = {
        .scheme = "GETFILE",
        .status = "OK",
        .header_count = 2,
        .headers = {
            {"Content-Length", "1234"},
            {"Content-Type", "application/octet-stream"}
        }
    };

    char buffer[1024];
    build_response(buffer, sizeof(buffer), &resp);

    printf("\nBuilt response:\n%s", buffer);
}

/* ============================================================================
MAIN
============================================================================
*/

int main(void) {
    printf("\n================================================\n");
    printf("  Module 04: Protocol Parser\n");
    printf("================================================\n");
    printf("\nThis module prepares you for GIOS Project 1!\n");

    test_parse_request_line();
    test_parse_headers();
    test_parse_complete_request();
    test_build_response();

    printf("\n================================================\n");
    printf("  Protocol Parser Complete!\n");
    printf("  Next: Module 05 - File Transfer\n");
    printf("================================================\n\n");

    return 0;
}
