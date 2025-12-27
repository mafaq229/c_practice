/*
 * cache_process.c - Separate Cache Process (Part D)
 *
 * A standalone process that manages the file cache and communicates
 * with the proxy via IPC (message queues and shared memory).
 *
 * This demonstrates process separation for:
 * - Fault isolation (cache crash doesn't kill proxy)
 * - Independent scaling
 * - Memory isolation
 *
 * Usage: ./cache_process
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

#include "../include/cache.h"
#include "../include/ipc_protocol.h"
#include "../include/shm_manager.h"

/* Check for Linux */
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

#define CACHE_SIZE          (10 * 1024 * 1024)  /* 10 MB cache */

static volatile sig_atomic_t running = 1;

/* IPC resources */
static shm_handle_t *shm = NULL;
static mqd_t mq_from_proxy = -1;
static mqd_t mq_to_proxy = -1;
static cache_t *cache = NULL;

/* ============================================================================
 * Signal Handler
 * ============================================================================ */

static void signal_handler(int sig) {
    (void)sig;
    running = 0;
}

/* ============================================================================
 * IPC Setup
 * ============================================================================ */

static int setup_ipc(void) {
    /*
     * TODO: Initialize IPC resources (as creator)
     *
     * Steps:
     * 1. Create shared memory:
     *    shm = shm_create(SHM_CACHE_NAME, MAX_SHM_SIZE);
     *
     * 2. Create/open message queues:
     *    struct mq_attr attr = {0};
     *    attr.mq_maxmsg = 10;
     *    attr.mq_msgsize = MAX_MQ_MSG_SIZE;
     *
     *    mq_from_proxy = mq_open(MQ_PROXY_TO_CACHE,
     *                           O_RDONLY | O_CREAT | O_EXCL, 0666, &attr);
     *    mq_to_proxy = mq_open(MQ_CACHE_TO_PROXY,
     *                          O_WRONLY | O_CREAT | O_EXCL, 0666, &attr);
     *
     * 3. Create in-memory cache:
     *    cache = cache_create(CACHE_SIZE);
     *
     * Return 0 on success, -1 on error
     */

#if HAS_MQ
    printf("Setting up IPC resources (as creator)...\n");

    /* YOUR CODE HERE */

    return -1;  /* Placeholder */
#else
    fprintf(stderr, "Error: Requires Linux. Use Docker.\n");
    return -1;
#endif
}

static void cleanup_ipc(void) {
#if HAS_MQ
    printf("Cleaning up IPC resources...\n");

    /* Close and unlink message queues (we are the creator) */
    if (mq_from_proxy != (mqd_t)-1) {
        mq_close(mq_from_proxy);
        mq_unlink(MQ_PROXY_TO_CACHE);
    }
    if (mq_to_proxy != (mqd_t)-1) {
        mq_close(mq_to_proxy);
        mq_unlink(MQ_CACHE_TO_PROXY);
    }

    /* Destroy shared memory (we are the creator) */
    if (shm != NULL) {
        shm_destroy(shm);
    }

    /* Destroy cache */
    if (cache != NULL) {
        cache_destroy(cache);
    }
#endif
}

/* ============================================================================
 * Request Handlers
 * ============================================================================ */

static void handle_lookup(const ipc_lookup_request_t *request) {
    /*
     * TODO: Handle cache lookup request
     *
     * Steps:
     * 1. Look up in cache:
     *    char *data;
     *    size_t size;
     *    if (cache_get_copy(cache, request->path, &data, &size)) {
     *        // Hit - copy data to shared memory
     *        size_t offset;
     *        void *shm_data = shm_alloc(shm, size, &offset);
     *        if (shm_data) {
     *            memcpy(shm_data, data, size);
     *            // Send hit response
     *            ipc_lookup_hit_t response;
     *            ipc_create_lookup_hit(&response, request->header.request_id,
     *                                   size, offset);
     *            mq_send(mq_to_proxy, (char *)&response, sizeof(response), 0);
     *        }
     *        free(data);
     *    } else {
     *        // Miss - send miss response
     *        ipc_generic_response_t response;
     *        ipc_create_generic_response(&response, MSG_LOOKUP_MISS,
     *                                     request->header.request_id, 0, "Cache miss");
     *        mq_send(mq_to_proxy, (char *)&response, sizeof(response), 0);
     *    }
     */

    printf("Lookup request for: %s\n", request->path);

#if HAS_MQ
    /* YOUR CODE HERE */

    (void)request;
#else
    (void)request;
#endif
}

static void handle_store(const ipc_store_request_t *request) {
    /*
     * TODO: Handle cache store request
     *
     * Steps:
     * 1. Get data from shared memory:
     *    void *shm_data = shm_get_ptr(shm, request->shm_offset);
     *
     * 2. Store in cache:
     *    cache_put(cache, request->path, shm_data, request->file_size);
     *
     * 3. Free shared memory (proxy allocated it):
     *    shm_free(shm, request->shm_offset, request->file_size);
     *
     * 4. Send acknowledgment:
     *    ipc_generic_response_t response;
     *    ipc_create_generic_response(&response, MSG_STORE_OK,
     *                                 request->header.request_id, 0, "Stored");
     *    mq_send(mq_to_proxy, (char *)&response, sizeof(response), 0);
     */

    printf("Store request for: %s (%zu bytes)\n", request->path, request->file_size);

#if HAS_MQ
    /* YOUR CODE HERE */

    (void)request;
#else
    (void)request;
#endif
}

static void handle_stats(uint32_t request_id) {
    /*
     * TODO: Handle stats request
     *
     * Get cache statistics and send response.
     */

    printf("Stats request\n");

#if HAS_MQ
    /* YOUR CODE HERE */

    (void)request_id;
#else
    (void)request_id;
#endif
}

/* ============================================================================
 * Main Loop
 * ============================================================================ */

static int run_cache_process(void) {
    /*
     * TODO: Implement the cache process main loop
     *
     * Steps:
     * 1. Set up IPC resources
     *
     * 2. Main loop:
     *    while (running) {
     *        // Receive message from proxy
     *        char msg_buf[MAX_MQ_MSG_SIZE];
     *        ssize_t n = mq_receive(mq_from_proxy, msg_buf, sizeof(msg_buf), NULL);
     *        if (n < 0) {
     *            if (errno == EINTR) continue;
     *            break;
     *        }
     *
     *        // Dispatch based on message type
     *        ipc_msg_header_t *header = (ipc_msg_header_t *)msg_buf;
     *        switch (header->type) {
     *            case MSG_LOOKUP_REQUEST:
     *                handle_lookup((ipc_lookup_request_t *)msg_buf);
     *                break;
     *            case MSG_STORE_REQUEST:
     *                handle_store((ipc_store_request_t *)msg_buf);
     *                break;
     *            case MSG_STATS_REQUEST:
     *                handle_stats(header->request_id);
     *                break;
     *            case MSG_SHUTDOWN_REQUEST:
     *                running = 0;
     *                break;
     *            default:
     *                fprintf(stderr, "Unknown message type: %d\n", header->type);
     *        }
     *    }
     *
     * 3. Cleanup
     */

    printf("Starting cache process...\n");

    if (setup_ipc() < 0) {
        fprintf(stderr, "Failed to set up IPC\n");
        return -1;
    }

    printf("Cache process ready, waiting for requests...\n");

#if HAS_MQ
    /* YOUR CODE HERE */
#endif

    cleanup_ipc();
    return 0;
}

/* ============================================================================
 * Main
 * ============================================================================ */

int main(void) {
#if !HAS_MQ
    fprintf(stderr, "=== WARNING ===\n");
    fprintf(stderr, "This program requires Linux for POSIX message queues.\n");
    fprintf(stderr, "On macOS, use Docker:\n");
    fprintf(stderr, "  docker-compose run dev\n");
    fprintf(stderr, "  make part_d\n");
    fprintf(stderr, "===============\n\n");
    return 1;
#endif

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    printf("=== Mini-GIOS Cache Process ===\n");
    printf("Cache size: %d MB\n", CACHE_SIZE / (1024 * 1024));
    printf("Press Ctrl+C to stop\n\n");

    int result = run_cache_process();

    printf("\nCache process stopped.\n");
    return result;
}
