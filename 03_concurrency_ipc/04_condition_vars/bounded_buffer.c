/*
CS-6200 Preparation - Module 04: Bounded Buffer

A complete, reusable thread-safe bounded buffer implementation.
This is exactly what you'll need for GIOS thread pool work queue!

Compile: clang -Wall -Wextra -g -pthread bounded_buffer.c -o bounded_buffer
Run:     ./bounded_buffer

Difficulty: [HARD]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

/* ============================================================================
BOUNDED BUFFER IMPLEMENTATION
============================================================================
*/

typedef struct {
    void **items;               /* Array of item pointers */
    int capacity;               /* Maximum number of items */
    int count;                  /* Current number of items */
    int head;                   /* Index of first item (for removal) */
    int tail;                   /* Index of next free slot (for insertion) */

    pthread_mutex_t mutex;      /* Protects all fields */
    pthread_cond_t not_empty;   /* Signaled when items are added */
    pthread_cond_t not_full;    /* Signaled when items are removed */

    int shutdown;               /* Flag to signal shutdown */
} BoundedBuffer;

/*
TODO: Initialize the bounded buffer
*/
BoundedBuffer *buffer_create(int capacity) {
    BoundedBuffer *buf = malloc(sizeof(BoundedBuffer));
    if (buf == NULL) {
        return NULL;
    }

    buf->items = malloc(capacity * sizeof(void *));
    if (buf->items == NULL) {
        free(buf);
        return NULL;
    }

    buf->capacity = capacity;
    buf->count = 0;
    buf->head = 0;
    buf->tail = 0;
    buf->shutdown = 0;

    pthread_mutex_init(&buf->mutex, NULL);
    pthread_cond_init(&buf->not_empty, NULL);
    pthread_cond_init(&buf->not_full, NULL);

    return buf;
}

/*
TODO: Destroy the bounded buffer
*/
void buffer_destroy(BoundedBuffer *buf) {
    if (buf == NULL) return;

    pthread_mutex_destroy(&buf->mutex);
    pthread_cond_destroy(&buf->not_empty);
    pthread_cond_destroy(&buf->not_full);

    free(buf->items);
    free(buf);
}

/*
TODO: Add an item to the buffer (blocks if full)

Returns 0 on success, -1 if shutdown
*/
int buffer_put(BoundedBuffer *buf, void *item) {
    pthread_mutex_lock(&buf->mutex);

    /* Wait while buffer is full (and not shutting down) */
    while (buf->count == buf->capacity && !buf->shutdown) {
        pthread_cond_wait(&buf->not_full, &buf->mutex);
    }

    /* Check for shutdown */
    if (buf->shutdown) {
        pthread_mutex_unlock(&buf->mutex);
        return -1;
    }

    /* Add item */
    buf->items[buf->tail] = item;
    buf->tail = (buf->tail + 1) % buf->capacity;
    buf->count++;

    /* Signal waiting consumers */
    pthread_cond_signal(&buf->not_empty);

    pthread_mutex_unlock(&buf->mutex);
    return 0;
}

/*
TODO: Remove an item from the buffer (blocks if empty)

Returns item on success, NULL if shutdown and empty
*/
void *buffer_get(BoundedBuffer *buf) {
    pthread_mutex_lock(&buf->mutex);

    /* Wait while buffer is empty (and not shutting down) */
    while (buf->count == 0 && !buf->shutdown) {
        pthread_cond_wait(&buf->not_empty, &buf->mutex);
    }

    /* Check for shutdown with empty buffer */
    if (buf->count == 0 && buf->shutdown) {
        pthread_mutex_unlock(&buf->mutex);
        return NULL;
    }

    /* Remove item */
    void *item = buf->items[buf->head];
    buf->head = (buf->head + 1) % buf->capacity;
    buf->count--;

    /* Signal waiting producers */
    pthread_cond_signal(&buf->not_full);

    pthread_mutex_unlock(&buf->mutex);
    return item;
}

/*
TODO: Try to add without blocking

Returns 0 on success, -1 if full or shutdown
*/
int buffer_try_put(BoundedBuffer *buf, void *item) {
    pthread_mutex_lock(&buf->mutex);

    if (buf->shutdown || buf->count == buf->capacity) {
        pthread_mutex_unlock(&buf->mutex);
        return -1;
    }

    buf->items[buf->tail] = item;
    buf->tail = (buf->tail + 1) % buf->capacity;
    buf->count++;

    pthread_cond_signal(&buf->not_empty);

    pthread_mutex_unlock(&buf->mutex);
    return 0;
}

/*
TODO: Try to get without blocking

Returns item on success, NULL if empty
*/
void *buffer_try_get(BoundedBuffer *buf) {
    pthread_mutex_lock(&buf->mutex);

    if (buf->count == 0) {
        pthread_mutex_unlock(&buf->mutex);
        return NULL;
    }

    void *item = buf->items[buf->head];
    buf->head = (buf->head + 1) % buf->capacity;
    buf->count--;

    pthread_cond_signal(&buf->not_full);

    pthread_mutex_unlock(&buf->mutex);
    return item;
}

/*
TODO: Signal shutdown and wake all waiting threads
*/
void buffer_shutdown(BoundedBuffer *buf) {
    pthread_mutex_lock(&buf->mutex);

    buf->shutdown = 1;

    /* Wake ALL waiting threads */
    pthread_cond_broadcast(&buf->not_empty);
    pthread_cond_broadcast(&buf->not_full);

    pthread_mutex_unlock(&buf->mutex);
}

/*
Get current count (for debugging/testing)
*/
int buffer_count(BoundedBuffer *buf) {
    pthread_mutex_lock(&buf->mutex);
    int count = buf->count;
    pthread_mutex_unlock(&buf->mutex);
    return count;
}

/* ============================================================================
TEST: Basic Operations
============================================================================
*/

void test_basic(void) {
    printf("\n=== Test: Basic Operations ===\n");

    BoundedBuffer *buf = buffer_create(3);

    int a = 1, b = 2, c = 3;

    printf("Adding items: 1, 2, 3\n");
    buffer_put(buf, &a);
    buffer_put(buf, &b);
    buffer_put(buf, &c);

    printf("Buffer count: %d\n", buffer_count(buf));

    printf("Removing items: ");
    printf("%d ", *(int *)buffer_get(buf));
    printf("%d ", *(int *)buffer_get(buf));
    printf("%d\n", *(int *)buffer_get(buf));

    printf("Buffer count: %d\n", buffer_count(buf));

    buffer_destroy(buf);
    printf("Test passed!\n");
}

/* ============================================================================
TEST: Producer-Consumer Threads
============================================================================
*/

typedef struct {
    BoundedBuffer *buffer;
    int id;
    int count;
} WorkerArgs;

void *producer_thread(void *arg) {
    WorkerArgs *args = (WorkerArgs *)arg;

    for (int i = 0; i < args->count; i++) {
        int *item = malloc(sizeof(int));
        *item = args->id * 1000 + i;

        if (buffer_put(args->buffer, item) == 0) {
            printf("Producer %d: Added %d\n", args->id, *item);
        } else {
            printf("Producer %d: Shutdown, stopping\n", args->id);
            free(item);
            break;
        }

        usleep(rand() % 50000);
    }

    printf("Producer %d: Done\n", args->id);
    return NULL;
}

void *consumer_thread(void *arg) {
    WorkerArgs *args = (WorkerArgs *)arg;
    int consumed = 0;

    while (1) {
        int *item = buffer_get(args->buffer);

        if (item == NULL) {
            printf("Consumer %d: Got NULL, stopping\n", args->id);
            break;
        }

        printf("Consumer %d: Got %d\n", args->id, *item);
        free(item);
        consumed++;

        usleep(rand() % 100000);
    }

    printf("Consumer %d: Done (consumed %d items)\n", args->id, consumed);
    return NULL;
}

void test_threaded(void) {
    printf("\n=== Test: Producer-Consumer Threads ===\n");

    srand(time(NULL));

    BoundedBuffer *buf = buffer_create(5);

    pthread_t producers[2];
    pthread_t consumers[2];
    WorkerArgs prod_args[2];
    WorkerArgs cons_args[2];

    /* Start consumers first */
    for (int i = 0; i < 2; i++) {
        cons_args[i].buffer = buf;
        cons_args[i].id = i;
        cons_args[i].count = 0;  /* Not used */
        pthread_create(&consumers[i], NULL, consumer_thread, &cons_args[i]);
    }

    /* Start producers */
    for (int i = 0; i < 2; i++) {
        prod_args[i].buffer = buf;
        prod_args[i].id = i;
        prod_args[i].count = 5;
        pthread_create(&producers[i], NULL, producer_thread, &prod_args[i]);
    }

    /* Wait for producers */
    for (int i = 0; i < 2; i++) {
        pthread_join(producers[i], NULL);
    }

    printf("\nAll producers done, waiting for buffer to drain...\n");
    while (buffer_count(buf) > 0) {
        usleep(100000);
    }

    printf("Buffer empty, shutting down...\n");
    buffer_shutdown(buf);

    /* Wait for consumers */
    for (int i = 0; i < 2; i++) {
        pthread_join(consumers[i], NULL);
    }

    buffer_destroy(buf);
    printf("Test passed!\n");
}

/* ============================================================================
TEST: Try operations
============================================================================
*/

void test_try_operations(void) {
    printf("\n=== Test: Non-blocking Operations ===\n");

    BoundedBuffer *buf = buffer_create(2);

    int a = 1, b = 2, c = 3;

    printf("try_put(1): %s\n", buffer_try_put(buf, &a) == 0 ? "success" : "failed");
    printf("try_put(2): %s\n", buffer_try_put(buf, &b) == 0 ? "success" : "failed");
    printf("try_put(3): %s (buffer full)\n", buffer_try_put(buf, &c) == 0 ? "success" : "failed");

    printf("\ntry_get: ");
    void *item = buffer_try_get(buf);
    if (item) printf("%d\n", *(int *)item);
    else printf("NULL\n");

    printf("try_get: ");
    item = buffer_try_get(buf);
    if (item) printf("%d\n", *(int *)item);
    else printf("NULL\n");

    printf("try_get: %s (buffer empty)\n", buffer_try_get(buf) ? "got item" : "NULL");

    buffer_destroy(buf);
    printf("Test passed!\n");
}

/* ============================================================================
TEST: Shutdown behavior
============================================================================
*/

void *waiting_consumer(void *arg) {
    BoundedBuffer *buf = (BoundedBuffer *)arg;

    printf("Consumer: Waiting for item...\n");
    void *item = buffer_get(buf);

    if (item == NULL) {
        printf("Consumer: Got NULL (shutdown)\n");
    } else {
        printf("Consumer: Got item\n");
    }

    return NULL;
}

void test_shutdown(void) {
    printf("\n=== Test: Shutdown Behavior ===\n");

    BoundedBuffer *buf = buffer_create(5);
    pthread_t consumer;

    pthread_create(&consumer, NULL, waiting_consumer, buf);

    sleep(1);
    printf("Main: Calling shutdown...\n");
    buffer_shutdown(buf);

    pthread_join(consumer, NULL);

    buffer_destroy(buf);
    printf("Test passed!\n");
}

/* ============================================================================
MAIN
============================================================================
*/

int main(int argc, char *argv[]) {
    printf("\n================================================\n");
    printf("  Module 04: Bounded Buffer\n");
    printf("================================================\n");

    if (argc > 1) {
        int test = atoi(argv[1]);
        switch (test) {
            case 1: test_basic(); break;
            case 2: test_threaded(); break;
            case 3: test_try_operations(); break;
            case 4: test_shutdown(); break;
            default: printf("Unknown test: %d\n", test);
        }
    } else {
        test_basic();
        test_try_operations();
        test_shutdown();
        test_threaded();
    }

    printf("\n================================================\n");
    printf("  Bounded Buffer Complete!\n");
    printf("  Use this as your GIOS work queue foundation!\n");
    printf("================================================\n\n");

    return 0;
}
