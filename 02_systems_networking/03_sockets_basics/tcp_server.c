/*
CS-6200 Preparation - Module 03: TCP Echo Server

This is a simple TCP echo server. It receives data from clients
and sends it back. This pattern is the foundation for GIOS Project 1.

Compile: clang -Wall -Wextra -g tcp_server.c -o tcp_server
Run:     ./tcp_server [port]
Test:    In another terminal: nc localhost 8080

Difficulty: [MEDIUM]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DEFAULT_PORT 8080
#define BUFFER_SIZE 4096
#define BACKLOG 10

/* Global for signal handler */
volatile sig_atomic_t running = 1;

/*
Signal handler for graceful shutdown
*/
void handle_signal(int sig) {
    (void)sig;
    running = 0;
    printf("\nShutting down...\n");
}

/*
TODO: Create and configure a server socket.
Returns socket fd on success, -1 on failure.
*/
int create_server_socket(int port) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Create socket: socket(AF_INET, SOCK_STREAM, 0)
     * 2. Set SO_REUSEADDR option (allows quick restart)
     * 3. Create address structure:
     *    struct sockaddr_in addr = {
     *        .sin_family = AF_INET,
     *        .sin_addr.s_addr = INADDR_ANY,
     *        .sin_port = htons(port)
     *    };
     * 4. Bind socket to address
     * 5. Start listening: listen(sockfd, BACKLOG)
     * 6. Return socket fd
     */

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    /* Allow address reuse (prevents "Address already in use" error) */
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(sockfd);
        return -1;
    }

    /* TODO: Bind and listen */
    /* Your code here... */

    printf("TODO: Complete bind() and listen() in create_server_socket\n");
    close(sockfd);
    return -1;  /* TODO: Return sockfd instead when implemented */
}

/*
TODO: Handle a single client connection.
Echo back any data received.
*/
void handle_client(int client_fd, struct sockaddr_in *client_addr) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Print client info (IP and port)
     * 2. Loop:
     *    a. recv() data into buffer
     *    b. If recv returns 0, client disconnected
     *    c. If recv returns -1, error occurred
     *    d. Echo data back with send()
     * 3. Close client socket
     */

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr->sin_addr, client_ip, sizeof(client_ip));
    int client_port = ntohs(client_addr->sin_port);

    printf("Client connected: %s:%d\n", client_ip, client_port);

    char buffer[BUFFER_SIZE];

    /* TODO: Implement echo loop */
    /* Your code here... */

    printf("TODO: Implement echo loop in handle_client\n");

    printf("Client disconnected: %s:%d\n", client_ip, client_port);
    close(client_fd);
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
Main server loop
*/
void run_server(int port) {
    printf("Starting server on port %d...\n", port);

    int server_fd = create_server_socket(port);
    if (server_fd < 0) {
        fprintf(stderr, "Failed to create server socket\n");
        return;
    }

    printf("Server listening on port %d\n", port);
    printf("Press Ctrl+C to stop\n\n");

    /* Install signal handler */
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    while (running) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);

        /* Accept new connection */
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_fd < 0) {
            if (errno == EINTR) continue;  /* Interrupted by signal */
            perror("accept");
            continue;
        }

        /* Handle the client (single-threaded - blocks on each client) */
        handle_client(client_fd, &client_addr);
    }

    close(server_fd);
    printf("Server stopped.\n");
}

/*
A simpler working example for reference
*/
void simple_server_example(int port) {
    printf("\n=== Simple Server Example (Reference) ===\n");
    printf("This shows the complete implementation.\n\n");

    /* Create socket */
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return;
    }

    /* Set socket options */
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    /* Bind to address */
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(port)
    };

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(server_fd);
        return;
    }

    /* Start listening */
    if (listen(server_fd, BACKLOG) < 0) {
        perror("listen");
        close(server_fd);
        return;
    }

    printf("Simple server listening on port %d\n", port);
    printf("Connect with: nc localhost %d\n", port);
    printf("Press Ctrl+C to stop\n\n");

    signal(SIGINT, handle_signal);

    while (running) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_fd < 0) {
            if (!running) break;
            perror("accept");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        printf("Client connected: %s:%d\n", client_ip, ntohs(client_addr.sin_port));

        /* Echo loop */
        char buffer[BUFFER_SIZE];
        ssize_t n;

        while ((n = recv(client_fd, buffer, sizeof(buffer), 0)) > 0) {
            printf("Received %zd bytes\n", n);
            send_all(client_fd, buffer, n);
        }

        if (n < 0) {
            perror("recv");
        }

        printf("Client disconnected\n");
        close(client_fd);
    }

    close(server_fd);
}

int main(int argc, char *argv[]) {
    printf("\n================================================\n");
    printf("  Module 03: TCP Echo Server\n");
    printf("================================================\n\n");

    int port = DEFAULT_PORT;
    if (argc > 1) {
        port = atoi(argv[1]);
    }

    if (argc > 2 && strcmp(argv[2], "--example") == 0) {
        /* Run the working example */
        simple_server_example(port);
    } else {
        /* Run the TODO version */
        run_server(port);
    }

    return 0;
}
