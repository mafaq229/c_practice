/*
socket_utils.c - Socket Helper Functions Implementation

These utilities handle common socket programming patterns:
- Handling partial sends and receives
- Dealing with EINTR (interrupted system calls)
- Connection setup for clients and servers

These patterns are CRITICAL for GIOS projects!
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include "../include/socket_utils.h"
#include "../include/protocol.h"

/* ============================================================================
Connection Setup Functions
============================================================================ */

/*
create_server_socket - Create a listening socket
*/
int create_server_socket(int port, int backlog) {
    /*
     * TODO: Implement this function
     *
     * Steps:
     * 1. Create socket: socket(AF_INET, SOCK_STREAM, 0)
     * 2. Set SO_REUSEADDR option (allows quick restart)
     * 3. Set up address structure:
     *    - sin_family = AF_INET
     *    - sin_addr.s_addr = INADDR_ANY (accept connections on any interface)
     *    - sin_port = htons(port)  <-- DON'T FORGET htons()!
     * 4. Bind socket to address: bind(fd, ...)
     * 5. Start listening: listen(fd, backlog)
     * 6. Return socket fd, or -1 on any error
     *
     * Error handling:
     * - Print error with perror() for debugging
     * - Close socket if any step fails after creation
     *
     * Example:
     *   int server_fd = create_server_socket(8080, 128);
     */

    int fd;
    int opt = 1;
    struct sockaddr_in addr;

    /* Step 1: Create socket */
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return -1;
    }

    /* Step 2: Set SO_REUSEADDR */
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(fd);
        return -1;
    }

    /* YOUR CODE HERE: Steps 3-5 */

    (void)port;     /* Remove when implemented */
    (void)backlog;  /* Remove when implemented */
    (void)addr;     /* Remove when implemented */

    /* Placeholder - close and return error */
    close(fd);
    return -1;
}

/*
create_client_socket - Connect to a server
*/
int create_client_socket(const char *hostname, int port) {
    /*
     * TODO: Implement this function
     *
     * Use getaddrinfo() for modern, protocol-agnostic lookup:
     *
     * Steps:
     * 1. Set up hints structure:
     *    - ai_family = AF_INET (IPv4)
     *    - ai_socktype = SOCK_STREAM (TCP)
     * 2. Convert port to string
     * 3. Call getaddrinfo(hostname, port_str, &hints, &result)
     * 4. Try each address until one connects:
     *    - socket()
     *    - connect()
     * 5. Free the address list: freeaddrinfo()
     * 6. Return connected socket or -1
     *
     * Alternative (simpler but less portable):
     * - Use inet_pton() for IP addresses
     * - Set up sockaddr_in directly
     */

    struct addrinfo hints, *result, *rp;
    char port_str[16];
    int fd = -1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    snprintf(port_str, sizeof(port_str), "%d", port);

    /* YOUR CODE HERE */

    (void)hostname;
    (void)result;
    (void)rp;

    return fd;
}

/*
accept_client - Accept a client connection with EINTR handling
*/
int accept_client(int server_fd, struct sockaddr *client_addr, socklen_t *addr_len) {
    /*
     * TODO: Implement this function
     *
     * Handle EINTR - accept() can be interrupted by signals.
     * Loop until accept succeeds or a real error occurs.
     *
     * Pattern:
     *   do {
     *       client_fd = accept(server_fd, client_addr, addr_len);
     *   } while (client_fd < 0 && errno == EINTR);
     *
     * Return client_fd (>= 0 on success, -1 on error)
     */

    int client_fd;

    /* YOUR CODE HERE */

    (void)server_fd;
    (void)client_addr;
    (void)addr_len;

    client_fd = -1;  /* Placeholder */

    return client_fd;
}

/* ============================================================================
Data Transfer Functions
============================================================================ */

/*
send_all - Send all bytes, handling partial sends
*/
ssize_t send_all(int fd, const void *buffer, size_t length) {
    /*
     * TODO: Implement this function
     *
     * This is one of the MOST IMPORTANT functions for networking!
     *
     * send() may send fewer bytes than requested.
     * You MUST loop until all bytes are sent.
     *
     * Pattern:
     *   size_t total_sent = 0;
     *   while (total_sent < length) {
     *       ssize_t n = send(fd, buffer + total_sent, length - total_sent, 0);
     *       if (n < 0) {
     *           if (errno == EINTR) continue;  // Retry on interrupt
     *           return -1;  // Real error
     *       }
     *       if (n == 0) {
     *           // Connection closed? (unusual for send)
     *           break;
     *       }
     *       total_sent += n;
     *   }
     *   return total_sent;
     *
     * Cast buffer: (const char *)buffer + total_sent
     */

    (void)fd;
    (void)buffer;
    (void)length;

    /* YOUR CODE HERE */

    return -1;  /* Placeholder */
}

/*
recv_all - Receive exactly n bytes
*/
ssize_t recv_all(int fd, void *buffer, size_t length) {
    /*
     * TODO: Implement this function
     *
     * Similar to send_all, but for receiving.
     * Keeps receiving until exactly `length` bytes received.
     *
     * IMPORTANT: Handle connection close (recv returns 0)
     *
     * Pattern:
     *   size_t total_recv = 0;
     *   while (total_recv < length) {
     *       ssize_t n = recv(fd, buffer + total_recv, length - total_recv, 0);
     *       if (n < 0) {
     *           if (errno == EINTR) continue;
     *           return -1;
     *       }
     *       if (n == 0) {
     *           // Connection closed
     *           break;
     *       }
     *       total_recv += n;
     *   }
     *   return total_recv;
     */

    (void)fd;
    (void)buffer;
    (void)length;

    /* YOUR CODE HERE */

    return -1;  /* Placeholder */
}

/*
recv_until - Receive until delimiter found
*/
ssize_t recv_until(int fd, void *buffer, size_t max_len, const char *delim) {
    /*
     * TODO: Implement this function
     *
     * This is useful for receiving headers that end with \r\n\r\n.
     *
     * Strategy:
     * - Receive one byte (or small chunks) at a time
     * - After each receive, check if buffer contains delimiter
     * - Stop when delimiter found or max_len reached
     *
     * Hint: Use memmem() to search for delimiter:
     *   void *found = memmem(buffer, total, delim, strlen(delim));
     *   if (found) { found delimiter }
     *
     * Alternative: Receive in chunks and search after each chunk
     *
     * Return: total bytes received (including delimiter),
     *         0 if connection closed,
     *         -1 on error or if max_len reached without finding delimiter
     */

    char *buf = (char *)buffer;
    size_t total = 0;
    size_t delim_len = strlen(delim);

    (void)fd;
    (void)buf;
    (void)max_len;
    (void)delim_len;

    /* YOUR CODE HERE */

    return -1;  /* Placeholder */
}

/* ============================================================================
File Transfer Functions
============================================================================ */

/*
send_file - Send a file's contents over a socket
*/
int send_file(int fd, const char *filepath, size_t *bytes_sent) {
    /*
     * TODO: Implement this function
     *
     * Steps:
     * 1. Open the file for reading (open() with O_RDONLY)
     * 2. Read chunks into a buffer
     * 3. Send each chunk using send_all()
     * 4. Track total bytes sent
     * 5. Close the file
     * 6. Return 0 on success, -1 on error
     *
     * Use a reasonable buffer size (e.g., 8192 bytes)
     *
     * Pattern:
     *   char buf[BUFFER_SIZE];
     *   ssize_t n;
     *   size_t total = 0;
     *   while ((n = read(file_fd, buf, sizeof(buf))) > 0) {
     *       if (send_all(fd, buf, n) != n) {
     *           // Send error
     *           close(file_fd);
     *           return -1;
     *       }
     *       total += n;
     *   }
     *
     * Update bytes_sent if not NULL.
     */

    (void)fd;
    (void)filepath;
    (void)bytes_sent;

    /* YOUR CODE HERE */

    return -1;  /* Placeholder */
}

/*
recv_file - Receive file contents and save to disk
*/
int recv_file(int fd, const char *filepath, size_t file_size, size_t *bytes_received) {
    /*
     * TODO: Implement this function
     *
     * Steps:
     * 1. Open/create output file (open() with O_WRONLY|O_CREAT|O_TRUNC, mode 0644)
     * 2. Receive chunks from socket
     * 3. Write each chunk to file
     * 4. Track total bytes received
     * 5. Stop when file_size bytes received
     * 6. Close the file
     * 7. Return 0 on success, -1 on error
     *
     * Calculate remaining bytes for each recv:
     *   remaining = file_size - total;
     *   to_recv = (remaining < BUFFER_SIZE) ? remaining : BUFFER_SIZE;
     */

    (void)fd;
    (void)filepath;
    (void)file_size;
    (void)bytes_received;

    /* YOUR CODE HERE */

    return -1;  /* Placeholder */
}

/* ============================================================================
Utility Functions
============================================================================ */

/*
set_socket_timeout - Set socket timeouts
*/
int set_socket_timeout(int fd, int timeout_sec) {
    /*
     * TODO: Implement this function
     *
     * Set both receive and send timeouts.
     *
     * struct timeval tv;
     * tv.tv_sec = timeout_sec;
     * tv.tv_usec = 0;
     *
     * setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
     * setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
     */

    (void)fd;
    (void)timeout_sec;

    /* YOUR CODE HERE */

    return -1;  /* Placeholder */
}

/*
close_socket - Clean socket shutdown
*/
void close_socket(int fd) {
    /*
     * TODO: Implement this function
     *
     * For clean shutdown:
     * 1. shutdown(fd, SHUT_RDWR) - stop both reading and writing
     * 2. close(fd)
     *
     * shutdown() is optional but helps ensure clean TCP termination.
     */

    if (fd >= 0) {
        /* YOUR CODE HERE */
        close(fd);  /* Basic implementation */
    }
}
