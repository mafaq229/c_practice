/*
CS-6200 Preparation - Module 02: File Copy

Learn low-level file I/O using open/read/write/close.
These are the same calls used with sockets!

Compile: clang -Wall -Wextra -g file_copy.c -o file_copy
Run:     ./file_copy source.txt dest.txt

Difficulty: [MEDIUM]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>      /* open() flags */
#include <unistd.h>     /* read(), write(), close() */
#include <errno.h>
#include <sys/stat.h>

#define BUFFER_SIZE 4096

/*
TODO: Copy a file using low-level I/O.
Returns 0 on success, -1 on error.
*/
int copy_file_lowlevel(const char *src_path, const char *dst_path) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Open source file for reading: open(src_path, O_RDONLY)
     * 2. Open dest file for writing: open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0644)
     * 3. Loop:
     *    a. Read chunk from source into buffer
     *    b. If read returns 0, we're done
     *    c. Write chunk to destination
     *    d. Handle partial writes!
     * 4. Close both files
     * 5. Return 0 on success
     *
     * IMPORTANT: write() may not write all bytes! Loop until all are written.
     */

    // Single-line comment uses //; block comment uses /* ... */ and can span lines.
    int src_file = open(src_path, O_RDONLY);
    if (src_file < 0) return -1;
    // write only access + create if not exists + truc to 0 if file exists flags. 
    // 0644 is file permissions used only if the file is created. Octal: rw-r--r-- (owner: read+write, group: read, others: read)
    // third argument is only required if we pass O_CREAT flag
    int dst_file = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_file < 0) {
        close(src_file);
        return -1;
    }
    char buf[BUFFER_SIZE];  // arrays “decay” to a pointer to their first element
    ssize_t read_chunk; // signed type for sizes and byte counts (unsigned is size_t). read and write can return -1
    while ((read_chunk = read(src_file, buf, sizeof(buf))) > 0) {
        ssize_t total_written = 0;
        while (total_written < read_chunk) {
            // Advance the buffer pointer past bytes already written (buf); write the remaining bytes from buffer (nbyte).
            ssize_t write_chunk = write(dst_file, buf + total_written,
                                        (size_t)(read_chunk - total_written));
            if (write_chunk < 0) {
                // EINTR means the syscall was interrupted by a signal; retry.
                if (errno == EINTR) continue;
                close(src_file);
                close(dst_file);
                return -1;
            }
            total_written += write_chunk;
        }
    }

    if (read_chunk == -1) {
        close(src_file);
        close(dst_file);
        return -1;
    }

    close(src_file);
    close(dst_file);
    return 0;  /* TODO: Fix this */
}

/*
Helper: Write all bytes (handles partial writes)
*/
ssize_t write_all(int fd, const void *buf, size_t count) {
    const char *ptr = buf;
    size_t remaining = count;

    while (remaining > 0) {
        ssize_t written = write(fd, ptr, remaining);
        if (written < 0) {
            if (errno == EINTR) continue;  /* Interrupted, retry */
            return -1;  /* Real error */
        }
        ptr += written;
        remaining -= written;
    }

    return count;
}

/*
Alternative: Copy using standard C library (fopen, fread, fwrite)
*/
int copy_file_stdio(const char *src_path, const char *dst_path) {
    FILE *src = fopen(src_path, "rb");
    if (src == NULL) {
        perror("fopen source");
        return -1;
    }

    FILE *dst = fopen(dst_path, "wb");
    if (dst == NULL) {
        perror("fopen dest");
        fclose(src);
        return -1;
    }

    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, src)) > 0) {
        size_t bytes_written = fwrite(buffer, 1, bytes_read, dst);
        if (bytes_written != bytes_read) {
            perror("fwrite");
            fclose(src);
            fclose(dst);
            return -1;
        }
    }

    if (ferror(src)) {
        perror("fread");
        fclose(src);
        fclose(dst);
        return -1;
    }

    fclose(src);
    fclose(dst);
    return 0;
}

/*
Get file size
*/
off_t get_file_size(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        return -1;
    }
    return st.st_size;
}

/*
Test the implementation
*/
void run_tests(void) {
    printf("\n=== File Copy Tests ===\n");

    /* Create a test file */
    const char *test_content = "This is a test file.\n"
                               "It has multiple lines.\n"
                               "Testing 1, 2, 3...\n";

    FILE *f = fopen("test_source.txt", "w");
    if (f == NULL) {
        perror("Failed to create test file");
        return;
    }
    fputs(test_content, f);
    fclose(f);

    printf("Created test_source.txt (%ld bytes)\n",
           (long)get_file_size("test_source.txt"));

    /* Test low-level copy */
    printf("\nTesting low-level copy...\n");
    if (copy_file_lowlevel("test_source.txt", "test_dest_low.txt") == 0) {
        off_t src_size = get_file_size("test_source.txt");
        off_t dst_size = get_file_size("test_dest_low.txt");
        if (src_size == dst_size) {
            printf("SUCCESS: Files match (%ld bytes)\n", (long)src_size);
        } else {
            printf("FAIL: Size mismatch (src=%ld, dst=%ld)\n",
                   (long)src_size, (long)dst_size);
        }
    } else {
        printf("Low-level copy not implemented yet.\n");
    }

    /* Test stdio copy */
    printf("\nTesting stdio copy...\n");
    if (copy_file_stdio("test_source.txt", "test_dest_stdio.txt") == 0) {
        off_t src_size = get_file_size("test_source.txt");
        off_t dst_size = get_file_size("test_dest_stdio.txt");
        if (src_size == dst_size) {
            printf("SUCCESS: Files match (%ld bytes)\n", (long)src_size);
        } else {
            printf("FAIL: Size mismatch\n");
        }
    }

    /* Cleanup */
    unlink("test_source.txt");
    unlink("test_dest_low.txt");
    unlink("test_dest_stdio.txt");
    printf("\nTest files cleaned up.\n");
}

int main(int argc, char *argv[]) {
    printf("\n================================================\n");
    printf("  Module 02: File Copy\n");
    printf("================================================\n");

    if (argc == 3) {
        /* Copy mode */
        printf("Copying %s to %s...\n", argv[1], argv[2]);
        if (copy_file_lowlevel(argv[1], argv[2]) == 0) {
            printf("Copy successful!\n");
        } else {
            printf("Copy failed.\n");
            return 1;
        }
    } else if (argc == 1) {
        /* Test mode */
        run_tests();
    } else {
        printf("Usage: %s [source] [destination]\n", argv[0]);
        printf("       %s                        (run tests)\n", argv[0]);
        return 1;
    }

    printf("\n================================================\n\n");
    return 0;
}
