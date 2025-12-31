/*
CS-6200 Preparation - Module 04: Producer-Consumer

The classic producer-consumer problem using condition variables.
This pattern is the foundation for thread pools in GIOS!

Compile: clang -Wall -Wextra -g -pthread producer_consumer.c -o producer_consumer
Run:     ./producer_consumer

Difficulty: [HARD]
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/* ============================================================================
CONCEPT: Condition Variables
============================================================================

A condition variable allows threads to WAIT for a condition to become true.

Why not just use a loop with mutex?
  while (!ready) {
      pthread_mutex_unlock(&mutex);
      pthread_mutex_lock(&mutex);
  }
This is BUSY WAITING - wastes CPU cycles!

Condition variables allow efficient waiting:
- pthread_cond_wait() atomically unlocks mutex and sleeps
- When signaled, it wakes up and re-acquires mutex

Key functions:
- pthread_cond_wait(&cond, &mutex)   - Wait for signal
- pthread_cond_signal(&cond)         - Wake ONE waiting thread
- pthread_cond_broadcast(&cond)      - Wake ALL waiting threads

CRITICAL RULE: Always use WHILE, not IF, before cond_wait!
  while (!condition) {
      pthread_cond_wait(&cond, &mutex);
  }
This handles spurious wakeups and race conditions.
*/

/* ============================================================================
Simple Producer-Consumer with Single Item
============================================================================
*/

int buffer = 0;          /* Shared buffer (single item) */
int has_data = 0;        /* Is there data in the buffer? */

pthread_mutex_t pc_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_produced = PTHREAD_COND_INITIALIZER;  /* Data available */
pthread_cond_t cond_consumed = PTHREAD_COND_INITIALIZER;  /* Space available */

void *simple_producer(void *arg) {
    int items = *(int *)arg;

    for (int i = 0; i < items; i++) {
        pthread_mutex_lock(&pc_mutex);

        /* Wait until buffer is empty */
        while (has_data) {
            printf("Producer: Buffer full, waiting...\n");
            pthread_cond_wait(&cond_consumed, &pc_mutex);
        }

        /* Produce item */
        buffer = i + 1;
        has_data = 1;
        printf("Producer: Produced item %d\n", buffer);

        /* Signal consumer */
        pthread_cond_signal(&cond_produced);

        pthread_mutex_unlock(&pc_mutex);

        usleep(100000);  /* Simulate production time */
    }

    printf("Producer: Done!\n");
    return NULL;
}

void *simple_consumer(void *arg) {
    int items = *(int *)arg;

    for (int i = 0; i < items; i++) {
        pthread_mutex_lock(&pc_mutex);

        /* Wait until buffer has data */
        while (!has_data) {
            printf("Consumer: Buffer empty, waiting...\n");
            pthread_cond_wait(&cond_produced, &pc_mutex);
        }

        /* Consume item */
        int item = buffer;
        has_data = 0;
        printf("Consumer: Consumed item %d\n", item);

        /* Signal producer */
        pthread_cond_signal(&cond_consumed);

        pthread_mutex_unlock(&pc_mutex);

        usleep(150000);  /* Simulate consumption time */
    }

    printf("Consumer: Done!\n");
    return NULL;
}

void exercise1_simple_pc(void) {
    printf("\n=== Exercise 1: Simple Producer-Consumer ===\n");

    pthread_t producer, consumer;
    int num_items = 5;

    has_data = 0;

    pthread_create(&producer, NULL, simple_producer, &num_items);
    pthread_create(&consumer, NULL, simple_consumer, &num_items);

    pthread_join(producer, NULL);
    pthread_join(consumer, NULL);
}

/* ============================================================================
Multiple Producers and Consumers
============================================================================
*/

#define QUEUE_SIZE 5
#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 3
#define ITEMS_PER_PRODUCER 4

int queue[QUEUE_SIZE];
int queue_count = 0;
int queue_in = 0;   /* Next write position */
int queue_out = 0;  /* Next read position */
int production_done = 0;

pthread_mutex_t q_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t q_not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t q_not_empty = PTHREAD_COND_INITIALIZER;

void queue_put(int item) {
    queue[queue_in] = item;
    queue_in = (queue_in + 1) % QUEUE_SIZE;
    queue_count++;
}

int queue_get(void) {
    int item = queue[queue_out];
    queue_out = (queue_out + 1) % QUEUE_SIZE;
    queue_count--;
    return item;
}

void *multi_producer(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < ITEMS_PER_PRODUCER; i++) {
        int item = id * 100 + i;

        pthread_mutex_lock(&q_mutex);

        /* Wait while queue is full */
        while (queue_count == QUEUE_SIZE) {
            pthread_cond_wait(&q_not_full, &q_mutex);
        }

        queue_put(item);
        printf("Producer %d: Added item %d (queue size: %d)\n",
               id, item, queue_count);

        pthread_cond_signal(&q_not_empty);

        pthread_mutex_unlock(&q_mutex);

        usleep(rand() % 100000);
    }

    printf("Producer %d: Finished\n", id);
    return NULL;
}

void *multi_consumer(void *arg) {
    int id = *(int *)arg;
    int items_consumed = 0;

    while (1) {
        pthread_mutex_lock(&q_mutex);

        /* Wait while queue is empty */
        while (queue_count == 0 && !production_done) {
            pthread_cond_wait(&q_not_empty, &q_mutex);
        }

        /* Check if we should exit */
        if (queue_count == 0 && production_done) {
            pthread_mutex_unlock(&q_mutex);
            break;
        }

        int item = queue_get();
        items_consumed++;
        printf("Consumer %d: Got item %d (queue size: %d)\n",
               id, item, queue_count);

        pthread_cond_signal(&q_not_full);

        pthread_mutex_unlock(&q_mutex);

        usleep(rand() % 150000);
    }

    printf("Consumer %d: Finished (consumed %d items)\n", id, items_consumed);
    return NULL;
}

void exercise2_multi_pc(void) {
    printf("\n=== Exercise 2: Multiple Producers and Consumers ===\n");

    srand(time(NULL));

    queue_count = 0;
    queue_in = 0;
    queue_out = 0;
    production_done = 0;

    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumers[NUM_CONSUMERS];
    int producer_ids[NUM_PRODUCERS];
    int consumer_ids[NUM_CONSUMERS];

    /* Start consumers first */
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        consumer_ids[i] = i;
        pthread_create(&consumers[i], NULL, multi_consumer, &consumer_ids[i]);
    }

    /* Start producers */
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        producer_ids[i] = i;
        pthread_create(&producers[i], NULL, multi_producer, &producer_ids[i]);
    }

    /* Wait for producers to finish */
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }

    /* Signal consumers that production is done */
    pthread_mutex_lock(&q_mutex);
    production_done = 1;
    pthread_cond_broadcast(&q_not_empty);  /* Wake all consumers */
    pthread_mutex_unlock(&q_mutex);

    /* Wait for consumers */
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
    }

    printf("\nAll producers and consumers finished!\n");
    printf("Total items produced: %d\n", NUM_PRODUCERS * ITEMS_PER_PRODUCER);
}

/* ============================================================================
Exercise 3: Why WHILE, not IF?
============================================================================
*/

void exercise3_while_vs_if(void) {
    printf("\n=== Exercise 3: Why WHILE, not IF? ===\n");
    printf("\n");
    printf("WRONG (using if):\n");
    printf("  if (!ready) {\n");
    printf("      pthread_cond_wait(&cond, &mutex);\n");
    printf("  }\n");
    printf("  // Assume ready is true here - DANGEROUS!\n");
    printf("\n");
    printf("RIGHT (using while):\n");
    printf("  while (!ready) {\n");
    printf("      pthread_cond_wait(&cond, &mutex);\n");
    printf("  }\n");
    printf("  // ready is GUARANTEED to be true here\n");
    printf("\n");
    printf("Reasons:\n");
    printf("1. SPURIOUS WAKEUPS: pthread_cond_wait() can return\n");
    printf("   even without a signal (implementation-dependent).\n");
    printf("\n");
    printf("2. STOLEN WAKEUPS: With broadcast, multiple threads wake\n");
    printf("   but only one might get the resource. Others must re-wait.\n");
    printf("\n");
    printf("3. CONDITION CHANGE: Between signal and wakeup, another\n");
    printf("   thread might change the condition.\n");
    printf("\n");
    printf("ALWAYS use WHILE with condition variables!\n");
}

/* ============================================================================
Exercise 4: Signal vs Broadcast
============================================================================
*/

int ready_flag = 0;
pthread_mutex_t ready_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ready_cond = PTHREAD_COND_INITIALIZER;

void *waiter_thread(void *arg) {
    int id = *(int *)arg;

    pthread_mutex_lock(&ready_mutex);

    printf("Waiter %d: Waiting for ready signal...\n", id);

    while (!ready_flag) {
        pthread_cond_wait(&ready_cond, &ready_mutex);
    }

    printf("Waiter %d: Got the signal! ready_flag = %d\n", id, ready_flag);

    pthread_mutex_unlock(&ready_mutex);

    return NULL;
}

void exercise4_signal_vs_broadcast(void) {
    printf("\n=== Exercise 4: Signal vs Broadcast ===\n");

    pthread_t waiters[3];
    int ids[] = {0, 1, 2};

    ready_flag = 0;

    /* Create waiting threads */
    for (int i = 0; i < 3; i++) {
        pthread_create(&waiters[i], NULL, waiter_thread, &ids[i]);
    }

    sleep(1);  /* Let threads start waiting */

    printf("\nMain: Using SIGNAL (wakes ONE thread)...\n");

    pthread_mutex_lock(&ready_mutex);
    ready_flag = 1;
    pthread_cond_signal(&ready_cond);  /* Only one thread wakes */
    pthread_mutex_unlock(&ready_mutex);

    sleep(1);

    printf("\nMain: Using BROADCAST (wakes ALL threads)...\n");

    pthread_mutex_lock(&ready_mutex);
    pthread_cond_broadcast(&ready_cond);  /* All threads wake */
    pthread_mutex_unlock(&ready_mutex);

    for (int i = 0; i < 3; i++) {
        pthread_join(waiters[i], NULL);
    }

    printf("\n");
    printf("Use signal() when: One thread should handle the event\n");
    printf("Use broadcast() when: All threads should check the condition\n");
}

/* ============================================================================
Exercise 5: Common mistakes
============================================================================
*/

void exercise5_common_mistakes(void) {
    printf("\n=== Exercise 5: Common Condition Variable Mistakes ===\n");

    printf("\n");
    printf("MISTAKE 1: Forgetting to hold the mutex\n");
    printf("  // WRONG\n");
    printf("  pthread_cond_wait(&cond, &mutex);  // mutex not locked!\n");
    printf("  \n");
    printf("  // RIGHT\n");
    printf("  pthread_mutex_lock(&mutex);\n");
    printf("  pthread_cond_wait(&cond, &mutex);\n");
    printf("  pthread_mutex_unlock(&mutex);\n");

    printf("\n");
    printf("MISTAKE 2: Signaling without changing the condition\n");
    printf("  // WRONG\n");
    printf("  pthread_cond_signal(&cond);  // ready_flag still false!\n");
    printf("  \n");
    printf("  // RIGHT\n");
    printf("  pthread_mutex_lock(&mutex);\n");
    printf("  ready_flag = 1;  // Change condition first!\n");
    printf("  pthread_cond_signal(&cond);\n");
    printf("  pthread_mutex_unlock(&mutex);\n");

    printf("\n");
    printf("MISTAKE 3: Using wrong mutex with condition variable\n");
    printf("  // The mutex passed to cond_wait must be the one\n");
    printf("  // protecting the condition being checked!\n");

    printf("\n");
    printf("MISTAKE 4: Not broadcasting when needed\n");
    printf("  // If multiple threads might need to wake up,\n");
    printf("  // use broadcast() instead of signal()\n");
    printf("  // Example: when shutting down a thread pool\n");
}

/* ============================================================================
MAIN
============================================================================
*/

int main(int argc, char *argv[]) {
    printf("\n================================================\n");
    printf("  Module 04: Producer-Consumer\n");
    printf("================================================\n");

    if (argc > 1) {
        int ex = atoi(argv[1]);
        switch (ex) {
            case 1: exercise1_simple_pc(); break;
            case 2: exercise2_multi_pc(); break;
            case 3: exercise3_while_vs_if(); break;
            case 4: exercise4_signal_vs_broadcast(); break;
            case 5: exercise5_common_mistakes(); break;
            default: printf("Unknown exercise: %d\n", ex);
        }
    } else {
        exercise1_simple_pc();
        exercise3_while_vs_if();
        exercise5_common_mistakes();
    }

    printf("\n================================================\n");
    printf("  Producer-Consumer Complete!\n");
    printf("  This pattern is essential for thread pools!\n");
    printf("================================================\n\n");

    return 0;
}
