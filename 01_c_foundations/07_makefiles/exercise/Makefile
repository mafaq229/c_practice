# =========================
# Makefile (Beginner-Friendly)
# =========================
# A Makefile automates building your C program.
# It describes:
#   1) what files to build (targets),
#   2) what each target depends on (prerequisites),
#   3) and the commands to produce them (recipes).

# -------------------------------------------------
# Compiler and compilation flags
# -------------------------------------------------

# CC is a variable that holds the C compiler command.
# We will use it in recipes as $(CC).
CC = clang

# CFLAGS is a variable that holds compiler flags.
# -Wall     : enable most common warnings
# -Wextra   : enable extra warnings
# -g        : include debug symbols (useful for lldb debugging)
CFLAGS = -Wall -Wextra -g

# -------------------------------------------------
# Target output name and input source files
# -------------------------------------------------

# TARGET is the name of the final executable we want to build.
# This will produce a file named "myprogram".
TARGET = myprogram

# SRCS is the list of C source files in the project.
SRCS = main.c math_utils.c string_utils.c

# OBJS is the list of object files (.o) that correspond to SRCS.
# $(SRCS:.c=.o) is Make's substitution syntax:
#   - Take each word in SRCS
#   - Replace ".c" with ".o"
# Example:
#   main.c -> main.o
#   math_utils.c -> math_utils.o
#   string_utils.c -> string_utils.o
OBJS = $(SRCS:.c=.o)

# -------------------------------------------------
# Phony targets
# -------------------------------------------------

# .PHONY declares targets that are NOT actual files.
# Without this, if a file named "clean" existed, Make might think
# the "clean" target is already up-to-date and skip running it.
.PHONY: all clean run debug

# -------------------------------------------------
# Default target
# -------------------------------------------------

# all is a conventional "default" target.
# When you run just "make", Make typically builds the first target it sees.
# This rule says:
#   To satisfy "all", first build $(TARGET) (i.e., "myprogram").
all: $(TARGET)

# -------------------------------------------------
# Link step: build the executable from object files
# -------------------------------------------------

# This rule says:
#   To build the executable $(TARGET) (myprogram),
#   we need all object files in $(OBJS) first.
$(TARGET): $(OBJS)
	# Recipe lines MUST start with a TAB character (not spaces).
	#
	# $(CC) expands to clang
	# $(CFLAGS) expands to -Wall -Wextra -g
	#
	# Automatic variables:
	#   $@  = the current target name (here: myprogram)
	#   $^  = all prerequisites for this rule (here: main.o math_utils.o string_utils.o)
	#
	# This command links object files into the final executable:
	# clang -Wall -Wextra -g -o myprogram main.o math_utils.o string_utils.o
	$(CC) $(CFLAGS) -o $@ $^

# -------------------------------------------------
# Compile step: build any .o file from its matching .c file
# -------------------------------------------------

# Pattern rule:
#   %.o : %.c
# means:
#   "For any object file X.o, it depends on source file X.c"
#
# Example:
#   main.o depends on main.c
#   math_utils.o depends on math_utils.c
#
# The % is a wildcard called the "stem" (the shared part of the filename).
%.o: %.c
	# -c means "compile only" (do not link)
	#
	# Automatic variables:
	#   $<  = the first prerequisite (here: the matching .c file)
	#   $@  = the target (here: the .o file being produced)
	#
	# Example expansion for main.o:
	# clang -Wall -Wextra -g -c main.c -o main.o
	$(CC) $(CFLAGS) -c $< -o $@

# -------------------------------------------------
# Clean: remove generated build artifacts
# -------------------------------------------------

# clean is a convenience target to delete output files.
clean:
	# rm -f means:
	#   rm  : remove files
	#   -f  : force (do not error if files don't exist)
	#
	# This removes all object files and the executable.
	rm -f $(OBJS) $(TARGET)

# -------------------------------------------------
# Run: build then execute the program
# -------------------------------------------------

# run depends on $(TARGET), so it will build myprogram if needed first.
run: $(TARGET)
	# Run the executable from the current directory.
	./$(TARGET)

# -------------------------------------------------
# Debug: build then start LLDB on the program
# -------------------------------------------------

# debug depends on $(TARGET), so it will build myprogram if needed first.
debug: $(TARGET)
	# Start lldb with the executable loaded.
	lldb ./$(TARGET)
