/*
 * server_mt.c - Multi-Threaded File Server (Part B)
 *
 * This server uses a thread pool to handle multiple clients concurrently.
 * It extends the single-threaded server with the boss-worker pattern.
 *
 * Usage: ./server_mt [port] [num_threads]
 *
 * Boss-Worker Pattern:
 * - Boss (main thread): accepts connections, adds to work queue
 * - Workers (thread pool): handle client requests concurrently
 *
 * This is similar to GIOS Project 1 Part 2!
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
#include "../include/thread_pool.h"

/* ============================================================================
 * Global State
 * ============================================================================ */

static volatile sig_atomic_t running = 1;
static int server_fd = -1;
static thread_pool_t *pool = NULL;

/* ============================================================================
 * Signal Handler
 * ============================================================================ */

static void signal_handler(int sig) {
    (void)sig;
    running = 0;
    if (server_fd >= 0) {
        close(server_fd);
        server_fd = -1;
    }
}

/* ============================================================================
 * Statistics Display
 * ============================================================================ */

/*
 * print_stats - Print thread pool statistics
 */
static void print_stats(void) {
    /*
     * TODO: Implement statistics display
     *
     * Call thread_pool_get_stats() and display the results.
     *
     * int tasks, active;
     * thread_pool_get_stats(pool, &tasks, &active);
     * printf("Tasks completed: %d, Active workers: %d\n", tasks, active);
     */

    if (pool == NULL) {
        return;
    }

    /* YOUR CODE HERE */
}

/* ============================================================================
 * Main Server Loop
 * ============================================================================ */

/*
 * run_server - Boss thread main loop
 */
static int run_server(int port, int num_threads) {
    /*
     * TODO: Implement the multi-threaded server
     *
     * Steps:
     *
     * 1. Create thread pool
     *
     *    pool = thread_pool_create(num_threads);
     *    if (pool == NULL) {
     *        fprintf(stderr, "Failed to create thread pool\n");
     *        return -1;
     *    }
     *    printf("Created thread pool with %d workers\n", num_threads);
     *
     * 2. Create server socket
     *
     *    server_fd = create_server_socket(port, BACKLOG);
     *    if (server_fd < 0) {
     *        thread_pool_destroy(pool);
     *        return -1;
     *    }
     *    printf("Server listening on port %d\n", port);
     *
     * 3. Boss loop - accept connections and submit to thread pool
     *
     *    while (running) {
     *        struct sockaddr_in client_addr;
     *        socklen_t addr_len = sizeof(client_addr);
     *
     *        int client_fd = accept_client(server_fd,
     *                                      (struct sockaddr *)&client_addr,
     *                                      &addr_len);
     *        if (client_fd < 0) {
     *            if (!running) break;
     *            perror("accept");
     *            continue;
     *        }
     *
     *        // Log connection
     *        char client_ip[INET_ADDRSTRLEN];
     *        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
     *        printf("Connection from %s:%d\n", client_ip, ntohs(client_addr.sin_port));
     *
     *        // Submit to thread pool (non-blocking!)
     *        if (thread_pool_submit(pool, client_fd) < 0) {
     *            fprintf(stderr, "Failed to submit to thread pool\n");
     *            close(client_fd);
     *        }
     *    }
     *
     * 4. Cleanup
     *
     *    print_stats();  // Final statistics
     *    if (server_fd >= 0) {
     *        close(server_fd);
     *    }
     *    thread_pool_destroy(pool);
     *    return 0;
     *
     * KEY DIFFERENCE from single-threaded:
     * - Boss does NOT call handle_client() directly
     * - Boss just submits client_fd to the thread pool
     * - Workers (in thread_pool.c) handle the actual requests
     */

    printf("Starting multi-threaded server\n");
    printf("Port: %d, Threads: %d\n", port, num_threads);

    /* YOUR CODE HERE */

    (void)port;
    (void)num_threads;

    return -1;  /* Placeholder */
}

/* ============================================================================
 * Main Entry Point
 * ============================================================================ */

int main(int argc, char *argv[]) {
    int port = DEFAULT_PORT;
    int num_threads = DEFAULT_NUM_THREADS;

    /* Parse command line arguments */
    if (argc > 1) {
        port = atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            fprintf(stderr, "Invalid port number: %s\n", argv[1]);
            fprintf(stderr, "Usage: %s [port] [num_threads]\n", argv[0]);
            return 1;
        }
    }
    if (argc > 2) {
        num_threads = atoi(argv[2]);
        if (num_threads <= 0 || num_threads > 100) {
            fprintf(stderr, "Invalid thread count: %s (must be 1-100)\n", argv[2]);
            return 1;
        }
    }

    /* Set up signal handlers */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGPIPE, SIG_IGN);

    printf("=== Mini-GIOS Multi-Threaded Server ===\n");
    printf("Serving files from: %s\n", FILE_ROOT);
    printf("Thread pool size: %d\n", num_threads);
    printf("Press Ctrl+C to stop\n\n");

    /* Run the server */
    int result = run_server(port, num_threads);

    printf("\nServer stopped.\n");
    return result;
}
