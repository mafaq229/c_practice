/*
 * CS-6200 Preparation - Module 05: Thread Pool Interface
 *
 * This is the boss-worker pattern you'll implement in GIOS Project 1 Part 2!
 *
 * Boss: Main thread that accepts connections and submits work
 * Workers: Thread pool that processes the work queue
 *
 * Difficulty: [HARD]
 */

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>
#include "work_queue.h"

/* Thread pool structure */
typedef struct {
    pthread_t *threads;          /* Array of worker threads */
    int num_threads;             /* Number of worker threads */
    WorkQueue work_queue;        /* Queue of pending work */
    int started;                 /* Pool started flag */
} ThreadPool;

/*
 * Create and start a thread pool.
 *
 * num_threads: Number of worker threads to create
 *
 * Returns pointer to pool on success, NULL on failure.
 */
ThreadPool *thread_pool_create(int num_threads);

/*
 * Submit work to the thread pool.
 *
 * function: Function to execute
 * arg: Argument to pass to function
 *
 * Returns 0 on success, -1 on failure.
 */
int thread_pool_submit(ThreadPool *pool, void (*function)(void *), void *arg);

/*
 * Wait for all work to complete and destroy the pool.
 *
 * This will:
 * 1. Signal shutdown to workers
 * 2. Wait for workers to finish current work
 * 3. Join all worker threads
 * 4. Free all resources
 */
void thread_pool_destroy(ThreadPool *pool);

/*
 * Get number of pending work items.
 */
int thread_pool_pending(ThreadPool *pool);

#endif /* THREAD_POOL_H */
