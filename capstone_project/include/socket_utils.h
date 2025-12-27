/*
 * socket_utils.h - Socket Helper Functions
 *
 * This header provides utility functions for socket operations.
 * These helpers handle common patterns like partial sends/receives,
 * connection setup, and error handling.
 *
 * Key Concepts:
 * - TCP sockets don't guarantee message boundaries
 * - send() and recv() may transfer fewer bytes than requested
 * - System calls can be interrupted (EINTR)
 */

#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <stddef.h>
#include <sys/types.h>

/* ============================================================================
 * Connection Setup Functions
 * ============================================================================ */

/*
 * create_server_socket - Create and bind a listening socket
 *
 * @param port: Port number to bind to
 * @param backlog: Maximum pending connections (use BACKLOG from protocol.h)
 * @return: Socket file descriptor, or -1 on error
 *
 * This function should:
 * 1. Create a TCP socket (AF_INET, SOCK_STREAM)
 * 2. Set SO_REUSEADDR option
 * 3. Bind to the specified port (any address)
 * 4. Start listening
 *
 * Don't forget:
 * - Use htons() for port number
 * - Handle errors at each step
 */
int create_server_socket(int port, int backlog);

/*
 * create_client_socket - Create and connect to a server
 *
 * @param hostname: Server hostname or IP address
 * @param port: Server port number
 * @return: Connected socket file descriptor, or -1 on error
 *
 * This function should:
 * 1. Resolve hostname using getaddrinfo()
 * 2. Create a TCP socket
 * 3. Connect to the server
 *
 * Note: getaddrinfo() is preferred over gethostbyname()
 */
int create_client_socket(const char *hostname, int port);

/*
 * accept_client - Accept a new client connection
 *
 * @param server_fd: Listening socket file descriptor
 * @param client_addr: Buffer to store client address (can be NULL)
 * @param addr_len: Pointer to address length (can be NULL)
 * @return: Client socket file descriptor, or -1 on error
 *
 * This is a wrapper around accept() that handles EINTR.
 */
int accept_client(int server_fd, struct sockaddr *client_addr, socklen_t *addr_len);

/* ============================================================================
 * Data Transfer Functions
 * ============================================================================ */

/*
 * send_all - Send all bytes in buffer
 *
 * @param fd: Socket file descriptor
 * @param buffer: Data to send
 * @param length: Number of bytes to send
 * @return: Number of bytes sent, or -1 on error
 *
 * IMPORTANT: This function must handle:
 * - Partial sends (send() may not send all bytes)
 * - EINTR (retry if interrupted)
 *
 * Loop until all bytes are sent or an error occurs.
 */
ssize_t send_all(int fd, const void *buffer, size_t length);

/*
 * recv_all - Receive exactly n bytes
 *
 * @param fd: Socket file descriptor
 * @param buffer: Buffer to store received data
 * @param length: Exact number of bytes to receive
 * @return: Number of bytes received, 0 on connection close, -1 on error
 *
 * IMPORTANT: This function must handle:
 * - Partial receives (recv() may not receive all bytes)
 * - EINTR (retry if interrupted)
 * - Connection close (recv() returns 0)
 *
 * Loop until exactly 'length' bytes received or connection closes.
 */
ssize_t recv_all(int fd, void *buffer, size_t length);

/*
 * recv_until - Receive until delimiter or max bytes
 *
 * @param fd: Socket file descriptor
 * @param buffer: Buffer to store received data
 * @param max_len: Maximum bytes to receive
 * @param delim: Delimiter string to search for
 * @return: Number of bytes received (including delimiter),
 *          0 on connection close, -1 on error
 *
 * This is useful for receiving headers that end with \r\n\r\n.
 * Keep receiving until:
 * - Delimiter found (return total including delimiter)
 * - max_len reached (return -1, request too large)
 * - Connection closed (return 0)
 *
 * Hint: Use memmem() to search for delimiter in buffer
 */
ssize_t recv_until(int fd, void *buffer, size_t max_len, const char *delim);

/* ============================================================================
 * File Transfer Functions
 * ============================================================================ */

/*
 * send_file - Send a file over a socket
 *
 * @param fd: Socket file descriptor
 * @param filepath: Path to file to send
 * @param bytes_sent: Output parameter for bytes actually sent (can be NULL)
 * @return: 0 on success, -1 on error
 *
 * Steps:
 * 1. Open the file
 * 2. Read chunks into buffer
 * 3. Send each chunk using send_all()
 * 4. Close the file
 *
 * Track bytes_sent for progress reporting.
 */
int send_file(int fd, const char *filepath, size_t *bytes_sent);

/*
 * recv_file - Receive a file over a socket
 *
 * @param fd: Socket file descriptor
 * @param filepath: Path to save received file
 * @param file_size: Expected file size
 * @param bytes_received: Output parameter for bytes actually received (can be NULL)
 * @return: 0 on success, -1 on error
 *
 * Steps:
 * 1. Open/create the file
 * 2. Receive chunks from socket
 * 3. Write each chunk to file
 * 4. Close the file
 *
 * Stop when file_size bytes received or connection closes.
 */
int recv_file(int fd, const char *filepath, size_t file_size, size_t *bytes_received);

/* ============================================================================
 * Utility Functions
 * ============================================================================ */

/*
 * set_socket_timeout - Set send/receive timeout on socket
 *
 * @param fd: Socket file descriptor
 * @param timeout_sec: Timeout in seconds
 * @return: 0 on success, -1 on error
 *
 * Use SO_RCVTIMEO and SO_SNDTIMEO socket options.
 */
int set_socket_timeout(int fd, int timeout_sec);

/*
 * close_socket - Close a socket safely
 *
 * @param fd: Socket file descriptor
 *
 * Use shutdown() before close() for clean disconnection.
 */
void close_socket(int fd);

#endif /* SOCKET_UTILS_H */
