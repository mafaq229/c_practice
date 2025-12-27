# Recommended Reading and Resources

## Essential (Free Online)

### Operating Systems: Three Easy Pieces (OSTEP)
**URL:** https://pages.cs.wisc.edu/~remzi/OSTEP/
**Why:** Primary textbook for GIOS, covers all exam topics
**Read:** Chapters 4-50 (virtualization, concurrency, persistence)

### Beej's Guide to Network Programming
**URL:** https://beej.us/guide/bgnet/
**Why:** Best introduction to socket programming
**Read:** Before Project 1

### Beej's Guide to C Programming
**URL:** https://beej.us/guide/bgc/
**Why:** C refresher with good examples
**Read:** Before the course

## Recommended Books

### The Linux Programming Interface
**Author:** Michael Kerrisk
**Why:** Comprehensive reference for Linux system calls
**Use:** Reference during projects

### Advanced Programming in the UNIX Environment
**Authors:** Stevens, Rago
**Why:** Classic systems programming reference
**Use:** Deep dives into specific topics

### C Programming: A Modern Approach
**Author:** K.N. King
**Why:** Thorough C coverage
**Use:** If you need more C practice

### Modern C++ Design
**Author:** Andrei Alexandrescu
**Why:** Advanced C++ patterns
**Use:** Before Project 4 (optional)

## Online Tutorials

### C
- Learn C the Hard Way: https://learncodethehardway.org/c/
- C Tutorial: https://www.cprogramming.com/tutorial/c-tutorial.html

### Makefiles
- Makefile Tutorial: https://makefiletutorial.com/
- GNU Make Manual: https://www.gnu.org/software/make/manual/

### GDB
- GDB Tutorial: https://www.cs.cmu.edu/~gilpin/tutorial/
- GDB Cheat Sheet: https://darkdust.net/files/GDB%20Cheat%20Sheet.pdf

### gRPC
- gRPC C++ Basics: https://grpc.io/docs/languages/cpp/basics/
- Protocol Buffers Guide: https://protobuf.dev/

## GIOS-Specific Resources

### Course Notes
- https://coursedocs.readthedocs.io/en/latest/gatech/cs6200-O01/

### OMSCentral Reviews
- https://www.omscentral.com/courses/graduate-introduction-to-operating-systems/reviews

### Reddit OMSCS
- https://www.reddit.com/r/OMSCS/

## Video Resources

### MIT OpenCourseWare - Operating Systems
**URL:** https://ocw.mit.edu/courses/6-828-operating-system-engineering-fall-2012/
**Why:** Another perspective on OS concepts

### UC Berkeley CS162
**URL:** https://cs162.org/
**Why:** High-quality OS course materials

## Practice Platforms

### LeetCode
**URL:** https://leetcode.com/
**Use:** Practice C/C++ coding (not directly GIOS related)

### Exercism C Track
**URL:** https://exercism.org/tracks/c
**Use:** C practice with mentoring

## Quick References

### Man Pages
```bash
man 2 open    # System calls
man 3 printf  # Library functions
man 7 pthreads
man 7 shm_overview
```

### Online Man Pages
- https://man7.org/linux/man-pages/

### C Reference
- https://en.cppreference.com/w/c

### POSIX Reference
- https://pubs.opengroup.org/onlinepubs/9699919799/

## Reading Order Recommendation

### Week 1-2 (Before Course)
1. Beej's Guide to C (skim)
2. OSTEP Ch 4-6 (Processes)
3. Beej's Guide to Network Programming (Ch 1-6)

### Week 3 (Project 1 Start)
1. OSTEP Ch 26-27 (Threads)
2. OSTEP Ch 28-30 (Locks, Condition Variables)
3. Linux Programming Interface (socket chapters)

### Ongoing
1. OSTEP chapters as covered in lectures
2. Man pages as needed
3. Course notes for exam prep
