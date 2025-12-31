/*
CS-6200 Preparation - Module 06: Shared Memory Writer

POSIX shared memory for inter-process communication.
This is essential for GIOS Project 3 (proxy cache)!

NOTE: This requires Linux - use Docker on macOS!

Compile: clang -Wall -Wextra -g shm_writer.c -o shm_writer -lrt -pthread
Run:     ./shm_writer

Difficulty: [HARD]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

/* ============================================================================
CONCEPT: POSIX Shared Memory
============================================================================

Shared memory allows multiple processes to access the same memory region.
It's the fastest IPC mechanism because data doesn't need to be copied.

Key functions:
- shm_open():    Create/open shared memory object
- ftruncate():   Set size of shared memory
- mmap():        Map shared memory into process address space
- munmap():      Unmap when done
- shm_unlink():  Remove shared memory object

GIOS Connection:
In Project 3, the proxy and cache processes share memory:
- Cache writes file data to shared memory
- Proxy reads from shared memory and sends to client
*/

#define SHM_NAME "/gios_prep_shm"
#define SHM_SIZE 4096

/* Shared data structure */
typedef struct {
    int counter;
    char message[256];
    int ready;  /* Flag to indicate data is ready */
} SharedData;

int main(void) {
    printf("\n================================================\n");
    printf("  Module 06: Shared Memory Writer\n");
    printf("================================================\n\n");

    /*
     * Step 1: Create shared memory object
     *
     * shm_open() creates a file descriptor for shared memory.
     * O_CREAT: Create if doesn't exist
     * O_RDWR:  Read/write access
     * 0666:    Permissions
     */
    printf("Creating shared memory object '%s'...\n", SHM_NAME);

    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd < 0) {
        perror("shm_open");
        if (errno == EACCES) {
            printf("Note: On macOS, use Docker for POSIX shared memory.\n");
        }
        return 1;
    }

    printf("Shared memory fd: %d\n", fd);

    /*
     * Step 2: Set the size of shared memory
     *
     * ftruncate() sets the size. This is required after creation.
     */
    printf("Setting size to %d bytes...\n", SHM_SIZE);

    if (ftruncate(fd, SHM_SIZE) < 0) {
        perror("ftruncate");
        close(fd);
        shm_unlink(SHM_NAME);
        return 1;
    }

    /*
     * Step 3: Map shared memory into our address space
     *
     * mmap() returns a pointer to the shared memory region.
     * MAP_SHARED: Changes are visible to other processes
     */
    printf("Mapping shared memory...\n");

    void *ptr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        shm_unlink(SHM_NAME);
        return 1;
    }

    printf("Mapped at address: %p\n", ptr);

    /* File descriptor can be closed after mmap */
    close(fd);

    /*
     * Step 4: Use the shared memory
     */
    SharedData *data = (SharedData *)ptr;

    printf("\nWriting data to shared memory...\n");

    data->counter = 0;
    data->ready = 0;

    for (int i = 0; i < 5; i++) {
        data->counter = i + 1;
        snprintf(data->message, sizeof(data->message),
                "Message #%d from writer (PID %d)", i + 1, getpid());
        data->ready = 1;

        printf("  Wrote: counter=%d, message='%s'\n",
               data->counter, data->message);

        sleep(2);  /* Give reader time to read */
        data->ready = 0;
        sleep(1);
    }

    printf("\nWriter done. Setting final message...\n");
    strncpy(data->message, "Writer finished!", sizeof(data->message));
    data->ready = 1;

    sleep(2);

    /*
     * Step 5: Cleanup
     */
    printf("Unmapping and unlinking shared memory...\n");

    if (munmap(ptr, SHM_SIZE) < 0) {
        perror("munmap");
    }

    /* Unlink removes the shared memory object */
    if (shm_unlink(SHM_NAME) < 0) {
        perror("shm_unlink");
    }

    printf("\n================================================\n");
    printf("  Writer Complete!\n");
    printf("================================================\n\n");

    return 0;
}
