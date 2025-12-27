/*
 * server_single.c - Single-Threaded File Server (Part A)
 *
 * This is the simplest version of the server - handles one client at a time.
 * It implements the GETFILE protocol to serve files to clients.
 *
 * Usage: ./server_single [port]
 *
 * This is your starting point! Once this works, you can add multithreading.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../include/protocol.h"
#include "../include/socket_utils.h"
#include "../include/file_utils.h"

/* ============================================================================
 * Global State
 * ============================================================================ */

static volatile sig_atomic_t running = 1;
static int server_fd = -1;

/* ============================================================================
 * Signal Handler
 * ============================================================================ */

/*
 * Handle Ctrl+C for graceful shutdown
 */
static void signal_handler(int sig) {
    (void)sig;
    running = 0;
    if (server_fd >= 0) {
        /* Close server socket to unblock accept() */
        close(server_fd);
        server_fd = -1;
    }
}

/* ============================================================================
 * Request Handling
 * ============================================================================ */

/*
 * send_error_response - Send an error response to the client
 */
static int send_error_response(int client_fd, gf_status_t status) {
    /*
     * TODO: Implement this function
     *
     * Steps:
     * 1. Create response header using gf_create_response_header()
     * 2. Send it using send_all()
     * 3. Return 0 on success, -1 on error
     *
     * Example:
     *   char header[256];
     *   int len = gf_create_response_header(header, sizeof(header), status, 0);
     *   if (len < 0) return -1;
     *   if (send_all(client_fd, header, len) != len) return -1;
     *   return 0;
     */

    (void)client_fd;
    (void)status;

    /* YOUR CODE HERE */

    return -1;  /* Placeholder */
}

/*
 * handle_client - Handle a single client connection
 */
static void handle_client(int client_fd) {
    /*
     * TODO: Implement this function
     *
     * This is the main request handling logic!
     *
     * Steps:
     * 1. Receive request header (use recv_until to get header ending with \r\n\r\n)
     *
     *    char buffer[MAX_HEADER_LEN];
     *    ssize_t n = recv_until(client_fd, buffer, sizeof(buffer), HEADER_DELIM);
     *    if (n <= 0) {
     *        // Connection closed or error
     *        return;
     *    }
     *
     * 2. Parse request
     *
     *    gf_request_t request;
     *    if (gf_parse_request(buffer, n, &request) <= 0 || !request.valid) {
     *        send_error_response(client_fd, STATUS_INVALID);
     *        return;
     *    }
     *
     * 3. Validate path (security!)
     *
     *    if (!validate_path(request.path)) {
     *        fprintf(stderr, "Invalid path: %s\n", request.path);
     *        send_error_response(client_fd, STATUS_INVALID);
     *        return;
     *    }
     *
     * 4. Build full file path
     *
     *    char filepath[MAX_PATH_LEN];
     *    if (build_full_path(filepath, sizeof(filepath), FILE_ROOT, request.path) < 0) {
     *        send_error_response(client_fd, STATUS_ERROR);
     *        return;
     *    }
     *
     * 5. Check if file exists
     *
     *    if (!file_exists(filepath) || !is_regular_file(filepath)) {
     *        send_error_response(client_fd, STATUS_FILE_NOT_FOUND);
     *        return;
     *    }
     *
     * 6. Get file size
     *
     *    ssize_t file_size = get_file_size(filepath);
     *    if (file_size < 0) {
     *        send_error_response(client_fd, STATUS_ERROR);
     *        return;
     *    }
     *
     * 7. Send success header
     *
     *    char header[256];
     *    int header_len = gf_create_response_header(header, sizeof(header),
     *                                                STATUS_OK, file_size);
     *    if (send_all(client_fd, header, header_len) != header_len) {
     *        return;  // Send error
     *    }
     *
     * 8. Send file content
     *
     *    size_t bytes_sent;
     *    if (send_file(client_fd, filepath, &bytes_sent) < 0) {
     *        fprintf(stderr, "Error sending file\n");
     *    } else {
     *        printf("Sent %zu bytes for %s\n", bytes_sent, request.path);
     *    }
     */

    printf("Handling client connection\n");

    /* YOUR CODE HERE */

    (void)client_fd;
}

/* ============================================================================
 * Main Server Loop
 * ============================================================================ */

/*
 * run_server - Main server loop
 */
static int run_server(int port) {
    /*
     * TODO: Implement the server main loop
     *
     * Steps:
     * 1. Create server socket
     *
     *    server_fd = create_server_socket(port, BACKLOG);
     *    if (server_fd < 0) {
     *        return -1;
     *    }
     *    printf("Server listening on port %d\n", port);
     *
     * 2. Accept and handle clients in a loop
     *
     *    while (running) {
     *        struct sockaddr_in client_addr;
     *        socklen_t addr_len = sizeof(client_addr);
     *
     *        int client_fd = accept_client(server_fd,
     *                                      (struct sockaddr *)&client_addr,
     *                                      &addr_len);
     *        if (client_fd < 0) {
     *            if (!running) break;  // Server shutting down
     *            perror("accept");
     *            continue;
     *        }
     *
     *        // Log connection
     *        char client_ip[INET_ADDRSTRLEN];
     *        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
     *        printf("Connection from %s:%d\n", client_ip, ntohs(client_addr.sin_port));
     *
     *        // Handle client (blocking - only one at a time!)
     *        handle_client(client_fd);
     *
     *        // Close client connection
     *        close_socket(client_fd);
     *    }
     *
     * 3. Cleanup
     *
     *    if (server_fd >= 0) {
     *        close(server_fd);
     *    }
     *    return 0;
     */

    printf("Starting single-threaded server on port %d\n", port);

    /* YOUR CODE HERE */

    (void)port;

    return -1;  /* Placeholder */
}

/* ============================================================================
 * Main Entry Point
 * ============================================================================ */

int main(int argc, char *argv[]) {
    int port = DEFAULT_PORT;

    /* Parse command line arguments */
    if (argc > 1) {
        port = atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            fprintf(stderr, "Invalid port number: %s\n", argv[1]);
            return 1;
        }
    }

    /* Set up signal handler for graceful shutdown */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    /* Ignore SIGPIPE (occurs when client disconnects during send) */
    signal(SIGPIPE, SIG_IGN);

    printf("=== Mini-GIOS Single-Threaded Server ===\n");
    printf("Serving files from: %s\n", FILE_ROOT);
    printf("Press Ctrl+C to stop\n\n");

    /* Run the server */
    int result = run_server(port);

    printf("\nServer stopped.\n");
    return result;
}
