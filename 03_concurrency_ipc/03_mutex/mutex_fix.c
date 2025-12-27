/*
 * CS-6200 Preparation - Module 03: Mutex Fix
 *
 * Learn how to fix race conditions using mutexes.
 * Essential for GIOS Project 1 Part 2!
 *
 * Compile: clang -Wall -Wextra -g -pthread mutex_fix.c -o mutex_fix
 * Run:     ./mutex_fix
 *
 * Difficulty: [HARD]
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/* ============================================================================
 * CONCEPT: Mutex (Mutual Exclusion)
 * ============================================================================
 *
 * A mutex ensures only ONE thread can access protected code at a time.
 *
 * Basic usage:
 *   pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
 *
 *   pthread_mutex_lock(&mutex);    // Acquire lock (blocks if held)
 *   // ... critical section ...     // Only one thread at a time here
 *   pthread_mutex_unlock(&mutex);  // Release lock
 *
 * Rules:
 * 1. Always unlock what you lock
 * 2. Keep critical sections short
 * 3. Don't forget to unlock on all code paths (including errors!)
 * 4. The same thread must lock and unlock (no cross-thread unlocking)
 */

/* ============================================================================
 * EXERCISE 1: Fix the counter race condition
 * ============================================================================
 */

int counter = 0;
pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

#define ITERATIONS 1000000

void *safe_increment(void *arg) {
    (void)arg;

    for (int i = 0; i < ITERATIONS; i++) {
        /*
         * TODO: Protect counter increment with mutex
         *
         * Pattern:
         * 1. Lock mutex
         * 2. Modify shared data
         * 3. Unlock mutex
         */
        pthread_mutex_lock(&counter_mutex);
        counter++;
        pthread_mutex_unlock(&counter_mutex);
    }

    return NULL;
}

void exercise1_counter_fix(void) {
    printf("\n=== Exercise 1: Fixed Counter ===\n");

    counter = 0;
    pthread_t t1, t2;

    printf("Expected: %d\n", ITERATIONS * 2);

    pthread_create(&t1, NULL, safe_increment, NULL);
    pthread_create(&t2, NULL, safe_increment, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Actual:   %d\n", counter);
    printf("Match: %s\n", counter == ITERATIONS * 2 ? "YES!" : "NO");
}

/* ============================================================================
 * EXERCISE 2: Fix the bank account race condition
 * ============================================================================
 */

typedef struct {
    int balance;
    pthread_mutex_t mutex;
} BankAccount;

BankAccount account;

void init_account(int initial_balance) {
    account.balance = initial_balance;
    pthread_mutex_init(&account.mutex, NULL);
}

void destroy_account(void) {
    pthread_mutex_destroy(&account.mutex);
}

int safe_withdraw(int amount) {
    /*
     * TODO: Implement safe withdrawal
     *
     * 1. Lock mutex
     * 2. Check balance
     * 3. If sufficient, withdraw
     * 4. Unlock mutex (on ALL paths!)
     * 5. Return success/failure
     */

    int success = 0;

    pthread_mutex_lock(&account.mutex);

    if (account.balance >= amount) {
        account.balance -= amount;
        success = 1;
        printf("Withdrew %d, new balance: %d\n", amount, account.balance);
    } else {
        printf("Failed to withdraw %d: insufficient funds (%d available)\n",
               amount, account.balance);
    }

    pthread_mutex_unlock(&account.mutex);

    return success;
}

void *withdrawal_thread(void *arg) {
    int amount = *(int *)arg;
    safe_withdraw(amount);
    return NULL;
}

void exercise2_bank_fix(void) {
    printf("\n=== Exercise 2: Fixed Bank Account ===\n");

    init_account(100);

    pthread_t threads[5];
    int amount = 50;

    printf("Initial balance: 100\n");
    printf("5 threads trying to withdraw 50 each\n\n");

    for (int i = 0; i < 5; i++) {
        pthread_create(&threads[i], NULL, withdrawal_thread, &amount);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\nFinal balance: %d (should be >= 0)\n", account.balance);

    destroy_account();
}

/* ============================================================================
 * EXERCISE 3: Fix the linked list race condition
 * ============================================================================
 */

typedef struct Node {
    int value;
    struct Node *next;
} Node;

typedef struct {
    Node *head;
    pthread_mutex_t mutex;
} ThreadSafeList;

ThreadSafeList list;

void list_init(void) {
    list.head = NULL;
    pthread_mutex_init(&list.mutex, NULL);
}

void list_destroy(void) {
    pthread_mutex_lock(&list.mutex);

    Node *current = list.head;
    while (current != NULL) {
        Node *next = current->next;
        free(current);
        current = next;
    }
    list.head = NULL;

    pthread_mutex_unlock(&list.mutex);
    pthread_mutex_destroy(&list.mutex);
}

void list_add(int value) {
    /*
     * TODO: Add node to front of list (thread-safe)
     */

    Node *new_node = malloc(sizeof(Node));
    new_node->value = value;

    pthread_mutex_lock(&list.mutex);

    new_node->next = list.head;
    list.head = new_node;

    pthread_mutex_unlock(&list.mutex);
}

int list_count(void) {
    pthread_mutex_lock(&list.mutex);

    int count = 0;
    Node *current = list.head;
    while (current != NULL) {
        count++;
        current = current->next;
    }

    pthread_mutex_unlock(&list.mutex);
    return count;
}

void *list_adder(void *arg) {
    int value = *(int *)arg;
    list_add(value);
    return NULL;
}

void exercise3_list_fix(void) {
    printf("\n=== Exercise 3: Fixed Linked List ===\n");

    list_init();

    pthread_t threads[100];
    int values[100];

    printf("Adding 100 nodes with 100 threads...\n");

    for (int i = 0; i < 100; i++) {
        values[i] = i;
        pthread_create(&threads[i], NULL, list_adder, &values[i]);
    }

    for (int i = 0; i < 100; i++) {
        pthread_join(threads[i], NULL);
    }

    int count = list_count();
    printf("Expected: 100\n");
    printf("Actual:   %d\n", count);
    printf("Match: %s\n", count == 100 ? "YES!" : "NO");

    list_destroy();
}

/* ============================================================================
 * EXERCISE 4: Using dynamic mutex initialization
 * ============================================================================
 */

void exercise4_dynamic_mutex(void) {
    printf("\n=== Exercise 4: Dynamic Mutex Initialization ===\n");

    /*
     * Two ways to initialize a mutex:
     *
     * 1. Static (at declaration):
     *    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
     *
     * 2. Dynamic (at runtime):
     *    pthread_mutex_t mutex;
     *    pthread_mutex_init(&mutex, NULL);
     *    // ... use mutex ...
     *    pthread_mutex_destroy(&mutex);
     *
     * Dynamic is useful when:
     * - Mutex is in a struct allocated with malloc
     * - You want to use special attributes
     */

    pthread_mutex_t *mutex = malloc(sizeof(pthread_mutex_t));

    if (pthread_mutex_init(mutex, NULL) != 0) {
        fprintf(stderr, "Mutex init failed\n");
        free(mutex);
        return;
    }

    printf("Mutex initialized dynamically\n");

    pthread_mutex_lock(mutex);
    printf("Locked!\n");
    pthread_mutex_unlock(mutex);
    printf("Unlocked!\n");

    pthread_mutex_destroy(mutex);
    printf("Destroyed!\n");

    free(mutex);
}

/* ============================================================================
 * EXERCISE 5: trylock (non-blocking lock)
 * ============================================================================
 */

pthread_mutex_t trylock_mutex = PTHREAD_MUTEX_INITIALIZER;

void *trylock_thread(void *arg) {
    int id = *(int *)arg;

    /*
     * pthread_mutex_trylock() returns:
     * - 0 if lock acquired
     * - EBUSY if lock not available
     *
     * Useful when you don't want to block.
     */

    if (pthread_mutex_trylock(&trylock_mutex) == 0) {
        printf("Thread %d: Got the lock!\n", id);
        sleep(1);
        pthread_mutex_unlock(&trylock_mutex);
        printf("Thread %d: Released the lock\n", id);
    } else {
        printf("Thread %d: Lock busy, doing other work instead\n", id);
    }

    return NULL;
}

void exercise5_trylock(void) {
    printf("\n=== Exercise 5: Non-blocking trylock ===\n");

    pthread_t t1, t2;
    int id1 = 1, id2 = 2;

    pthread_create(&t1, NULL, trylock_thread, &id1);
    usleep(100000);  /* Give t1 time to get lock */
    pthread_create(&t2, NULL, trylock_thread, &id2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
}

/* ============================================================================
 * EXERCISE 6: Protecting a structure with multiple fields
 * ============================================================================
 */

typedef struct {
    int x;
    int y;
    int z;
    pthread_mutex_t mutex;
} Point3D;

Point3D point = {0, 0, 0, PTHREAD_MUTEX_INITIALIZER};

void *update_point(void *arg) {
    int value = *(int *)arg;

    /*
     * When updating related fields, lock the entire update
     * to maintain consistency.
     */
    pthread_mutex_lock(&point.mutex);

    point.x = value;
    point.y = value;
    point.z = value;

    pthread_mutex_unlock(&point.mutex);

    return NULL;
}

void *check_point(void *arg) {
    int *inconsistencies = (int *)arg;
    *inconsistencies = 0;

    for (int i = 0; i < 1000; i++) {
        pthread_mutex_lock(&point.mutex);

        /* Check that all coordinates match */
        if (point.x != point.y || point.y != point.z) {
            (*inconsistencies)++;
        }

        pthread_mutex_unlock(&point.mutex);
    }

    return NULL;
}

void exercise6_struct_protection(void) {
    printf("\n=== Exercise 6: Protecting Multi-field Structure ===\n");

    pthread_t writer, checker;
    int value = 42;
    int inconsistencies;

    pthread_create(&writer, NULL, update_point, &value);
    pthread_create(&checker, NULL, check_point, &inconsistencies);

    pthread_join(writer, NULL);
    pthread_join(checker, NULL);

    printf("Inconsistencies found: %d\n", inconsistencies);
    printf("With proper locking, should always be 0!\n");
}

/* ============================================================================
 * MAIN
 * ============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n================================================\n");
    printf("  Module 03: Mutex Fix\n");
    printf("================================================\n");

    if (argc > 1) {
        int ex = atoi(argv[1]);
        switch (ex) {
            case 1: exercise1_counter_fix(); break;
            case 2: exercise2_bank_fix(); break;
            case 3: exercise3_list_fix(); break;
            case 4: exercise4_dynamic_mutex(); break;
            case 5: exercise5_trylock(); break;
            case 6: exercise6_struct_protection(); break;
            default: printf("Unknown exercise: %d\n", ex);
        }
    } else {
        exercise1_counter_fix();
        exercise2_bank_fix();
        exercise3_list_fix();
        exercise4_dynamic_mutex();
    }

    printf("\n================================================\n");
    printf("  Mutex Fix Complete!\n");
    printf("  Race conditions eliminated!\n");
    printf("================================================\n\n");

    return 0;
}
