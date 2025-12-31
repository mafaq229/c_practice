/*
CS-6200 Preparation - Module 06: Dynamic Buffer

A growable byte buffer is essential for network programming.
This module creates a reusable buffer for GIOS projects.

Compile: clang -Wall -Wextra -g dynamic_buffer.c -o dynamic_buffer
Run:     ./dynamic_buffer

Difficulty: [MEDIUM]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

/* ============================================================================
DYNAMIC BUFFER IMPLEMENTATION
============================================================================
*/

#define BUFFER_INITIAL_CAPACITY 64
#define BUFFER_GROWTH_FACTOR 2

typedef struct {
    uint8_t *data;
    size_t length;    /* Current data length */
    size_t capacity;  /* Allocated capacity */
} Buffer;

/*
TODO: Initialize buffer with default capacity.
*/
int buffer_init(Buffer *buf) {
    return buffer_init_capacity(buf, BUFFER_INITIAL_CAPACITY);
}

/*
TODO: Initialize buffer with specific capacity.
*/
int buffer_init_capacity(Buffer *buf, size_t capacity) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Allocate 'capacity' bytes
     * 2. Set length to 0
     * 3. Set capacity
     * 4. Return 0 on success, -1 on failure
     */

    buf->data = NULL;
    buf->length = 0;
    buf->capacity = 0;
    return -1;  /* TODO: Fix this */
}

/*
TODO: Free buffer resources.
*/
void buffer_free(Buffer *buf) {
    /* TODO: Implement this function */
}

/*
TODO: Clear buffer (keep allocation).
*/
void buffer_clear(Buffer *buf) {
    buf->length = 0;
}

/*
TODO: Ensure buffer can hold at least 'size' more bytes.
*/
int buffer_ensure_capacity(Buffer *buf, size_t additional) {
    /* TODO: Implement this function
     *
     * If length + additional > capacity, grow the buffer.
     * Use BUFFER_GROWTH_FACTOR to determine new size.
     */

    return -1;  /* TODO: Fix this */
}

/*
TODO: Append raw bytes to buffer.
*/
int buffer_append(Buffer *buf, const void *data, size_t len) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Ensure capacity
     * 2. Copy data to buf->data + buf->length
     * 3. Update length
     * 4. Return 0 on success, -1 on failure
     */

    return -1;  /* TODO: Fix this */
}

/*
TODO: Append a single byte.
*/
int buffer_append_byte(Buffer *buf, uint8_t byte) {
    return buffer_append(buf, &byte, 1);
}

/*
TODO: Append null-terminated string.
*/
int buffer_append_string(Buffer *buf, const char *str) {
    return buffer_append(buf, str, strlen(str));
}

/*
TODO: Append formatted string (like printf).
*/
int buffer_appendf(Buffer *buf, const char *format, ...) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Use vsnprintf to get required size
     * 2. Ensure capacity
     * 3. Use vsnprintf to write to buffer
     * 4. Update length
     *
     * HINT: Use va_list, va_start, va_end, va_copy
     */

    (void)format;  /* Silence unused warning */
    return -1;  /* TODO: Fix this */
}

/*
TODO: Remove bytes from beginning of buffer.
(Useful for parsing - consume processed data)
*/
void buffer_consume(Buffer *buf, size_t len) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. If len >= length, just clear
     * 2. Otherwise, memmove remaining data to front
     * 3. Update length
     */
}

/*
TODO: Get pointer to data at offset.
Returns NULL if offset is out of bounds.
*/
uint8_t *buffer_at(Buffer *buf, size_t offset) {
    if (offset >= buf->length) return NULL;
    return buf->data + offset;
}

/*
TODO: Search for a byte sequence in buffer.
Returns offset if found, -1 if not found.
*/
ssize_t buffer_find(Buffer *buf, const void *needle, size_t needle_len) {
    /* TODO: Implement this function
     *
     * Search for 'needle' in buffer data.
     * Return offset of first occurrence, or -1 if not found.
     */

    return -1;  /* TODO: Fix this */
}

/*
TODO: Search for a string in buffer (excludes null terminator).
*/
ssize_t buffer_find_string(Buffer *buf, const char *str) {
    return buffer_find(buf, str, strlen(str));
}

/*
Print buffer as hex dump (for debugging).
*/
void buffer_hexdump(Buffer *buf) {
    printf("Buffer (%zu bytes):\n", buf->length);
    for (size_t i = 0; i < buf->length; i++) {
        printf("%02x ", buf->data[i]);
        if ((i + 1) % 16 == 0) printf("\n");
    }
    if (buf->length % 16 != 0) printf("\n");
}

/*
Print buffer as string (assumes printable characters).
*/
void buffer_print_string(Buffer *buf) {
    printf("'%.*s'\n", (int)buf->length, buf->data);
}

/* ============================================================================
TEST FUNCTIONS
============================================================================
*/

void test_basic_operations(void) {
    printf("\n=== Test Basic Operations ===\n");

    Buffer buf;
    if (buffer_init(&buf) != 0) {
        printf("TODO: Implement buffer_init\n");
        return;
    }

    /* Test append */
    buffer_append_string(&buf, "Hello, ");
    buffer_append_string(&buf, "World!");

    printf("After appending strings: ");
    buffer_print_string(&buf);
    printf("Length: %zu, Capacity: %zu\n", buf.length, buf.capacity);

    /* Test append byte */
    buffer_append_byte(&buf, '\n');
    buffer_append_byte(&buf, '!');

    printf("After appending bytes: ");
    buffer_print_string(&buf);

    buffer_free(&buf);
    printf("Buffer freed.\n");
}

void test_formatted_append(void) {
    printf("\n=== Test Formatted Append ===\n");

    Buffer buf;
    if (buffer_init(&buf) != 0) return;

    buffer_appendf(&buf, "Status: %d\n", 200);
    buffer_appendf(&buf, "Content-Type: %s\n", "text/html");
    buffer_appendf(&buf, "Content-Length: %zu\n", (size_t)1234);
    buffer_appendf(&buf, "\n");

    printf("HTTP-like response:\n");
    buffer_print_string(&buf);

    buffer_free(&buf);
}

void test_consume(void) {
    printf("\n=== Test Consume ===\n");

    Buffer buf;
    if (buffer_init(&buf) != 0) return;

    buffer_append_string(&buf, "Header: Value\r\nBody data here");
    printf("Before consume: ");
    buffer_print_string(&buf);

    /* Find and consume header line */
    ssize_t pos = buffer_find_string(&buf, "\r\n");
    if (pos >= 0) {
        printf("Found \\r\\n at position %zd\n", pos);
        buffer_consume(&buf, pos + 2);  /* +2 for \r\n */
        printf("After consuming header: ");
        buffer_print_string(&buf);
    }

    buffer_free(&buf);
}

void test_growth(void) {
    printf("\n=== Test Growth ===\n");

    Buffer buf;
    if (buffer_init_capacity(&buf, 8) != 0) return;

    printf("Initial capacity: %zu\n", buf.capacity);

    /* Append data to trigger growth */
    for (int i = 0; i < 10; i++) {
        buffer_appendf(&buf, "Line %d\n", i);
        printf("After line %d: length=%zu, capacity=%zu\n",
               i, buf.length, buf.capacity);
    }

    buffer_free(&buf);
}

void test_binary_data(void) {
    printf("\n=== Test Binary Data ===\n");

    Buffer buf;
    if (buffer_init(&buf) != 0) return;

    /* Binary data with embedded nulls */
    uint8_t binary[] = {0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x00, 0x57, 0x6f, 0x72, 0x6c, 0x64};
    buffer_append(&buf, binary, sizeof(binary));

    printf("Binary buffer:\n");
    buffer_hexdump(&buf);

    /* Find binary sequence */
    uint8_t needle[] = {0x00, 0x57};
    ssize_t pos = buffer_find(&buf, needle, sizeof(needle));
    printf("Found 0x00 0x57 at position: %zd (expected: 5)\n", pos);

    buffer_free(&buf);
}

/* ============================================================================
MAIN FUNCTION
============================================================================
*/
int main(void) {
    printf("\n");
    printf("================================================\n");
    printf("  Module 06: Dynamic Buffer\n");
    printf("================================================\n");

    test_basic_operations();
    test_formatted_append();
    test_consume();
    test_growth();
    test_binary_data();

    printf("\n================================================\n");
    printf("  Dynamic Buffer Complete!\n");
    printf("  This buffer is useful for GIOS projects.\n");
    printf("  Next: Module 07 - Makefiles\n");
    printf("================================================\n\n");

    return 0;
}
