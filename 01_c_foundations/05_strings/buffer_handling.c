/*
CS-6200 Preparation - Module 05: Buffer Handling

Proper buffer handling is critical for network programming.
This module covers safe buffer operations used in GIOS.

Compile: clang -Wall -Wextra -g buffer_handling.c -o buffer_handling
Run:     ./buffer_handling

Difficulty: [MEDIUM] to [HARD]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* ============================================================================
CONCEPT: Binary vs Text Data
============================================================================

Text data: Null-terminated strings, can use strlen, strcpy, etc.
Binary data: May contain null bytes, must track length separately.

Network programming uses BINARY data (files can contain any bytes).
*/

void demonstrate_binary_vs_text(void) {
    printf("\n=== Binary vs Text Data ===\n");

    /* Text data */
    char text[] = "Hello, World!";
    printf("Text: '%s' (strlen=%zu)\n", text, strlen(text));

    /* Binary data - contains null bytes! */
    uint8_t binary[] = {0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x00, 0x57, 0x6f, 0x72, 0x6c, 0x64};
    size_t binary_len = sizeof(binary);

    printf("Binary (hex): ");
    for (size_t i = 0; i < binary_len; i++) {
        printf("%02x ", binary[i]);
    }
    printf("\n");

    /* strlen would stop at the null byte! */
    printf("strlen on binary: %zu (WRONG! Should be %zu)\n",
           strlen((char*)binary), binary_len);

    printf("\nKey insight: Always track binary data length separately!\n");
}

/* ============================================================================
EXERCISE 1: Buffer Structure [MEDIUM]
============================================================================

A proper buffer structure tracks data and length.
*/

typedef struct {
    uint8_t *data;
    size_t length;     /* Current data length */
    size_t capacity;   /* Allocated size */
} Buffer;

/*
TODO: Initialize a buffer with given capacity.
*/
int buffer_init(Buffer *buf, size_t capacity) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Allocate 'capacity' bytes
     * 2. Set length to 0
     * 3. Set capacity
     * 4. Return 0 on success, -1 on failure
     */

    return -1;  /* TODO: Fix this */
}

/*
TODO: Free buffer resources.
*/
void buffer_free(Buffer *buf) {
    /* TODO: Implement this function */
}

/*
TODO: Append data to buffer.
Grows buffer if needed.
*/
int buffer_append(Buffer *buf, const void *data, size_t len) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Check if we need to grow (length + len > capacity)
     * 2. If so, realloc to larger size
     * 3. Copy data to buf->data + buf->length
     * 4. Update length
     * 5. Return 0 on success, -1 on failure
     */

    return -1;  /* TODO: Fix this */
}

/*
TODO: Reset buffer (keep allocation, just clear data).
*/
void buffer_reset(Buffer *buf) {
    /* TODO: Implement this function */
}

void exercise1_buffer_struct(void) {
    printf("\n=== Exercise 1: Buffer Structure ===\n");

    Buffer buf;
    if (buffer_init(&buf, 64) != 0) {
        printf("TODO: Implement buffer_init\n");
        return;
    }

    /* Append some data */
    buffer_append(&buf, "Hello, ", 7);
    buffer_append(&buf, "World!", 6);

    printf("Buffer contents: '");
    for (size_t i = 0; i < buf.length; i++) {
        printf("%c", buf.data[i]);
    }
    printf("' (len=%zu, cap=%zu)\n", buf.length, buf.capacity);

    /* Reset and reuse */
    buffer_reset(&buf);
    printf("After reset: len=%zu\n", buf.length);

    buffer_free(&buf);
}

/* ============================================================================
EXERCISE 2: Circular Buffer [HARD]
============================================================================

Circular buffers are used for streaming data.
*/

typedef struct {
    uint8_t *data;
    size_t capacity;
    size_t head;  /* Read position */
    size_t tail;  /* Write position */
    size_t count; /* Number of bytes stored */
} CircularBuffer;

/*
TODO: Initialize circular buffer.
*/
int cbuf_init(CircularBuffer *cb, size_t capacity) {
    /* TODO: Implement this function */

    return -1;  /* TODO: Fix this */
}

/*
TODO: Write data to circular buffer.
Returns number of bytes written.
*/
size_t cbuf_write(CircularBuffer *cb, const void *data, size_t len) {
    /* TODO: Implement this function
     *
     * Key insight: Handle wrap-around at capacity.
     * Only write as much as there's free space.
     */

    return 0;  /* TODO: Fix this */
}

/*
TODO: Read data from circular buffer.
Returns number of bytes read.
*/
size_t cbuf_read(CircularBuffer *cb, void *dest, size_t len) {
    /* TODO: Implement this function
     *
     * Key insight: Handle wrap-around at capacity.
     * Only read as much as there's data.
     */

    return 0;  /* TODO: Fix this */
}

/*
TODO: Get available data length.
*/
size_t cbuf_available(const CircularBuffer *cb) {
    return cb->count;
}

/*
TODO: Get free space.
*/
size_t cbuf_free_space(const CircularBuffer *cb) {
    return cb->capacity - cb->count;
}

void cbuf_free(CircularBuffer *cb) {
    if (cb->data) {
        free(cb->data);
        cb->data = NULL;
    }
}

void exercise2_circular_buffer(void) {
    printf("\n=== Exercise 2: Circular Buffer ===\n");

    CircularBuffer cb;
    if (cbuf_init(&cb, 16) != 0) {
        printf("TODO: Implement cbuf_init\n");
        return;
    }

    /* Write some data */
    const char *msg1 = "Hello!";
    size_t written = cbuf_write(&cb, msg1, strlen(msg1));
    printf("Wrote %zu bytes: '%s'\n", written, msg1);
    printf("Available: %zu, Free: %zu\n", cbuf_available(&cb), cbuf_free_space(&cb));

    /* Read some data */
    char buf[32];
    size_t read = cbuf_read(&cb, buf, 3);
    buf[read] = '\0';
    printf("Read %zu bytes: '%s'\n", read, buf);

    /* Write more (should wrap around) */
    const char *msg2 = "World! Foo Bar";
    written = cbuf_write(&cb, msg2, strlen(msg2));
    printf("Wrote %zu bytes\n", written);

    /* Read all remaining */
    read = cbuf_read(&cb, buf, sizeof(buf) - 1);
    buf[read] = '\0';
    printf("Read %zu bytes: '%s'\n", read, buf);

    cbuf_free(&cb);
}

/* ============================================================================
EXERCISE 3: Packet Buffer with Header [HARD]
============================================================================

This is similar to network packet handling.
*/

/* Simple packet format:
+--------+--------+----------------+
| Length | Type   | Payload        |
| 2 bytes| 1 byte | Length bytes   |
+--------+--------+----------------+
*/

#define PACKET_TYPE_DATA 0x01
#define PACKET_TYPE_CONTROL 0x02
#define PACKET_TYPE_END 0x03

typedef struct {
    uint16_t length;   /* Payload length */
    uint8_t type;
    uint8_t payload[]; /* Flexible array member */
} __attribute__((packed)) Packet;

#define PACKET_HEADER_SIZE (sizeof(uint16_t) + sizeof(uint8_t))

/*
TODO: Create a packet with given payload.
Returns pointer to allocated packet (caller must free).
*/
Packet *packet_create(uint8_t type, const void *payload, size_t payload_len) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Allocate PACKET_HEADER_SIZE + payload_len bytes
     * 2. Set length and type fields
     * 3. Copy payload
     * 4. Return packet
     */

    return NULL;  /* TODO: Fix this */
}

/*
TODO: Parse a packet from raw bytes.
Returns 0 on success, -1 on failure.
Sets *packet to point to the packet (within raw_data).
*/
int packet_parse(const uint8_t *raw_data, size_t raw_len, Packet **packet) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Check if raw_len >= PACKET_HEADER_SIZE
     * 2. Cast raw_data to Packet*
     * 3. Check if raw_len >= PACKET_HEADER_SIZE + packet->length
     * 4. Set *packet
     */

    return -1;  /* TODO: Fix this */
}

void exercise3_packet_buffer(void) {
    printf("\n=== Exercise 3: Packet Buffer ===\n");

    /* Create a packet */
    const char *data = "Hello, World!";
    Packet *pkt = packet_create(PACKET_TYPE_DATA, data, strlen(data));

    if (pkt == NULL) {
        printf("TODO: Implement packet_create\n");
        return;
    }

    printf("Created packet:\n");
    printf("  Type: 0x%02x\n", pkt->type);
    printf("  Length: %u\n", pkt->length);
    printf("  Payload: '%.*s'\n", pkt->length, pkt->payload);

    /* Simulate receiving raw bytes and parsing */
    size_t total_size = PACKET_HEADER_SIZE + pkt->length;
    uint8_t *raw = (uint8_t*)pkt;

    printf("\nRaw bytes: ");
    for (size_t i = 0; i < total_size; i++) {
        printf("%02x ", raw[i]);
    }
    printf("\n");

    Packet *parsed;
    if (packet_parse(raw, total_size, &parsed) == 0) {
        printf("Parsed successfully!\n");
    } else {
        printf("TODO: Implement packet_parse\n");
    }

    free(pkt);
}

/* ============================================================================
EXERCISE 4: Safe Memory Copy [MEDIUM]
============================================================================

memcpy vs memmove - know the difference!
*/
void exercise4_memcpy(void) {
    printf("\n=== Exercise 4: Safe Memory Copy ===\n");

    uint8_t src[] = {1, 2, 3, 4, 5, 6, 7, 8};
    uint8_t dst[8] = {0};

    /* memcpy - non-overlapping regions only! */
    memcpy(dst, src, sizeof(src));
    printf("After memcpy: ");
    for (int i = 0; i < 8; i++) printf("%d ", dst[i]);
    printf("\n");

    /* memmove - handles overlapping regions */
    uint8_t overlap[] = {1, 2, 3, 4, 5, 6, 7, 8};
    printf("Before overlap move: ");
    for (int i = 0; i < 8; i++) printf("%d ", overlap[i]);
    printf("\n");

    /* Move bytes 0-5 to position 2-7 (overlapping!) */
    memmove(overlap + 2, overlap, 6);
    printf("After memmove:       ");
    for (int i = 0; i < 8; i++) printf("%d ", overlap[i]);
    printf("\n");

    printf("\nKey rule: Use memmove for overlapping, memcpy for non-overlapping.\n");
    printf("When in doubt, use memmove (slightly slower but always safe).\n");
}

/* ============================================================================
EXERCISE 5: Buffer to Network Format [MEDIUM]
============================================================================

Network byte order is big-endian. x86 is little-endian.
*/
void exercise5_endianness(void) {
    printf("\n=== Exercise 5: Endianness ===\n");

    uint32_t value = 0x12345678;
    uint8_t *bytes = (uint8_t*)&value;

    printf("Value: 0x%08x\n", value);
    printf("Bytes in memory: ");
    for (int i = 0; i < 4; i++) {
        printf("%02x ", bytes[i]);
    }
    printf("\n");

    /* Check endianness */
    if (bytes[0] == 0x78) {
        printf("This system is LITTLE-ENDIAN (least significant byte first)\n");
    } else {
        printf("This system is BIG-ENDIAN (most significant byte first)\n");
    }

    /* Converting to network byte order */
    /* htonl = host to network long (32-bit) */
    /* htons = host to network short (16-bit) */
    /* ntohl, ntohs = network to host */

    printf("\nFor network programming, use htonl/htons before sending\n");
    printf("and ntohl/ntohs after receiving multi-byte integers.\n");
}

/* ============================================================================
MAIN FUNCTION
============================================================================
*/
int main(void) {
    printf("\n");
    printf("================================================\n");
    printf("  Module 05: Buffer Handling\n");
    printf("================================================\n");

    demonstrate_binary_vs_text();
    exercise1_buffer_struct();
    exercise2_circular_buffer();
    exercise3_packet_buffer();
    exercise4_memcpy();
    exercise5_endianness();

    printf("\n================================================\n");
    printf("  Buffer Handling Complete!\n");
    printf("  Next: Module 06 - Data Structures\n");
    printf("================================================\n\n");

    return 0;
}
