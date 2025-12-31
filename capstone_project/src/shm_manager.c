/*
shm_manager.c - Shared Memory Manager Implementation

This implements POSIX shared memory management for inter-process
communication in Part D.

NOTE: This requires Linux! On macOS, use Docker.

Key concepts:
- shm_open() creates/opens shared memory
- mmap() maps it into address space
- Synchronization with process-shared mutex
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "../include/shm_manager.h"

/* Check for Linux (POSIX shared memory) */
#ifdef __linux__
#define HAS_SHM 1
#else
#define HAS_SHM 0
#endif

/* ============================================================================
Helper Functions
============================================================================ */

/*
Calculate number of blocks needed for a given size
*/
static int blocks_needed(size_t size) {
    return (size + SHM_BLOCK_SIZE - 1) / SHM_BLOCK_SIZE;
}

/*
Initialize process-shared mutex
*/
static int init_shared_mutex(pthread_mutex_t *mutex) {
    /*
     * TODO: Initialize a process-shared mutex
     *
     * Steps:
     * 1. Create mutex attribute: pthread_mutexattr_t attr;
     * 2. Initialize: pthread_mutexattr_init(&attr);
     * 3. Set process-shared: pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
     * 4. Initialize mutex: pthread_mutex_init(mutex, &attr);
     * 5. Destroy attribute: pthread_mutexattr_destroy(&attr);
     *
     * The PTHREAD_PROCESS_SHARED attribute allows the mutex
     * to be used between processes via shared memory!
     */

#if HAS_SHM
    pthread_mutexattr_t attr;

    /* YOUR CODE HERE */

    (void)mutex;
    (void)attr;
#else
    (void)mutex;
    fprintf(stderr, "Shared memory not supported on this platform\n");
#endif

    return -1;  /* Placeholder */
}

/* ============================================================================
Shared Memory Lifecycle
============================================================================ */

/*
shm_create - Create a new shared memory segment
*/
shm_handle_t *shm_create(const char *name, size_t size) {
    /*
     * TODO: Implement shared memory creation
     *
     * Steps:
     * 1. Allocate shm_handle_t structure
     *
     * 2. Calculate total size:
     *    - Header: SHM_HEADER_SIZE
     *    - Bitmap: (SHM_MAX_BLOCKS / 8) bytes
     *    - Data: size bytes
     *    total = SHM_HEADER_SIZE + bitmap_size + size
     *
     * 3. Create shared memory:
     *    fd = shm_open(name, O_CREAT | O_EXCL | O_RDWR, 0666);
     *    O_EXCL ensures we create new, fail if exists
     *
     * 4. Set size:
     *    ftruncate(fd, total);
     *
     * 5. Map into address space:
     *    base = mmap(NULL, total, PROT_READ | PROT_WRITE,
     *                MAP_SHARED, fd, 0);
     *
     * 6. Initialize header (at beginning of mapped region):
     *    header = (shm_header_t *)base;
     *    header->magic = SHM_MAGIC;
     *    header->version = 1;
     *    header->total_size = size;
     *    header->block_size = SHM_BLOCK_SIZE;
     *    header->num_blocks = blocks_needed(size);
     *    header->free_blocks = header->num_blocks;
     *    header->next_free = 0;
     *    init_shared_mutex(&header->mutex);
     *
     * 7. Initialize bitmap (all zeros = all free):
     *    memset(bitmap, 0, bitmap_size);
     *
     * 8. Set handle fields and return
     */

#if HAS_SHM
    printf("Creating shared memory: %s, size=%zu\n", name, size);

    /* YOUR CODE HERE */

    (void)name;
    (void)size;

    return NULL;  /* Placeholder */
#else
    fprintf(stderr, "Error: Shared memory requires Linux. Use Docker.\n");
    fprintf(stderr, "Run: docker-compose run dev\n");
    (void)name;
    (void)size;
    return NULL;
#endif
}

/*
shm_attach - Attach to an existing shared memory segment
*/
shm_handle_t *shm_attach(const char *name) {
    /*
     * TODO: Implement attaching to existing shared memory
     *
     * Steps:
     * 1. Allocate shm_handle_t
     *
     * 2. Open existing shared memory:
     *    fd = shm_open(name, O_RDWR, 0);
     *    (no O_CREAT - must exist)
     *
     * 3. Get size using fstat()
     *
     * 4. Map into address space
     *
     * 5. Verify magic number:
     *    if (header->magic != SHM_MAGIC) {
     *        // Invalid or corrupted segment!
     *    }
     *
     * 6. Set handle fields and return
     */

#if HAS_SHM
    printf("Attaching to shared memory: %s\n", name);

    /* YOUR CODE HERE */

    (void)name;

    return NULL;  /* Placeholder */
#else
    fprintf(stderr, "Error: Shared memory requires Linux. Use Docker.\n");
    (void)name;
    return NULL;
#endif
}

/*
shm_detach - Detach from shared memory (don't destroy)
*/
void shm_detach(shm_handle_t *handle) {
    /*
     * TODO: Implement detachment
     *
     * Steps:
     * 1. If handle is NULL, return
     * 2. Unmap memory: munmap(handle->base, handle->size);
     * 3. Close file descriptor: close(handle->fd);
     * 4. Free handle structure
     *
     * Don't call shm_unlink - that destroys the segment!
     */

    if (handle == NULL) {
        return;
    }

#if HAS_SHM
    /* YOUR CODE HERE */
#endif

    free(handle);
}

/*
shm_destroy - Destroy shared memory segment
*/
void shm_destroy(shm_handle_t *handle) {
    /*
     * TODO: Implement destruction
     *
     * Steps:
     * 1. If handle is NULL, return
     * 2. If creator:
     *    - Destroy mutex: pthread_mutex_destroy(&handle->header->mutex);
     *    - Unlink: shm_unlink(handle->name);
     * 3. Unmap and close like shm_detach
     * 4. Free handle
     *
     * Only the creator should call shm_unlink!
     */

    if (handle == NULL) {
        return;
    }

#if HAS_SHM
    printf("Destroying shared memory: %s\n", handle->name);

    /* YOUR CODE HERE */
#endif

    free(handle);
}

/* ============================================================================
Memory Allocation
============================================================================ */

/*
shm_alloc - Allocate space in shared memory
*/
void *shm_alloc(shm_handle_t *handle, size_t size, size_t *offset) {
    /*
     * TODO: Implement allocation
     *
     * Steps:
     * 1. Lock header mutex
     *
     * 2. Calculate blocks needed: blocks_needed(size)
     *
     * 3. Find contiguous free blocks:
     *    - Scan bitmap for 'n' consecutive zero bits
     *    - Start from header->next_free as hint
     *
     * 4. If not enough free blocks, unlock and return NULL
     *
     * 5. Mark blocks as used in bitmap:
     *    - Set bits to 1
     *
     * 6. Update header:
     *    - header->free_blocks -= n;
     *    - Update next_free hint
     *
     * 7. Calculate offset and pointer:
     *    - offset = first_block * SHM_BLOCK_SIZE
     *    - ptr = handle->data + offset
     *
     * 8. Unlock mutex
     *
     * 9. Return pointer, set *offset
     *
     * Bitmap manipulation hints:
     *   byte_index = block / 8;
     *   bit_index = block % 8;
     *   bitmap[byte_index] |= (1 << bit_index);  // Set bit
     *   bitmap[byte_index] & (1 << bit_index);   // Check bit
     */

    if (handle == NULL || size == 0 || offset == NULL) {
        return NULL;
    }

#if HAS_SHM
    /* YOUR CODE HERE */

    (void)size;
    (void)offset;
#else
    (void)size;
    (void)offset;
#endif

    return NULL;  /* Placeholder */
}

/*
shm_free - Free previously allocated space
*/
void shm_free(shm_handle_t *handle, size_t offset, size_t size) {
    /*
     * TODO: Implement deallocation
     *
     * Steps:
     * 1. Lock header mutex
     * 2. Calculate first block: offset / SHM_BLOCK_SIZE
     * 3. Calculate number of blocks: blocks_needed(size)
     * 4. Clear bits in bitmap
     * 5. Update header->free_blocks
     * 6. Update next_free hint
     * 7. Unlock mutex
     */

    if (handle == NULL) {
        return;
    }

#if HAS_SHM
    /* YOUR CODE HERE */

    (void)offset;
    (void)size;
#else
    (void)offset;
    (void)size;
#endif
}

/*
shm_get_ptr - Get pointer to data at offset
*/
void *shm_get_ptr(shm_handle_t *handle, size_t offset) {
    /*
     * TODO: Implement pointer retrieval
     *
     * Simply return handle->data + offset
     * Validate offset is within bounds!
     */

    if (handle == NULL) {
        return NULL;
    }

#if HAS_SHM
    /* YOUR CODE HERE */

    (void)offset;
#else
    (void)offset;
#endif

    return NULL;  /* Placeholder */
}

/* ============================================================================
Utility Functions
============================================================================ */

size_t shm_get_free_space(shm_handle_t *handle) {
    if (handle == NULL || handle->header == NULL) {
        return 0;
    }
    return handle->header->free_blocks * SHM_BLOCK_SIZE;
}

size_t shm_get_used_space(shm_handle_t *handle) {
    if (handle == NULL || handle->header == NULL) {
        return 0;
    }
    return (handle->header->num_blocks - handle->header->free_blocks) * SHM_BLOCK_SIZE;
}

void shm_dump_stats(shm_handle_t *handle) {
    if (handle == NULL || handle->header == NULL) {
        printf("No shared memory handle\n");
        return;
    }

    printf("\n=== Shared Memory Stats ===\n");
    printf("Name: %s\n", handle->name);
    printf("Total blocks: %d\n", handle->header->num_blocks);
    printf("Free blocks: %d\n", handle->header->free_blocks);
    printf("Used space: %zu bytes\n", shm_get_used_space(handle));
    printf("Free space: %zu bytes\n", shm_get_free_space(handle));
    printf("===========================\n");
}
