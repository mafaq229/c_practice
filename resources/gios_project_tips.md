# GIOS Project Tips

## General Advice

### Start Early
- Projects are harder than they look
- Office hours get crowded near deadlines
- Give yourself time for debugging

### Read the Rubric
- Points are distributed across requirements
- Easy to miss small requirements
- Follow the specification exactly

### Test Locally First
- Develop on your machine
- Test with Gradescope sparingly
- Limited submissions per day

## Project 1: File Transfer

### Part 1: Single-Threaded
**Key Challenges:**
- Protocol parsing (GET /path\r\n\r\n)
- Handling partial sends/receives
- Proper error handling

**Tips:**
- Test with small files first
- Use netcat (nc) to debug protocol
- Handle EINTR from system calls

```c
// Always handle partial sends
ssize_t send_all(int fd, const void *buf, size_t len) {
    size_t sent = 0;
    while (sent < len) {
        ssize_t n = send(fd, buf + sent, len - sent, 0);
        if (n < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        sent += n;
    }
    return sent;
}
```

### Part 2: Multi-Threaded
**Key Challenges:**
- Thread pool implementation
- Work queue synchronization
- Graceful shutdown

**Tips:**
- Get single-threaded working first
- Use condition variables for queue
- Test with many concurrent clients

```c
// Boss-worker pattern
while (1) {
    int client = accept(server_fd, ...);
    // Add to work queue (signal workers)
    work_queue_push(queue, client);
}

// Worker thread
while (1) {
    int client = work_queue_pop(queue);
    if (client < 0) break;  // Shutdown
    handle_client(client);
}
```

## Project 2: Proxy Server

### Key Challenges
- HTTP parsing
- Caching implementation
- Memory management

### Tips
- Use libcurl for HTTP (check if allowed)
- Implement LRU cache properly
- Watch for memory leaks

## Project 3: Inter-Process Communication

### Part 1: Shared Memory
**Key Challenges:**
- POSIX shared memory API
- Proper synchronization
- Segment management

**Tips:**
- shm_open, ftruncate, mmap
- Use semaphores for sync
- Clean up on all exit paths

```c
// Basic shared memory setup
int fd = shm_open("/myshm", O_CREAT | O_RDWR, 0666);
ftruncate(fd, size);
void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
```

### Part 2: Message Queues
- mq_open, mq_send, mq_receive
- Handle message priorities
- Proper cleanup with mq_unlink

## Project 4: gRPC Distributed File System

### Key Challenges
- C++ and gRPC learning curve
- Streaming RPCs
- Distributed coordination

### Tips
- Start with simple unary RPCs
- Test proto definitions separately
- Handle network failures

```cpp
// Basic gRPC server setup
class MyServiceImpl : public MyService::Service {
    Status MyMethod(ServerContext* ctx, const Request* req,
                   Response* resp) override {
        // Implementation
        return Status::OK;
    }
};
```

## Debugging Tips

### Use GDB/LLDB
```bash
gdb ./server
(gdb) break handle_client
(gdb) run
```

### Use Valgrind (Docker)
```bash
valgrind --leak-check=full ./program
```

### Use Address Sanitizer (Docker)
```bash
gcc -fsanitize=address -g program.c -o program
./program
```

### Add Logging
```c
#define DEBUG 1
#if DEBUG
#define LOG(fmt, ...) fprintf(stderr, "[%s:%d] " fmt "\n", __func__, __LINE__, ##__VA_ARGS__)
#else
#define LOG(fmt, ...)
#endif
```

## Common Mistakes

1. **Not handling partial reads/writes**
2. **Race conditions in thread pool**
3. **Memory leaks (especially on error paths)**
4. **Not checking return values**
5. **Hardcoding buffer sizes**
6. **Forgetting to close file descriptors**

## Testing Strategy

1. **Unit test components** before integration
2. **Test edge cases:**
   - Empty files
   - Large files
   - Many concurrent clients
   - Network errors
3. **Use automated tests** when possible
4. **Test on Gradescope environment** (Ubuntu)

## Time Estimates

| Project | Reported Hours |
|---------|---------------|
| P1 Part 1 | 10-15 hours |
| P1 Part 2 | 15-20 hours |
| P2 | 15-25 hours |
| P3 | 20-30 hours |
| P4 | 20-30 hours |

Start early!
