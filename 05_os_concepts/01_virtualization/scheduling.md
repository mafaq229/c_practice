# CPU Scheduling Algorithms

## Key Metrics

### Turnaround Time
Time from job arrival to completion.
```
T_turnaround = T_completion - T_arrival
```

### Response Time
Time from arrival to first run.
```
T_response = T_first_run - T_arrival
```

### Throughput
Jobs completed per unit time.

## Scheduling Algorithms

### 1. FIFO (First In, First Out)
- **How it works:** Jobs run in arrival order
- **Pros:** Simple, fair
- **Cons:** Convoy effect (short jobs wait behind long jobs)

**Example:**
```
Jobs: A(100s), B(10s), C(10s) - arrive at time 0
Timeline: |---A(100)---|--B(10)--|--C(10)--|
Turnaround: A=100, B=110, C=120
Average: (100+110+120)/3 = 110
```

### 2. SJF (Shortest Job First)
- **How it works:** Run shortest job first
- **Pros:** Optimal average turnaround (for non-preemptive)
- **Cons:** Requires knowing job length, starvation of long jobs

**Example:**
```
Jobs: A(100s), B(10s), C(10s) - arrive at time 0
Timeline: |--B(10)--|--C(10)--|---A(100)---|
Turnaround: B=10, C=20, A=120
Average: (10+20+120)/3 = 50
```

### 3. STCF (Shortest Time-to-Completion First)
- **How it works:** Preemptive SJF - switch to shorter job when it arrives
- **Pros:** Better than SJF when jobs arrive at different times
- **Cons:** Still requires knowing job length

### 4. Round Robin (RR)
- **How it works:** Run each job for a time slice, then switch
- **Pros:** Good response time, fair
- **Cons:** Bad turnaround time, context switch overhead

**Time slice trade-off:**
- Too short: Too much context switch overhead
- Too long: Poor response time
- Typical: 10-100ms

**Example:**
```
Jobs: A(5s), B(5s), C(5s), time slice = 1s
Timeline: |A|B|C|A|B|C|A|B|C|A|B|C|A|B|C|
Response: A=0, B=1, C=2 (excellent!)
Turnaround: A=13, B=14, C=15 (poor)
```

### 5. MLFQ (Multi-Level Feedback Queue)
- **How it works:** Multiple queues with different priorities
- **Rules:**
  1. Higher priority runs first
  2. Same priority: Round Robin
  3. New jobs start at highest priority
  4. If job uses entire time slice, move down
  5. Periodically boost all jobs to top queue

- **Pros:** Learns job behavior, good for interactive jobs
- **Cons:** Complex, can be gamed

### 6. Lottery Scheduling
- **How it works:** Jobs get tickets, winner runs
- **Pros:** Proportional share, simple
- **Cons:** Randomness can cause unfairness short-term

## Comparison Table

| Algorithm | Preemptive | Turnaround | Response | Starvation |
|-----------|------------|------------|----------|------------|
| FIFO | No | Poor | Poor | No |
| SJF | No | Good | Poor | Yes |
| STCF | Yes | Best | Medium | Yes |
| RR | Yes | Poor | Best | No |
| MLFQ | Yes | Good | Good | Possible |

## Practice Problems

### Problem 1
Jobs A(8), B(4), C(2), D(1) arrive at time 0.
Calculate average turnaround for FIFO and SJF.

**FIFO:**
```
Order: A, B, C, D
Completion: A=8, B=12, C=14, D=15
Turnaround: 8, 12, 14, 15
Average: (8+12+14+15)/4 = 12.25
```

**SJF:**
```
Order: D, C, B, A
Completion: D=1, C=3, B=7, A=15
Turnaround: 1, 3, 7, 15
Average: (1+3+7+15)/4 = 6.5
```

### Problem 2
Jobs A(6), B(3), C(3) with RR (time slice = 1).
What is the average response time?

```
Timeline: |A|B|C|A|B|C|A|B|C|A|A|A|
Response: A=0, B=1, C=2
Average: (0+1+2)/3 = 1
```

### Problem 3
Why might MLFQ use priority boost?

**Answer:** To prevent starvation. Without boost, interactive jobs that become CPU-bound would stay at low priority forever. Periodic boost gives all jobs a chance to demonstrate their behavior again.
