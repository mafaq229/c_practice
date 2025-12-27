# Project 3: Concurrency & IPC

## Overview

This project teaches multithreading, synchronization, and inter-process communication - the foundation for GIOS Project 1 Part 2 (multithreaded file transfer server).

**Estimated Time:** 12-15 hours
**Difficulty:** Hard
**Prerequisites:** Complete Project 1 (C Foundations) and Project 2 (Systems & Networking)

## Why This Matters for GIOS

GIOS Project 1 Part 2 requires you to:
- Add multithreading to your file transfer server
- Implement a **boss-worker thread pool pattern**
- Use **mutexes** to protect shared data
- Use **condition variables** for thread coordination
- Handle concurrent client connections efficiently

GIOS Project 3 requires:
- POSIX shared memory (`shm_open`, `mmap`)
- Proper synchronization between processes
- Understanding of memory-mapped files

## Module Overview

| Module | Topic | Time | Key Concepts |
|--------|-------|------|--------------|
| 01 | Process Basics | 1 hr | fork(), exec(), wait() |
| 02 | Threads Basics | 3 hrs | pthread_create/join, thread arguments |
| 03 | Mutex Locks | 2 hrs | Race conditions, critical sections |
| 04 | Condition Variables | 3 hrs | Producer-consumer, signaling |
| 05 | Thread Pool | 2 hrs | Boss-worker pattern (GIOS P1 Part 2) |
| 06 | Shared Memory | 2 hrs | shm_open, mmap (requires Docker) |
| 07 | Message Queues | 2 hrs | mq_open, mq_send/receive (requires Docker) |

## Building

```bash
# Build all exercises
make

# Build specific module
make processes
make threads
make mutex
make condvars
make threadpool
make shm      # Requires Docker
make mq       # Requires Docker

# Clean
make clean
```

## Module Details

### Module 01: Process Basics (1 hour)

Understanding processes before threads.

**Files:**
- `01_processes/fork_basics.c` - fork() and process creation
- `01_processes/exec_example.c` - exec() family functions
- `01_processes/process_tree.c` - Creating process hierarchies

**Key Concepts:**
- `fork()` creates a copy of the process
- Child gets PID 0 from fork(), parent gets child's PID
- `exec()` replaces process image with new program
- `wait()` / `waitpid()` collect child exit status
- Zombie processes: terminated but not waited for
- Orphan processes: parent terminated first

### Module 02: Threads Basics (3 hours)

POSIX threads (pthreads) fundamentals.

**Files:**
- `02_threads_basics/thread_create.c` - Creating and joining threads
- `02_threads_basics/thread_args.c` - Passing arguments to threads
- `02_threads_basics/thread_return.c` - Getting return values from threads

**Key Concepts:**
- Threads share address space (unlike processes)
- `pthread_create()` starts a new thread
- `pthread_join()` waits for thread completion
- Thread function signature: `void *func(void *arg)`
- Cast carefully when passing/returning data

**Compile with:** `-pthread` flag

### Module 03: Mutex Locks (2 hours)

Protecting shared data from race conditions.

**Files:**
- `03_mutex/race_condition.c` - Demonstrating race conditions
- `03_mutex/mutex_fix.c` - Fixing races with mutexes
- `03_mutex/deadlock.c` - Identifying and avoiding deadlocks
- `03_mutex/counter_benchmark.c` - Performance implications

**Key Concepts:**
- Race condition: outcome depends on thread scheduling
- Critical section: code that accesses shared data
- Mutex: MUTual EXclusion lock
- `pthread_mutex_lock()` / `pthread_mutex_unlock()`
- Rule: Always unlock what you lock
- Deadlock: threads waiting for each other forever

### Module 04: Condition Variables (3 hours)

Coordinating threads - essential for thread pools!

**Files:**
- `04_condition_vars/producer_consumer.c` - Classic P-C problem
- `04_condition_vars/bounded_buffer.c` - Thread-safe bounded buffer
- `04_condition_vars/barrier.c` - Thread synchronization barrier

**Key Concepts:**
- Condition variable: wait for a condition to become true
- Always use with a mutex
- `pthread_cond_wait()`: atomically unlocks mutex and waits
- `pthread_cond_signal()`: wake one waiting thread
- `pthread_cond_broadcast()`: wake all waiting threads
- **Always use while loop, not if** (spurious wakeups!)

**Pattern:**
```c
pthread_mutex_lock(&mutex);
while (!condition) {
    pthread_cond_wait(&cond, &mutex);
}
// condition is true, do work
pthread_mutex_unlock(&mutex);
```

### Module 05: Thread Pool (2 hours)

Boss-worker pattern - exactly what GIOS Project 1 needs!

**Files:**
- `05_thread_pool/thread_pool.h` - Thread pool interface
- `05_thread_pool/thread_pool.c` - Thread pool implementation
- `05_thread_pool/work_queue.h` - Work queue interface
- `05_thread_pool/work_queue.c` - Work queue implementation
- `05_thread_pool/pool_test.c` - Test harness

**Key Concepts:**
- Boss thread: accepts work, adds to queue
- Worker threads: pull work from queue, execute
- Work queue: thread-safe queue of tasks
- Graceful shutdown: signal workers, join all threads

**GIOS Connection:**
- Main thread accepts client connections (boss)
- Worker threads handle file transfers
- Work queue holds pending requests
- This is exactly the pattern for Project 1 Part 2!

### Module 06: Shared Memory (2 hours)

Inter-process communication via shared memory.

**Note:** Requires Docker/Linux - POSIX shared memory not available on macOS.

**Files:**
- `06_shared_memory/shm_writer.c` - Write to shared memory
- `06_shared_memory/shm_reader.c` - Read from shared memory
- `06_shared_memory/shm_sync.c` - Synchronized shared memory access

**Key Concepts:**
- `shm_open()`: create/open shared memory object
- `ftruncate()`: set size of shared memory
- `mmap()`: map shared memory into address space
- `munmap()`: unmap when done
- `shm_unlink()`: remove shared memory object
- Must synchronize access (semaphores or mutexes)

**GIOS Connection:**
- Project 3 uses shared memory for cache
- Proxy and cache processes share memory segment
- Critical for high-performance IPC

### Module 07: Message Queues (2 hours)

Inter-process communication via message passing.

**Note:** Requires Docker/Linux - POSIX message queues not available on macOS.

**Files:**
- `07_message_queues/mq_sender.c` - Send messages
- `07_message_queues/mq_receiver.c` - Receive messages
- `07_message_queues/mq_priority.c` - Priority message queues

**Key Concepts:**
- `mq_open()`: create/open message queue
- `mq_send()`: send a message
- `mq_receive()`: receive a message (blocks by default)
- `mq_close()`: close queue descriptor
- `mq_unlink()`: remove queue
- Messages have priority - higher priority delivered first

## Common Pitfalls

### 1. Forgetting -pthread flag
```bash
# Wrong
gcc program.c -o program

# Right
gcc -pthread program.c -o program
```

### 2. Race conditions in increment
```c
// WRONG - race condition
counter++;

// RIGHT - protected
pthread_mutex_lock(&mutex);
counter++;
pthread_mutex_unlock(&mutex);
```

### 3. Using if instead of while with condition variables
```c
// WRONG - spurious wakeup can cause bugs
if (!ready) {
    pthread_cond_wait(&cond, &mutex);
}

// RIGHT - re-check condition after wakeup
while (!ready) {
    pthread_cond_wait(&cond, &mutex);
}
```

### 4. Deadlock from lock ordering
```c
// Thread 1: lock A, then B
// Thread 2: lock B, then A
// = DEADLOCK

// Solution: Always lock in same order
// Thread 1: lock A, then B
// Thread 2: lock A, then B
```

### 5. Not handling thread creation failure
```c
int ret = pthread_create(&tid, NULL, func, arg);
if (ret != 0) {
    fprintf(stderr, "pthread_create failed: %s\n", strerror(ret));
    // handle error
}
```

## Testing with Docker

For shared memory and message queues:

```bash
# Build Docker image (from project root)
cd ..
docker-compose build

# Run exercises in Docker
docker-compose run --rm dev bash
cd 03_concurrency_ipc
make shm
./06_shared_memory/shm_writer &
./06_shared_memory/shm_reader
```

## Debugging Threaded Programs

Threading bugs are notoriously hard to debug. Tips:

1. **Use Thread Sanitizer** (via Docker):
```bash
gcc -fsanitize=thread -g program.c -o program -pthread
./program
```

2. **Add logging with thread IDs:**
```c
printf("[Thread %lu] doing something\n", pthread_self());
```

3. **Use Helgrind** (Valgrind tool):
```bash
valgrind --tool=helgrind ./program
```

4. **Reduce thread count for debugging:**
Start with 2 threads, increase only when working.

## Progress Checklist

- [ ] Module 01: fork(), exec(), wait()
- [ ] Module 02: pthread_create(), pthread_join()
- [ ] Module 03: Mutex locks and race conditions
- [ ] Module 04: Condition variables
- [ ] Module 05: Thread pool implementation
- [ ] Module 06: Shared memory (Docker)
- [ ] Module 07: Message queues (Docker)

## What's Next?

After completing this project:
1. **Review** the thread pool pattern - you'll use it in GIOS
2. **Practice** debugging race conditions
3. **Continue** to Project 4 (C++ & gRPC) or Project 5 (OS Concepts)
4. **Start GIOS!** You're ready for Projects 1-3

## Resources

- [OSTEP: Threads](https://pages.cs.wisc.edu/~remzi/OSTEP/threads-intro.pdf)
- [OSTEP: Locks](https://pages.cs.wisc.edu/~remzi/OSTEP/threads-locks.pdf)
- [OSTEP: Condition Variables](https://pages.cs.wisc.edu/~remzi/OSTEP/threads-cv.pdf)
- [Pthreads Tutorial](https://computing.llnl.gov/tutorials/pthreads/)
- [POSIX Shared Memory](https://man7.org/linux/man-pages/man7/shm_overview.7.html)
