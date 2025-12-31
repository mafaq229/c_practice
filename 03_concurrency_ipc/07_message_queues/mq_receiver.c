/*
CS-6200 Preparation - Module 07: Message Queue Receiver

Receives messages from the queue created by mq_sender.

NOTE: This requires Linux - use Docker on macOS!

Usage:
  Terminal 1: ./mq_sender
  Terminal 2: ./mq_receiver

Compile: clang -Wall -Wextra -g mq_receiver.c -o mq_receiver -lrt
Run:     ./mq_receiver

Difficulty: [HARD]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <mqueue.h>
#include <errno.h>

#define MQ_NAME "/gios_prep_mq"
#define MAX_MSG_SIZE 256

/* Message structure - must match sender! */
typedef struct {
    int id;
    char text[MAX_MSG_SIZE - sizeof(int)];
} Message;

int main(void) {
    printf("\n================================================\n");
    printf("  Module 07: Message Queue Receiver\n");
    printf("================================================\n\n");

    /*
     * Open existing message queue (read-only)
     */
    printf("Opening message queue '%s'...\n", MQ_NAME);

    mqd_t mq = mq_open(MQ_NAME, O_RDONLY);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        if (errno == ENOENT) {
            printf("Queue doesn't exist. Run mq_sender first!\n");
        }
        return 1;
    }

    printf("Message queue opened.\n");

    /* Get queue attributes */
    struct mq_attr attr;
    mq_getattr(mq, &attr);

    printf("Queue info: maxmsg=%ld, msgsize=%ld, curmsgs=%ld\n\n",
           attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);

    /*
     * Receive messages
     *
     * mq_receive() signature:
     *   mq_receive(mqd_t mq, char *msg_ptr, size_t msg_len, unsigned *priority)
     *
     * Note: msg_len must be >= mq_msgsize from queue attributes!
     * Messages are received in priority order (highest first).
     */
    printf("Receiving messages...\n\n");

    Message msg;
    unsigned int priority;

    while (1) {
        ssize_t bytes = mq_receive(mq, (char *)&msg, sizeof(Message), &priority);

        if (bytes < 0) {
            perror("mq_receive");
            break;
        }

        printf("  Received: id=%d, priority=%u, text='%s'\n",
               msg.id, priority, msg.text);

        if (strcmp(msg.text, "DONE") == 0) {
            printf("\nReceived completion message.\n");
            break;
        }
    }

    /* Close and cleanup */
    mq_close(mq);

    printf("\nCleaning up queue...\n");
    if (mq_unlink(MQ_NAME) < 0) {
        perror("mq_unlink");
    }

    printf("\n================================================\n");
    printf("  Receiver Complete!\n");
    printf("================================================\n\n");

    return 0;
}
