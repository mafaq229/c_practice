/*
 * CS-6200 Preparation - Module 05: Work Queue Interface
 *
 * Thread-safe work queue for the thread pool.
 *
 * Difficulty: [HARD]
 */

#ifndef WORK_QUEUE_H
#define WORK_QUEUE_H

#include <pthread.h>

/* Work item - represents a task to be executed */
typedef struct WorkItem {
    void (*function)(void *arg);  /* Function to execute */
    void *arg;                     /* Argument to pass to function */
    struct WorkItem *next;         /* Next item in queue */
} WorkItem;

/* Thread-safe work queue */
typedef struct {
    WorkItem *head;               /* Front of queue (for removal) */
    WorkItem *tail;               /* Back of queue (for insertion) */
    int count;                    /* Number of items in queue */

    pthread_mutex_t mutex;        /* Protects queue */
    pthread_cond_t not_empty;     /* Signaled when items added */

    int shutdown;                 /* Shutdown flag */
} WorkQueue;

/*
 * Initialize the work queue.
 * Returns 0 on success, -1 on failure.
 */
int work_queue_init(WorkQueue *queue);

/*
 * Destroy the work queue.
 * Any remaining work items are freed.
 */
void work_queue_destroy(WorkQueue *queue);

/*
 * Add a work item to the queue.
 * Returns 0 on success, -1 if shutdown.
 */
int work_queue_push(WorkQueue *queue, void (*function)(void *), void *arg);

/*
 * Remove and return a work item from the queue.
 * Blocks if queue is empty.
 * Returns NULL if shutdown and queue is empty.
 */
WorkItem *work_queue_pop(WorkQueue *queue);

/*
 * Signal shutdown and wake all waiting threads.
 */
void work_queue_shutdown(WorkQueue *queue);

/*
 * Get current queue size.
 */
int work_queue_size(WorkQueue *queue);

#endif /* WORK_QUEUE_H */
