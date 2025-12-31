/*
work_queue.c - Thread-Safe Work Queue Implementation

This is a bounded buffer implementation using a circular array.
It uses a mutex and condition variables for thread synchronization.

Key concepts:
- Mutex protects all queue state
- Condition variables for efficient waiting
- While loops for spurious wakeup handling

This is the foundation of the boss-worker thread pool pattern!
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/work_queue.h"

/* ============================================================================
Queue Lifecycle Functions
============================================================================ */

/*
work_queue_create - Create and initialize a work queue
*/
work_queue_t *work_queue_create(void) {
    /*
     * TODO: Implement this function
     *
     * Steps:
     * 1. Allocate work_queue_t structure
     * 2. Initialize head = tail = count = 0
     * 3. Initialize shutdown = false
     * 4. Initialize mutex:
     *    pthread_mutex_init(&queue->mutex, NULL);
     * 5. Initialize condition variables:
     *    pthread_cond_init(&queue->not_empty, NULL);
     *    pthread_cond_init(&queue->not_full, NULL);
     * 6. Return pointer to queue
     *
     * Error handling:
     * - Check malloc return
     * - Check pthread_* return values (non-zero = error)
     * - Clean up on error
     */

    work_queue_t *queue = malloc(sizeof(work_queue_t));
    if (queue == NULL) {
        perror("malloc work_queue");
        return NULL;
    }

    /* Initialize indices */
    queue->head = 0;
    queue->tail = 0;
    queue->count = 0;
    queue->shutdown = false;

    /* YOUR CODE HERE: Initialize mutex and condition variables */

    /*
     * Hints:
     *
     * if (pthread_mutex_init(&queue->mutex, NULL) != 0) {
     *     perror("pthread_mutex_init");
     *     free(queue);
     *     return NULL;
     * }
     *
     * Similar for pthread_cond_init for not_empty and not_full
     */

    return queue;
}

/*
work_queue_destroy - Destroy a work queue
*/
void work_queue_destroy(work_queue_t *queue) {
    /*
     * TODO: Implement this function
     *
     * Steps:
     * 1. Check if queue is NULL
     * 2. Signal shutdown and wake all waiters
     * 3. Close any remaining client FDs in the queue
     * 4. Destroy mutex and condition variables:
     *    pthread_mutex_destroy(&queue->mutex);
     *    pthread_cond_destroy(&queue->not_empty);
     *    pthread_cond_destroy(&queue->not_full);
     * 5. Free the queue structure
     *
     * Closing remaining FDs prevents resource leaks!
     */

    if (queue == NULL) {
        return;
    }

    /* YOUR CODE HERE */

    free(queue);
}

/* ============================================================================
Queue Operations
============================================================================ */

/*
work_queue_push - Add a work item to the queue (producer)
*/
int work_queue_push(work_queue_t *queue, int client_fd) {
    /*
     * TODO: Implement this function
     *
     * This is called by the boss thread to add work for workers.
     *
     * Steps:
     * 1. Lock the mutex
     * 2. Wait while queue is full (use WHILE loop!):
     *    while (queue->count >= MAX_QUEUE_SIZE && !queue->shutdown) {
     *        pthread_cond_wait(&queue->not_full, &queue->mutex);
     *    }
     * 3. Check for shutdown - if true, unlock and return -1
     * 4. Add item at tail:
     *    queue->items[queue->tail].client_fd = client_fd;
     *    queue->tail = (queue->tail + 1) % MAX_QUEUE_SIZE;
     *    queue->count++;
     * 5. Signal that queue is not empty (wake one worker):
     *    pthread_cond_signal(&queue->not_empty);
     * 6. Unlock mutex
     * 7. Return 0
     *
     * IMPORTANT: The while loop handles spurious wakeups!
     * pthread_cond_wait can return even without a signal.
     */

    if (queue == NULL) {
        return -1;
    }

    /* YOUR CODE HERE */

    (void)client_fd;

    return -1;  /* Placeholder */
}

/*
work_queue_pop - Remove and return a work item (consumer)
*/
int work_queue_pop(work_queue_t *queue) {
    /*
     * TODO: Implement this function
     *
     * This is called by worker threads to get work.
     *
     * Steps:
     * 1. Lock the mutex
     * 2. Wait while queue is empty (use WHILE loop!):
     *    while (queue->count == 0 && !queue->shutdown) {
     *        pthread_cond_wait(&queue->not_empty, &queue->mutex);
     *    }
     * 3. Check for shutdown AND empty queue - if true, unlock and return -1
     *    (Allow processing remaining items even during shutdown)
     * 4. Remove item from head:
     *    int client_fd = queue->items[queue->head].client_fd;
     *    queue->head = (queue->head + 1) % MAX_QUEUE_SIZE;
     *    queue->count--;
     * 5. Signal that queue is not full (wake producer if waiting):
     *    pthread_cond_signal(&queue->not_full);
     * 6. Unlock mutex
     * 7. Return client_fd
     *
     * Workers block here when no work is available.
     */

    if (queue == NULL) {
        return -1;
    }

    /* YOUR CODE HERE */

    return -1;  /* Placeholder */
}

/*
work_queue_shutdown - Signal the queue to shut down
*/
void work_queue_shutdown(work_queue_t *queue) {
    /*
     * TODO: Implement this function
     *
     * Steps:
     * 1. Lock mutex
     * 2. Set shutdown = true
     * 3. Broadcast on BOTH condition variables to wake ALL waiters:
     *    pthread_cond_broadcast(&queue->not_empty);
     *    pthread_cond_broadcast(&queue->not_full);
     * 4. Unlock mutex
     *
     * Use broadcast (not signal) to wake ALL waiting threads!
     */

    if (queue == NULL) {
        return;
    }

    /* YOUR CODE HERE */
}

/* ============================================================================
Query Functions
============================================================================ */

/*
work_queue_size - Get current queue size
*/
int work_queue_size(work_queue_t *queue) {
    /*
     * TODO: Implement this function
     *
     * Thread-safe: lock mutex, read count, unlock mutex.
     */

    if (queue == NULL) {
        return 0;
    }

    /* YOUR CODE HERE */

    return 0;  /* Placeholder */
}

/*
work_queue_is_empty - Check if queue is empty
*/
bool work_queue_is_empty(work_queue_t *queue) {
    return work_queue_size(queue) == 0;
}

/*
work_queue_is_full - Check if queue is full
*/
bool work_queue_is_full(work_queue_t *queue) {
    return work_queue_size(queue) >= MAX_QUEUE_SIZE;
}

/* ============================================================================
Testing helper
============================================================================ */

#ifdef WORK_QUEUE_TEST_MAIN
/*
Simple test for work queue (single-threaded)
Compile: gcc -DWORK_QUEUE_TEST_MAIN -g -pthread work_queue.c -o test_wq
*/
int main(void) {
    work_queue_t *queue;
    int fd;

    printf("Testing work queue...\n\n");

    /* Create queue */
    printf("Creating queue...\n");
    queue = work_queue_create();
    if (queue == NULL) {
        printf("FAILED to create queue\n");
        return 1;
    }
    printf("Queue created, size=%d\n", work_queue_size(queue));

    /* Push some items */
    printf("\nPushing items 10, 20, 30...\n");
    work_queue_push(queue, 10);
    work_queue_push(queue, 20);
    work_queue_push(queue, 30);
    printf("Queue size after push: %d\n", work_queue_size(queue));

    /* Pop items */
    printf("\nPopping items...\n");
    fd = work_queue_pop(queue);
    printf("Popped: %d (expected 10)\n", fd);
    fd = work_queue_pop(queue);
    printf("Popped: %d (expected 20)\n", fd);
    printf("Queue size: %d\n", work_queue_size(queue));

    /* Shutdown test */
    printf("\nTesting shutdown...\n");
    work_queue_shutdown(queue);
    fd = work_queue_pop(queue);  /* Should return 30 (remaining item) */
    printf("Popped after shutdown: %d (expected 30)\n", fd);
    fd = work_queue_pop(queue);  /* Should return -1 (shutdown, empty) */
    printf("Pop on empty+shutdown: %d (expected -1)\n", fd);

    /* Cleanup */
    work_queue_destroy(queue);
    printf("\nQueue destroyed. Test complete!\n");

    return 0;
}
#endif /* WORK_QUEUE_TEST_MAIN */
