/*
 * CS-6200 Preparation - Module 02: Buffered vs Unbuffered I/O
 *
 * Understanding the difference between buffered and unbuffered I/O
 * is important for performance optimization.
 *
 * Compile: clang -Wall -Wextra -g -O2 buffered_io.c -o buffered_io
 * Run:     ./buffered_io
 *
 * Difficulty: [MEDIUM]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define FILE_SIZE (10 * 1024 * 1024)  /* 10 MB */

/*
 * Get current time in microseconds
 */
long long get_time_us(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)tv.tv_sec * 1000000 + tv.tv_usec;
}

/*
 * Create a test file of specified size
 */
void create_test_file(const char *filename, size_t size) {
    printf("Creating %zu byte test file...\n", size);

    FILE *f = fopen(filename, "wb");
    if (!f) {
        perror("Failed to create test file");
        return;
    }

    char buffer[4096];
    memset(buffer, 'X', sizeof(buffer));

    size_t written = 0;
    while (written < size) {
        size_t to_write = sizeof(buffer);
        if (written + to_write > size) {
            to_write = size - written;
        }
        fwrite(buffer, 1, to_write, f);
        written += to_write;
    }

    fclose(f);
    printf("Test file created.\n");
}

/* ============================================================================
 * TEST 1: Single byte reads - Buffered vs Unbuffered
 * ============================================================================
 */

void test_single_byte_reads(void) {
    printf("\n=== Test 1: Single Byte Reads ===\n");
    printf("Reading 100,000 bytes one at a time...\n\n");

    const int bytes_to_read = 100000;
    long long start, end;
    char byte;

    /* Unbuffered (read syscall) */
    int fd = open("test_file.bin", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return;
    }

    start = get_time_us();
    for (int i = 0; i < bytes_to_read; i++) {
        read(fd, &byte, 1);  /* System call for EACH byte! */
    }
    end = get_time_us();
    close(fd);

    printf("Unbuffered (read syscall):  %lld us\n", end - start);

    /* Buffered (fgetc) */
    FILE *f = fopen("test_file.bin", "rb");
    if (!f) {
        perror("fopen");
        return;
    }

    start = get_time_us();
    for (int i = 0; i < bytes_to_read; i++) {
        byte = fgetc(f);  /* Reads from internal buffer */
    }
    end = get_time_us();
    fclose(f);

    printf("Buffered (fgetc):           %lld us\n", end - start);

    printf("\nBuffered I/O is MUCH faster for small reads!\n");
    printf("stdio maintains an internal buffer (usually 4-8KB).\n");
}

/* ============================================================================
 * TEST 2: Large reads - Buffer size impact
 * ============================================================================
 */

void test_buffer_sizes(void) {
    printf("\n=== Test 2: Buffer Size Impact ===\n");
    printf("Reading entire file with different buffer sizes...\n\n");

    int buffer_sizes[] = {1, 16, 256, 4096, 65536, 262144};
    int num_sizes = sizeof(buffer_sizes) / sizeof(buffer_sizes[0]);

    for (int i = 0; i < num_sizes; i++) {
        int buf_size = buffer_sizes[i];
        char *buffer = malloc(buf_size);
        if (!buffer) continue;

        int fd = open("test_file.bin", O_RDONLY);
        if (fd < 0) continue;

        long long start = get_time_us();
        ssize_t total = 0;
        ssize_t n;
        while ((n = read(fd, buffer, buf_size)) > 0) {
            total += n;
        }
        long long end = get_time_us();

        close(fd);
        free(buffer);

        printf("Buffer %6d bytes: %8lld us  (read %zd bytes)\n",
               buf_size, end - start, total);
    }

    printf("\nOptimal buffer size is typically 4KB-64KB.\n");
    printf("Larger buffers have diminishing returns.\n");
}

/* ============================================================================
 * TEST 3: Write performance
 * ============================================================================
 */

void test_write_performance(void) {
    printf("\n=== Test 3: Write Performance ===\n");
    printf("Writing 1MB with different methods...\n\n");

    const int size = 1024 * 1024;  /* 1 MB */
    char *data = malloc(size);
    if (!data) return;
    memset(data, 'A', size);

    long long start, end;

    /* Single large write */
    int fd = open("test_write.bin", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    start = get_time_us();
    write(fd, data, size);
    fsync(fd);  /* Force write to disk */
    end = get_time_us();
    close(fd);
    printf("Single 1MB write:          %8lld us\n", end - start);

    /* Many small writes (1KB each) */
    fd = open("test_write.bin", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    start = get_time_us();
    for (int i = 0; i < size; i += 1024) {
        write(fd, data + i, 1024);
    }
    fsync(fd);
    end = get_time_us();
    close(fd);
    printf("1024 x 1KB writes:         %8lld us\n", end - start);

    /* Buffered writes (fprintf equivalent) */
    FILE *f = fopen("test_write.bin", "wb");
    start = get_time_us();
    for (int i = 0; i < size; i += 1024) {
        fwrite(data + i, 1, 1024, f);
    }
    fflush(f);
    end = get_time_us();
    fclose(f);
    printf("1024 x 1KB fwrite:         %8lld us\n", end - start);

    free(data);
    remove("test_write.bin");
}

/* ============================================================================
 * TEST 4: fsync and data integrity
 * ============================================================================
 */

void test_fsync(void) {
    printf("\n=== Test 4: fsync for Data Integrity ===\n");

    const int size = 1024 * 1024;
    char *data = malloc(size);
    if (!data) return;
    memset(data, 'B', size);

    long long start, end;

    /* Without fsync */
    int fd = open("test_sync.bin", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    start = get_time_us();
    write(fd, data, size);
    close(fd);  /* close() doesn't guarantee data is on disk! */
    end = get_time_us();
    printf("Write + close (no fsync):  %8lld us\n", end - start);

    /* With fsync */
    fd = open("test_sync.bin", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    start = get_time_us();
    write(fd, data, size);
    fsync(fd);  /* Forces data to physical storage */
    close(fd);
    end = get_time_us();
    printf("Write + fsync + close:     %8lld us\n", end - start);

    printf("\nfsync ensures data is on disk (important for databases).\n");
    printf("For network file transfer, fsync is usually not needed.\n");

    free(data);
    remove("test_sync.bin");
}

/* ============================================================================
 * SUMMARY
 * ============================================================================
 */

void print_summary(void) {
    printf("\n=== I/O Performance Summary ===\n\n");

    printf("1. BUFFERING:\n");
    printf("   - Use buffered I/O (fread/fwrite) for many small operations\n");
    printf("   - Use unbuffered I/O (read/write) for large blocks\n");
    printf("   - Sockets are unbuffered - buffer yourself!\n\n");

    printf("2. BUFFER SIZE:\n");
    printf("   - 4KB-64KB is optimal for most cases\n");
    printf("   - Match to page size (4KB) or disk block size\n");
    printf("   - For GIOS projects, 4096 is a good choice\n\n");

    printf("3. PARTIAL OPERATIONS:\n");
    printf("   - read() and write() may not transfer all bytes\n");
    printf("   - Always loop until all data is transferred\n");
    printf("   - This is CRITICAL for socket programming!\n\n");

    printf("4. SYNCING:\n");
    printf("   - fflush() flushes stdio buffer to kernel\n");
    printf("   - fsync() flushes kernel buffer to disk\n");
    printf("   - close() doesn't guarantee data is on disk\n\n");
}

/* ============================================================================
 * MAIN
 * ============================================================================
 */

int main(void) {
    printf("\n================================================\n");
    printf("  Module 02: Buffered vs Unbuffered I/O\n");
    printf("================================================\n");

    /* Create test file */
    create_test_file("test_file.bin", FILE_SIZE);

    /* Run tests */
    test_single_byte_reads();
    test_buffer_sizes();
    test_write_performance();
    test_fsync();

    /* Cleanup */
    remove("test_file.bin");

    /* Print summary */
    print_summary();

    printf("================================================\n");
    printf("  Buffered I/O Complete!\n");
    printf("  Next: Module 03 - Socket Basics\n");
    printf("================================================\n\n");

    return 0;
}
