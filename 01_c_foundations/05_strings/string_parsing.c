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
    if (input == NULL || key == NULL || value == NULL) return -1;
    if (key_size == 0 || value_size == 0) return -1;

    // Find the delimiter in the input string
    const char *delim_pos = strchr(input, delimiter); // searches for first occurance of delimiter
    if (delim_pos == NULL) return -1;

    // Copy key part (before delimiter), trimming whitespace.
    const char *key_start = input;
    const char *key_end = delim_pos;
    while (key_start < key_end && isspace((unsigned char)*key_start)) {
        key_start++;
    }
    while (key_end > key_start && isspace((unsigned char)*(key_end - 1))) {
        key_end--;
    }
    size_t key_len = (size_t)(key_end - key_start);
    if (key_len >= key_size) key_len = key_size - 1;
    memcpy(key, key_start, key_len);
    key[key_len] = '\0';

    /* Copy value part (after delimiter), trimming whitespace. */
    const char *value_start = delim_pos + 1;
    while (*value_start != '\0' && isspace((unsigned char)*value_start)) {
        value_start++;
    }
    const char *value_end = input + strlen(input);
    while (value_end > value_start && isspace((unsigned char)*(value_end - 1))) {
        value_end--;
    }
    size_t value_len = (size_t)(value_end - value_start);
    if (value_len >= value_size) value_len = value_size - 1;
    memcpy(value, value_start, value_len);
    value[value_len] = '\0';

    return 0;
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
    if (line == NULL || req == NULL) return -1;

    /* Use sscanf to split into three whitespace-separated tokens. */
    int matched = sscanf(line, "%15s %255s %15s", req->method, req->path, req->version);
    if (matched != 3) return -1;
    return 0;
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
    if (input == NULL || list == NULL) return -1;

    list->count = 0;

    /* Make a writable copy because strtok modifies the string. */
    size_t len = strlen(input);
    char *copy = (char *)malloc(len + 1);
    if (copy == NULL) return -1;
    memcpy(copy, input, len + 1);

    // splits into lines
    char *saveptr = NULL;
    char *line = strtok_r(copy, "\r\n", &saveptr); 
    while (line != NULL && list->count < MAX_HEADERS) {
        /* Find the ':' separator in the header line. */
        char *colon = strchr(line, ':');
        if (colon != NULL) {
            *colon = '\0';

            /* Trim whitespace from name and value. */
            char *name = line;
            char *value = colon + 1;
            while (*name != '\0' && isspace((unsigned char)*name)) name++;
            while (*value != '\0' && isspace((unsigned char)*value)) value++;

            char *name_end = name + strlen(name);
            while (name_end > name && isspace((unsigned char)*(name_end - 1))) {
                name_end--;
            }
            *name_end = '\0';

            char *value_end = value + strlen(value);
            while (value_end > value && isspace((unsigned char)*(value_end - 1))) {
                value_end--;
            }
            *value_end = '\0';

            /* Copy into the header list with bounds checks. */
            strncpy(list->headers[list->count].name, name, MAX_HEADER_NAME - 1);
            list->headers[list->count].name[MAX_HEADER_NAME - 1] = '\0';
            strncpy(list->headers[list->count].value, value, MAX_HEADER_VALUE - 1);
            list->headers[list->count].value[MAX_HEADER_VALUE - 1] = '\0';
            list->count++;
        }
        line = strtok_r(NULL, "\r\n", &saveptr);
    }

    free(copy);
    return 0;
}

/*
TODO: Find a header by name (case-insensitive).
Returns pointer to value or NULL if not found.
*/
const char *get_header(const HeaderList *list, const char *name) {
    /* TODO: Implement this function */
    if (list == NULL || name == NULL) return NULL;

    for (int i = 0; i < list->count; i++) {
        const char *a = list->headers[i].name;
        const char *b = name;
        /* Case-insensitive compare without extra libraries. */
        while (*a != '\0' && *b != '\0' &&
               tolower((unsigned char)*a) == tolower((unsigned char)*b)) {
            a++;
            b++;
        }
        if (*a == '\0' && *b == '\0') {
            return list->headers[i].value;
        }
    }
    return NULL;
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
    if (line == NULL || fields == NULL || max_fields <= 0) return 0;

    int count = 0;
    const char *p = line;

    while (*p != '\0' && count < max_fields) {
        /* Skip leading whitespace before a field. */
        while (*p != '\0' && isspace((unsigned char)*p)) p++;

        /* Copy characters until comma or end. */
        int i = 0;
        const char *field_start = p;
        while (*p != '\0' && *p != ',') p++;
        const char *field_end = p;

        /* Trim trailing whitespace. */
        while (field_end > field_start && isspace((unsigned char)*(field_end - 1))) {
            field_end--;
        }

        /* Copy the trimmed field into the buffer. */
        const char *it = field_start;
        while (it < field_end && i < MAX_FIELD_LEN - 1) {
            fields[count][i++] = *it++;
        }
        fields[count][i] = '\0';
        count++;

        if (*p == ',') p++; /* Skip comma and move to next field. */
    }

    return count;
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
    if (url == NULL || parsed == NULL) return -1;

    const char *p = url;

    /* Parse scheme (up to "://"). */
    const char *scheme_end = strstr(p, "://");
    if (scheme_end == NULL) return -1;
    size_t scheme_len = (size_t)(scheme_end - p);
    if (scheme_len >= sizeof(parsed->scheme)) scheme_len = sizeof(parsed->scheme) - 1;
    memcpy(parsed->scheme, p, scheme_len);
    parsed->scheme[scheme_len] = '\0';
    p = scheme_end + 3;

    /* Optional user:pass@ */
    const char *at_sign = strchr(p, '@');
    const char *host_start = p;
    if (at_sign != NULL) {
        const char *colon = strchr(p, ':');
        if (colon != NULL && colon < at_sign) {
            size_t user_len = (size_t)(colon - p);
            if (user_len >= sizeof(parsed->user)) user_len = sizeof(parsed->user) - 1;
            memcpy(parsed->user, p, user_len);
            parsed->user[user_len] = '\0';

            size_t pass_len = (size_t)(at_sign - colon - 1);
            if (pass_len >= sizeof(parsed->password)) pass_len = sizeof(parsed->password) - 1;
            memcpy(parsed->password, colon + 1, pass_len);
            parsed->password[pass_len] = '\0';
        } else {
            size_t user_len = (size_t)(at_sign - p);
            if (user_len >= sizeof(parsed->user)) user_len = sizeof(parsed->user) - 1;
            memcpy(parsed->user, p, user_len);
            parsed->user[user_len] = '\0';
        }
        host_start = at_sign + 1;
    }

    /* Parse host[:port] up to /, ?, or #. */
    const char *path_start = host_start;
    while (*path_start != '\0' && *path_start != '/' &&
           *path_start != '?' && *path_start != '#') {
        path_start++;
    }
    const char *host_end = path_start;
    const char *port_sep = NULL;
    for (const char *it = host_start; it < host_end; it++) {
        if (*it == ':') {
            port_sep = it;
            break;
        }
    }

    if (port_sep != NULL) {
        size_t host_len = (size_t)(port_sep - host_start);
        if (host_len >= sizeof(parsed->host)) host_len = sizeof(parsed->host) - 1;
        memcpy(parsed->host, host_start, host_len);
        parsed->host[host_len] = '\0';

        /* Parse port number after ':' */
        int port = 0;
        const char *port_str = port_sep + 1;
        if (*port_str != '\0') {
            while (*port_str != '\0' && port_str < host_end) {
                if (!isdigit((unsigned char)*port_str)) {
                    return -1;
                }
                port = port * 10 + (*port_str - '0');
                port_str++;
            }
            parsed->port = port;
        }
    } else {
        size_t host_len = (size_t)(host_end - host_start);
        if (host_len >= sizeof(parsed->host)) host_len = sizeof(parsed->host) - 1;
        memcpy(parsed->host, host_start, host_len);
        parsed->host[host_len] = '\0';
    }

    /* Parse path, query, and fragment if present. */
    if (*path_start == '/') {
        const char *path_end = path_start;
        while (*path_end != '\0' && *path_end != '?' && *path_end != '#') {
            path_end++;
        }
        size_t path_len = (size_t)(path_end - path_start);
        if (path_len >= sizeof(parsed->path)) path_len = sizeof(parsed->path) - 1;
        memcpy(parsed->path, path_start, path_len);
        parsed->path[path_len] = '\0';
        path_start = path_end;
    }

    if (*path_start == '?') {
        const char *query_start = path_start + 1;
        const char *query_end = query_start;
        while (*query_end != '\0' && *query_end != '#') query_end++;
        size_t query_len = (size_t)(query_end - query_start);
        if (query_len >= sizeof(parsed->query)) query_len = sizeof(parsed->query) - 1;
        memcpy(parsed->query, query_start, query_len);
        parsed->query[query_len] = '\0';
        path_start = query_end;
    }

    if (*path_start == '#') {
        const char *frag_start = path_start + 1;
        size_t frag_len = strlen(frag_start);
        if (frag_len >= sizeof(parsed->fragment)) frag_len = sizeof(parsed->fragment) - 1;
        memcpy(parsed->fragment, frag_start, frag_len);
        parsed->fragment[frag_len] = '\0';
    }

    return 0;
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
