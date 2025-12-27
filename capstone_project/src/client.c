/*
 * client.c - GETFILE Client
 *
 * A client that connects to the server and requests files using
 * the GETFILE protocol.
 *
 * Usage: ./client <host> <port> <path> [output_file]
 *
 * Examples:
 *   ./client localhost 8080 /small.txt
 *   ./client localhost 8080 /large.bin output.bin
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "../include/protocol.h"
#include "../include/socket_utils.h"
#include "../include/file_utils.h"

/* ============================================================================
 * Request Functions
 * ============================================================================ */

/*
 * request_file - Request a file from the server
 *
 * Returns: 0 on success, -1 on error
 */
static int request_file(const char *host, int port, const char *path,
                        const char *output_path) {
    /*
     * TODO: Implement file request
     *
     * Steps:
     *
     * 1. Connect to server
     *
     *    int fd = create_client_socket(host, port);
     *    if (fd < 0) {
     *        fprintf(stderr, "Failed to connect to %s:%d\n", host, port);
     *        return -1;
     *    }
     *    printf("Connected to %s:%d\n", host, port);
     *
     * 2. Build and send request
     *
     *    char request[MAX_REQUEST_LEN];
     *    int req_len = gf_create_request(request, sizeof(request), path);
     *    if (req_len < 0) {
     *        fprintf(stderr, "Failed to create request\n");
     *        close(fd);
     *        return -1;
     *    }
     *    printf("Requesting: %s\n", path);
     *    if (send_all(fd, request, req_len) != req_len) {
     *        fprintf(stderr, "Failed to send request\n");
     *        close(fd);
     *        return -1;
     *    }
     *
     * 3. Receive response header
     *
     *    char buffer[MAX_HEADER_LEN];
     *    ssize_t n = recv_until(fd, buffer, sizeof(buffer), HEADER_DELIM);
     *    if (n <= 0) {
     *        fprintf(stderr, "Failed to receive response\n");
     *        close(fd);
     *        return -1;
     *    }
     *
     * 4. Parse response header
     *
     *    gf_response_t response;
     *    int header_len = gf_parse_response_header(buffer, n, &response);
     *    if (header_len <= 0 || !response.header_complete) {
     *        fprintf(stderr, "Invalid response\n");
     *        close(fd);
     *        return -1;
     *    }
     *
     * 5. Check status
     *
     *    printf("Status: %s\n", gf_status_to_string(response.status));
     *    if (response.status != STATUS_OK && response.status != STATUS_CACHED) {
     *        close(fd);
     *        return -1;
     *    }
     *    printf("Content-Length: %zu bytes\n", response.content_length);
     *
     * 6. Receive file content
     *
     *    Option A: Save to file
     *    if (output_path) {
     *        size_t received;
     *        if (recv_file(fd, output_path, response.content_length, &received) < 0) {
     *            fprintf(stderr, "Failed to receive file\n");
     *            close(fd);
     *            return -1;
     *        }
     *        printf("Saved %zu bytes to %s\n", received, output_path);
     *    }
     *
     *    Option B: Print to stdout (for text files)
     *    else {
     *        char *content = malloc(response.content_length + 1);
     *        if (recv_all(fd, content, response.content_length) != response.content_length) {
     *            fprintf(stderr, "Failed to receive content\n");
     *            free(content);
     *            close(fd);
     *            return -1;
     *        }
     *        content[response.content_length] = '\0';
     *        printf("\n--- File Content ---\n%s\n--- End ---\n", content);
     *        free(content);
     *    }
     *
     * 7. Cleanup
     *    close_socket(fd);
     *    return 0;
     */

    printf("Requesting file %s from %s:%d\n", path, host, port);

    /* YOUR CODE HERE */

    (void)output_path;

    return -1;  /* Placeholder */
}

/* ============================================================================
 * Main Entry Point
 * ============================================================================ */

static void print_usage(const char *prog) {
    fprintf(stderr, "Usage: %s <host> <port> <path> [output_file]\n", prog);
    fprintf(stderr, "\nExamples:\n");
    fprintf(stderr, "  %s localhost 8080 /small.txt\n", prog);
    fprintf(stderr, "  %s localhost 8080 /large.bin output.bin\n", prog);
    fprintf(stderr, "  %s 127.0.0.1 8888 /test.txt (via proxy)\n", prog);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        print_usage(argv[0]);
        return 1;
    }

    const char *host = argv[1];
    int port = atoi(argv[2]);
    const char *path = argv[3];
    const char *output = (argc > 4) ? argv[4] : NULL;

    /* Validate port */
    if (port <= 0 || port > 65535) {
        fprintf(stderr, "Invalid port: %s\n", argv[2]);
        return 1;
    }

    /* Validate path */
    if (path[0] != '/') {
        fprintf(stderr, "Path must start with /: %s\n", path);
        return 1;
    }

    printf("=== Mini-GIOS Client ===\n\n");

    int result = request_file(host, port, path, output);

    if (result == 0) {
        printf("\nRequest successful!\n");
    } else {
        printf("\nRequest failed!\n");
    }

    return result;
}
