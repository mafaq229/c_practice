# Capstone Project: Mini-GIOS

## The Ultimate GIOS Preparation Challenge

This capstone project combines **everything** you've learned into a single, comprehensive system that mirrors the actual GIOS projects. By completing this, you'll have hands-on experience with all the core concepts before the course begins.

## Project Overview

You will build a **Distributed Caching File Server** - a system with multiple components that work together to serve files efficiently.

```
┌─────────────────────────────────────────────────────────────────┐
│                         ARCHITECTURE                             │
├─────────────────────────────────────────────────────────────────┤
│                                                                   │
│   ┌──────────┐       ┌──────────┐       ┌──────────────────┐   │
│   │  Client  │──────▶│  Proxy   │──────▶│   File Server    │   │
│   │          │◀──────│          │◀──────│   (Threaded)     │   │
│   └──────────┘       └────┬─────┘       └──────────────────┘   │
│                           │                                      │
│                           │ IPC                                  │
│                           ▼                                      │
│                      ┌──────────┐                                │
│                      │  Cache   │                                │
│                      │ Process  │                                │
│                      └──────────┘                                │
│                                                                   │
└─────────────────────────────────────────────────────────────────┘
```

## Skills Covered

| Part | Skills | Maps to GIOS Project |
|------|--------|---------------------|
| Part A | Sockets, Protocol, File I/O | Project 1 Part 1 |
| Part B | Threading, Thread Pool | Project 1 Part 2 |
| Part C | Proxy Pattern, Caching | Project 2 |
| Part D | Shared Memory, IPC | Project 3 |
| Part E | Statistics, Monitoring | Bonus |

## Estimated Time
- Part A: 4-6 hours
- Part B: 4-6 hours
- Part C: 4-6 hours
- Part D: 4-6 hours
- Part E: 2-3 hours
- **Total: 18-27 hours**

---

## Part A: Single-Threaded File Server

### Objective
Build a TCP file server that handles one client at a time.

### Protocol Specification

**Request Format:**
```
GETFILE GET <path>\r\n\r\n
```

**Response Format (Success):**
```
GETFILE OK <length>\r\n\r\n<file data>
```

**Response Format (Error):**
```
GETFILE FILE_NOT_FOUND\r\n\r\n
GETFILE ERROR <message>\r\n\r\n
GETFILE INVALID\r\n\r\n
```

### Requirements
1. Parse the GETFILE protocol correctly
2. Handle partial sends/receives
3. Serve files from a configurable root directory
4. Return appropriate error responses
5. Handle EINTR from system calls
6. Validate paths (prevent directory traversal with ..)

### Files to Implement
- `src/server_single.c` - Single-threaded server
- `include/protocol.h` - Protocol definitions
- `src/protocol.c` - Protocol parsing functions
- `src/file_utils.c` - File handling utilities

### Testing
```bash
# Terminal 1: Start server
./server_single 8080 ./test_files

# Terminal 2: Test with client
./client localhost 8080 /small.txt

# Or with netcat
echo -e "GETFILE GET /small.txt\r\n\r\n" | nc localhost 8080
```

### Hints
- Start by getting protocol parsing working
- Test with small files first
- Use `send_all()` for reliable sending
- Check all return values!

---

## Part B: Multi-Threaded File Server

### Objective
Add a boss-worker thread pool to handle multiple concurrent clients.

### Requirements
1. Implement a work queue (thread-safe)
2. Implement a thread pool (N worker threads)
3. Boss thread accepts connections, adds to queue
4. Worker threads process requests from queue
5. Graceful shutdown on SIGINT
6. Handle at least 100 concurrent connections

### Files to Implement
- `src/server_mt.c` - Multi-threaded server
- `include/thread_pool.h` - Thread pool interface
- `src/thread_pool.c` - Thread pool implementation
- `include/work_queue.h` - Work queue interface
- `src/work_queue.c` - Work queue implementation

### Testing
```bash
# Start server with 8 worker threads
./server_mt 8080 ./test_files 8

# Run concurrent clients
./test_concurrent.sh localhost 8080 50
```

### Hints
- Get Part A fully working first
- Use condition variables for the work queue
- Remember: `pthread_cond_wait` needs a while loop!
- Test shutdown thoroughly

---

## Part C: Caching Proxy

### Objective
Build a proxy that sits between clients and the file server, caching responses.

### Architecture
```
Client ──▶ Proxy ──▶ File Server
              │
              ▼
           Cache
```

### Requirements
1. Accept client connections
2. Forward requests to backend server
3. Cache responses in memory
4. Return cached responses for repeated requests
5. Implement LRU eviction when cache is full
6. Cache has configurable size limit
7. Track hit/miss statistics

### Files to Implement
- `src/proxy.c` - Proxy server
- `include/cache.h` - Cache interface
- `src/cache.c` - LRU cache implementation

### Cache Interface
```c
typedef struct Cache Cache;

Cache *cache_create(size_t max_size);
void cache_destroy(Cache *cache);

// Returns cached data or NULL if not found
CacheEntry *cache_get(Cache *cache, const char *path);

// Stores data in cache, evicts if necessary
int cache_put(Cache *cache, const char *path, const void *data, size_t size);

// Get statistics
void cache_stats(Cache *cache, size_t *hits, size_t *misses);
```

### Testing
```bash
# Terminal 1: Start file server
./server_mt 8081 ./test_files 4

# Terminal 2: Start proxy
./proxy 8080 localhost 8081 1048576  # 1MB cache

# Terminal 3: Test
./client localhost 8080 /large.bin   # Cache miss
./client localhost 8080 /large.bin   # Cache hit!
```

### Hints
- Implement cache as a hash table with LRU list
- Be careful about memory management
- Track total cached bytes for eviction decisions

---

## Part D: IPC Cache Process

### Objective
Move the cache to a separate process, communicating via shared memory and message queues.

### Architecture
```
Client ──▶ Proxy ◀──IPC──▶ Cache Process
              │
              ▼
         File Server
```

### Requirements
1. Cache runs as a separate process
2. Proxy communicates with cache via POSIX shared memory
3. Use message queues for cache lookup/store commands
4. Shared memory holds cached file data
5. Handle cache process crash gracefully
6. Proper cleanup of IPC resources

### IPC Design
```
Message Queue: Proxy → Cache (requests)
  - LOOKUP: Check if file is cached
  - STORE: Cache a file
  - STATS: Get cache statistics

Message Queue: Cache → Proxy (responses)
  - FOUND: File is cached, data in shared memory at offset X
  - NOT_FOUND: File not cached
  - STORED: File cached successfully
  - STATS_RESPONSE: Hit/miss counts

Shared Memory: File data storage
  - Fixed-size segments
  - Managed by cache process
```

### Files to Implement
- `src/cache_process.c` - Standalone cache process
- `src/proxy_ipc.c` - Proxy with IPC
- `include/ipc_protocol.h` - IPC message definitions
- `src/shm_manager.c` - Shared memory management

### Testing
```bash
# NOTE: Requires Linux (use Docker on macOS)

# Terminal 1: Start file server
./server_mt 8081 ./test_files 4

# Terminal 2: Start cache process
./cache_process /gios_cache 10485760  # 10MB cache

# Terminal 3: Start proxy
./proxy_ipc 8080 localhost 8081 /gios_cache

# Terminal 4: Test
./client localhost 8080 /large.bin
```

### Hints
- Start with message queues only (no shared memory)
- Add shared memory for actual data transfer
- Use semaphores to coordinate access
- Test cleanup on all exit paths

---

## Part E: Monitoring & Statistics

### Objective
Add comprehensive monitoring and statistics to the system.

### Requirements
1. Real-time statistics endpoint
2. Log all requests with timing
3. Monitor thread pool utilization
4. Track cache efficiency over time
5. Graceful degradation under load

### Statistics to Track
- Requests per second
- Average response time
- Cache hit rate
- Active connections
- Thread pool queue depth
- Memory usage

### Optional: gRPC Stats Client
If you completed Project 4, implement a gRPC service for statistics:

```protobuf
service MonitoringService {
    rpc GetStats(StatsRequest) returns (StatsResponse);
    rpc StreamStats(StatsRequest) returns (stream StatsResponse);
}
```

---

## Building the Project

```bash
cd capstone_project

# Build all components
make all

# Build specific parts
make part_a    # Single-threaded server + client
make part_b    # Multi-threaded server
make part_c    # Proxy with in-process cache
make part_d    # IPC-based cache (Linux/Docker only)
make part_e    # Monitoring

# Run tests
make test

# Clean
make clean
```

## Project Structure

```
capstone_project/
├── README.md                 # This file
├── Makefile                  # Build system
├── include/
│   ├── protocol.h            # GETFILE protocol
│   ├── file_utils.h          # File operations
│   ├── socket_utils.h        # Socket helpers
│   ├── thread_pool.h         # Thread pool
│   ├── work_queue.h          # Work queue
│   ├── cache.h               # In-process cache
│   ├── ipc_protocol.h        # IPC messages
│   └── shm_manager.h         # Shared memory
├── src/
│   ├── server_single.c       # Part A: Single-threaded server
│   ├── server_mt.c           # Part B: Multi-threaded server
│   ├── proxy.c               # Part C: Caching proxy
│   ├── proxy_ipc.c           # Part D: IPC proxy
│   ├── cache_process.c       # Part D: Cache process
│   ├── client.c              # Test client
│   ├── protocol.c            # Protocol implementation
│   ├── file_utils.c          # File utilities
│   ├── socket_utils.c        # Socket utilities
│   ├── thread_pool.c         # Thread pool
│   ├── work_queue.c          # Work queue
│   ├── cache.c               # LRU cache
│   ├── shm_manager.c         # Shared memory
│   └── monitor.c             # Part E: Monitoring
├── tests/
│   ├── test_protocol.c       # Protocol unit tests
│   ├── test_cache.c          # Cache unit tests
│   ├── test_thread_pool.c    # Thread pool tests
│   └── test_concurrent.sh    # Concurrent client test
└── test_files/
    ├── small.txt             # Small test file
    ├── medium.bin            # Medium test file
    └── large.bin             # Large test file
```

## Grading Yourself

### Part A (25 points)
- [ ] Protocol parsing correct (5)
- [ ] Files served correctly (5)
- [ ] Error responses correct (5)
- [ ] Handles partial sends (5)
- [ ] Path validation (5)

### Part B (25 points)
- [ ] Thread pool creates workers (5)
- [ ] Work queue is thread-safe (5)
- [ ] Handles concurrent clients (5)
- [ ] Graceful shutdown (5)
- [ ] No race conditions (5)

### Part C (25 points)
- [ ] Proxy forwards requests (5)
- [ ] Cache stores responses (5)
- [ ] Cache returns hits (5)
- [ ] LRU eviction works (5)
- [ ] Statistics accurate (5)

### Part D (25 points)
- [ ] Cache runs as process (5)
- [ ] Message queues work (5)
- [ ] Shared memory for data (5)
- [ ] Proper synchronization (5)
- [ ] Clean resource cleanup (5)

---

## Tips for Success

1. **Build incrementally** - Get each part working before moving on
2. **Test constantly** - Write tests for each component
3. **Use Valgrind** - Check for memory leaks
4. **Add logging** - Debug output is essential
5. **Handle errors** - Check every return value
6. **Read man pages** - They're your best friend

## Resources

- OSTEP Chapters: 26-33 (Concurrency)
- Beej's Network Guide
- man pages: socket, pthread, shm_open, mq_open

## What You'll Have Learned

By completing this capstone, you will have:

1. ✅ Built a production-quality file server
2. ✅ Implemented a thread pool from scratch
3. ✅ Designed and parsed a network protocol
4. ✅ Built a caching proxy
5. ✅ Used POSIX shared memory
6. ✅ Used POSIX message queues
7. ✅ Coordinated multiple processes
8. ✅ Debugged concurrent code

**You'll be ready for GIOS!**
