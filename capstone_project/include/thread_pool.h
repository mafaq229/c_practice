/*
 * thread_pool.h - Thread Pool for Multi-Threaded Server
 *
 * This header defines the thread pool interface used in Part B.
 * The thread pool uses the boss-worker pattern:
 * - Boss (main thread): Accepts connections, adds to work queue
 * - Workers: Pull work from queue, handle client requests
 *
 * This pattern is critical for GIOS Project 1 Part 2!
 */

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>
#include <stdbool.h>
#include "work_queue.h"

/* ============================================================================
 * Constants
 * ============================================================================ */

/* Default number of worker threads */
#define DEFAULT_NUM_THREADS     4

/* ============================================================================
 * Thread Pool Structure
 * ============================================================================ */

/*
 * Thread pool state
 *
 * TODO: You'll need to track:
 * - Array of worker thread IDs
 * - Number of threads
 * - Work queue
 * - Shutdown flag
 * - Synchronization primitives
 */
typedef struct {
    pthread_t *threads;         /* Array of worker thread IDs */
    int num_threads;            /* Number of worker threads */
    work_queue_t *queue;        /* Shared work queue */

    /* Shutdown coordination */
    bool shutdown;              /* Signal workers to exit */
    pthread_mutex_t lock;       /* Protects shutdown flag */

    /* Statistics (optional, for Part E) */
    int tasks_completed;        /* Total tasks handled */
    int active_workers;         /* Currently busy workers */
    pthread_mutex_t stats_lock; /* Protects statistics */

} thread_pool_t;

/* ============================================================================
 * Function Prototypes
 * ============================================================================ */

/*
 * thread_pool_create - Create and start a thread pool
 *
 * @param num_threads: Number of worker threads to create
 * @return: Pointer to thread pool, or NULL on error
 *
 * Steps:
 * 1. Allocate thread_pool_t structure
 * 2. Initialize work queue
 * 3. Initialize mutexes
 * 4. Create worker threads (each runs worker_function)
 *
 * All workers should start waiting on the work queue immediately.
 */
thread_pool_t *thread_pool_create(int num_threads);

/*
 * thread_pool_submit - Submit a task to the thread pool
 *
 * @param pool: Thread pool
 * @param client_fd: Client socket file descriptor
 * @return: 0 on success, -1 on error
 *
 * This function:
 * 1. Creates a work item with the client fd
 * 2. Adds it to the work queue
 * 3. Returns immediately (non-blocking)
 *
 * The work queue will signal waiting workers.
 */
int thread_pool_submit(thread_pool_t *pool, int client_fd);

/*
 * thread_pool_destroy - Shut down and destroy thread pool
 *
 * @param pool: Thread pool to destroy
 *
 * Clean shutdown steps:
 * 1. Set shutdown flag
 * 2. Signal all workers (broadcast on condition variable)
 * 3. Wait for all workers to finish (pthread_join)
 * 4. Destroy work queue
 * 5. Destroy mutexes
 * 6. Free memory
 *
 * IMPORTANT: Handle any pending work items appropriately
 * (close remaining client sockets).
 */
void thread_pool_destroy(thread_pool_t *pool);

/*
 * thread_pool_get_stats - Get thread pool statistics
 *
 * @param pool: Thread pool
 * @param tasks_completed: Output for completed task count
 * @param active_workers: Output for currently active workers
 *
 * Used for monitoring in Part E.
 */
void thread_pool_get_stats(thread_pool_t *pool, int *tasks_completed, int *active_workers);

/* ============================================================================
 * Internal Functions (called by workers)
 * ============================================================================ */

/*
 * worker_function - Entry point for worker threads
 *
 * @param arg: Pointer to thread_pool_t
 * @return: NULL
 *
 * Worker loop:
 * 1. Lock mutex / wait on condition
 * 2. Check for shutdown
 * 3. Pop work item from queue
 * 4. Unlock mutex
 * 5. Handle client request
 * 6. Update statistics
 * 7. Repeat
 *
 * Exit gracefully when shutdown is signaled.
 */
void *worker_function(void *arg);

#endif /* THREAD_POOL_H */
