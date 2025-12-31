/*
CS-6200 Preparation - Module 06: Synchronized Shared Memory

Demonstrates proper synchronization with shared memory using semaphores.
This is how GIOS Project 3 coordinates proxy and cache!

NOTE: This requires Linux - use Docker on macOS!

Compile: clang -Wall -Wextra -g shm_sync.c -o shm_sync -lrt -pthread
Run:     ./shm_sync

Difficulty: [HARD]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <errno.h>

#define SHM_NAME "/gios_sync_shm"
#define SEM_WRITER "/gios_sem_writer"
#define SEM_READER "/gios_sem_reader"
#define SHM_SIZE 4096
#define NUM_MESSAGES 5

/* ============================================================================
CONCEPT: Synchronized Shared Memory
============================================================================

Without synchronization, shared memory access is unsafe:
- Writer might overwrite data before reader reads it
- Reader might read incomplete/inconsistent data

Solution: Use semaphores to coordinate access
- Writer waits for reader to be ready
- Reader waits for writer to produce data
- Classic producer-consumer pattern!
*/

/* Shared data structure */
typedef struct {
    int sequence;
    char data[256];
    int done;
} SharedBuffer;

/*
Writer process
*/
void run_writer(void) {
    printf("[Writer] Starting...\n");

    /* Open shared memory */
    int fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (fd < 0) {
        perror("shm_open");
        exit(1);
    }

    SharedBuffer *buf = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE,
                             MAP_SHARED, fd, 0);
    close(fd);

    if (buf == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    /* Open semaphores */
    sem_t *sem_writer = sem_open(SEM_WRITER, 0);
    sem_t *sem_reader = sem_open(SEM_READER, 0);

    if (sem_writer == SEM_FAILED || sem_reader == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    /* Produce data */
    for (int i = 0; i < NUM_MESSAGES; i++) {
        /* Wait for permission to write */
        sem_wait(sem_writer);

        /* Write data */
        buf->sequence = i + 1;
        snprintf(buf->data, sizeof(buf->data),
                "Message %d: Hello from writer (PID %d)!", i + 1, getpid());
        buf->done = (i == NUM_MESSAGES - 1);

        printf("[Writer] Wrote sequence %d: '%s'\n", buf->sequence, buf->data);

        /* Signal reader that data is ready */
        sem_post(sem_reader);

        usleep(500000);  /* Simulate work */
    }

    /* Cleanup */
    sem_close(sem_writer);
    sem_close(sem_reader);
    munmap(buf, SHM_SIZE);

    printf("[Writer] Done!\n");
}

/*
Reader process
*/
void run_reader(void) {
    printf("[Reader] Starting...\n");

    /* Open shared memory */
    int fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    if (fd < 0) {
        perror("shm_open");
        exit(1);
    }

    SharedBuffer *buf = mmap(NULL, SHM_SIZE, PROT_READ, MAP_SHARED, fd, 0);
    close(fd);

    if (buf == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    /* Open semaphores */
    sem_t *sem_writer = sem_open(SEM_WRITER, 0);
    sem_t *sem_reader = sem_open(SEM_READER, 0);

    if (sem_writer == SEM_FAILED || sem_reader == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    /* Consume data */
    while (1) {
        /* Wait for data to be ready */
        sem_wait(sem_reader);

        printf("[Reader] Read sequence %d: '%s'\n", buf->sequence, buf->data);

        int done = buf->done;

        /* Signal writer that we're ready for more */
        sem_post(sem_writer);

        if (done) {
            break;
        }
    }

    /* Cleanup */
    sem_close(sem_writer);
    sem_close(sem_reader);
    munmap(buf, SHM_SIZE);

    printf("[Reader] Done!\n");
}

int main(void) {
    printf("\n================================================\n");
    printf("  Module 06: Synchronized Shared Memory\n");
    printf("================================================\n\n");

    printf("This demonstrates synchronized shared memory access.\n");
    printf("Parent creates shared resources, forks child processes.\n\n");

    /* Create shared memory */
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd < 0) {
        perror("shm_open");
        return 1;
    }

    if (ftruncate(fd, SHM_SIZE) < 0) {
        perror("ftruncate");
        close(fd);
        shm_unlink(SHM_NAME);
        return 1;
    }

    /* Initialize shared buffer */
    SharedBuffer *buf = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE,
                             MAP_SHARED, fd, 0);
    close(fd);

    if (buf == MAP_FAILED) {
        perror("mmap");
        shm_unlink(SHM_NAME);
        return 1;
    }

    memset(buf, 0, sizeof(SharedBuffer));

    /* Create semaphores
     * sem_writer starts at 1: writer can write first
     * sem_reader starts at 0: reader must wait for data
     */
    sem_unlink(SEM_WRITER);  /* Remove if exists */
    sem_unlink(SEM_READER);

    sem_t *sem_writer = sem_open(SEM_WRITER, O_CREAT, 0666, 1);
    sem_t *sem_reader = sem_open(SEM_READER, O_CREAT, 0666, 0);

    if (sem_writer == SEM_FAILED || sem_reader == SEM_FAILED) {
        perror("sem_open");
        munmap(buf, SHM_SIZE);
        shm_unlink(SHM_NAME);
        return 1;
    }

    sem_close(sem_writer);
    sem_close(sem_reader);

    printf("Shared memory and semaphores created.\n");
    printf("Forking writer and reader processes...\n\n");

    /* Fork writer */
    pid_t writer_pid = fork();
    if (writer_pid == 0) {
        run_writer();
        exit(0);
    }

    /* Fork reader */
    pid_t reader_pid = fork();
    if (reader_pid == 0) {
        run_reader();
        exit(0);
    }

    /* Parent waits for both */
    waitpid(writer_pid, NULL, 0);
    waitpid(reader_pid, NULL, 0);

    /* Cleanup */
    printf("\nCleaning up resources...\n");
    munmap(buf, SHM_SIZE);
    shm_unlink(SHM_NAME);
    sem_unlink(SEM_WRITER);
    sem_unlink(SEM_READER);

    printf("\n================================================\n");
    printf("  Synchronized Shared Memory Complete!\n");
    printf("\n");
    printf("  Key Points:\n");
    printf("  - Use semaphores to coordinate access\n");
    printf("  - Writer waits before writing\n");
    printf("  - Reader waits before reading\n");
    printf("  - No data races or lost updates!\n");
    printf("================================================\n\n");

    return 0;
}
