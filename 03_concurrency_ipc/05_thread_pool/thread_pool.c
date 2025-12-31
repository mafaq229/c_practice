/*
CS-6200 Preparation - Module 05: Thread Pool Implementation

This is the boss-worker pattern you'll implement in GIOS Project 1 Part 2!

Compile: (compiled as part of pool_test)

Difficulty: [HARD]
*/

#include <stdio.h>
#include <stdlib.h>
#include "thread_pool.h"
#include "work_queue.h"

/*
Worker thread function.

Each worker:
1. Waits for work on the queue
2. Executes the work function
3. Frees the work item
4. Repeats until shutdown

This is the pattern for GIOS file transfer workers!
*/
static void *worker_thread(void *arg) {
    ThreadPool *pool = (ThreadPool *)arg;

    while (1) {
        /* Block waiting for work */
        WorkItem *item = work_queue_pop(&pool->work_queue);

        if (item == NULL) {
            /* Shutdown signal received and queue is empty */
            break;
        }

        /* Execute the work */
        item->function(item->arg);

        /* Free the work item (function is responsible for arg if needed) */
        free(item);
    }

    return NULL;
}

/*
TODO: Create and start a thread pool
*/
ThreadPool *thread_pool_create(int num_threads) {
    if (num_threads <= 0) {
        fprintf(stderr, "thread_pool_create: num_threads must be positive\n");
        return NULL;
    }

    /* Allocate pool structure */
    ThreadPool *pool = malloc(sizeof(ThreadPool));
    if (pool == NULL) {
        fprintf(stderr, "thread_pool_create: malloc failed\n");
        return NULL;
    }

    pool->num_threads = num_threads;
    pool->started = 0;

    /* Initialize work queue */
    if (work_queue_init(&pool->work_queue) != 0) {
        fprintf(stderr, "thread_pool_create: work_queue_init failed\n");
        free(pool);
        return NULL;
    }

    /* Allocate thread array */
    pool->threads = malloc(num_threads * sizeof(pthread_t));
    if (pool->threads == NULL) {
        fprintf(stderr, "thread_pool_create: malloc threads failed\n");
        work_queue_destroy(&pool->work_queue);
        free(pool);
        return NULL;
    }

    /* Create worker threads */
    for (int i = 0; i < num_threads; i++) {
        if (pthread_create(&pool->threads[i], NULL, worker_thread, pool) != 0) {
            fprintf(stderr, "thread_pool_create: pthread_create failed\n");

            /* Shutdown already-created threads */
            work_queue_shutdown(&pool->work_queue);
            for (int j = 0; j < i; j++) {
                pthread_join(pool->threads[j], NULL);
            }

            free(pool->threads);
            work_queue_destroy(&pool->work_queue);
            free(pool);
            return NULL;
        }
    }

    pool->started = 1;

    return pool;
}

/*
TODO: Submit work to the thread pool
*/
int thread_pool_submit(ThreadPool *pool, void (*function)(void *), void *arg) {
    if (pool == NULL || !pool->started) {
        return -1;
    }

    return work_queue_push(&pool->work_queue, function, arg);
}

/*
TODO: Destroy the thread pool

Steps:
1. Signal shutdown to work queue
2. Join all worker threads
3. Destroy work queue
4. Free all memory
*/
void thread_pool_destroy(ThreadPool *pool) {
    if (pool == NULL) {
        return;
    }

    if (pool->started) {
        /* Signal shutdown - workers will exit after finishing current work */
        work_queue_shutdown(&pool->work_queue);

        /* Wait for all workers to exit */
        for (int i = 0; i < pool->num_threads; i++) {
            pthread_join(pool->threads[i], NULL);
        }

        pool->started = 0;
    }

    /* Clean up */
    if (pool->threads != NULL) {
        free(pool->threads);
    }

    work_queue_destroy(&pool->work_queue);
    free(pool);
}

/*
Get number of pending work items
*/
int thread_pool_pending(ThreadPool *pool) {
    if (pool == NULL) {
        return 0;
    }

    return work_queue_size(&pool->work_queue);
}
