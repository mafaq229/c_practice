/*
 * ipc_protocol.h - Inter-Process Communication Protocol
 *
 * This header defines the message format used for communication
 * between the proxy and the cache process in Part D.
 *
 * Architecture:
 * +--------+           +---------------+
 * | Proxy  | <--IPC--> | Cache Process |
 * +--------+           +---------------+
 *     |                       |
 *     | Message Queue         | Shared Memory
 *     | (requests/responses)  | (file data)
 *
 * The proxy sends lookup requests via message queue.
 * The cache process responds via message queue.
 * Large file data is transferred via shared memory.
 */

#ifndef IPC_PROTOCOL_H
#define IPC_PROTOCOL_H

#include <stddef.h>
#include <stdint.h>

/* ============================================================================
 * Constants
 * ============================================================================ */

/* Message queue names (POSIX) */
#define MQ_PROXY_TO_CACHE       "/gios_proxy_to_cache"
#define MQ_CACHE_TO_PROXY       "/gios_cache_to_proxy"

/* Shared memory segment name */
#define SHM_CACHE_NAME          "/gios_cache_shm"

/* Maximum sizes */
#define MAX_MQ_MSG_SIZE         8192
#define MAX_SHM_SIZE            (16 * 1024 * 1024)  /* 16 MB */

/* Timeout for message queue operations (seconds) */
#define MQ_TIMEOUT_SEC          5

/* ============================================================================
 * Message Types
 * ============================================================================ */

typedef enum {
    /* Requests (proxy -> cache) */
    MSG_LOOKUP_REQUEST = 1,     /* Look up file in cache */
    MSG_STORE_REQUEST,          /* Store file in cache */
    MSG_INVALIDATE_REQUEST,     /* Remove file from cache */
    MSG_STATS_REQUEST,          /* Get cache statistics */
    MSG_SHUTDOWN_REQUEST,       /* Shut down cache process */

    /* Responses (cache -> proxy) */
    MSG_LOOKUP_HIT = 100,       /* File found in cache */
    MSG_LOOKUP_MISS,            /* File not in cache */
    MSG_STORE_OK,               /* File stored successfully */
    MSG_STORE_FAIL,             /* Failed to store file */
    MSG_STATS_RESPONSE,         /* Statistics data */
    MSG_ACK,                    /* Generic acknowledgment */
    MSG_ERROR                   /* Error response */
} ipc_msg_type_t;

/* ============================================================================
 * Message Structures
 * ============================================================================ */

/*
 * Message header - common to all messages
 */
typedef struct {
    ipc_msg_type_t type;        /* Message type */
    uint32_t request_id;        /* ID to match requests with responses */
    uint32_t payload_size;      /* Size of payload following header */
} ipc_msg_header_t;

/*
 * Lookup request - ask cache if file is cached
 */
typedef struct {
    ipc_msg_header_t header;
    char path[512];             /* File path to look up */
} ipc_lookup_request_t;

/*
 * Lookup hit response - file found in cache
 *
 * The actual file data is in shared memory at shm_offset.
 */
typedef struct {
    ipc_msg_header_t header;
    size_t file_size;           /* Size of file data */
    size_t shm_offset;          /* Offset in shared memory segment */
} ipc_lookup_hit_t;

/*
 * Store request - add file to cache
 *
 * File data should be written to shared memory first,
 * then this message sent with the offset.
 */
typedef struct {
    ipc_msg_header_t header;
    char path[512];             /* File path (cache key) */
    size_t file_size;           /* Size of file data */
    size_t shm_offset;          /* Offset in shared memory */
} ipc_store_request_t;

/*
 * Invalidate request - remove file from cache
 */
typedef struct {
    ipc_msg_header_t header;
    char path[512];             /* File path to invalidate */
} ipc_invalidate_request_t;

/*
 * Stats response - cache statistics
 */
typedef struct {
    ipc_msg_header_t header;
    uint64_t hits;
    uint64_t misses;
    uint64_t evictions;
    size_t current_size;
    size_t max_size;
    uint32_t num_entries;
} ipc_stats_response_t;

/*
 * Generic response - for ACK, ERROR, LOOKUP_MISS
 */
typedef struct {
    ipc_msg_header_t header;
    int error_code;             /* 0 on success, errno on error */
    char message[256];          /* Human-readable message */
} ipc_generic_response_t;

/* ============================================================================
 * Helper Functions
 * ============================================================================ */

/*
 * ipc_msg_type_to_string - Convert message type to string
 *
 * @param type: Message type
 * @return: String representation
 */
const char *ipc_msg_type_to_string(ipc_msg_type_t type);

/*
 * ipc_create_lookup_request - Create a lookup request message
 *
 * @param msg: Output message buffer
 * @param request_id: Request ID for matching response
 * @param path: File path to look up
 * @return: 0 on success, -1 on error
 */
int ipc_create_lookup_request(ipc_lookup_request_t *msg, uint32_t request_id,
                               const char *path);

/*
 * ipc_create_lookup_hit - Create a lookup hit response
 *
 * @param msg: Output message buffer
 * @param request_id: Request ID from original request
 * @param file_size: Size of file data
 * @param shm_offset: Offset in shared memory
 * @return: 0 on success, -1 on error
 */
int ipc_create_lookup_hit(ipc_lookup_hit_t *msg, uint32_t request_id,
                           size_t file_size, size_t shm_offset);

/*
 * ipc_create_store_request - Create a store request message
 *
 * @param msg: Output message buffer
 * @param request_id: Request ID
 * @param path: File path (cache key)
 * @param file_size: Size of file data
 * @param shm_offset: Offset in shared memory where data is stored
 * @return: 0 on success, -1 on error
 */
int ipc_create_store_request(ipc_store_request_t *msg, uint32_t request_id,
                              const char *path, size_t file_size, size_t shm_offset);

/*
 * ipc_create_generic_response - Create a generic response message
 *
 * @param msg: Output message buffer
 * @param type: Response type (MSG_ACK, MSG_ERROR, MSG_LOOKUP_MISS)
 * @param request_id: Request ID from original request
 * @param error_code: Error code (0 for success)
 * @param message: Human-readable message
 * @return: 0 on success, -1 on error
 */
int ipc_create_generic_response(ipc_generic_response_t *msg, ipc_msg_type_t type,
                                 uint32_t request_id, int error_code,
                                 const char *message);

#endif /* IPC_PROTOCOL_H */
