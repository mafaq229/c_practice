/*
 * CS-6200 Preparation - Module 05: File Transfer Client
 *
 * Client for the file transfer protocol.
 *
 * Protocol:
 *   Request:  GET /path/to/file\r\n
 *   Response: OK <size>\r\n<file data>
 *          or ERROR <message>\r\n
 *
 * Compile: clang -Wall -Wextra -g ft_client.c -o ft_client
 * Run:     ./ft_client <host> <port> <path> [output_file]
 *
 * Example: ./ft_client localhost 8080 /small.txt
 *
 * Difficulty: [HARD]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFFER_SIZE 4096

/* Send all data */
ssize_t send_all(int fd, const void *buf, size_t len) {
    const char *ptr = buf;
    size_t remaining = len;
    while (remaining > 0) {
        ssize_t sent = send(fd, ptr, remaining, 0);
        if (sent < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        ptr += sent;
        remaining -= sent;
    }
    return len;
}

/* Receive a line (up to \n) */
ssize_t recv_line(int fd, char *buf, size_t max_len) {
    size_t received = 0;
    while (received < max_len - 1) {
        ssize_t n = recv(fd, buf + received, 1, 0);
        if (n < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (n == 0) break;
        received++;
        if (buf[received - 1] == '\n') break;
    }
    buf[received] = '\0';
    return received;
}

/*
 * TODO: Connect to server.
 */
int connect_to_server(const char *host, int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port)
    };

    if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) {
        struct hostent *he = gethostbyname(host);
        if (he == NULL) {
            fprintf(stderr, "Could not resolve hostname: %s\n", host);
            close(sockfd);
            return -1;
        }
        memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);
    }

    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

/*
 * TODO: Parse response header.
 *
 * Format: "OK <size>\r\n" or "ERROR <message>\r\n"
 *
 * Returns file size on success, -1 on error.
 */
long parse_response(const char *response) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Check if response starts with "OK "
     * 2. Parse size after "OK "
     * 3. Return size
     *
     * If response starts with "ERROR ", print error and return -1.
     */

    if (strncmp(response, "OK ", 3) == 0) {
        return atol(response + 3);
    } else if (strncmp(response, "ERROR ", 6) == 0) {
        fprintf(stderr, "Server error: %s", response + 6);
        return -1;
    } else {
        fprintf(stderr, "Invalid response: %s", response);
        return -1;
    }
}

/*
 * TODO: Download a file from the server.
 */
int download_file(const char *host, int port, const char *path, const char *output) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Connect to server
     * 2. Send request: "GET <path>\r\n"
     * 3. Receive response header
     * 4. Parse response
     * 5. If OK, receive file data and write to output
     * 6. Close connection
     */

    printf("Connecting to %s:%d...\n", host, port);

    int sockfd = connect_to_server(host, port);
    if (sockfd < 0) {
        return -1;
    }

    printf("Connected. Requesting %s...\n", path);

    /* Send request */
    char request[512];
    snprintf(request, sizeof(request), "GET %s\r\n", path);
    if (send_all(sockfd, request, strlen(request)) < 0) {
        perror("send");
        close(sockfd);
        return -1;
    }

    /* Receive response header */
    char response[256];
    if (recv_line(sockfd, response, sizeof(response)) <= 0) {
        fprintf(stderr, "Failed to receive response\n");
        close(sockfd);
        return -1;
    }

    printf("Response: %s", response);

    /* Parse response */
    long file_size = parse_response(response);
    if (file_size < 0) {
        close(sockfd);
        return -1;
    }

    printf("File size: %ld bytes\n", file_size);

    /* Open output file */
    int out_fd = -1;
    if (output != NULL) {
        out_fd = open(output, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (out_fd < 0) {
            perror("open output file");
            close(sockfd);
            return -1;
        }
        printf("Saving to: %s\n", output);
    }

    /* Receive file data */
    char buffer[BUFFER_SIZE];
    size_t total_received = 0;

    while (total_received < (size_t)file_size) {
        size_t to_receive = file_size - total_received;
        if (to_receive > sizeof(buffer)) {
            to_receive = sizeof(buffer);
        }

        ssize_t n = recv(sockfd, buffer, to_receive, 0);
        if (n < 0) {
            if (errno == EINTR) continue;
            perror("recv");
            break;
        }
        if (n == 0) {
            fprintf(stderr, "Connection closed unexpectedly\n");
            break;
        }

        if (out_fd >= 0) {
            /* Write to file */
            write(out_fd, buffer, n);
        } else {
            /* Print to stdout (for small files) */
            if (file_size < 1000) {
                printf("%.*s", (int)n, buffer);
            }
        }

        total_received += n;

        /* Progress indicator */
        if (file_size > 10000) {
            int percent = (int)(100.0 * total_received / file_size);
            printf("\rProgress: %d%% (%zu/%ld bytes)", percent, total_received, file_size);
            fflush(stdout);
        }
    }

    if (file_size > 10000) {
        printf("\n");
    }

    if (out_fd >= 0) {
        close(out_fd);
    }

    close(sockfd);

    if (total_received == (size_t)file_size) {
        printf("Download complete! Received %zu bytes.\n", total_received);
        return 0;
    } else {
        fprintf(stderr, "Download incomplete: %zu/%ld bytes\n", total_received, file_size);
        return -1;
    }
}

int main(int argc, char *argv[]) {
    printf("\n================================================\n");
    printf("  Module 05: File Transfer Client\n");
    printf("================================================\n\n");

    if (argc < 4) {
        printf("Usage: %s <host> <port> <path> [output_file]\n", argv[0]);
        printf("\nExamples:\n");
        printf("  %s localhost 8080 /small.txt\n", argv[0]);
        printf("  %s localhost 8080 /large.bin large.bin\n", argv[0]);
        return 1;
    }

    const char *host = argv[1];
    int port = atoi(argv[2]);
    const char *path = argv[3];
    const char *output = (argc > 4) ? argv[4] : NULL;

    int result = download_file(host, port, path, output);

    printf("\n================================================\n\n");

    return result;
}
