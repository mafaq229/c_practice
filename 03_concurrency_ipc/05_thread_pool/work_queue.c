/*
 * CS-6200 Preparation - Module 05: Work Queue Implementation
 *
 * Thread-safe work queue for the thread pool.
 *
 * Compile: (compiled as part of thread pool)
 *
 * Difficulty: [HARD]
 */

#include <stdio.h>
#include <stdlib.h>
#include "work_queue.h"

/*
 * TODO: Initialize the work queue
 */
int work_queue_init(WorkQueue *queue) {
    if (queue == NULL) {
        return -1;
    }

    queue->head = NULL;
    queue->tail = NULL;
    queue->count = 0;
    queue->shutdown = 0;

    if (pthread_mutex_init(&queue->mutex, NULL) != 0) {
        return -1;
    }

    if (pthread_cond_init(&queue->not_empty, NULL) != 0) {
        pthread_mutex_destroy(&queue->mutex);
        return -1;
    }

    return 0;
}

/*
 * TODO: Destroy the work queue
 */
void work_queue_destroy(WorkQueue *queue) {
    if (queue == NULL) {
        return;
    }

    pthread_mutex_lock(&queue->mutex);

    /* Free any remaining work items */
    WorkItem *current = queue->head;
    while (current != NULL) {
        WorkItem *next = current->next;
        free(current);
        current = next;
    }

    queue->head = NULL;
    queue->tail = NULL;
    queue->count = 0;

    pthread_mutex_unlock(&queue->mutex);

    pthread_cond_destroy(&queue->not_empty);
    pthread_mutex_destroy(&queue->mutex);
}

/*
 * TODO: Add a work item to the queue
 *
 * Creates a new WorkItem and adds it to the tail of the queue.
 * Signals waiting threads that work is available.
 */
int work_queue_push(WorkQueue *queue, void (*function)(void *), void *arg) {
    if (queue == NULL || function == NULL) {
        return -1;
    }

    /* Allocate work item */
    WorkItem *item = malloc(sizeof(WorkItem));
    if (item == NULL) {
        return -1;
    }

    item->function = function;
    item->arg = arg;
    item->next = NULL;

    pthread_mutex_lock(&queue->mutex);

    /* Check for shutdown */
    if (queue->shutdown) {
        pthread_mutex_unlock(&queue->mutex);
        free(item);
        return -1;
    }

    /* Add to tail of queue */
    if (queue->tail == NULL) {
        /* Queue was empty */
        queue->head = item;
        queue->tail = item;
    } else {
        queue->tail->next = item;
        queue->tail = item;
    }

    queue->count++;

    /* Signal waiting threads */
    pthread_cond_signal(&queue->not_empty);

    pthread_mutex_unlock(&queue->mutex);

    return 0;
}

/*
 * TODO: Remove and return a work item from the queue
 *
 * Blocks if queue is empty.
 * Returns NULL if shutdown is signaled and queue is empty.
 *
 * IMPORTANT: Caller is responsible for freeing the returned WorkItem!
 */
WorkItem *work_queue_pop(WorkQueue *queue) {
    if (queue == NULL) {
        return NULL;
    }

    pthread_mutex_lock(&queue->mutex);

    /* Wait while queue is empty and not shutting down */
    while (queue->count == 0 && !queue->shutdown) {
        pthread_cond_wait(&queue->not_empty, &queue->mutex);
    }

    /* Check for shutdown with empty queue */
    if (queue->count == 0 && queue->shutdown) {
        pthread_mutex_unlock(&queue->mutex);
        return NULL;
    }

    /* Remove from head of queue */
    WorkItem *item = queue->head;
    queue->head = item->next;

    if (queue->head == NULL) {
        /* Queue is now empty */
        queue->tail = NULL;
    }

    queue->count--;

    pthread_mutex_unlock(&queue->mutex);

    return item;
}

/*
 * TODO: Signal shutdown
 *
 * Sets shutdown flag and wakes all waiting threads.
 */
void work_queue_shutdown(WorkQueue *queue) {
    if (queue == NULL) {
        return;
    }

    pthread_mutex_lock(&queue->mutex);

    queue->shutdown = 1;

    /* Wake ALL waiting threads */
    pthread_cond_broadcast(&queue->not_empty);

    pthread_mutex_unlock(&queue->mutex);
}

/*
 * Get current queue size (thread-safe)
 */
int work_queue_size(WorkQueue *queue) {
    if (queue == NULL) {
        return 0;
    }

    pthread_mutex_lock(&queue->mutex);
    int size = queue->count;
    pthread_mutex_unlock(&queue->mutex);

    return size;
}
