/*
CS-6200 Preparation - Module 03: TCP Client

A simple TCP client that connects to a server and sends/receives data.

Compile: clang -Wall -Wextra -g tcp_client.c -o tcp_client
Run:     ./tcp_client [host] [port]

First start the server in another terminal:
  ./tcp_server 8080

Difficulty: [MEDIUM]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT 8080
#define BUFFER_SIZE 4096

/*
TODO: Connect to server.
Returns socket fd on success, -1 on failure.
*/
int connect_to_server(const char *host, int port) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Create socket: socket(AF_INET, SOCK_STREAM, 0)
     * 2. Create address structure:
     *    struct sockaddr_in addr = {
     *        .sin_family = AF_INET,
     *        .sin_port = htons(port)
     *    };
     *    inet_pton(AF_INET, host, &addr.sin_addr);
     * 3. Connect: connect(sockfd, (struct sockaddr*)&addr, sizeof(addr))
     * 4. Return socket fd
     */

    // AF_INET is for IPv4, SOCK_STREAM is for TCP
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    /* TODO: Connect to server */
    /* Your code here... */
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port) // Convert port to network byte order
    };

    // inet_pton converts the IP address from text to binary form
    if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockfd);
        return -1;
    }

    // first connection is is made then checked for errors
    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sockfd);
        return -1;
    }
    return sockfd; // return the connected socket file descriptor
}

/*
Send all data (handles partial sends)
*/
ssize_t send_all(int fd, const void *buf, size_t len) {
    const char *ptr = buf;
    size_t remaining = len;

    while (remaining > 0) {
        ssize_t sent = send(fd, ptr, remaining, 0);
        if (sent < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        ptr += sent;
        remaining -= sent;
    }

    return len;
}

/*
Receive all data (handles partial receives)
*/
ssize_t recv_all(int fd, void *buf, size_t len) {
    char *ptr = buf;
    size_t remaining = len;

    while (remaining > 0) {
        ssize_t received = recv(fd, ptr, remaining, 0);
        if (received < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (received == 0) {
            /* Connection closed */
            break;
        }
        ptr += received;
        remaining -= received;
    }

    return len - remaining;
}

/*
Interactive client session
*/
void interactive_session(int sockfd) {
    printf("Connected! Type messages to send (Ctrl+D to quit)\n\n");

    char buffer[BUFFER_SIZE];

    while (1) {
        printf("> ");
        fflush(stdout);

        /* Read from stdin */
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break;  /* EOF or error */
        }

        size_t len = strlen(buffer);
        if (len == 0) continue;

        /* Send to server */
        if (send_all(sockfd, buffer, len) < 0) {
            perror("send");
            break;
        }

        /* Receive response */
        ssize_t n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (n < 0) {
            perror("recv");
            break;
        }
        if (n == 0) {
            printf("Server closed connection\n");
            break;
        }

        buffer[n] = '\0';
        printf("< %s", buffer);
    }
}

/*
A simpler working example for reference
*/
void simple_client_example(const char *host, int port) {
    printf("\n=== Simple Client Example (Reference) ===\n\n");

    /* Create socket */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return;
    }

    /* Set up server address */
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port)
    };

    if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockfd);
        return;
    }

    /* Connect */
    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sockfd);
        return;
    }

    printf("Connected to %s:%d\n", host, port);

    /* Send a test message */
    const char *msg = "Hello, Server!\n";
    send(sockfd, msg, strlen(msg), 0);
    printf("Sent: %s", msg);

    /* Receive response */
    char buffer[BUFFER_SIZE];
    ssize_t n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Received: %s", buffer);
    }

    close(sockfd);
    printf("Connection closed.\n");
}

/*
Run the client
*/
void run_client(const char *host, int port) {
    printf("Connecting to %s:%d...\n", host, port);

    int sockfd = connect_to_server(host, port);
    if (sockfd < 0) {
        fprintf(stderr, "Failed to connect to server\n");
        return;
    }

    printf("Connected!\n");
    interactive_session(sockfd);

    close(sockfd);
    printf("Disconnected.\n");
}

int main(int argc, char *argv[]) {
    printf("\n================================================\n");
    printf("  Module 03: TCP Client\n");
    printf("================================================\n\n");

    const char *host = DEFAULT_HOST;
    int port = DEFAULT_PORT;

    if (argc > 1) {
        host = argv[1];
    }
    if (argc > 2) {
        port = atoi(argv[2]);
    }

    if (argc > 3 && strcmp(argv[3], "--example") == 0) {
        /* Run the working example */
        simple_client_example(host, port);
    } else {
        /* Run the TODO version */
        run_client(host, port);
    }

    return 0;
}
