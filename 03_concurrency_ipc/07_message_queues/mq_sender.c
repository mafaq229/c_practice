/*
 * CS-6200 Preparation - Module 07: Message Queue Sender
 *
 * POSIX message queues for inter-process communication.
 * Alternative to shared memory for IPC.
 *
 * NOTE: This requires Linux - use Docker on macOS!
 *
 * Compile: clang -Wall -Wextra -g mq_sender.c -o mq_sender -lrt
 * Run:     ./mq_sender
 *
 * Difficulty: [HARD]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <mqueue.h>
#include <errno.h>

/* ============================================================================
 * CONCEPT: POSIX Message Queues
 * ============================================================================
 *
 * Message queues provide a way to pass discrete messages between processes.
 * Unlike shared memory, data is copied (not shared).
 *
 * Advantages:
 * - Built-in synchronization (no need for semaphores)
 * - Message boundaries preserved
 * - Can have priority ordering
 *
 * Key functions:
 * - mq_open():    Create/open message queue
 * - mq_send():    Send a message
 * - mq_receive(): Receive a message (blocks by default)
 * - mq_close():   Close queue descriptor
 * - mq_unlink():  Remove queue
 */

#define MQ_NAME "/gios_prep_mq"
#define MAX_MSG_SIZE 256
#define MAX_MESSAGES 10

/* Message structure */
typedef struct {
    int id;
    char text[MAX_MSG_SIZE - sizeof(int)];
} Message;

int main(void) {
    printf("\n================================================\n");
    printf("  Module 07: Message Queue Sender\n");
    printf("================================================\n\n");

    /*
     * Create/open message queue
     *
     * mq_attr specifies queue properties:
     * - mq_maxmsg: Maximum number of messages in queue
     * - mq_msgsize: Maximum size of each message
     */
    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = MAX_MESSAGES,
        .mq_msgsize = sizeof(Message),
        .mq_curmsgs = 0
    };

    printf("Creating message queue '%s'...\n", MQ_NAME);

    mqd_t mq = mq_open(MQ_NAME, O_CREAT | O_WRONLY, 0666, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        if (errno == ENOSYS) {
            printf("Note: POSIX message queues not supported. Use Docker.\n");
        }
        return 1;
    }

    printf("Message queue created successfully.\n\n");

    /*
     * Send messages
     *
     * mq_send() signature:
     *   mq_send(mqd_t mq, const char *msg_ptr, size_t msg_len, unsigned priority)
     *
     * Priority: higher values = higher priority (delivered first)
     */
    printf("Sending messages...\n");

    for (int i = 0; i < 5; i++) {
        Message msg;
        msg.id = i + 1;
        snprintf(msg.text, sizeof(msg.text),
                "Hello from sender (PID %d), message #%d", getpid(), msg.id);

        /* Send with priority equal to message id */
        if (mq_send(mq, (char *)&msg, sizeof(Message), msg.id) < 0) {
            perror("mq_send");
            break;
        }

        printf("  Sent: id=%d, priority=%d, text='%s'\n",
               msg.id, msg.id, msg.text);

        sleep(1);
    }

    /* Send "done" message with highest priority */
    Message done_msg = {.id = 0};
    strncpy(done_msg.text, "DONE", sizeof(done_msg.text));
    mq_send(mq, (char *)&done_msg, sizeof(Message), 99);

    printf("\nSent completion message.\n");

    /* Close (but don't unlink - let receiver do that) */
    mq_close(mq);

    printf("\n================================================\n");
    printf("  Sender Complete!\n");
    printf("  Run mq_receiver to read messages.\n");
    printf("================================================\n\n");

    return 0;
}
