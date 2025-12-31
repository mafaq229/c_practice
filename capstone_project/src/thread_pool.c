/*
thread_pool.c - Thread Pool Implementation

This implements the boss-worker pattern:
- Boss (main thread): accepts connections, adds to work queue
- Workers: pull work from queue, handle client requests

This is the core of GIOS Project 1 Part 2!
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/thread_pool.h"
#include "../include/protocol.h"
#include "../include/socket_utils.h"
#include "../include/file_utils.h"

/* Forward declaration of the client handler */
static void handle_client_request(int client_fd);

/* ============================================================================
Thread Pool Lifecycle
============================================================================ */

/*
thread_pool_create - Create and start a thread pool
*/
thread_pool_t *thread_pool_create(int num_threads) {
    /*
     * TODO: Implement this function
     *
     * Steps:
     * 1. Validate num_threads > 0
     * 2. Allocate thread_pool_t structure
     * 3. Create work queue using work_queue_create()
     * 4. Initialize mutexes (lock, stats_lock)
     * 5. Allocate thread array: malloc(num_threads * sizeof(pthread_t))
     * 6. Initialize statistics (tasks_completed = 0, active_workers = 0)
     * 7. Set shutdown = false
     * 8. Create worker threads:
     *    for (int i = 0; i < num_threads; i++) {
     *        if (pthread_create(&pool->threads[i], NULL,
     *                           worker_function, pool) != 0) {
     *            // Error handling - clean up partially created threads
     *        }
     *    }
     * 9. Store num_threads
     * 10. Return pool
     *
     * Error handling is critical:
     * - If thread creation fails partway, you need to clean up
     * - Set shutdown flag and join already-created threads
     */

    if (num_threads <= 0) {
        fprintf(stderr, "thread_pool_create: invalid num_threads\n");
        return NULL;
    }

    thread_pool_t *pool = malloc(sizeof(thread_pool_t));
    if (pool == NULL) {
        perror("malloc thread_pool");
        return NULL;
    }

    /* Initialize to safe defaults */
    pool->threads = NULL;
    pool->num_threads = 0;
    pool->queue = NULL;
    pool->shutdown = false;
    pool->tasks_completed = 0;
    pool->active_workers = 0;

    /* YOUR CODE HERE */

    /*
     * Hints:
     *
     * pool->queue = work_queue_create();
     * if (pool->queue == NULL) {
     *     free(pool);
     *     return NULL;
     * }
     *
     * pool->threads = malloc(num_threads * sizeof(pthread_t));
     * ...
     *
     * for (int i = 0; i < num_threads; i++) {
     *     if (pthread_create(&pool->threads[i], NULL, worker_function, pool) != 0) {
     *         // Error! Clean up...
     *     }
     * }
     */

    return pool;
}

/*
thread_pool_destroy - Shut down and destroy thread pool
*/
void thread_pool_destroy(thread_pool_t *pool) {
    /*
     * TODO: Implement this function
     *
     * Clean shutdown is important! Steps:
     *
     * 1. Check if pool is NULL
     * 2. Set shutdown flag (thread-safe):
     *    pthread_mutex_lock(&pool->lock);
     *    pool->shutdown = true;
     *    pthread_mutex_unlock(&pool->lock);
     * 3. Signal work queue to shut down:
     *    work_queue_shutdown(pool->queue);
     * 4. Wait for all worker threads to finish:
     *    for (int i = 0; i < pool->num_threads; i++) {
     *        pthread_join(pool->threads[i], NULL);
     *    }
     * 5. Destroy work queue
     * 6. Destroy mutexes
     * 7. Free thread array
     * 8. Free pool structure
     *
     * The order matters! Signal shutdown before joining.
     */

    if (pool == NULL) {
        return;
    }

    printf("Thread pool shutting down...\n");

    /* YOUR CODE HERE */

    free(pool);
}

/* ============================================================================
Task Submission
============================================================================ */

/*
thread_pool_submit - Submit a task (client connection) to the pool
*/
int thread_pool_submit(thread_pool_t *pool, int client_fd) {
    /*
     * TODO: Implement this function
     *
     * Simply add the client_fd to the work queue.
     *
     * Steps:
     * 1. Check if pool is NULL or shutting down
     * 2. Call work_queue_push(pool->queue, client_fd)
     * 3. Return result
     *
     * This is called by the boss thread (main accept loop).
     * The work queue handles all synchronization.
     */

    if (pool == NULL) {
        return -1;
    }

    /* YOUR CODE HERE */

    (void)client_fd;

    return -1;  /* Placeholder */
}

/* ============================================================================
Worker Thread
============================================================================ */

/*
worker_function - Entry point for worker threads
*/
void *worker_function(void *arg) {
    /*
     * TODO: Implement this function
     *
     * This is the main loop for each worker thread.
     *
     * Pattern:
     *   thread_pool_t *pool = (thread_pool_t *)arg;
     *
     *   while (1) {
     *       // Get work from queue (blocks if empty)
     *       int client_fd = work_queue_pop(pool->queue);
     *
     *       // Check for shutdown signal
     *       if (client_fd < 0) {
     *           break;  // Queue signaled shutdown
     *       }
     *
     *       // Update statistics (thread-safe)
     *       pthread_mutex_lock(&pool->stats_lock);
     *       pool->active_workers++;
     *       pthread_mutex_unlock(&pool->stats_lock);
     *
     *       // Handle the client request
     *       handle_client_request(client_fd);
     *
     *       // Close client socket
     *       close(client_fd);
     *
     *       // Update statistics
     *       pthread_mutex_lock(&pool->stats_lock);
     *       pool->active_workers--;
     *       pool->tasks_completed++;
     *       pthread_mutex_unlock(&pool->stats_lock);
     *   }
     *
     *   return NULL;
     */

    thread_pool_t *pool = (thread_pool_t *)arg;
    (void)pool;

    printf("Worker thread started\n");

    /* YOUR CODE HERE */

    printf("Worker thread exiting\n");
    return NULL;
}

/*
handle_client_request - Process a single client request

This is the actual file serving logic.
*/
static void handle_client_request(int client_fd) {
    /*
     * TODO: Implement this function
     *
     * This is where you handle the GETFILE protocol!
     *
     * Steps:
     * 1. Receive request (use recv_until for header)
     * 2. Parse request using gf_parse_request()
     * 3. Validate path using validate_path()
     * 4. Build full path using build_full_path()
     * 5. Check if file exists
     * 6. Get file size
     * 7. Send response header using gf_create_response_header()
     * 8. If OK, send file content using send_file()
     *
     * Error cases:
     * - Invalid request: send GETFILE INVALID
     * - Path validation fails: send GETFILE INVALID
     * - File not found: send GETFILE FILE_NOT_FOUND
     * - Other errors: send GETFILE ERROR
     *
     * Example flow:
     *   char buffer[MAX_HEADER_LEN];
     *   gf_request_t request;
     *
     *   // Receive header
     *   ssize_t n = recv_until(client_fd, buffer, sizeof(buffer), HEADER_DELIM);
     *   if (n <= 0) return;
     *
     *   // Parse request
     *   if (gf_parse_request(buffer, n, &request) <= 0 || !request.valid) {
     *       send_error_response(client_fd, STATUS_INVALID);
     *       return;
     *   }
     *
     *   // Validate and serve file...
     */

    (void)client_fd;

    /* YOUR CODE HERE */

    printf("Handled client request\n");
}

/* ============================================================================
Statistics
============================================================================ */

/*
thread_pool_get_stats - Get thread pool statistics
*/
void thread_pool_get_stats(thread_pool_t *pool, int *tasks_completed, int *active_workers) {
    /*
     * TODO: Implement this function
     *
     * Thread-safe read of statistics.
     *
     * pthread_mutex_lock(&pool->stats_lock);
     * if (tasks_completed) *tasks_completed = pool->tasks_completed;
     * if (active_workers) *active_workers = pool->active_workers;
     * pthread_mutex_unlock(&pool->stats_lock);
     */

    if (pool == NULL) {
        if (tasks_completed) *tasks_completed = 0;
        if (active_workers) *active_workers = 0;
        return;
    }

    /* YOUR CODE HERE */
}
