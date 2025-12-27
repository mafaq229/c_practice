# Project 1: C Foundations - Makefile
#
# Usage:
#   make          - Build all exercises
#   make test     - Run all tests
#   make clean    - Remove build artifacts
#   make module02 - Build specific module

CC = clang
CFLAGS = -Wall -Wextra -g -std=c11

# Directories
DIRS = 01_environment 02_basics 03_pointers 04_memory 05_strings 06_data_structures 08_gdb

# All targets
.PHONY: all clean test

all: environment basics pointers memory strings data_structures gdb

# Module 01: Environment Setup
.PHONY: environment
environment:
	@echo "Module 01: Environment Setup"
	$(CC) $(CFLAGS) 01_environment/test_setup.c -o 01_environment/test_setup

# Module 02: Basics
.PHONY: basics
basics:
	@echo "Module 02: C Basics"
	$(CC) $(CFLAGS) 02_basics/exercises.c -o 02_basics/exercises

# Module 03: Pointers
.PHONY: pointers
pointers:
	@echo "Module 03: Pointers"
	$(CC) $(CFLAGS) 03_pointers/pointer_basics.c -o 03_pointers/pointer_basics
	$(CC) $(CFLAGS) 03_pointers/double_pointers.c -o 03_pointers/double_pointers
	$(CC) $(CFLAGS) 03_pointers/pointer_bugs.c -o 03_pointers/pointer_bugs

# Module 04: Memory
.PHONY: memory
memory:
	@echo "Module 04: Memory Management"
	$(CC) $(CFLAGS) 04_memory/dynamic_arrays.c -o 04_memory/dynamic_arrays
	$(CC) $(CFLAGS) 04_memory/memory_leaks.c -o 04_memory/memory_leaks
	$(CC) $(CFLAGS) 04_memory/custom_allocator.c -o 04_memory/custom_allocator

# Module 05: Strings
.PHONY: strings
strings:
	@echo "Module 05: Strings"
	$(CC) $(CFLAGS) 05_strings/string_utils.c -o 05_strings/string_utils
	$(CC) $(CFLAGS) 05_strings/string_parsing.c -o 05_strings/string_parsing
	$(CC) $(CFLAGS) 05_strings/buffer_handling.c -o 05_strings/buffer_handling

# Module 06: Data Structures
.PHONY: data_structures
data_structures:
	@echo "Module 06: Data Structures"
	$(CC) $(CFLAGS) 06_data_structures/linked_list.c -o 06_data_structures/linked_list
	$(CC) $(CFLAGS) 06_data_structures/hash_table.c -o 06_data_structures/hash_table
	$(CC) $(CFLAGS) 06_data_structures/dynamic_buffer.c -o 06_data_structures/dynamic_buffer

# Module 08: GDB
.PHONY: gdb
gdb:
	@echo "Module 08: GDB Debugging"
	$(CC) $(CFLAGS) 08_gdb/buggy_program.c -o 08_gdb/buggy_program
	$(CC) $(CFLAGS) 08_gdb/segfault_examples.c -o 08_gdb/segfault_examples

# Testing
.PHONY: test
test:
	@echo "Running Project 1 Tests..."
	@echo "========================="
	@echo "Testing environment setup..."
	@./01_environment/test_setup && echo "Environment: PASS" || echo "Environment: FAIL"
	@echo ""
	@echo "Note: Other tests depend on completing the exercises."
	@echo "Run individual executables to test your implementations."

# Clean
.PHONY: clean
clean:
	@echo "Cleaning Project 1..."
	rm -f 01_environment/test_setup
	rm -f 02_basics/exercises
	rm -f 03_pointers/pointer_basics 03_pointers/double_pointers 03_pointers/pointer_bugs
	rm -f 04_memory/dynamic_arrays 04_memory/memory_leaks 04_memory/custom_allocator
	rm -f 05_strings/string_utils 05_strings/string_parsing 05_strings/buffer_handling
	rm -f 06_data_structures/linked_list 06_data_structures/hash_table 06_data_structures/dynamic_buffer
	rm -f 08_gdb/buggy_program 08_gdb/segfault_examples
	@echo "Clean complete."

# Help
.PHONY: help
help:
	@echo "Project 1: C Foundations"
	@echo ""
	@echo "Available targets:"
	@echo "  make            - Build all modules"
	@echo "  make environment - Build Module 01"
	@echo "  make basics     - Build Module 02"
	@echo "  make pointers   - Build Module 03"
	@echo "  make memory     - Build Module 04"
	@echo "  make strings    - Build Module 05"
	@echo "  make data_structures - Build Module 06"
	@echo "  make gdb        - Build Module 08"
	@echo "  make test       - Run tests"
	@echo "  make clean      - Remove build artifacts"
