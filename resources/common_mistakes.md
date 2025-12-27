# Common Mistakes to Avoid

## C Programming Mistakes

### 1. Not Checking Return Values
```c
// WRONG
file = fopen("data.txt", "r");
fread(buffer, 1, size, file);  // file could be NULL!

// RIGHT
file = fopen("data.txt", "r");
if (file == NULL) {
    perror("fopen");
    return -1;
}
```

### 2. Buffer Overflows
```c
// WRONG
char buffer[10];
strcpy(buffer, user_input);  // No bounds checking!

// RIGHT
char buffer[10];
strncpy(buffer, user_input, sizeof(buffer) - 1);
buffer[sizeof(buffer) - 1] = '\0';
```

### 3. Memory Leaks
```c
// WRONG
char *str = malloc(100);
if (error_condition) {
    return -1;  // Memory leak!
}

// RIGHT
char *str = malloc(100);
if (error_condition) {
    free(str);
    return -1;
}
```

### 4. Use After Free
```c
// WRONG
free(ptr);
printf("%d\n", *ptr);  // Undefined behavior!

// RIGHT
free(ptr);
ptr = NULL;  // Prevent accidental use
```

### 5. Off-by-One Errors
```c
// WRONG
for (int i = 0; i <= array_size; i++) {  // <= should be <
    array[i] = 0;
}

// RIGHT
for (int i = 0; i < array_size; i++) {
    array[i] = 0;
}
```

### 6. Integer Overflow
```c
// WRONG
int size = width * height;  // Could overflow!

// RIGHT
if (width > INT_MAX / height) {
    // Handle overflow
}
int size = width * height;
```

## Socket Programming Mistakes

### 7. Not Handling Partial Reads/Writes
```c
// WRONG
read(fd, buffer, size);  // May read less than size!

// RIGHT
size_t total = 0;
while (total < size) {
    ssize_t n = read(fd, buffer + total, size - total);
    if (n <= 0) break;
    total += n;
}
```

### 8. Not Handling EINTR
```c
// WRONG
n = recv(fd, buf, size, 0);
if (n < 0) return -1;  // Could be EINTR!

// RIGHT
do {
    n = recv(fd, buf, size, 0);
} while (n < 0 && errno == EINTR);
```

### 9. Forgetting htons/ntohs
```c
// WRONG
addr.sin_port = 8080;  // Wrong byte order on most systems!

// RIGHT
addr.sin_port = htons(8080);
```

### 10. Not Closing File Descriptors
```c
// WRONG
int fd = open("file.txt", O_RDONLY);
// ... use fd ...
// Forgot to close!

// RIGHT
int fd = open("file.txt", O_RDONLY);
// ... use fd ...
close(fd);
```

## Threading Mistakes

### 11. Race Conditions
```c
// WRONG
counter++;  // Not atomic!

// RIGHT
pthread_mutex_lock(&mutex);
counter++;
pthread_mutex_unlock(&mutex);
```

### 12. Passing Stack Variables to Threads
```c
// WRONG
for (int i = 0; i < n; i++) {
    pthread_create(&threads[i], NULL, func, &i);  // All threads see same i!
}

// RIGHT
for (int i = 0; i < n; i++) {
    int *arg = malloc(sizeof(int));
    *arg = i;
    pthread_create(&threads[i], NULL, func, arg);
}
```

### 13. Not Joining Threads
```c
// WRONG
pthread_create(&thread, NULL, func, NULL);
// Main exits, thread may not finish!

// RIGHT
pthread_create(&thread, NULL, func, NULL);
pthread_join(thread, NULL);
```

### 14. Deadlock from Lock Ordering
```c
// WRONG - Deadlock possible!
// Thread A: lock(A), lock(B)
// Thread B: lock(B), lock(A)

// RIGHT - Always lock in same order
// Thread A: lock(A), lock(B)
// Thread B: lock(A), lock(B)
```

### 15. Using if Instead of while with Condition Variables
```c
// WRONG - Spurious wakeup!
if (!ready) {
    pthread_cond_wait(&cond, &mutex);
}

// RIGHT
while (!ready) {
    pthread_cond_wait(&cond, &mutex);
}
```

## Memory Management Mistakes

### 16. Double Free
```c
// WRONG
free(ptr);
free(ptr);  // Undefined behavior!

// RIGHT
free(ptr);
ptr = NULL;
```

### 17. Wrong sizeof
```c
// WRONG
int *arr = malloc(10);  // Only 10 bytes, not 10 ints!

// RIGHT
int *arr = malloc(10 * sizeof(int));
// Or better:
int *arr = malloc(10 * sizeof(*arr));
```

### 18. Forgetting Null Terminator
```c
// WRONG
char str[5];
strncpy(str, "hello", 5);  // No null terminator!

// RIGHT
char str[6];
strncpy(str, "hello", 5);
str[5] = '\0';
```

## Project-Specific Mistakes

### 19. Not Following Protocol Exactly
```
// WRONG - Extra space
"GET  /path\r\n"

// RIGHT - Exact format
"GET /path\r\n\r\n"
```

### 20. Assuming Buffer Sizes
```c
// WRONG
char path[100];  // What if path is longer?

// RIGHT
#define MAX_PATH 4096
char path[MAX_PATH];
// Or use dynamic allocation
```

## Debugging Tips

1. **Use Valgrind:** `valgrind --leak-check=full ./program`
2. **Use Address Sanitizer:** `gcc -fsanitize=address`
3. **Print debug info:** `fprintf(stderr, "DEBUG: x = %d\n", x);`
4. **Use GDB:** Set breakpoints, inspect variables
5. **Check errno:** `perror("error message");`
