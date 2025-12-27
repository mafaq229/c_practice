/*
 * CS-6200 Preparation - Module 06: Shared Memory Reader
 *
 * Reads from shared memory created by shm_writer.
 *
 * NOTE: This requires Linux - use Docker on macOS!
 *
 * Usage:
 *   Terminal 1: ./shm_writer
 *   Terminal 2: ./shm_reader
 *
 * Compile: clang -Wall -Wextra -g shm_reader.c -o shm_reader -lrt -pthread
 * Run:     ./shm_reader
 *
 * Difficulty: [HARD]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

#define SHM_NAME "/gios_prep_shm"
#define SHM_SIZE 4096

/* Shared data structure - must match writer! */
typedef struct {
    int counter;
    char message[256];
    int ready;
} SharedData;

int main(void) {
    printf("\n================================================\n");
    printf("  Module 06: Shared Memory Reader\n");
    printf("================================================\n\n");

    printf("Attempting to open shared memory '%s'...\n", SHM_NAME);
    printf("(Make sure shm_writer is running first!)\n\n");

    /*
     * Open existing shared memory (no O_CREAT)
     */
    int fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    if (fd < 0) {
        perror("shm_open");
        if (errno == ENOENT) {
            printf("Shared memory doesn't exist. Run shm_writer first!\n");
        }
        return 1;
    }

    printf("Opened shared memory, fd: %d\n", fd);

    /*
     * Map shared memory (read-only for safety)
     */
    void *ptr = mmap(NULL, SHM_SIZE, PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    printf("Mapped at address: %p\n\n", ptr);
    close(fd);

    SharedData *data = (SharedData *)ptr;

    /*
     * Read data from shared memory
     *
     * NOTE: This is a simple polling approach.
     * In real code, you'd use semaphores or mutexes for synchronization!
     */
    printf("Reading from shared memory (Ctrl+C to stop)...\n\n");

    int last_counter = 0;
    int iterations = 0;
    int max_iterations = 20;

    while (iterations < max_iterations) {
        if (data->ready && data->counter != last_counter) {
            printf("  Read: counter=%d, message='%s'\n",
                   data->counter, data->message);
            last_counter = data->counter;

            if (strcmp(data->message, "Writer finished!") == 0) {
                printf("\nWriter signaled completion.\n");
                break;
            }
        }

        usleep(100000);  /* Poll every 100ms */
        iterations++;
    }

    /*
     * Cleanup - just unmap, don't unlink (writer does that)
     */
    if (munmap(ptr, SHM_SIZE) < 0) {
        perror("munmap");
    }

    printf("\n================================================\n");
    printf("  Reader Complete!\n");
    printf("================================================\n\n");

    return 0;
}
