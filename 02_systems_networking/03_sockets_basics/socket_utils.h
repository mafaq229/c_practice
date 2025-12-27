/*
 * Socket Utilities Header
 *
 * Common socket functions for client-server applications.
 */

#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/*
 * Send all data, handling partial sends.
 * Returns total bytes sent, or -1 on error.
 */
ssize_t send_all(int fd, const void *buf, size_t len);

/*
 * Receive exactly 'len' bytes.
 * Returns bytes received, or -1 on error.
 * Returns less than 'len' if connection closed.
 */
ssize_t recv_all(int fd, void *buf, size_t len);

/*
 * Receive until delimiter is found.
 * Returns bytes received (including delimiter), or -1 on error.
 */
ssize_t recv_until(int fd, void *buf, size_t max_len, const char *delim);

/*
 * Create a server socket, bind, and listen.
 * Returns socket fd, or -1 on error.
 */
int create_server_socket(int port, int backlog);

/*
 * Create a client socket and connect.
 * Returns socket fd, or -1 on error.
 */
int connect_to_server(const char *host, int port);

/*
 * Set socket to non-blocking mode.
 * Returns 0 on success, -1 on error.
 */
int set_nonblocking(int fd);

/*
 * Set TCP_NODELAY (disable Nagle's algorithm).
 * Returns 0 on success, -1 on error.
 */
int set_tcp_nodelay(int fd);

#endif /* SOCKET_UTILS_H */
