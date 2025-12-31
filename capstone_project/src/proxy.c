/*
proxy.c - Caching Proxy Server (Part C)

A proxy server that sits between clients and the file server.
It caches file responses to reduce load on the backend server.

Architecture:
  Client <-> Proxy <-> File Server
             |
          [Cache]

Usage: ./proxy [proxy_port] [server_host] [server_port]

The proxy:
1. Receives requests from clients
2. Checks cache for requested file
3. If cache hit: return cached content
4. If cache miss: forward request to server, cache response, return to client
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
#include "../include/cache.h"

/* ============================================================================
Configuration
============================================================================ */

#define CACHE_SIZE          (10 * 1024 * 1024)  /* 10 MB cache */

static volatile sig_atomic_t running = 1;
static int proxy_fd = -1;
static cache_t *cache = NULL;

/* Backend server configuration */
static char server_host[256] = "localhost";
static int server_port = DEFAULT_PORT;

/* ============================================================================
Signal Handler
============================================================================ */

static void signal_handler(int sig) {
    (void)sig;
    running = 0;
    if (proxy_fd >= 0) {
        close(proxy_fd);
        proxy_fd = -1;
    }
}

/* ============================================================================
Backend Server Communication
============================================================================ */

/*
fetch_from_server - Fetch a file from the backend server

Returns: 0 on success (data and size filled), -1 on error
On success, caller must free *data!
*/
static int fetch_from_server(const char *path, char **data, size_t *size) {
    /*
     * TODO: Implement fetching from backend server
     *
     * Steps:
     * 1. Connect to backend server
     *    int fd = create_client_socket(server_host, server_port);
     *
     * 2. Send GETFILE request
     *    char request[MAX_REQUEST_LEN];
     *    int len = gf_create_request(request, sizeof(request), path);
     *    send_all(fd, request, len);
     *
     * 3. Receive response header
     *    char header_buf[MAX_HEADER_LEN];
     *    recv_until(fd, header_buf, sizeof(header_buf), HEADER_DELIM);
     *
     * 4. Parse response
     *    gf_response_t response;
     *    gf_parse_response_header(header_buf, n, &response);
     *
     * 5. If not OK, return -1
     *
     * 6. Receive file content
     *    *data = malloc(response.content_length);
     *    recv_all(fd, *data, response.content_length);
     *    *size = response.content_length;
     *
     * 7. Close connection, return 0
     */

    printf("Fetching %s from backend server %s:%d\n", path, server_host, server_port);

    /* YOUR CODE HERE */

    (void)path;
    (void)data;
    (void)size;

    return -1;  /* Placeholder */
}

/* ============================================================================
Request Handling
============================================================================ */

/*
send_cached_response - Send cached file to client
*/
static int send_cached_response(int client_fd, const char *data, size_t size) {
    /*
     * TODO: Send a cached response
     *
     * Steps:
     * 1. Create response header with STATUS_CACHED
     *    gf_create_response_header(header, sizeof(header), STATUS_CACHED, size);
     * 2. Send header
     *    send_all(client_fd, header, header_len);
     * 3. Send data
     *    send_all(client_fd, data, size);
     */

    (void)client_fd;
    (void)data;
    (void)size;

    /* YOUR CODE HERE */

    return -1;  /* Placeholder */
}

/*
handle_proxy_request - Handle a single proxy request
*/
static void handle_proxy_request(int client_fd) {
    /*
     * TODO: Implement proxy request handling
     *
     * This is the core proxy logic!
     *
     * Steps:
     * 1. Receive and parse client request
     *    char buffer[MAX_HEADER_LEN];
     *    ssize_t n = recv_until(client_fd, buffer, sizeof(buffer), HEADER_DELIM);
     *    gf_request_t request;
     *    gf_parse_request(buffer, n, &request);
     *
     * 2. Validate request
     *    if (!request.valid || !validate_path(request.path)) {
     *        // Send error response
     *        return;
     *    }
     *
     * 3. Check cache
     *    char *data;
     *    size_t size;
     *    if (cache_get_copy(cache, request.path, &data, &size)) {
     *        // CACHE HIT!
     *        printf("Cache HIT for %s\n", request.path);
     *        send_cached_response(client_fd, data, size);
     *        free(data);
     *        return;
     *    }
     *
     * 4. Cache miss - fetch from backend
     *    printf("Cache MISS for %s\n", request.path);
     *    if (fetch_from_server(request.path, &data, &size) < 0) {
     *        // Send error response to client
     *        return;
     *    }
     *
     * 5. Cache the response
     *    cache_put(cache, request.path, data, size);
     *
     * 6. Send response to client (use STATUS_OK, not CACHED)
     *    char header[256];
     *    gf_create_response_header(header, sizeof(header), STATUS_OK, size);
     *    send_all(client_fd, header, header_len);
     *    send_all(client_fd, data, size);
     *
     * 7. Cleanup
     *    free(data);
     */

    printf("Handling proxy request\n");

    /* YOUR CODE HERE */

    (void)client_fd;
}

/* ============================================================================
Statistics Display
============================================================================ */

static void print_cache_stats(void) {
    if (cache == NULL) return;

    cache_stats_t stats;
    cache_get_stats(cache, &stats);

    printf("\n=== Cache Statistics ===\n");
    printf("Entries: %d\n", stats.num_entries);
    printf("Size: %zu / %zu bytes (%.1f%%)\n",
           stats.current_size, stats.max_size,
           100.0 * stats.current_size / stats.max_size);
    printf("Hits: %lu, Misses: %lu, Hit Rate: %.1f%%\n",
           stats.hits, stats.misses, stats.hit_rate * 100);
    printf("Evictions: %lu\n", stats.evictions);
    printf("========================\n");
}

/* ============================================================================
Main Proxy Loop
============================================================================ */

static int run_proxy(int proxy_port) {
    /*
     * TODO: Implement the proxy main loop
     *
     * Steps:
     * 1. Create cache
     *    cache = cache_create(CACHE_SIZE);
     *
     * 2. Create proxy socket
     *    proxy_fd = create_server_socket(proxy_port, BACKLOG);
     *
     * 3. Accept and handle clients
     *    while (running) {
     *        int client_fd = accept_client(proxy_fd, ...);
     *        handle_proxy_request(client_fd);
     *        close_socket(client_fd);
     *    }
     *
     * 4. Cleanup
     *    print_cache_stats();
     *    cache_destroy(cache);
     *
     * Note: This is single-threaded. For better performance,
     * you could add a thread pool (like server_mt).
     */

    printf("Starting proxy on port %d\n", proxy_port);
    printf("Backend server: %s:%d\n", server_host, server_port);

    /* YOUR CODE HERE */

    (void)proxy_port;

    return -1;  /* Placeholder */
}

/* ============================================================================
Main Entry Point
============================================================================ */

static void print_usage(const char *prog) {
    fprintf(stderr, "Usage: %s [proxy_port] [server_host] [server_port]\n", prog);
    fprintf(stderr, "\nDefaults:\n");
    fprintf(stderr, "  proxy_port:  %d\n", PROXY_PORT);
    fprintf(stderr, "  server_host: localhost\n");
    fprintf(stderr, "  server_port: %d\n", DEFAULT_PORT);
}

int main(int argc, char *argv[]) {
    int proxy_port = PROXY_PORT;

    /* Parse arguments */
    if (argc > 1) {
        proxy_port = atoi(argv[1]);
        if (proxy_port <= 0 || proxy_port > 65535) {
            fprintf(stderr, "Invalid proxy port: %s\n", argv[1]);
            print_usage(argv[0]);
            return 1;
        }
    }
    if (argc > 2) {
        strncpy(server_host, argv[2], sizeof(server_host) - 1);
        server_host[sizeof(server_host) - 1] = '\0';
    }
    if (argc > 3) {
        server_port = atoi(argv[3]);
        if (server_port <= 0 || server_port > 65535) {
            fprintf(stderr, "Invalid server port: %s\n", argv[3]);
            return 1;
        }
    }

    /* Set up signal handlers */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGPIPE, SIG_IGN);

    printf("=== Mini-GIOS Caching Proxy ===\n");
    printf("Cache size: %d MB\n", CACHE_SIZE / (1024 * 1024));
    printf("Press Ctrl+C to stop\n\n");

    int result = run_proxy(proxy_port);

    printf("\nProxy stopped.\n");
    return result;
}
