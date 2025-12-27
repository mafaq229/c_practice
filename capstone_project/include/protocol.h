/*
 * protocol.h - Mini-GIOS Protocol Definitions
 *
 * This header defines the GETFILE protocol used throughout the capstone project.
 * The protocol is similar to what you'll encounter in GIOS Project 1.
 *
 * Protocol Format:
 * ================
 *
 * Client Request:
 *   GETFILE GET <path>\r\n\r\n
 *
 * Server Response (Success):
 *   GETFILE OK <length>\r\n\r\n<file_content>
 *
 * Server Response (Error):
 *   GETFILE FILE_NOT_FOUND\r\n\r\n
 *   GETFILE ERROR\r\n\r\n
 *   GETFILE INVALID\r\n\r\n
 *
 * Proxy Extensions (Part C):
 *   GETFILE CACHED <length>\r\n\r\n<file_content>  (served from cache)
 *
 * IPC Protocol (Part D):
 *   Uses separate message format for inter-process communication
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stddef.h>
#include <stdint.h>

/* ============================================================================
 * Constants
 * ============================================================================ */

#define PROTOCOL_NAME       "GETFILE"
#define PROTOCOL_VERSION    "1.0"

/* Maximum sizes */
#define MAX_PATH_LEN        4096
#define MAX_HEADER_LEN      8192
#define MAX_REQUEST_LEN     (MAX_PATH_LEN + 64)
#define BUFFER_SIZE         8192

/* Default network settings */
#define DEFAULT_PORT        8080
#define PROXY_PORT          8888
#define BACKLOG             128

/* Protocol delimiters */
#define HEADER_DELIM        "\r\n\r\n"
#define HEADER_DELIM_LEN    4

/* ============================================================================
 * Status Codes
 * ============================================================================ */

typedef enum {
    STATUS_OK = 0,
    STATUS_FILE_NOT_FOUND,
    STATUS_ERROR,
    STATUS_INVALID,
    STATUS_CACHED          /* Extension for proxy */
} gf_status_t;

/* String representations of status codes */
static const char *STATUS_STRINGS[] = {
    "OK",
    "FILE_NOT_FOUND",
    "ERROR",
    "INVALID",
    "CACHED"
};

/* ============================================================================
 * Request Structure
 * ============================================================================ */

/*
 * Parsed client request
 */
typedef struct {
    char path[MAX_PATH_LEN];    /* Requested file path */
    size_t path_len;            /* Length of path string */
    int valid;                  /* 1 if request is valid, 0 otherwise */
} gf_request_t;

/* ============================================================================
 * Response Structure
 * ============================================================================ */

/*
 * Server response header (before file content)
 */
typedef struct {
    gf_status_t status;         /* Response status */
    size_t content_length;      /* File size (only valid if status == OK/CACHED) */
    int header_complete;        /* 1 if full header received */
} gf_response_t;

/* ============================================================================
 * Function Prototypes
 * ============================================================================ */

/*
 * TODO: Implement these functions in src/protocol.c
 */

/* ---- Request Functions ---- */

/*
 * gf_create_request - Build a GETFILE request string
 *
 * @param buffer: Output buffer for the request
 * @param buflen: Size of the buffer
 * @param path: File path to request
 * @return: Number of bytes written, or -1 on error
 *
 * Example output: "GETFILE GET /path/to/file\r\n\r\n"
 */
int gf_create_request(char *buffer, size_t buflen, const char *path);

/*
 * gf_parse_request - Parse a GETFILE request
 *
 * @param buffer: Input buffer containing the request
 * @param buflen: Number of bytes in buffer
 * @param request: Output structure for parsed request
 * @return: Number of bytes consumed, 0 if incomplete, -1 on error
 *
 * Must handle:
 * - Partial headers (return 0)
 * - Invalid format (return -1, set request->valid = 0)
 * - Valid request (return bytes consumed, set request->valid = 1)
 */
int gf_parse_request(const char *buffer, size_t buflen, gf_request_t *request);

/* ---- Response Functions ---- */

/*
 * gf_create_response_header - Build a GETFILE response header
 *
 * @param buffer: Output buffer for the header
 * @param buflen: Size of the buffer
 * @param status: Response status code
 * @param content_length: File size (ignored if status != OK/CACHED)
 * @return: Number of bytes written, or -1 on error
 *
 * Example output: "GETFILE OK 1234\r\n\r\n"
 */
int gf_create_response_header(char *buffer, size_t buflen,
                               gf_status_t status, size_t content_length);

/*
 * gf_parse_response_header - Parse a GETFILE response header
 *
 * @param buffer: Input buffer containing the response
 * @param buflen: Number of bytes in buffer
 * @param response: Output structure for parsed response
 * @return: Number of bytes consumed (header size), 0 if incomplete, -1 on error
 *
 * Must handle:
 * - Partial headers (return 0)
 * - Invalid format (return -1)
 * - All valid status codes
 */
int gf_parse_response_header(const char *buffer, size_t buflen,
                              gf_response_t *response);

/* ---- Utility Functions ---- */

/*
 * gf_status_to_string - Convert status code to string
 *
 * @param status: Status code
 * @return: String representation (e.g., "OK", "FILE_NOT_FOUND")
 */
const char *gf_status_to_string(gf_status_t status);

/*
 * gf_string_to_status - Convert string to status code
 *
 * @param str: Status string
 * @return: Status code, or STATUS_INVALID if not recognized
 */
gf_status_t gf_string_to_status(const char *str);

/*
 * gf_find_header_end - Find the end of the header (\r\n\r\n)
 *
 * @param buffer: Input buffer
 * @param buflen: Number of bytes in buffer
 * @return: Position after \r\n\r\n, or 0 if not found
 */
size_t gf_find_header_end(const char *buffer, size_t buflen);

#endif /* PROTOCOL_H */
