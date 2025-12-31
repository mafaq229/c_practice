/*
CS-6200 Preparation - Module 05: String Parsing

GIOS Project 1 requires parsing HTTP-like protocol headers.
This module teaches you to parse structured text.

Compile: clang -Wall -Wextra -g string_parsing.c -o string_parsing
Run:     ./string_parsing

Difficulty: [MEDIUM] to [HARD]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ============================================================================
EXERCISE 1: Parse Key-Value Pairs [MEDIUM]
============================================================================

Parse strings like "key=value" or "name: John"
*/

/*
TODO: Parse a key-value string.
Returns 0 on success, -1 on failure.
key and value buffers must be provided by caller.
*/
int parse_key_value(const char *input, char delimiter,
                    char *key, size_t key_size,
                    char *value, size_t value_size) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Find the delimiter in input
     * 2. Copy everything before delimiter to key
     * 3. Copy everything after delimiter to value
     * 4. Trim whitespace if needed
     * 5. Return 0 on success, -1 if delimiter not found
     */

    return -1;  /* TODO: Fix this */
}

void exercise1_key_value(void) {
    printf("\n=== Exercise 1: Parse Key-Value ===\n");

    char key[64], value[64];

    const char *tests[] = {
        "name=John",
        "Content-Length: 1024",
        "host:localhost:8080",
        "empty=",
        "invalid_no_delimiter",
    };

    for (int i = 0; i < 5; i++) {
        char delim = (i == 1) ? ':' : '=';
        if (parse_key_value(tests[i], delim, key, sizeof(key),
                           value, sizeof(value)) == 0) {
            printf("'%s' -> key='%s', value='%s'\n", tests[i], key, value);
        } else {
            printf("'%s' -> parse failed\n", tests[i]);
        }
    }

    printf("\nExpected:\n");
    printf("'name=John' -> key='name', value='John'\n");
    printf("'Content-Length: 1024' -> key='Content-Length', value='1024'\n");
    printf("etc.\n");
}

/* ============================================================================
EXERCISE 2: Parse HTTP-like Request Line [HARD]
============================================================================

This is similar to what you'll do in GIOS Project 1!
Parse: "METHOD /path HTTP/1.1"
*/

typedef struct {
    char method[16];
    char path[256];
    char version[16];
} RequestLine;

/*
TODO: Parse an HTTP-like request line.
Returns 0 on success, -1 on failure.
*/
int parse_request_line(const char *line, RequestLine *req) {
    /* TODO: Implement this function
     *
     * Parse "GET /index.html HTTP/1.1" into:
     *   method = "GET"
     *   path = "/index.html"
     *   version = "HTTP/1.1"
     *
     * HINT: Use sscanf or manual parsing
     */

    return -1;  /* TODO: Fix this */
}

void exercise2_request_line(void) {
    printf("\n=== Exercise 2: Parse Request Line ===\n");

    const char *requests[] = {
        "GET /index.html HTTP/1.1",
        "POST /api/data HTTP/1.0",
        "GetFile /path/to/file GETFILE/1.0",  /* GIOS-like */
    };

    for (int i = 0; i < 3; i++) {
        RequestLine req = {0};
        if (parse_request_line(requests[i], &req) == 0) {
            printf("'%s'\n  -> method='%s', path='%s', version='%s'\n",
                   requests[i], req.method, req.path, req.version);
        } else {
            printf("'%s' -> parse failed\n", requests[i]);
        }
    }
}

/* ============================================================================
EXERCISE 3: Parse Headers [HARD]
============================================================================

Parse multiple headers into a key-value structure.
*/

#define MAX_HEADERS 32
#define MAX_HEADER_NAME 64
#define MAX_HEADER_VALUE 256

typedef struct {
    char name[MAX_HEADER_NAME];
    char value[MAX_HEADER_VALUE];
} Header;

typedef struct {
    Header headers[MAX_HEADERS];
    int count;
} HeaderList;

/*
TODO: Parse a multi-line header string.
Headers are separated by \r\n or \n.
Format: "Header-Name: Header-Value"
*/
int parse_headers(const char *input, HeaderList *list) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Make a copy of input (strtok modifies the string)
     * 2. Split by lines (\r\n or \n)
     * 3. For each line, split by ': ' into name and value
     * 4. Store in list->headers, increment list->count
     *
     * Return 0 on success
     */

    list->count = 0;
    return -1;  /* TODO: Fix this */
}

/*
TODO: Find a header by name (case-insensitive).
Returns pointer to value or NULL if not found.
*/
const char *get_header(const HeaderList *list, const char *name) {
    /* TODO: Implement this function */

    return NULL;  /* TODO: Fix this */
}

void exercise3_headers(void) {
    printf("\n=== Exercise 3: Parse Headers ===\n");

    const char *headers_str =
        "Content-Type: text/html\r\n"
        "Content-Length: 1234\r\n"
        "Host: localhost:8080\r\n"
        "Connection: keep-alive\r\n";

    HeaderList list = {0};
    if (parse_headers(headers_str, &list) == 0) {
        printf("Parsed %d headers:\n", list.count);
        for (int i = 0; i < list.count; i++) {
            printf("  '%s': '%s'\n", list.headers[i].name, list.headers[i].value);
        }

        /* Test lookup */
        const char *content_length = get_header(&list, "Content-Length");
        const char *host = get_header(&list, "host");  /* Case insensitive */

        printf("\nLookup tests:\n");
        printf("  Content-Length: %s\n", content_length ? content_length : "(null)");
        printf("  host: %s\n", host ? host : "(null)");
    } else {
        printf("TODO: Implement parse_headers\n");
    }
}

/* ============================================================================
EXERCISE 4: Parse CSV Line [MEDIUM]
============================================================================
*/

#define MAX_FIELDS 32
#define MAX_FIELD_LEN 128

/*
TODO: Parse a CSV line into fields.
Returns number of fields parsed.

Simple version: no quotes handling.
*/
int parse_csv_line(const char *line, char fields[][MAX_FIELD_LEN], int max_fields) {
    /* TODO: Implement this function
     *
     * Split by comma, store each field.
     * Trim whitespace from each field.
     */

    return 0;  /* TODO: Fix this */
}

void exercise4_csv(void) {
    printf("\n=== Exercise 4: Parse CSV ===\n");

    const char *csv_line = "Alice, 25, New York, Engineer";
    char fields[MAX_FIELDS][MAX_FIELD_LEN];

    int count = parse_csv_line(csv_line, fields, MAX_FIELDS);
    printf("Input: '%s'\n", csv_line);
    printf("Parsed %d fields:\n", count);
    for (int i = 0; i < count; i++) {
        printf("  [%d] '%s'\n", i, fields[i]);
    }
}

/* ============================================================================
EXERCISE 5: Parse URL [HARD]
============================================================================

Parse: "http://user:pass@host:port/path?query#fragment"
*/

typedef struct {
    char scheme[16];      /* http, https */
    char user[64];        /* Optional */
    char password[64];    /* Optional */
    char host[256];
    int port;             /* -1 if not specified */
    char path[512];
    char query[512];      /* After ? */
    char fragment[64];    /* After # */
} ParsedURL;

/*
TODO: Parse a URL.
This is a challenging exercise - start with simple URLs.
*/
int parse_url(const char *url, ParsedURL *parsed) {
    /* TODO: Implement this function
     *
     * Start simple:
     * 1. Parse scheme (everything before ://)
     * 2. Parse host (everything before next / or end)
     * 3. Parse path (everything after host until ? or # or end)
     *
     * Advanced:
     * 4. Parse port (after host:)
     * 5. Parse user:pass (between :// and @)
     * 6. Parse query (after ?)
     * 7. Parse fragment (after #)
     */

    memset(parsed, 0, sizeof(*parsed));
    parsed->port = -1;
    return -1;  /* TODO: Fix this */
}

void exercise5_url(void) {
    printf("\n=== Exercise 5: Parse URL ===\n");

    const char *urls[] = {
        "http://example.com/path",
        "https://localhost:8080/api/data?key=value",
        "http://user:pass@host.com:3000/path#section",
    };

    for (int i = 0; i < 3; i++) {
        ParsedURL p = {0};
        printf("\nURL: %s\n", urls[i]);

        if (parse_url(urls[i], &p) == 0) {
            printf("  scheme: %s\n", p.scheme);
            printf("  host: %s\n", p.host);
            if (p.port != -1) printf("  port: %d\n", p.port);
            if (p.path[0]) printf("  path: %s\n", p.path);
            if (p.query[0]) printf("  query: %s\n", p.query);
            if (p.user[0]) printf("  user: %s\n", p.user);
        } else {
            printf("  TODO: Implement parse_url\n");
        }
    }
}

/* ============================================================================
EXERCISE 6: Build Formatted Strings [MEDIUM]
============================================================================

Use snprintf to safely build strings.
*/
void exercise6_snprintf(void) {
    printf("\n=== Exercise 6: Build Formatted Strings ===\n");

    char buffer[100];

    /* Basic usage */
    int len = snprintf(buffer, sizeof(buffer), "Hello, %s! You are %d years old.",
                       "Alice", 25);
    printf("Result: '%s' (len=%d)\n", buffer, len);

    /* Check for truncation */
    char small[20];
    len = snprintf(small, sizeof(small), "This is a very long string that won't fit");
    printf("Truncated: '%s' (would need %d chars)\n", small, len);

    if (len >= (int)sizeof(small)) {
        printf("WARNING: Output was truncated!\n");
    }

    /* Building complex strings */
    char response[256];
    const char *method = "GET";
    const char *path = "/api/users";
    int content_length = 1234;

    snprintf(response, sizeof(response),
             "%s %s HTTP/1.1\r\n"
             "Content-Length: %d\r\n"
             "\r\n",
             method, path, content_length);

    printf("\nBuilt response:\n%s", response);
}

/* ============================================================================
MAIN FUNCTION
============================================================================
*/
int main(void) {
    printf("\n");
    printf("================================================\n");
    printf("  Module 05: String Parsing\n");
    printf("================================================\n");
    printf("\nThese exercises prepare you for GIOS Project 1!\n");

    exercise1_key_value();
    exercise2_request_line();
    exercise3_headers();
    exercise4_csv();
    exercise5_url();
    exercise6_snprintf();

    printf("\n================================================\n");
    printf("  String Parsing Complete!\n");
    printf("  Next: buffer_handling.c\n");
    printf("================================================\n\n");

    return 0;
}
