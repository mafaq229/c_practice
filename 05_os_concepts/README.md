# Project 5: OS Concepts Study Guide

## Overview

This study guide covers Operating Systems concepts tested in GIOS exams. It maps OSTEP book chapters to GIOS course content and provides summaries, key points, and practice questions.

**When to Use:** Throughout the course, especially before exams
**Primary Resource:** [OSTEP - Operating Systems: Three Easy Pieces](https://pages.cs.wisc.edu/~remzi/OSTEP/)

## GIOS Exam Topics

Based on course reviews, GIOS exams focus on:

1. **Processes and Threads** (heavily tested)
2. **Synchronization** (mutexes, semaphores, monitors)
3. **Scheduling Algorithms** (formulas!)
4. **Memory Management** (paging, TLBs)
5. **File Systems** (structure, caching)
6. **I/O and Distributed Systems**

## OSTEP Reading Guide

### Part 1: Virtualization (Weeks 1-4)

| Week | Chapters | Topics |
|------|----------|--------|
| 1 | 4-6 | Processes, Process API, Limited Direct Execution |
| 2 | 7-11 | CPU Scheduling (FIFO, SJF, RR, MLFQ) |
| 3 | 13-16 | Address Spaces, Segmentation |
| 4 | 18-23 | Paging, TLBs, Swapping |

### Part 2: Concurrency (Weeks 5-8)

| Week | Chapters | Topics |
|------|----------|--------|
| 5 | 26-27 | Threads, Thread API |
| 6 | 28-29 | Locks, Lock-based Data Structures |
| 7 | 30-31 | Condition Variables, Semaphores |
| 8 | 32-33 | Concurrency Bugs, Event-based Programming |

### Part 3: Persistence (Weeks 9-12)

| Week | Chapters | Topics |
|------|----------|--------|
| 9 | 36-37 | I/O Devices, Hard Disk Drives |
| 10 | 39-40 | Files and Directories, File System Implementation |
| 11 | 42-43 | FSCK, Journaling |
| 12 | 44, 48-50 | Flash, Distributed Systems |

## Study Materials

### 01_virtualization/
- `processes.md` - Processes and Process API
- `scheduling.md` - CPU Scheduling Algorithms (with formulas)
- `memory.md` - Memory Management, Paging, TLBs
- `quiz.md` - Practice questions

### 02_concurrency/
- `threads.md` - Threads and Thread API
- `locks.md` - Locks, Mutexes, Spinlocks
- `condition_vars.md` - Condition Variables, Semaphores
- `quiz.md` - Practice questions

### 03_persistence/
- `io_devices.md` - I/O and Device Drivers
- `file_systems.md` - File System Implementation
- `quiz.md` - Practice questions

## Key Formulas

### Scheduling

**Turnaround Time:**
```
T_turnaround = T_completion - T_arrival
```

**Response Time:**
```
T_response = T_first_run - T_arrival
```

**CPU Utilization:**
```
Utilization = (Time_CPU_busy / Time_total) × 100%
```

### Memory

**Page Table Size:**
```
Page_table_size = (Virtual_address_space / Page_size) × Entry_size
```

**TLB Hit Rate:**
```
Effective_access_time = Hit_rate × TLB_time + (1 - Hit_rate) × (TLB_time + Memory_time)
```

### Disk

**Disk Access Time:**
```
T_access = T_seek + T_rotation + T_transfer
```

**Average Rotation Time:**
```
T_rotation_avg = (1 / RPM) × 60 × 0.5 seconds
```

## Exam Tips

1. **Know the formulas** - Scheduling and memory questions are calculation-heavy
2. **Understand trade-offs** - Why choose one algorithm over another?
3. **Practice code reading** - Expect pseudocode for synchronization
4. **Draw diagrams** - Memory layouts, page tables, process states
5. **Review lectures** - Exams heavily based on lecture content

## Resources

- [OSTEP Free Online](https://pages.cs.wisc.edu/~remzi/OSTEP/)
- [Course Notes](https://coursedocs.readthedocs.io/en/latest/gatech/cs6200-O01/)
- [OMSCentral Reviews](https://www.omscentral.com/courses/graduate-introduction-to-operating-systems/reviews)
