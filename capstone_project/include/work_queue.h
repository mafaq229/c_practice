/*
 * work_queue.h - Thread-Safe Work Queue
 *
 * This header defines a thread-safe queue for passing work items
 * (client connections) from the boss thread to worker threads.
 *
 * Key concepts:
 * - Mutex for mutual exclusion
 * - Condition variable for efficient waiting
 * - Proper signaling when work is added
 *
 * This is a critical component for the boss-worker pattern!
 */

#ifndef WORK_QUEUE_H
#define WORK_QUEUE_H

#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>

/* ============================================================================
 * Constants
 * ============================================================================ */

/* Maximum queue size (bounded buffer) */
#define MAX_QUEUE_SIZE      1024

/* ============================================================================
 * Data Structures
 * ============================================================================ */

/*
 * Work item - represents one unit of work
 *
 * For this project, a work item is simply a client socket FD.
 * In more complex systems, you might include additional metadata.
 */
typedef struct {
    int client_fd;              /* Client socket to handle */
    /* Add more fields if needed (e.g., timestamp, priority) */
} work_item_t;

/*
 * Work queue - thread-safe bounded queue
 *
 * This uses a circular buffer implementation.
 * TODO: Complete this structure with synchronization primitives.
 */
typedef struct {
    work_item_t items[MAX_QUEUE_SIZE];  /* Circular buffer */
    int head;                           /* Index of first item */
    int tail;                           /* Index of next free slot */
    int count;                          /* Number of items in queue */

    /* TODO: Add synchronization primitives */
    pthread_mutex_t mutex;              /* Protects queue state */
    pthread_cond_t not_empty;           /* Signaled when item added */
    pthread_cond_t not_full;            /* Signaled when item removed */

    bool shutdown;                      /* Shutdown flag */

} work_queue_t;

/* ============================================================================
 * Function Prototypes
 * ============================================================================ */

/*
 * work_queue_create - Create and initialize a work queue
 *
 * @return: Pointer to work queue, or NULL on error
 *
 * Initialize:
 * - head = tail = count = 0
 * - mutex (pthread_mutex_init)
 * - condition variables (pthread_cond_init)
 * - shutdown = false
 */
work_queue_t *work_queue_create(void);

/*
 * work_queue_destroy - Destroy a work queue
 *
 * @param queue: Work queue to destroy
 *
 * Steps:
 * 1. Wake up any waiting threads
 * 2. Destroy mutex and condition variables
 * 3. Free memory
 *
 * Note: Close any remaining client FDs!
 */
void work_queue_destroy(work_queue_t *queue);

/*
 * work_queue_push - Add a work item to the queue
 *
 * @param queue: Work queue
 * @param client_fd: Client socket file descriptor
 * @return: 0 on success, -1 if queue is full or shutting down
 *
 * Steps:
 * 1. Lock mutex
 * 2. Wait while queue is full (use while loop for spurious wakeups!)
 * 3. Check for shutdown
 * 4. Add item at tail, update tail and count
 * 5. Signal not_empty condition
 * 6. Unlock mutex
 *
 * This is called by the boss thread.
 */
int work_queue_push(work_queue_t *queue, int client_fd);

/*
 * work_queue_pop - Remove and return a work item from the queue
 *
 * @param queue: Work queue
 * @return: Client socket FD, or -1 if shutdown/error
 *
 * Steps:
 * 1. Lock mutex
 * 2. Wait while queue is empty (use while loop!)
 * 3. Check for shutdown (return -1)
 * 4. Remove item from head, update head and count
 * 5. Signal not_full condition
 * 6. Unlock mutex
 * 7. Return client_fd
 *
 * This is called by worker threads.
 * Workers block here when no work is available.
 */
int work_queue_pop(work_queue_t *queue);

/*
 * work_queue_shutdown - Signal the queue to shut down
 *
 * @param queue: Work queue
 *
 * Steps:
 * 1. Lock mutex
 * 2. Set shutdown flag
 * 3. Broadcast on both condition variables (wake ALL waiters)
 * 4. Unlock mutex
 *
 * After this, push returns -1 and pop returns -1.
 */
void work_queue_shutdown(work_queue_t *queue);

/*
 * work_queue_size - Get current number of items in queue
 *
 * @param queue: Work queue
 * @return: Number of items
 *
 * Thread-safe: locks mutex to read count.
 */
int work_queue_size(work_queue_t *queue);

/*
 * work_queue_is_empty - Check if queue is empty
 *
 * @param queue: Work queue
 * @return: true if empty, false otherwise
 */
bool work_queue_is_empty(work_queue_t *queue);

/*
 * work_queue_is_full - Check if queue is full
 *
 * @param queue: Work queue
 * @return: true if full, false otherwise
 */
bool work_queue_is_full(work_queue_t *queue);

#endif /* WORK_QUEUE_H */
