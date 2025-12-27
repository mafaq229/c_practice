# Capstone Project: Mini-GIOS Makefile

CC = clang
CFLAGS = -Wall -Wextra -g -I include
LDFLAGS = -pthread

# For IPC (Linux only)
LDFLAGS_IPC = -lrt

# Directories
SRC_DIR = src
INC_DIR = include
TEST_DIR = tests

# Common source files
COMMON_SRCS = $(SRC_DIR)/protocol.c $(SRC_DIR)/file_utils.c $(SRC_DIR)/socket_utils.c

# Targets
.PHONY: all clean part_a part_b part_c part_d part_e test test_files

all: part_a part_b part_c

# ============================================================================
# Part A: Single-Threaded Server
# ============================================================================

part_a: server_single client test_files

server_single: $(SRC_DIR)/server_single.c $(COMMON_SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

client: $(SRC_DIR)/client.c $(COMMON_SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# ============================================================================
# Part B: Multi-Threaded Server
# ============================================================================

THREAD_SRCS = $(SRC_DIR)/thread_pool.c $(SRC_DIR)/work_queue.c

part_b: server_mt client test_files

server_mt: $(SRC_DIR)/server_mt.c $(COMMON_SRCS) $(THREAD_SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# ============================================================================
# Part C: Caching Proxy
# ============================================================================

part_c: proxy server_mt client test_files

proxy: $(SRC_DIR)/proxy.c $(SRC_DIR)/cache.c $(COMMON_SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# ============================================================================
# Part D: IPC Cache (Linux only)
# ============================================================================

part_d: proxy_ipc cache_process server_mt client test_files

proxy_ipc: $(SRC_DIR)/proxy_ipc.c $(SRC_DIR)/shm_manager.c $(COMMON_SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LDFLAGS_IPC)

cache_process: $(SRC_DIR)/cache_process.c $(SRC_DIR)/cache.c $(SRC_DIR)/shm_manager.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LDFLAGS_IPC)

# ============================================================================
# Part E: Monitoring
# ============================================================================

part_e: monitor
	@echo "Part E requires all previous parts"

monitor: $(SRC_DIR)/monitor.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# ============================================================================
# Tests
# ============================================================================

test: test_protocol test_cache test_thread_pool
	./test_protocol
	./test_cache
	./test_thread_pool

test_protocol: $(TEST_DIR)/test_protocol.c $(SRC_DIR)/protocol.c
	$(CC) $(CFLAGS) $^ -o $@

test_cache: $(TEST_DIR)/test_cache.c $(SRC_DIR)/cache.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

test_thread_pool: $(TEST_DIR)/test_thread_pool.c $(SRC_DIR)/thread_pool.c $(SRC_DIR)/work_queue.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# ============================================================================
# Test Files
# ============================================================================

test_files:
	@mkdir -p test_files
	@echo "This is a small test file." > test_files/small.txt
	@echo "Line 2 of the small file." >> test_files/small.txt
	@echo "Line 3 of the small file." >> test_files/small.txt
	@dd if=/dev/urandom of=test_files/medium.bin bs=1024 count=100 2>/dev/null
	@dd if=/dev/urandom of=test_files/large.bin bs=1024 count=1000 2>/dev/null
	@echo "Test files created in test_files/"

# ============================================================================
# Cleanup
# ============================================================================

clean:
	rm -f server_single server_mt proxy proxy_ipc cache_process client monitor
	rm -f test_protocol test_cache test_thread_pool
	rm -rf test_files

# ============================================================================
# Help
# ============================================================================

help:
	@echo "Capstone Project: Mini-GIOS"
	@echo ""
	@echo "Targets:"
	@echo "  all       - Build parts A, B, C"
	@echo "  part_a    - Single-threaded server + client"
	@echo "  part_b    - Multi-threaded server"
	@echo "  part_c    - Caching proxy"
	@echo "  part_d    - IPC cache (Linux/Docker only)"
	@echo "  part_e    - Monitoring"
	@echo "  test      - Run unit tests"
	@echo "  clean     - Remove all built files"
	@echo ""
	@echo "Build incrementally: make part_a, test, make part_b, test, ..."
