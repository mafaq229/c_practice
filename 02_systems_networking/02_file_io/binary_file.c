/*
CS-6200 Preparation - Module 02: Binary Files

Learn to read and write binary data (structs, raw bytes).
This is essential for protocol implementation.

Compile: clang -Wall -Wextra -g binary_file.c -o binary_file
Run:     ./binary_file

Difficulty: [MEDIUM]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* ============================================================================
EXERCISE 1: Write and Read Structs
============================================================================
*/

/* A simple record structure */
typedef struct {
    uint32_t id;
    char name[32];
    float score;
    uint8_t active;
} __attribute__((packed)) Record;

/*
TODO: Write an array of records to a binary file.
*/
int write_records(const char *filename, Record *records, int count) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Open file for binary writing: fopen(filename, "wb")
     * 2. Write count as first 4 bytes (uint32_t)
     * 3. Write all records: fwrite(records, sizeof(Record), count, file)
     * 4. Close file
     * 5. Return 0 on success
     */

    printf("TODO: Implement write_records\n");
    return -1;  /* TODO: Fix this */
}

/*
TODO: Read records from a binary file.
Returns number of records read, or -1 on error.
Caller must free the returned array.
*/
int read_records(const char *filename, Record **records) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Open file for binary reading: fopen(filename, "rb")
     * 2. Read count (first 4 bytes)
     * 3. Allocate memory for 'count' records
     * 4. Read all records: fread(*records, sizeof(Record), count, file)
     * 5. Close file
     * 6. Return count
     */

    printf("TODO: Implement read_records\n");
    *records = NULL;
    return -1;  /* TODO: Fix this */
}

void test_records(void) {
    printf("\n=== Exercise 1: Read/Write Structs ===\n");

    /* Create some records */
    Record records[] = {
        {1, "Alice", 95.5f, 1},
        {2, "Bob", 87.3f, 1},
        {3, "Charlie", 72.8f, 0},
        {4, "Diana", 91.2f, 1},
    };
    int count = sizeof(records) / sizeof(records[0]);

    /* Write to file */
    if (write_records("records.bin", records, count) == 0) {
        printf("Wrote %d records to records.bin\n", count);
    } else {
        printf("write_records not implemented yet.\n");
        return;
    }

    /* Read back */
    Record *loaded = NULL;
    int loaded_count = read_records("records.bin", &loaded);

    if (loaded_count > 0 && loaded != NULL) {
        printf("Read %d records:\n", loaded_count);
        for (int i = 0; i < loaded_count; i++) {
            printf("  [%u] %s: %.1f (%s)\n",
                   loaded[i].id, loaded[i].name, loaded[i].score,
                   loaded[i].active ? "active" : "inactive");
        }
        free(loaded);
    }

    remove("records.bin");
}

/* ============================================================================
EXERCISE 2: File with Header
============================================================================
*/

#define MAGIC_NUMBER 0x4D594654  /* "MYFT" */
#define VERSION 1

typedef struct {
    uint32_t magic;
    uint8_t version;
    uint32_t record_count;
    uint8_t reserved[7];  /* Padding to 16 bytes */
} __attribute__((packed)) FileHeader;

/*
TODO: Write file with header and records.
*/
int write_file_with_header(const char *filename, Record *records, int count) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Create and fill header
     * 2. Open file for binary write
     * 3. Write header
     * 4. Write records
     * 5. Close file
     */

    printf("TODO: Implement write_file_with_header\n");
    return -1;  /* TODO: Fix this */
}

/*
TODO: Read and validate file with header.
*/
int read_file_with_header(const char *filename, Record **records) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Open file
     * 2. Read header
     * 3. Validate magic number
     * 4. Check version compatibility
     * 5. Allocate and read records
     * 6. Return count
     */

    printf("TODO: Implement read_file_with_header\n");
    *records = NULL;
    return -1;  /* TODO: Fix this */
}

void test_header(void) {
    printf("\n=== Exercise 2: File with Header ===\n");

    printf("FileHeader size: %zu bytes (expected: 16)\n", sizeof(FileHeader));
    printf("Record size: %zu bytes\n", sizeof(Record));

    Record records[] = {
        {1, "Test1", 100.0f, 1},
        {2, "Test2", 200.0f, 0},
    };

    if (write_file_with_header("data.bin", records, 2) == 0) {
        printf("Wrote file with header.\n");

        Record *loaded = NULL;
        int count = read_file_with_header("data.bin", &loaded);
        if (count > 0) {
            printf("Read %d records from file with header.\n", count);
            free(loaded);
        }
        remove("data.bin");
    }
}

/* ============================================================================
EXERCISE 3: Seeking in Files
============================================================================
*/

void test_seeking(void) {
    printf("\n=== Exercise 3: File Seeking ===\n");

    /* Create a file with known content */
    FILE *f = fopen("seek_test.bin", "wb");
    if (!f) return;

    for (int i = 0; i < 100; i++) {
        uint32_t val = i * 10;
        fwrite(&val, sizeof(val), 1, f);
    }
    fclose(f);

    printf("Created file with 100 uint32_t values (0, 10, 20, ...)\n");

    /* Read specific positions */
    f = fopen("seek_test.bin", "rb");
    if (!f) return;

    /* TODO: Read value at position 50 (should be 500) */
    /* HINT: fseek(f, 50 * sizeof(uint32_t), SEEK_SET); */
    uint32_t val;

    /* Read position 0 */
    fseek(f, 0, SEEK_SET);
    fread(&val, sizeof(val), 1, f);
    printf("Position 0: %u (expected: 0)\n", val);

    /* Read position 50 */
    fseek(f, 50 * sizeof(uint32_t), SEEK_SET);
    fread(&val, sizeof(val), 1, f);
    printf("Position 50: %u (expected: 500)\n", val);

    /* Read last value */
    fseek(f, -sizeof(uint32_t), SEEK_END);
    fread(&val, sizeof(val), 1, f);
    printf("Last position: %u (expected: 990)\n", val);

    /* Get current position */
    long pos = ftell(f);
    printf("Current position: %ld\n", pos);

    fclose(f);
    remove("seek_test.bin");
}

/* ============================================================================
EXERCISE 4: Endianness
============================================================================
*/

void test_endianness(void) {
    printf("\n=== Exercise 4: Endianness ===\n");

    uint32_t value = 0x12345678;

    /* Write in native byte order */
    FILE *f = fopen("endian.bin", "wb");
    fwrite(&value, sizeof(value), 1, f);
    fclose(f);

    /* Read back as bytes */
    f = fopen("endian.bin", "rb");
    uint8_t bytes[4];
    fread(bytes, 1, 4, f);
    fclose(f);

    printf("Value: 0x%08X\n", value);
    printf("Bytes in file: %02X %02X %02X %02X\n",
           bytes[0], bytes[1], bytes[2], bytes[3]);

    if (bytes[0] == 0x78) {
        printf("System is LITTLE-ENDIAN (least significant byte first)\n");
    } else {
        printf("System is BIG-ENDIAN (most significant byte first)\n");
    }

    printf("\nFor network protocols, always use htonl/ntohl:\n");
    printf("  Before sending: network_value = htonl(host_value);\n");
    printf("  After receiving: host_value = ntohl(network_value);\n");

    remove("endian.bin");
}

/* ============================================================================
MAIN
============================================================================
*/

int main(void) {
    printf("\n================================================\n");
    printf("  Module 02: Binary Files\n");
    printf("================================================\n");

    test_records();
    test_header();
    test_seeking();
    test_endianness();

    printf("\n================================================\n");
    printf("  Binary Files Complete!\n");
    printf("  Next: buffered_io.c\n");
    printf("================================================\n\n");

    return 0;
}
