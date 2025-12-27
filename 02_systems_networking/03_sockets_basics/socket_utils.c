/*
 * Socket Utilities Implementation
 *
 * TODO: Implement these utility functions.
 * These will be reusable in your GIOS projects.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "socket_utils.h"

/*
 * Send all data, handling partial sends.
 */
ssize_t send_all(int fd, const void *buf, size_t len) {
    const char *ptr = buf;
    size_t remaining = len;

    while (remaining > 0) {
        ssize_t sent = send(fd, ptr, remaining, 0);
        if (sent < 0) {
            if (errno == EINTR) continue;  /* Retry on interrupt */
            return -1;
        }
        if (sent == 0) {
            break;  /* Connection closed */
        }
        ptr += sent;
        remaining -= sent;
    }

    return len - remaining;
}

/*
 * TODO: Receive exactly 'len' bytes.
 */
ssize_t recv_all(int fd, void *buf, size_t len) {
    /* TODO: Implement this function
     *
     * Similar to send_all, but for receiving.
     * Loop until all bytes received or connection closed.
     */

    char *ptr = buf;
    size_t remaining = len;

    while (remaining > 0) {
        ssize_t received = recv(fd, ptr, remaining, 0);
        if (received < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (received == 0) {
            break;  /* Connection closed */
        }
        ptr += received;
        remaining -= received;
    }

    return len - remaining;
}

/*
 * TODO: Receive until delimiter is found.
 *
 * This is useful for reading protocol headers that end with \r\n or similar.
 */
ssize_t recv_until(int fd, void *buf, size_t max_len, const char *delim) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Receive one byte at a time (or use buffering)
     * 2. Check if buffer ends with delimiter
     * 3. Return when delimiter found or max_len reached
     *
     * For GIOS Project 1, you'll need this to read headers.
     */

    char *ptr = buf;
    size_t received = 0;
    size_t delim_len = strlen(delim);

    if (delim_len == 0 || max_len == 0) {
        return 0;
    }

    while (received < max_len) {
        ssize_t n = recv(fd, ptr, 1, 0);
        if (n < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (n == 0) {
            break;  /* Connection closed */
        }

        received++;
        ptr++;

        /* Check if we've received the delimiter */
        if (received >= delim_len) {
            if (memcmp(ptr - delim_len, delim, delim_len) == 0) {
                break;  /* Found delimiter */
            }
        }
    }

    return received;
}

/*
 * TODO: Create a server socket, bind, and listen.
 */
int create_server_socket(int port, int backlog) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Create socket
     * 2. Set SO_REUSEADDR
     * 3. Bind to address
     * 4. Listen
     * 5. Return socket fd
     */

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    /* Allow address reuse */
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt SO_REUSEADDR");
        close(sockfd);
        return -1;
    }

    /* Bind to address */
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(port)
    };

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sockfd);
        return -1;
    }

    /* Start listening */
    if (listen(sockfd, backlog) < 0) {
        perror("listen");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

/*
 * TODO: Create a client socket and connect.
 */
int connect_to_server(const char *host, int port) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Create socket
     * 2. Resolve hostname (or use inet_pton for IP)
     * 3. Connect
     * 4. Return socket fd
     */

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    /* Set up server address */
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port)
    };

    /* Try to parse as IP address first */
    if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) {
        /* Not an IP address, try hostname lookup */
        struct hostent *he = gethostbyname(host);
        if (he == NULL) {
            fprintf(stderr, "Could not resolve hostname: %s\n", host);
            close(sockfd);
            return -1;
        }
        memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);
    }

    /* Connect */
    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

/*
 * Set socket to non-blocking mode.
 */
int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        perror("fcntl F_GETFL");
        return -1;
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("fcntl F_SETFL");
        return -1;
    }

    return 0;
}

/*
 * Set TCP_NODELAY (disable Nagle's algorithm).
 * Useful for low-latency applications.
 */
int set_tcp_nodelay(int fd) {
    int opt = 1;
    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)) < 0) {
        perror("setsockopt TCP_NODELAY");
        return -1;
    }
    return 0;
}
