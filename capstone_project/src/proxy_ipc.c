/*
 * proxy_ipc.c - IPC-Based Caching Proxy (Part D)
 *
 * This proxy uses inter-process communication to talk to a separate
 * cache process instead of an in-process cache.
 *
 * Architecture:
 *   Client <-> Proxy <----- IPC -----> Cache Process
 *                     Message Queue    Shared Memory
 *
 * Communication:
 * - Message queues for request/response signaling
 * - Shared memory for transferring file data
 *
 * Usage: ./proxy_ipc [proxy_port] [server_host] [server_port]
 *
 * NOTE: This requires Linux! Use Docker on macOS.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../include/protocol.h"
#include "../include/socket_utils.h"
#include "../include/file_utils.h"
#include "../include/ipc_protocol.h"
#include "../include/shm_manager.h"

/* Check for Linux (POSIX message queues) */
#ifdef __linux__
#include <mqueue.h>
#define HAS_MQ 1
#else
#define HAS_MQ 0
typedef int mqd_t;
#endif

/* ============================================================================
 * Configuration
 * ============================================================================ */

static volatile sig_atomic_t running = 1;
static int proxy_fd = -1;

/* IPC resources */
static shm_handle_t *shm = NULL;
static mqd_t mq_to_cache = -1;
static mqd_t mq_from_cache = -1;

/* Backend server */
static char server_host[256] = "localhost";
static int server_port = DEFAULT_PORT;

/* Request ID counter */
static uint32_t next_request_id = 1;

/* ============================================================================
 * Signal Handler
 * ============================================================================ */

static void signal_handler(int sig) {
    (void)sig;
    running = 0;
    if (proxy_fd >= 0) {
        close(proxy_fd);
        proxy_fd = -1;
    }
}

/* ============================================================================
 * IPC Setup
 * ============================================================================ */

/*
 * setup_ipc - Initialize IPC resources
 */
static int setup_ipc(void) {
    /*
     * TODO: Initialize message queues and shared memory
     *
     * Steps for message queues:
     * 1. Set up mq_attr:
     *    struct mq_attr attr;
     *    attr.mq_flags = 0;
     *    attr.mq_maxmsg = 10;
     *    attr.mq_msgsize = MAX_MQ_MSG_SIZE;
     *    attr.mq_curmsgs = 0;
     *
     * 2. Open/create queues:
     *    mq_to_cache = mq_open(MQ_PROXY_TO_CACHE, O_WRONLY | O_CREAT, 0666, &attr);
     *    mq_from_cache = mq_open(MQ_CACHE_TO_PROXY, O_RDONLY | O_CREAT, 0666, &attr);
     *
     * Steps for shared memory:
     * 1. Attach to existing shared memory (cache process creates it):
     *    shm = shm_attach(SHM_CACHE_NAME);
     *
     * Return 0 on success, -1 on error
     */

#if HAS_MQ
    printf("Setting up IPC resources...\n");

    /* YOUR CODE HERE */

    return -1;  /* Placeholder */
#else
    fprintf(stderr, "Error: Message queues require Linux. Use Docker.\n");
    return -1;
#endif
}

/*
 * cleanup_ipc - Clean up IPC resources
 */
static void cleanup_ipc(void) {
#if HAS_MQ
    /* Close message queues (don't unlink - cache process may still use them) */
    if (mq_to_cache != (mqd_t)-1) {
        mq_close(mq_to_cache);
    }
    if (mq_from_cache != (mqd_t)-1) {
        mq_close(mq_from_cache);
    }

    /* Detach from shared memory (don't destroy - cache process owns it) */
    if (shm != NULL) {
        shm_detach(shm);
    }
#endif
}

/* ============================================================================
 * Cache Communication
 * ============================================================================ */

/*
 * cache_lookup - Ask cache process if file is cached
 *
 * Returns: 1 if hit (data and size filled), 0 if miss, -1 on error
 */
static int cache_lookup(const char *path, char **data, size_t *size) {
    /*
     * TODO: Implement cache lookup via IPC
     *
     * Steps:
     * 1. Create lookup request message:
     *    ipc_lookup_request_t request;
     *    ipc_create_lookup_request(&request, next_request_id++, path);
     *
     * 2. Send to cache process:
     *    mq_send(mq_to_cache, (char *)&request, sizeof(request), 0);
     *
     * 3. Wait for response:
     *    char response_buf[MAX_MQ_MSG_SIZE];
     *    mq_receive(mq_from_cache, response_buf, sizeof(response_buf), NULL);
     *
     * 4. Parse response:
     *    ipc_msg_header_t *header = (ipc_msg_header_t *)response_buf;
     *    switch (header->type) {
     *        case MSG_LOOKUP_HIT: {
     *            ipc_lookup_hit_t *hit = (ipc_lookup_hit_t *)response_buf;
     *            // Get data from shared memory
     *            void *shm_data = shm_get_ptr(shm, hit->shm_offset);
     *            *data = malloc(hit->file_size);
     *            memcpy(*data, shm_data, hit->file_size);
     *            *size = hit->file_size;
     *            // Free shared memory
     *            shm_free(shm, hit->shm_offset, hit->file_size);
     *            return 1;  // Hit
     *        }
     *        case MSG_LOOKUP_MISS:
     *            return 0;  // Miss
     *        default:
     *            return -1;  // Error
     *    }
     */

#if HAS_MQ
    printf("Cache lookup for: %s\n", path);

    /* YOUR CODE HERE */

    (void)path;
    (void)data;
    (void)size;
#else
    (void)path;
    (void)data;
    (void)size;
#endif

    return -1;  /* Placeholder */
}

/*
 * cache_store - Tell cache process to store a file
 */
static int cache_store(const char *path, const char *data, size_t size) {
    /*
     * TODO: Implement cache storage via IPC
     *
     * Steps:
     * 1. Allocate space in shared memory:
     *    size_t offset;
     *    void *shm_data = shm_alloc(shm, size, &offset);
     *    if (shm_data == NULL) return -1;  // No space
     *
     * 2. Copy data to shared memory:
     *    memcpy(shm_data, data, size);
     *
     * 3. Create store request:
     *    ipc_store_request_t request;
     *    ipc_create_store_request(&request, next_request_id++, path, size, offset);
     *
     * 4. Send to cache process:
     *    mq_send(mq_to_cache, (char *)&request, sizeof(request), 0);
     *
     * 5. Wait for acknowledgment
     *
     * Return 0 on success, -1 on error
     */

#if HAS_MQ
    printf("Cache store for: %s (%zu bytes)\n", path, size);

    /* YOUR CODE HERE */

    (void)path;
    (void)data;
    (void)size;
#else
    (void)path;
    (void)data;
    (void)size;
#endif

    return -1;  /* Placeholder */
}

/* ============================================================================
 * Backend Communication
 * ============================================================================ */

static int fetch_from_server(const char *path, char **data, size_t *size) {
    /* Same as proxy.c - forward request to backend server */
    printf("Fetching %s from backend %s:%d\n", path, server_host, server_port);

    /* YOUR CODE HERE - copy from proxy.c */

    (void)path;
    (void)data;
    (void)size;

    return -1;  /* Placeholder */
}

/* ============================================================================
 * Request Handling
 * ============================================================================ */

static void handle_proxy_request(int client_fd) {
    /*
     * TODO: Implement IPC proxy request handling
     *
     * Similar to proxy.c, but use cache_lookup/cache_store
     * for IPC-based caching instead of direct cache access.
     */

    printf("Handling IPC proxy request\n");

    /* YOUR CODE HERE */

    (void)client_fd;
}

/* ============================================================================
 * Main
 * ============================================================================ */

static int run_proxy(int proxy_port) {
    /*
     * TODO: Implement the IPC proxy main loop
     *
     * Steps:
     * 1. Set up IPC resources
     * 2. Create proxy socket
     * 3. Accept and handle clients
     * 4. Clean up
     */

    printf("Starting IPC proxy on port %d\n", proxy_port);

    /* YOUR CODE HERE */

    (void)proxy_port;

    return -1;  /* Placeholder */
}

int main(int argc, char *argv[]) {
    int proxy_port = PROXY_PORT;

#if !HAS_MQ
    fprintf(stderr, "=== WARNING ===\n");
    fprintf(stderr, "This program requires Linux for POSIX message queues.\n");
    fprintf(stderr, "On macOS, use Docker:\n");
    fprintf(stderr, "  docker-compose run dev\n");
    fprintf(stderr, "  make part_d\n");
    fprintf(stderr, "===============\n\n");
#endif

    if (argc > 1) proxy_port = atoi(argv[1]);
    if (argc > 2) strncpy(server_host, argv[2], sizeof(server_host) - 1);
    if (argc > 3) server_port = atoi(argv[3]);

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGPIPE, SIG_IGN);

    printf("=== Mini-GIOS IPC Proxy ===\n");

    int result = run_proxy(proxy_port);

    cleanup_ipc();
    printf("\nIPC Proxy stopped.\n");
    return result;
}
