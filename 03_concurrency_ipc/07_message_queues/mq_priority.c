/*
CS-6200 Preparation - Module 07: Priority Message Queues

Demonstrates priority ordering in message queues.
Higher priority messages are delivered first!

NOTE: This requires Linux - use Docker on macOS!

Compile: clang -Wall -Wextra -g mq_priority.c -o mq_priority -lrt
Run:     ./mq_priority

Difficulty: [HARD]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/wait.h>
#include <errno.h>

#define MQ_NAME "/gios_priority_mq"
#define MAX_MSG_SIZE 128
#define MAX_MESSAGES 10

typedef struct {
    int id;
    char text[120];
} Message;

int main(void) {
    printf("\n================================================\n");
    printf("  Module 07: Priority Message Queues\n");
    printf("================================================\n\n");

    /* Remove any existing queue */
    mq_unlink(MQ_NAME);

    /* Create queue */
    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = MAX_MESSAGES,
        .mq_msgsize = sizeof(Message),
        .mq_curmsgs = 0
    };

    mqd_t mq = mq_open(MQ_NAME, O_CREAT | O_RDWR, 0666, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        return 1;
    }

    printf("Sending messages with different priorities...\n\n");

    /*
     * Send messages in random priority order
     */
    struct {
        int id;
        int priority;
        const char *text;
    } messages[] = {
        {1, 1, "Low priority (1)"},
        {2, 5, "Medium priority (5)"},
        {3, 10, "High priority (10)"},
        {4, 3, "Low-medium priority (3)"},
        {5, 7, "Medium-high priority (7)"},
        {6, 2, "Low priority (2)"},
        {7, 9, "High priority (9)"},
        {8, 4, "Medium priority (4)"},
    };

    int num_messages = sizeof(messages) / sizeof(messages[0]);

    for (int i = 0; i < num_messages; i++) {
        Message msg;
        msg.id = messages[i].id;
        strncpy(msg.text, messages[i].text, sizeof(msg.text));

        if (mq_send(mq, (char *)&msg, sizeof(Message), messages[i].priority) < 0) {
            perror("mq_send");
            break;
        }

        printf("  Sent: id=%d, priority=%d, text='%s'\n",
               msg.id, messages[i].priority, msg.text);
    }

    printf("\n--- All messages sent ---\n");
    printf("\nReceiving messages (should be in priority order)...\n\n");

    /*
     * Receive messages - they come out in priority order!
     * Highest priority first.
     */
    Message msg;
    unsigned int priority;
    int last_priority = 100;  /* Start high */

    for (int i = 0; i < num_messages; i++) {
        ssize_t bytes = mq_receive(mq, (char *)&msg, sizeof(Message), &priority);

        if (bytes < 0) {
            perror("mq_receive");
            break;
        }

        printf("  Received: id=%d, priority=%u, text='%s'",
               msg.id, priority, msg.text);

        if (priority > last_priority) {
            printf(" [ERROR: Out of order!]");
        }
        printf("\n");

        last_priority = priority;
    }

    /* Cleanup */
    mq_close(mq);
    mq_unlink(MQ_NAME);

    printf("\n================================================\n");
    printf("  Priority Queue Demo Complete!\n");
    printf("\n");
    printf("  Notice:\n");
    printf("  - Messages came out highest priority first\n");
    printf("  - Order within same priority is FIFO\n");
    printf("  - Useful for request prioritization\n");
    printf("================================================\n\n");

    return 0;
}
