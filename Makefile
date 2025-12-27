# CS-6200 Preparation Curriculum - Root Makefile
#
# Usage:
#   make all      - Build all projects
#   make project1 - Build Project 1 (C Foundations)
#   make project2 - Build Project 2 (Systems & Networking)
#   make project3 - Build Project 3 (Concurrency & IPC)
#   make project4 - Build Project 4 (C++ & gRPC)
#   make test     - Run all tests
#   make clean    - Clean all build artifacts
#   make docker   - Build Docker container

# Compiler settings
CC = clang
CXX = clang++
CFLAGS = -Wall -Wextra -g -std=c11
CXXFLAGS = -Wall -Wextra -g -std=c++17
LDFLAGS = -pthread

# Project directories
PROJECT1_DIR = 01_c_foundations
PROJECT2_DIR = 02_systems_networking
PROJECT3_DIR = 03_concurrency_ipc
PROJECT4_DIR = 04_cpp_grpc

# Default target
.PHONY: all
all: project1 project2 project3

# Individual projects
.PHONY: project1
project1:
	@echo "=========================================="
	@echo "Building Project 1: C Foundations"
	@echo "=========================================="
	$(MAKE) -C $(PROJECT1_DIR)

.PHONY: project2
project2:
	@echo "=========================================="
	@echo "Building Project 2: Systems & Networking"
	@echo "=========================================="
	$(MAKE) -C $(PROJECT2_DIR)

.PHONY: project3
project3:
	@echo "=========================================="
	@echo "Building Project 3: Concurrency & IPC"
	@echo "=========================================="
	$(MAKE) -C $(PROJECT3_DIR)

.PHONY: project4
project4:
	@echo "=========================================="
	@echo "Building Project 4: C++ & gRPC"
	@echo "=========================================="
	@echo "Note: Project 4 uses CMake. Run from 04_cpp_grpc directory:"
	@echo "  mkdir build && cd build && cmake .. && make"

# Testing
.PHONY: test
test: test1 test2 test3

.PHONY: test1
test1:
	@echo "Testing Project 1..."
	$(MAKE) -C $(PROJECT1_DIR) test

.PHONY: test2
test2:
	@echo "Testing Project 2..."
	$(MAKE) -C $(PROJECT2_DIR) test

.PHONY: test3
test3:
	@echo "Testing Project 3..."
	$(MAKE) -C $(PROJECT3_DIR) test

# Clean
.PHONY: clean
clean:
	@echo "Cleaning all projects..."
	$(MAKE) -C $(PROJECT1_DIR) clean
	$(MAKE) -C $(PROJECT2_DIR) clean
	$(MAKE) -C $(PROJECT3_DIR) clean
	@echo "Clean complete."

# Docker
.PHONY: docker
docker:
	@echo "Building Docker container..."
	docker build -t gios-prep ./docker

.PHONY: docker-shell
docker-shell:
	@echo "Starting Docker shell..."
	docker run -it --rm -v $(shell pwd):/code -w /code gios-prep bash

# Help
.PHONY: help
help:
	@echo "CS-6200 Preparation Curriculum"
	@echo ""
	@echo "Available targets:"
	@echo "  make all       - Build projects 1, 2, and 3"
	@echo "  make project1  - Build Project 1 (C Foundations)"
	@echo "  make project2  - Build Project 2 (Systems & Networking)"
	@echo "  make project3  - Build Project 3 (Concurrency & IPC)"
	@echo "  make project4  - Instructions for building Project 4"
	@echo "  make test      - Run all tests"
	@echo "  make test1     - Run Project 1 tests only"
	@echo "  make test2     - Run Project 2 tests only"
	@echo "  make test3     - Run Project 3 tests only"
	@echo "  make clean     - Remove all build artifacts"
	@echo "  make docker    - Build the Docker development container"
	@echo "  make docker-shell - Start interactive Docker shell"
	@echo "  make help      - Show this help message"
