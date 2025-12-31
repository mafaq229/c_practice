/*
CS-6200 Preparation - Module 03: Race Conditions

Understand why shared data needs protection.
This is THE most important concept in concurrent programming!

Compile: clang -Wall -Wextra -g -pthread race_condition.c -o race_condition
Run:     ./race_condition

Difficulty: [HARD]
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/* ============================================================================
CONCEPT: Race Conditions
============================================================================

A RACE CONDITION occurs when:
1. Multiple threads access shared data
2. At least one thread writes
3. No synchronization is used

The result depends on thread scheduling - it's non-deterministic!

Example: counter++ is NOT atomic! It consists of:
1. LOAD: Read counter from memory
2. ADD:  Add 1 to the value
3. STORE: Write new value back to memory

If two threads do this simultaneously, increments can be lost!

Thread A: LOAD(0), ADD(1), STORE(1)
Thread B: LOAD(0), ADD(1), STORE(1)
Result: 1 (should be 2!)
*/

/* ============================================================================
EXERCISE 1: Classic counter race condition
============================================================================
*/

volatile int counter = 0;  /* volatile prevents compiler optimization */
#define ITERATIONS 1000000

void *increment_counter(void *arg) {
    (void)arg;

    for (int i = 0; i < ITERATIONS; i++) {
        counter++;  /* RACE CONDITION! */
    }

    return NULL;
}

void exercise1_counter_race(void) {
    printf("\n=== Exercise 1: Counter Race Condition ===\n");

    counter = 0;
    pthread_t t1, t2;

    printf("Expected final count: %d\n", ITERATIONS * 2);
    printf("Running with 2 threads...\n");

    pthread_create(&t1, NULL, increment_counter, NULL);
    pthread_create(&t2, NULL, increment_counter, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Actual count: %d\n", counter);
    printf("Lost updates: %d\n", (ITERATIONS * 2) - counter);
    printf("\nThis demonstrates lost updates due to race condition.\n");
}

/* ============================================================================
EXERCISE 2: Visualizing the race
============================================================================
*/

int slow_counter = 0;

void *slow_increment(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < 3; i++) {
        /* Simulate the three steps of counter++ with delays */

        /* Step 1: READ */
        int temp = slow_counter;
        printf("Thread %d: Read value %d\n", id, temp);

        /* Small delay makes interleaving more visible */
        usleep(100);

        /* Step 2: INCREMENT */
        temp = temp + 1;
        printf("Thread %d: Incremented to %d\n", id, temp);

        usleep(100);

        /* Step 3: WRITE */
        slow_counter = temp;
        printf("Thread %d: Wrote value %d\n", id, slow_counter);

        printf("---\n");
    }

    return NULL;
}

void exercise2_visualize_race(void) {
    printf("\n=== Exercise 2: Visualizing the Race ===\n");

    slow_counter = 0;
    pthread_t t1, t2;
    int id1 = 1, id2 = 2;

    printf("Watch how threads interleave:\n\n");

    pthread_create(&t1, NULL, slow_increment, &id1);
    pthread_create(&t2, NULL, slow_increment, &id2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("\nExpected final: 6\n");
    printf("Actual final: %d\n", slow_counter);
}

/* ============================================================================
EXERCISE 3: Bank account race condition
============================================================================
*/

typedef struct {
    int balance;
} BankAccount;

BankAccount account = {1000};

void *withdraw(void *arg) {
    int amount = *(int *)arg;

    /* Check balance first (read) */
    if (account.balance >= amount) {
        printf("Withdrawal: Balance is %d, withdrawing %d\n",
               account.balance, amount);

        usleep(1);  /* Simulate processing delay */

        /* Withdraw (write) */
        account.balance -= amount;
        printf("Withdrawal: New balance is %d\n", account.balance);
    } else {
        printf("Withdrawal: Insufficient funds\n");
    }

    return NULL;
}

void exercise3_bank_race(void) {
    printf("\n=== Exercise 3: Bank Account Race ===\n");

    account.balance = 100;
    pthread_t threads[5];
    int amount = 50;

    printf("Initial balance: %d\n", account.balance);
    printf("5 threads each trying to withdraw %d\n\n", amount);

    for (int i = 0; i < 5; i++) {
        pthread_create(&threads[i], NULL, withdraw, &amount);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\nFinal balance: %d\n", account.balance);
    printf("Could go negative due to race condition!\n");
}

/* ============================================================================
EXERCISE 4: Read-Write race
============================================================================
*/

typedef struct {
    int x;
    int y;
} Point;

Point shared_point = {0, 0};

void *writer_thread(void *arg) {
    (void)arg;

    for (int i = 0; i < 100; i++) {
        /* Should always be x == y, but... */
        shared_point.x = i;
        shared_point.y = i;
    }

    return NULL;
}

void *reader_thread(void *arg) {
    int *mismatches = (int *)arg;
    *mismatches = 0;

    for (int i = 0; i < 1000; i++) {
        int x = shared_point.x;
        int y = shared_point.y;

        if (x != y) {
            (*mismatches)++;
            printf("Mismatch! x=%d, y=%d\n", x, y);
        }
    }

    return NULL;
}

void exercise4_read_write_race(void) {
    printf("\n=== Exercise 4: Read-Write Race ===\n");

    pthread_t writer, reader;
    int mismatches;

    printf("Point should always have x == y\n");
    printf("Writer sets both to same value\n");
    printf("Reader checks if they match\n\n");

    pthread_create(&writer, NULL, writer_thread, NULL);
    pthread_create(&reader, NULL, reader_thread, &mismatches);

    pthread_join(writer, NULL);
    pthread_join(reader, NULL);

    printf("\nTotal mismatches detected: %d\n", mismatches);
    printf("Note: You might not see mismatches on every run.\n");
    printf("Race conditions can be intermittent!\n");
}

/* ============================================================================
EXERCISE 5: Linked list race condition
============================================================================
*/

typedef struct Node {
    int value;
    struct Node *next;
} Node;

Node *head = NULL;

void *add_to_list(void *arg) {
    int value = *(int *)arg;

    /* Create new node */
    Node *new_node = malloc(sizeof(Node));
    new_node->value = value;

    /* Add to front of list (RACE CONDITION!) */
    new_node->next = head;  /* Read head */
    /* Another thread could modify head here! */
    head = new_node;        /* Write head */

    return NULL;
}

int count_list(void) {
    int count = 0;
    Node *current = head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

void free_list(void) {
    Node *current = head;
    while (current != NULL) {
        Node *next = current->next;
        free(current);
        current = next;
    }
    head = NULL;
}

void exercise5_list_race(void) {
    printf("\n=== Exercise 5: Linked List Race ===\n");

    head = NULL;
    pthread_t threads[100];
    int values[100];

    printf("Adding 100 nodes with 100 threads...\n");

    for (int i = 0; i < 100; i++) {
        values[i] = i;
        pthread_create(&threads[i], NULL, add_to_list, &values[i]);
    }

    for (int i = 0; i < 100; i++) {
        pthread_join(threads[i], NULL);
    }

    int count = count_list();
    printf("Expected nodes: 100\n");
    printf("Actual nodes: %d\n", count);

    if (count < 100) {
        printf("Lost %d nodes due to race condition!\n", 100 - count);
    }

    free_list();
}

/* ============================================================================
EXERCISE 6: String race condition
============================================================================
*/

char shared_buffer[64] = {0};

void *write_string(void *arg) {
    char *str = (char *)arg;

    /* Write one character at a time (maximizes race visibility) */
    for (size_t i = 0; str[i] != '\0' && i < sizeof(shared_buffer) - 1; i++) {
        shared_buffer[i] = str[i];
        shared_buffer[i + 1] = '\0';
    }

    return NULL;
}

void exercise6_string_race(void) {
    printf("\n=== Exercise 6: String Race ===\n");

    pthread_t t1, t2;

    printf("Two threads writing different strings to same buffer:\n\n");

    pthread_create(&t1, NULL, write_string, "AAAAAAAAAAAAAAAA");
    pthread_create(&t2, NULL, write_string, "BBBBBBBBBBBBBBBB");

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Result: '%s'\n", shared_buffer);
    printf("Could be all A's, all B's, or mixed!\n");
}

/* ============================================================================
MAIN
============================================================================
*/

int main(int argc, char *argv[]) {
    printf("\n================================================\n");
    printf("  Module 03: Race Conditions\n");
    printf("  (Run multiple times - results may vary!)\n");
    printf("================================================\n");

    if (argc > 1) {
        int ex = atoi(argv[1]);
        switch (ex) {
            case 1: exercise1_counter_race(); break;
            case 2: exercise2_visualize_race(); break;
            case 3: exercise3_bank_race(); break;
            case 4: exercise4_read_write_race(); break;
            case 5: exercise5_list_race(); break;
            case 6: exercise6_string_race(); break;
            default: printf("Unknown exercise: %d\n", ex);
        }
    } else {
        exercise1_counter_race();
        exercise3_bank_race();
        exercise5_list_race();
    }

    printf("\n================================================\n");
    printf("  Race Conditions Demo Complete!\n");
    printf("  Next: Learn to fix these with mutexes!\n");
    printf("================================================\n\n");

    return 0;
}
