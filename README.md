# CS-6200 (GIOS) Pre-Requisite Preparation Curriculum

A comprehensive, project-based preparation curriculum for Georgia Tech's **CS-6200 Introduction to Operating Systems** course. Designed for someone with Python/ML background but limited C/systems experience.

## Quick Start

```bash
# 1. Verify your environment is set up
cd 01_c_foundations/01_environment
make test

# 2. Start with the first exercise
cd ../02_basics
# Open exercises.c and follow the TODO comments
```

## Curriculum Overview

| Project | Focus | Time | Priority |
|---------|-------|------|----------|
| [01_c_foundations](01_c_foundations/) | Pointers, memory, strings, Makefiles, GDB | ~12-15 hrs | **Critical** |
| [02_systems_networking](02_systems_networking/) | File I/O, sockets, protocol design | ~12-15 hrs | **Critical** |
| [03_concurrency_ipc](03_concurrency_ipc/) | Pthreads, mutexes, condition variables, IPC | ~12-15 hrs | **Critical** |
| [04_cpp_grpc](04_cpp_grpc/) | C++ basics, Protocol Buffers, gRPC | ~5 hrs | Can do during course |
| [05_os_concepts](05_os_concepts/) | OSTEP reading guide, exam prep | Ongoing | Background reading |

## How This Maps to GIOS Projects

| GIOS Project | What You Build | This Curriculum Prep |
|--------------|----------------|---------------------|
| **Project 1** | Socket-based file transfer with multithreading | Projects 1, 2, 3 |
| **Project 2** | Proxy/cache server with shared memory | Projects 1, 3 |
| **Project 3** | Distributed file system with gRPC | Projects 1, 4 |

## How to Use This Curriculum

### Step 1: Environment Setup
Before starting, complete the setup in [SETUP.md](SETUP.md).

### Step 2: Work Through Projects in Order
Each project builds on the previous one. Within each project:

1. **Read the README.md** in each project folder
2. **Work through modules sequentially** (numbered folders)
3. **Complete all TODO exercises** - they're marked clearly in the code
4. **Test your solutions** using the provided test cases
5. **Check solutions** only after attempting (in `solutions/` folders)

### Step 3: Use the OS Concepts Guide
Read the OSTEP chapters in parallel with your coding exercises. The study guide provides:
- Suggested reading order
- Key concepts summaries
- Practice quiz questions

## File Naming Convention

```
filename.c          # Exercise file - YOU work on this
filename_test.c     # Test file - run to verify your solution
solutions/filename.c # Reference solution - check after completing
```

## Building and Testing

From the root directory:
```bash
# Build everything
make all

# Build specific project
make project1

# Clean build artifacts
make clean

# Run all tests
make test
```

From individual project directories:
```bash
cd 01_c_foundations
make          # Build all exercises
make test     # Run tests
make clean    # Clean up
```

## TODO Comments Guide

Throughout the code, you'll find these markers:

```c
// TODO: [Description of what to implement]
// Your code here

// HINT: [Helpful hint for the exercise]

// EXPECTED OUTPUT:
// [What the output should look like]

// TEST: Run with `./test_filename` to verify
```

## Difficulty Levels

Exercises are marked with difficulty:
- `[EASY]` - Warmup, straightforward implementation
- `[MEDIUM]` - Requires thinking, may need to look up docs
- `[HARD]` - Challenging, similar to GIOS project complexity
- `[CHALLENGE]` - Optional, for deeper understanding

## Time Estimates

Each module has an estimated time. If you're spending significantly more:
1. Check if you're missing prerequisite knowledge
2. Look at the hints provided
3. Review the relevant reading material
4. Check the solution to understand the approach, then try again

## Troubleshooting

### Compilation Errors
- Read the error message carefully - GCC is usually specific
- Check for missing semicolons, braces, includes
- Verify function signatures match declarations

### Segmentation Faults
- Run with GDB: `gdb ./program`
- Use backtrace: `bt` to find where it crashed
- Check for NULL pointer dereferences
- Verify array bounds

### Memory Leaks (via Docker)
```bash
docker run --rm -v $(pwd):/code gios-prep valgrind ./program
```

## Resources

### Essential Reading
- [OSTEP Book](https://pages.cs.wisc.edu/~remzi/OSTEP/) - Free online textbook
- [Beej's Network Guide](https://beej.us/guide/bgnet/) - Socket programming bible
- [Beej's C Guide](https://beej.us/guide/bgc/) - C programming reference

### Quick References
- [resources/recommended_reading.md](resources/recommended_reading.md)
- [resources/common_mistakes.md](resources/common_mistakes.md)
- [resources/gios_project_tips.md](resources/gios_project_tips.md)

## Progress Tracking

Use this checklist to track your progress:

### Week 1: C Foundations
- [ ] Environment setup complete
- [ ] Basics & compilation exercises
- [ ] Pointer exercises (all 3 files)
- [ ] Memory management exercises
- [ ] String exercises
- [ ] Data structures (linked list, hash table)
- [ ] Makefile exercise
- [ ] GDB debugging exercises

### Week 2: Systems & Networking
- [ ] Linux commands practice
- [ ] File I/O exercises
- [ ] Socket basics (echo server/client)
- [ ] Protocol parser implementation
- [ ] File transfer system complete

### Week 3: Concurrency & IPC
- [ ] Process exercises (fork, exec)
- [ ] Thread creation exercises
- [ ] Mutex exercises
- [ ] Condition variable exercises
- [ ] Thread pool implementation
- [ ] Shared memory exercises (Docker)
- [ ] Message queue exercises (Docker)

### Supplementary (During Course)
- [ ] C++ basics
- [ ] Protocol Buffers
- [ ] gRPC key-value store

## Getting Help

1. **Stack Overflow** - for specific C/systems questions
2. **man pages** - `man function_name` for documentation
3. **GIOS Slack** - once you're enrolled in the course
4. **OMSCentral** - for course-specific advice

## License

This curriculum is designed for personal educational use in preparation for Georgia Tech's CS-6200 course.

---

**Good luck with your GIOS preparation! The course is challenging but rewarding.**
