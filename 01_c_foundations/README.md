# Project 1: C Programming Foundations

**Estimated Time:** 12-15 hours
**Priority:** Critical - Complete before starting other projects

This project builds your C programming fundamentals, which are essential for all GIOS projects.

## Learning Objectives

After completing this project, you will be able to:
- Write, compile, and debug C programs
- Understand and manipulate pointers (including double pointers)
- Manage dynamic memory safely (malloc, free, no leaks)
- Work with C strings and structs
- Write and understand Makefiles
- Debug programs using GDB/LLDB

## Modules

| Module | Topic | Time | Files |
|--------|-------|------|-------|
| 01 | [Environment Setup](01_environment/) | 1 hr | test_setup.c |
| 02 | [C Basics & Compilation](02_basics/) | 2 hrs | exercises.c |
| 03 | [Pointers](03_pointers/) | 3 hrs | pointer_basics.c, double_pointers.c, pointer_bugs.c |
| 04 | [Memory Management](04_memory/) | 3 hrs | dynamic_arrays.c, memory_leaks.c, custom_allocator.c |
| 05 | [Strings & Data Structures](05_strings/) | 2 hrs | string_utils.c, string_parsing.c, buffer_handling.c |
| 06 | [Data Structures](06_data_structures/) | 2 hrs | linked_list.c, hash_table.c, dynamic_buffer.c |
| 07 | [Makefiles](07_makefiles/) | 1 hr | Makefile exercises |
| 08 | [GDB Debugging](08_gdb/) | 2 hrs | buggy_program.c, segfault_examples.c |

## How to Work Through This Project

### Step 1: Verify Environment
```bash
cd 01_environment
make test
```

### Step 2: Work Through Modules in Order
Each module builds on the previous one. Within each file:
1. Read the instructions at the top
2. Complete all TODO items
3. Compile and test your solution
4. Check the solution only after attempting

### Step 3: Build and Test
```bash
# Build all exercises in this project
make

# Run tests
make test

# Clean build artifacts
make clean
```

## Quick Reference

### Compilation
```bash
# Basic compilation
clang -Wall -Wextra -g file.c -o program

# With debug symbols (for GDB/LLDB)
clang -g file.c -o program

# Multiple files
clang -Wall -g main.c helpers.c -o program
```

### Common GCC/Clang Flags
| Flag | Purpose |
|------|---------|
| `-Wall` | Enable common warnings |
| `-Wextra` | Enable extra warnings |
| `-g` | Include debug symbols |
| `-O2` | Optimization level 2 |
| `-std=c11` | Use C11 standard |

### Debugging (LLDB on macOS)
```bash
lldb ./program
(lldb) breakpoint set --name main  # Set breakpoint
(lldb) run                          # Run program
(lldb) next                         # Step over
(lldb) step                         # Step into
(lldb) print variable              # Print value
(lldb) thread backtrace            # Show call stack
(lldb) quit                        # Exit
```

## Key Concepts to Master

### Pointers
```c
int x = 10;
int *ptr = &x;      // ptr holds address of x
int val = *ptr;     // dereference: val = 10
*ptr = 20;          // x is now 20

// Double pointer (used in GIOS Project 3)
int **dptr = &ptr;
**dptr = 30;        // x is now 30
```

### Memory Management
```c
// Allocate
int *arr = malloc(10 * sizeof(int));
if (arr == NULL) {
    // Handle error
}

// Use the memory
arr[0] = 42;

// Always free when done
free(arr);
arr = NULL;  // Good practice
```

### Strings in C
```c
char str[] = "Hello";     // Array, can modify
char *ptr = "Hello";      // Pointer to literal, don't modify!

// Safe copy
char dest[100];
strncpy(dest, source, sizeof(dest) - 1);
dest[sizeof(dest) - 1] = '\0';  // Ensure null-terminated
```

## Common Mistakes to Avoid

1. **Not checking malloc return value**
   ```c
   int *p = malloc(sizeof(int));
   if (p == NULL) { /* handle error */ }
   ```

2. **Memory leaks**
   ```c
   // BAD: Leak!
   ptr = malloc(100);
   ptr = malloc(200);  // First allocation leaked!

   // GOOD:
   free(ptr);
   ptr = malloc(200);
   ```

3. **Buffer overflow**
   ```c
   char buf[10];
   strcpy(buf, "This string is way too long!");  // BAD!
   strncpy(buf, src, sizeof(buf) - 1);           // Better
   ```

4. **Using freed memory**
   ```c
   free(ptr);
   *ptr = 10;  // UNDEFINED BEHAVIOR!
   ```

5. **Off-by-one errors**
   ```c
   for (int i = 0; i <= n; i++)  // BAD if array size is n
   for (int i = 0; i < n; i++)   // GOOD
   ```

## Recommended Reading

Before or alongside this project, read:
- OSTEP Chapter 13: Address Spaces
- OSTEP Chapter 14: Memory API
- [Beej's Guide to C](https://beej.us/guide/bgc/) - Chapters 1-12

## Next Steps

After completing this project, proceed to:
→ [Project 2: Systems Programming & Networking](../02_systems_networking/)
