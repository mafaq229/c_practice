# GDB/LLDB Commands Cheat Sheet

## Starting the Debugger

### LLDB (macOS - Recommended)
```bash
lldb ./program
lldb -- ./program arg1 arg2    # With arguments
```

### GDB (Linux/Docker)
```bash
gdb ./program
gdb --args ./program arg1 arg2  # With arguments
```

---

## Running the Program

| Action | LLDB | GDB |
|--------|------|-----|
| Run | `run` or `r` | `run` or `r` |
| Run with args | `run arg1 arg2` | `run arg1 arg2` |
| Restart | `run` | `run` |
| Kill | `process kill` | `kill` |

---

## Breakpoints

### Setting Breakpoints

| Action | LLDB | GDB |
|--------|------|-----|
| Break at function | `breakpoint set -n func` or `b func` | `break func` or `b func` |
| Break at line | `breakpoint set -f file.c -l 42` | `break file.c:42` |
| Break at current file | `b 42` | `b 42` |
| Conditional break | `b func -c 'x > 5'` | `break func if x > 5` |

### Managing Breakpoints

| Action | LLDB | GDB |
|--------|------|-----|
| List all | `breakpoint list` or `br l` | `info breakpoints` |
| Delete | `breakpoint delete 1` | `delete 1` |
| Disable | `breakpoint disable 1` | `disable 1` |
| Enable | `breakpoint enable 1` | `enable 1` |
| Delete all | `breakpoint delete` | `delete` |

---

## Stepping Through Code

| Action | LLDB | GDB |
|--------|------|-----|
| Step over (next line) | `next` or `n` | `next` or `n` |
| Step into function | `step` or `s` | `step` or `s` |
| Step out of function | `finish` | `finish` |
| Continue to next break | `continue` or `c` | `continue` or `c` |
| Step by instruction | `ni` / `si` | `nexti` / `stepi` |

---

## Examining Variables

### Printing Values

| Action | LLDB | GDB |
|--------|------|-----|
| Print variable | `print x` or `p x` | `print x` or `p x` |
| Print pointer value | `p ptr` | `p ptr` |
| Dereference pointer | `p *ptr` | `p *ptr` |
| Print array element | `p arr[0]` | `p arr[0]` |
| Print array (N elements) | `parray N arr` | `p *arr@N` |
| Print as hex | `p/x var` | `p/x var` |
| Print expression | `p a + b * 2` | `p a + b * 2` |

### Local Variables

| Action | LLDB | GDB |
|--------|------|-----|
| Show all locals | `frame variable` or `v` | `info locals` |
| Show arguments | `frame variable -a` | `info args` |
| Show global | `target variable glob` | `p glob` |

---

## Stack Frames

| Action | LLDB | GDB |
|--------|------|-----|
| Show backtrace | `thread backtrace` or `bt` | `backtrace` or `bt` |
| Short backtrace | `bt 5` (last 5 frames) | `bt 5` |
| Go to frame | `frame select 2` | `frame 2` |
| Show frame info | `frame info` | `info frame` |
| Move up stack | `up` | `up` |
| Move down stack | `down` | `down` |

---

## Memory Examination

| Action | LLDB | GDB |
|--------|------|-----|
| Examine memory | `memory read ptr` | `x ptr` |
| Examine as bytes | `memory read -fx -c16 ptr` | `x/16xb ptr` |
| Examine as words | `memory read -fw -c4 ptr` | `x/4xw ptr` |
| Examine as string | `memory read -s ptr` | `x/s ptr` |

---

## Watchpoints

Watch for variable changes:

| Action | LLDB | GDB |
|--------|------|-----|
| Watch variable | `watchpoint set variable x` | `watch x` |
| Watch expression | `watchpoint set expression -- &x` | `watch *(&x)` |
| List watchpoints | `watchpoint list` | `info watchpoints` |
| Delete watchpoint | `watchpoint delete 1` | `delete 1` |

---

## Threads

| Action | LLDB | GDB |
|--------|------|-----|
| List threads | `thread list` | `info threads` |
| Switch thread | `thread select 2` | `thread 2` |
| Backtrace all threads | `thread backtrace all` | `thread apply all bt` |

---

## Useful Tricks

### Breaking on Crashes

**LLDB:**
```
(lldb) run
# [program crashes]
(lldb) bt           # See where it crashed
(lldb) frame variable   # See local variables
```

**GDB:**
```
(gdb) run
# [program crashes]
(gdb) bt           # See where it crashed
(gdb) info locals  # See local variables
```

### Breaking Into Running Program

- **LLDB:** `Ctrl+C` then `thread backtrace`
- **GDB:** `Ctrl+C` then `backtrace`

### Core Dump Analysis

```bash
# Enable core dumps
ulimit -c unlimited

# Run program (it crashes and creates core file)
./program

# Analyze with debugger
lldb ./program -c core
gdb ./program core
```

### Scripting

**GDB:**
```
# Create .gdbinit file
break main
run
```

**LLDB:**
```
# Create .lldbinit file
breakpoint set -n main
run
```

---

## Common Workflows

### Finding a Segfault

1. Run in debugger: `lldb ./program`
2. `run`
3. When it crashes: `bt`
4. Go to relevant frame: `frame select N`
5. Examine variables: `frame variable`
6. Find the NULL pointer or invalid access

### Finding an Infinite Loop

1. Run in debugger: `lldb ./program`
2. `run`
3. When stuck: `Ctrl+C`
4. `bt` to see where it's stuck
5. Set breakpoint in loop: `b suspicious_function`
6. `continue`
7. Step through and watch variables

### Finding a Memory Leak

Use Valgrind (via Docker):
```bash
docker run --rm -v $(pwd):/code -w /code gios-prep \
    valgrind --leak-check=full ./program
```

### Finding Buffer Overflow

Use Address Sanitizer (via Docker):
```bash
docker run --rm -v $(pwd):/code -w /code gios-prep \
    bash -c "gcc -g -fsanitize=address prog.c -o prog && ./prog"
```

---

## Quick Reference Card

```
# Start
lldb ./program

# Common commands
b main          # Breakpoint at main
r               # Run
n               # Next line
s               # Step into
c               # Continue
p var           # Print variable
bt              # Backtrace
v               # Show locals
q               # Quit

# When it crashes
bt              # See where
frame 0         # Go to crash location
v               # See variables
```
