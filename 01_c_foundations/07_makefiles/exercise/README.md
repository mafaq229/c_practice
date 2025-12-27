# Makefile Exercise

## Instructions

In this exercise, you will write a Makefile from scratch for a small project.

### Project Structure

```
exercise/
├── README.md       (this file)
├── Makefile        (TODO: create this)
├── main.c          (provided)
├── math_utils.c    (provided)
├── math_utils.h    (provided)
├── string_utils.c  (provided)
└── string_utils.h  (provided)
```

### TODO: Create a Makefile

Your Makefile should:

1. **Compile all .c files to .o files**
   - Use clang (or gcc) as the compiler
   - Enable warnings: -Wall -Wextra
   - Include debug symbols: -g

2. **Link object files to create executable named `myprogram`**

3. **Have the following targets:**
   - `all` (default): Build the program
   - `clean`: Remove all .o files and the executable
   - `run`: Build and run the program
   - `debug`: Build and run with lldb (or gdb)

4. **Use variables for:**
   - CC (compiler)
   - CFLAGS (compiler flags)
   - TARGET (executable name)
   - SRCS (source files)
   - OBJS (object files)

### Hints

1. Start with the basic template from Makefile.basic
2. Use automatic variable substitution: `OBJS = $(SRCS:.c=.o)`
3. Use pattern rules: `%.o: %.c`
4. Remember: commands must start with a TAB, not spaces!

### Testing Your Makefile

```bash
# Build
make

# Run
make run

# Clean
make clean

# Verify clean worked
ls *.o        # Should show "No such file"
ls myprogram  # Should show "No such file"
```

### Expected Output

When you run `make run`, you should see:

```
$ make run
clang -Wall -Wextra -g -c main.c -o main.o
clang -Wall -Wextra -g -c math_utils.c -o math_utils.o
clang -Wall -Wextra -g -c string_utils.c -o string_utils.o
clang -Wall -Wextra -g -o myprogram main.o math_utils.o string_utils.o
./myprogram
=== Math Utils ===
add(5, 3) = 8
multiply(4, 7) = 28
factorial(5) = 120
=== String Utils ===
to_upper("hello") = HELLO
string_length("test") = 4
```

### Solution

After completing the exercise, compare your Makefile with the solution in the solutions folder.

Good luck!
