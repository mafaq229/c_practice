/*
CS-6200 Preparation - Module 05: File Transfer Server

This is a simplified version of what you'll build in GIOS Project 1.
The server receives file requests and sends back file contents.

Protocol:
  Request:  GET /path/to/file\r\n
  Response: OK <size>\r\n<file data>
         or ERROR <message>\r\n

Compile: clang -Wall -Wextra -g ft_server.c -o ft_server
Run:     ./ft_server [port] [root_dir]

Difficulty: [HARD]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DEFAULT_PORT 8080
#define DEFAULT_ROOT "./test_files"
#define BUFFER_SIZE 4096
#define MAX_PATH_LEN 256
#define BACKLOG 10

volatile sig_atomic_t running = 1;

void handle_signal(int sig) {
    (void)sig;
    running = 0;
}

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
        if (n == 0) break;  /* Connection closed */
        received++;
        if (buf[received - 1] == '\n') break;
    }
    buf[received] = '\0';
    return received;
}

/*
TODO: Parse a file request.
Format: "GET /path/to/file\r\n"

Returns 0 on success, stores path in 'path' buffer.
*/
int parse_request(const char *request, char *path, size_t path_size) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Check that request starts with "GET "
     * 2. Extract path (everything between "GET " and "\r\n" or "\n")
     * 3. Validate path (should start with /)
     * 4. Copy path to buffer
     *
     * Security note: In real code, validate path to prevent directory traversal!
     */

    if (strncmp(request, "GET ", 4) != 0) {
        return -1;
    }

    const char *path_start = request + 4;
    const char *path_end = strpbrk(path_start, "\r\n");

    size_t len;
    if (path_end) {
        len = path_end - path_start;
    } else {
        len = strlen(path_start);
    }

    if (len == 0 || len >= path_size) {
        return -1;
    }

    strncpy(path, path_start, len);
    path[len] = '\0';

    /* Basic security: reject paths with .. */
    if (strstr(path, "..") != NULL) {
        return -1;
    }

    return 0;
}

/*
TODO: Send a file to the client.

Response format:
  On success: "OK <size>\r\n" followed by file data
  On error:   "ERROR <message>\r\n"
*/
int send_file(int client_fd, const char *root_dir, const char *path) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Build full file path (root_dir + path)
     * 2. Check if file exists and get size
     * 3. If not found, send "ERROR File not found\r\n"
     * 4. Open file for reading
     * 5. Send "OK <size>\r\n"
     * 6. Read file in chunks and send to client
     * 7. Close file
     */

    /* Build full path */
    char full_path[MAX_PATH_LEN];
    snprintf(full_path, sizeof(full_path), "%s%s", root_dir, path);

    /* Check if file exists */
    struct stat st;
    if (stat(full_path, &st) != 0) {
        const char *error = "ERROR File not found\r\n";
        send_all(client_fd, error, strlen(error));
        return -1;
    }

    /* Check if it's a regular file */
    if (!S_ISREG(st.st_mode)) {
        const char *error = "ERROR Not a regular file\r\n";
        send_all(client_fd, error, strlen(error));
        return -1;
    }

    /* Open file */
    int fd = open(full_path, O_RDONLY);
    if (fd < 0) {
        const char *error = "ERROR Cannot open file\r\n";
        send_all(client_fd, error, strlen(error));
        return -1;
    }

    /* Send success header */
    char header[64];
    snprintf(header, sizeof(header), "OK %ld\r\n", (long)st.st_size);
    if (send_all(client_fd, header, strlen(header)) < 0) {
        close(fd);
        return -1;
    }

    /* Send file data */
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    size_t total_sent = 0;

    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        if (send_all(client_fd, buffer, bytes_read) < 0) {
            close(fd);
            return -1;
        }
        total_sent += bytes_read;
    }

    close(fd);

    if (bytes_read < 0) {
        return -1;
    }

    printf("Sent %zu bytes for %s\n", total_sent, path);
    return 0;
}

/*
Handle a single client connection.
*/
void handle_client(int client_fd, const char *root_dir) {
    char request[BUFFER_SIZE];

    /* Receive request line */
    ssize_t n = recv_line(client_fd, request, sizeof(request));
    if (n <= 0) {
        fprintf(stderr, "Failed to receive request\n");
        return;
    }

    printf("Request: %s", request);

    /* Parse request */
    char path[MAX_PATH_LEN];
    if (parse_request(request, path, sizeof(path)) != 0) {
        const char *error = "ERROR Invalid request\r\n";
        send_all(client_fd, error, strlen(error));
        return;
    }

    /* Send file */
    send_file(client_fd, root_dir, path);
}

/*
Create test files for testing
*/
void create_test_files(const char *root_dir) {
    /* Create directory if it doesn't exist */
    mkdir(root_dir, 0755);

    /* Create small test file */
    char path[256];
    snprintf(path, sizeof(path), "%s/small.txt", root_dir);
    FILE *f = fopen(path, "w");
    if (f) {
        fprintf(f, "This is a small test file.\n");
        fprintf(f, "It has a few lines.\n");
        fprintf(f, "Hello, World!\n");
        fclose(f);
        printf("Created %s\n", path);
    }

    /* Create larger test file */
    snprintf(path, sizeof(path), "%s/large.bin", root_dir);
    f = fopen(path, "wb");
    if (f) {
        for (int i = 0; i < 1000; i++) {
            fprintf(f, "Line %04d: This is test data for the file transfer exercise.\n", i);
        }
        fclose(f);
        printf("Created %s\n", path);
    }
}

int main(int argc, char *argv[]) {
    printf("\n================================================\n");
    printf("  Module 05: File Transfer Server\n");
    printf("================================================\n\n");

    int port = DEFAULT_PORT;
    const char *root_dir = DEFAULT_ROOT;

    if (argc > 1) port = atoi(argv[1]);
    if (argc > 2) root_dir = argv[2];

    /* Create test files */
    create_test_files(root_dir);

    /* Create server socket */
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(port)
    };

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, BACKLOG) < 0) {
        perror("listen");
        close(server_fd);
        return 1;
    }

    printf("Server listening on port %d\n", port);
    printf("Root directory: %s\n", root_dir);
    printf("Press Ctrl+C to stop\n\n");
    printf("Test with: ./ft_client localhost %d /small.txt\n\n", port);

    signal(SIGINT, handle_signal);

    while (running) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_fd < 0) {
            if (errno == EINTR) continue;
            perror("accept");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        printf("Client connected: %s:%d\n", client_ip, ntohs(client_addr.sin_port));

        handle_client(client_fd, root_dir);

        close(client_fd);
        printf("Client disconnected\n\n");
    }

    close(server_fd);
    printf("Server stopped.\n");

    return 0;
}
