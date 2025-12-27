# Project 2: Systems Programming & Networking

**Estimated Time:** 12-15 hours
**Priority:** Critical - This directly prepares you for GIOS Project 1

This project teaches file I/O and socket programming - the core skills needed
for GIOS Project 1 (socket-based file transfer).

## Learning Objectives

After completing this project, you will be able to:
- Perform low-level file I/O operations
- Create TCP client-server applications
- Design and implement network protocols
- Handle partial sends/receives correctly
- Build a file transfer system

## Modules

| Module | Topic | Time | Files |
|--------|-------|------|-------|
| 01 | [Linux Basics](01_linux_basics/) | 1 hr | commands_practice.md |
| 02 | [File I/O](02_file_io/) | 2 hrs | file_copy.c, binary_file.c, buffered_io.c |
| 03 | [Socket Basics](03_sockets_basics/) | 4 hrs | tcp_server.c, tcp_client.c |
| 04 | [Protocol Design](04_protocol_design/) | 2 hrs | protocol_parser.c |
| 05 | [File Transfer](05_file_transfer/) | 4 hrs | ft_server.c, ft_client.c |

## GIOS Project 1 Connection

GIOS Project 1 requires you to:
1. Implement a file transfer protocol with headers
2. Parse requests like "GETFILE GET /path/to/file\r\n\r\n"
3. Handle large file transfers efficiently
4. Add multi-threading (covered in Project 3)

This project prepares you for parts 1-3 (single-threaded version).

## Key Concepts

### File Descriptors
Everything in Unix is a file - including network sockets!
- 0: stdin
- 1: stdout
- 2: stderr
- 3+: files, sockets, pipes you open

### TCP Socket Flow

**Server:**
```
socket() → bind() → listen() → accept() → recv()/send() → close()
```

**Client:**
```
socket() → connect() → send()/recv() → close()
```

### Critical Networking Gotchas

1. **Partial sends/receives**: `send()` and `recv()` may not transfer all bytes!
   ```c
   // WRONG: Assumes all bytes sent
   send(sock, buffer, length, 0);

   // CORRECT: Loop until all sent
   size_t total = 0;
   while (total < length) {
       ssize_t sent = send(sock, buffer + total, length - total, 0);
       if (sent <= 0) { /* handle error */ }
       total += sent;
   }
   ```

2. **Byte order**: Network uses big-endian, most CPUs use little-endian.
   Use `htonl`/`ntohl` for 32-bit, `htons`/`ntohs` for 16-bit.

3. **Blocking I/O**: By default, sockets block until data arrives.

## Recommended Reading

Before starting:
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/) - Chapters 1-6
- Man pages: `man 2 socket`, `man 2 bind`, `man 2 accept`

## Building and Testing

```bash
# Build all modules
make

# Test a specific module
cd 03_sockets_basics
make
./tcp_server &     # Start server in background
./tcp_client       # Run client
kill %1            # Stop server

# Clean
make clean
```

## Quick Socket Reference

### Server Setup
```c
int sockfd = socket(AF_INET, SOCK_STREAM, 0);

struct sockaddr_in addr = {
    .sin_family = AF_INET,
    .sin_addr.s_addr = INADDR_ANY,
    .sin_port = htons(8080)
};

bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
listen(sockfd, 10);  // 10 = backlog

int client = accept(sockfd, NULL, NULL);
// Now use client for recv/send
```

### Client Setup
```c
int sockfd = socket(AF_INET, SOCK_STREAM, 0);

struct sockaddr_in addr = {
    .sin_family = AF_INET,
    .sin_port = htons(8080)
};
inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
// Now use sockfd for send/recv
```

## Common Mistakes

1. **Forgetting to close sockets** - causes resource leaks
2. **Not handling SIGPIPE** - crashes server when client disconnects
3. **Assuming message boundaries** - TCP is a byte stream, not messages!
4. **Not checking return values** - all socket calls can fail

## Next Steps

After completing this project:
→ [Project 3: Concurrency & IPC](../03_concurrency_ipc/) - Add multithreading to your server
