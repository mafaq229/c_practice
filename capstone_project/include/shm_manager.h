/*
 * shm_manager.h - Shared Memory Manager
 *
 * This header defines the shared memory management interface
 * used for efficient data transfer between processes in Part D.
 *
 * Shared memory layout:
 * +------------------+
 * | Control Header   | <- Metadata, locks, allocation info
 * +------------------+
 * | Allocation Bitmap| <- Tracks which blocks are in use
 * +------------------+
 * | Data Region      | <- Actual file data stored here
 * |                  |
 * +------------------+
 *
 * This avoids copying large files through message queues.
 *
 * NOTE: This requires Linux (POSIX shared memory).
 *       Use Docker on macOS.
 */

#ifndef SHM_MANAGER_H
#define SHM_MANAGER_H

#include <pthread.h>
#include <semaphore.h>
#include <stddef.h>
#include <stdbool.h>

/* ============================================================================
 * Constants
 * ============================================================================ */

/* Block size for allocation (4 KB) */
#define SHM_BLOCK_SIZE          4096

/* Maximum blocks in shared memory */
#define SHM_MAX_BLOCKS          4096    /* 16 MB total */

/* Control header size */
#define SHM_HEADER_SIZE         4096

/* ============================================================================
 * Data Structures
 * ============================================================================ */

/*
 * Shared memory control header
 *
 * This structure is stored at the beginning of the shared memory segment.
 * It must be carefully designed for inter-process synchronization.
 */
typedef struct {
    /* Magic number to verify segment integrity */
    uint32_t magic;
    #define SHM_MAGIC           0x47494F53  /* "GIOS" */

    /* Version for compatibility checking */
    uint32_t version;

    /* Segment configuration */
    size_t total_size;          /* Total size of data region */
    size_t block_size;          /* Size of each block */
    int num_blocks;             /* Number of blocks */

    /* Allocation tracking */
    int free_blocks;            /* Number of free blocks */
    int next_free;              /* Hint for next free block */

    /* Process synchronization */
    pthread_mutex_t mutex;      /* Protects all header fields */
    /* Note: Use pthread_mutexattr_setpshared() for inter-process! */

} shm_header_t;

/*
 * Shared memory handle - per-process state
 *
 * Each process that attaches to shared memory gets one of these.
 */
typedef struct {
    char name[256];             /* Shared memory segment name */
    int fd;                     /* File descriptor from shm_open */
    void *base;                 /* Base address from mmap */
    size_t size;                /* Total mapped size */

    /* Pointers into the mapped region */
    shm_header_t *header;       /* Control header */
    uint8_t *bitmap;            /* Allocation bitmap */
    char *data;                 /* Data region start */

    bool is_creator;            /* True if this process created the segment */

} shm_handle_t;

/* ============================================================================
 * Function Prototypes
 * ============================================================================ */

/*
 * shm_create - Create a new shared memory segment
 *
 * @param name: Segment name (e.g., "/gios_cache_shm")
 * @param size: Total data region size in bytes
 * @return: Handle to shared memory, or NULL on error
 *
 * Steps:
 * 1. shm_open() with O_CREAT | O_EXCL
 * 2. ftruncate() to set size
 * 3. mmap() to map into address space
 * 4. Initialize header and bitmap
 * 5. Initialize mutex with PTHREAD_PROCESS_SHARED
 *
 * The segment persists until shm_unlink() is called.
 */
shm_handle_t *shm_create(const char *name, size_t size);

/*
 * shm_attach - Attach to an existing shared memory segment
 *
 * @param name: Segment name
 * @return: Handle to shared memory, or NULL on error
 *
 * Steps:
 * 1. shm_open() without O_CREAT
 * 2. fstat() to get size
 * 3. mmap() to map into address space
 * 4. Verify magic number and version
 */
shm_handle_t *shm_attach(const char *name);

/*
 * shm_detach - Detach from shared memory (don't destroy)
 *
 * @param handle: Shared memory handle
 *
 * Unmaps the segment but doesn't unlink it.
 * The segment remains available for other processes.
 */
void shm_detach(shm_handle_t *handle);

/*
 * shm_destroy - Destroy shared memory segment
 *
 * @param handle: Shared memory handle
 *
 * Steps:
 * 1. Destroy mutex
 * 2. munmap()
 * 3. close()
 * 4. shm_unlink()
 *
 * Only the creator should call this!
 */
void shm_destroy(shm_handle_t *handle);

/* ============================================================================
 * Memory Allocation Functions
 * ============================================================================ */

/*
 * shm_alloc - Allocate space in shared memory
 *
 * @param handle: Shared memory handle
 * @param size: Number of bytes needed
 * @param offset: Output - offset from start of data region
 * @return: Pointer to allocated space, or NULL if no space
 *
 * Allocates contiguous blocks to satisfy the request.
 * Thread-safe: locks the header mutex.
 *
 * The offset is used in IPC messages to tell the other process
 * where to find the data.
 */
void *shm_alloc(shm_handle_t *handle, size_t size, size_t *offset);

/*
 * shm_free - Free previously allocated space
 *
 * @param handle: Shared memory handle
 * @param offset: Offset returned by shm_alloc
 * @param size: Size that was allocated
 *
 * Marks the blocks as free in the bitmap.
 * Thread-safe: locks the header mutex.
 */
void shm_free(shm_handle_t *handle, size_t offset, size_t size);

/*
 * shm_get_ptr - Get pointer to data at offset
 *
 * @param handle: Shared memory handle
 * @param offset: Offset in data region
 * @return: Pointer to data, or NULL if offset invalid
 *
 * Used to access data after receiving an offset via IPC.
 */
void *shm_get_ptr(shm_handle_t *handle, size_t offset);

/* ============================================================================
 * Utility Functions
 * ============================================================================ */

/*
 * shm_get_free_space - Get amount of free space
 *
 * @param handle: Shared memory handle
 * @return: Free space in bytes
 */
size_t shm_get_free_space(shm_handle_t *handle);

/*
 * shm_get_used_space - Get amount of used space
 *
 * @param handle: Shared memory handle
 * @return: Used space in bytes
 */
size_t shm_get_used_space(shm_handle_t *handle);

/*
 * shm_dump_stats - Print shared memory statistics (debugging)
 *
 * @param handle: Shared memory handle
 */
void shm_dump_stats(shm_handle_t *handle);

#endif /* SHM_MANAGER_H */
