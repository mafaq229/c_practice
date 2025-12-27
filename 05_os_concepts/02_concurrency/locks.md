# Locks and Synchronization

## Critical Section Problem

A **critical section** is code that accesses shared resources.
Requirements for a solution:
1. **Mutual Exclusion:** Only one thread in critical section
2. **Progress:** If no one in CS, someone waiting can enter
3. **Bounded Waiting:** No starvation

## Lock Types

### Spinlock
Busy-waits (spins) while waiting.

```c
typedef struct {
    int flag;
} spinlock_t;

void spin_lock(spinlock_t *lock) {
    while (test_and_set(&lock->flag, 1) == 1)
        ; // spin
}

void spin_unlock(spinlock_t *lock) {
    lock->flag = 0;
}
```

**Pros:** Fast if lock is held briefly
**Cons:** Wastes CPU cycles while spinning

### Mutex (Blocking Lock)
Puts thread to sleep while waiting.

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_lock(&mutex);
// critical section
pthread_mutex_unlock(&mutex);
```

**Pros:** Doesn't waste CPU
**Cons:** Context switch overhead

## Hardware Support

### Test-and-Set
Atomically sets value and returns old value.

```c
int test_and_set(int *ptr, int new) {
    int old = *ptr;
    *ptr = new;
    return old;
}
```

### Compare-and-Swap
Atomically compares and conditionally swaps.

```c
int compare_and_swap(int *ptr, int expected, int new) {
    int actual = *ptr;
    if (actual == expected)
        *ptr = new;
    return actual;
}
```

### Fetch-and-Add
Atomically increments and returns old value.

```c
int fetch_and_add(int *ptr) {
    int old = *ptr;
    *ptr = old + 1;
    return old;
}
```

## Common Synchronization Problems

### 1. Race Condition
Two threads access shared data, at least one writes.

```c
// Thread A           // Thread B
counter++;            counter++;

// counter++ is NOT atomic!
// 1. Load counter
// 2. Add 1
// 3. Store counter
```

### 2. Deadlock
Four conditions (Coffman):
1. Mutual exclusion
2. Hold and wait
3. No preemption
4. Circular wait

**Prevention:** Break one condition
- Lock ordering (break circular wait)
- Lock all at once (break hold and wait)
- Trylock with backoff

### 3. Priority Inversion
Low priority thread holds lock needed by high priority thread.

**Solution:** Priority inheritance - temporarily boost low priority thread.

## Semaphores

A semaphore is a counter with atomic operations.

```c
sem_wait(sem);   // Decrement, block if would go negative
sem_post(sem);   // Increment, wake waiting threads
```

### Binary Semaphore (Mutex)
```c
sem_t mutex;
sem_init(&mutex, 0, 1);  // Initialize to 1

sem_wait(&mutex);
// critical section
sem_post(&mutex);
```

### Counting Semaphore
```c
sem_t slots;
sem_init(&slots, 0, N);  // N available resources

sem_wait(&slots);  // Acquire one slot
// use resource
sem_post(&slots);  // Release slot
```

## Producer-Consumer with Semaphores

```c
sem_t empty;  // Initially N (buffer size)
sem_t full;   // Initially 0
sem_t mutex;  // Initially 1

// Producer
sem_wait(&empty);
sem_wait(&mutex);
// add item to buffer
sem_post(&mutex);
sem_post(&full);

// Consumer
sem_wait(&full);
sem_wait(&mutex);
// remove item from buffer
sem_post(&mutex);
sem_post(&empty);
```

## Reader-Writer Lock

Multiple readers OR one writer.

```c
// Reader
sem_wait(&mutex);
readers++;
if (readers == 1)
    sem_wait(&write_lock);
sem_post(&mutex);

// read data

sem_wait(&mutex);
readers--;
if (readers == 0)
    sem_post(&write_lock);
sem_post(&mutex);

// Writer
sem_wait(&write_lock);
// write data
sem_post(&write_lock);
```

## Practice Questions

### Q1: Why is test-and-set needed for spinlocks?
**A:** To atomically check if lock is free AND acquire it. Without atomicity, two threads could both see the lock as free and both acquire it.

### Q2: When are spinlocks better than mutexes?
**A:** When:
- Critical section is very short
- Few threads contend for lock
- On multiprocessor systems (spinning thread doesn't block others)

### Q3: How many semaphores for bounded buffer?
**A:** Three:
- `empty`: tracks empty slots
- `full`: tracks filled slots
- `mutex`: protects buffer access
