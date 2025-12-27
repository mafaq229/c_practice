# Project 3: Concurrency & IPC Makefile

CC = clang
CFLAGS = -Wall -Wextra -g -pthread
LDFLAGS = -pthread

# For shared memory and message queues (Linux only)
LDFLAGS_IPC = -lrt

# Directories
PROC_DIR = 01_processes
THREAD_DIR = 02_threads_basics
MUTEX_DIR = 03_mutex
CONDVAR_DIR = 04_condition_vars
POOL_DIR = 05_thread_pool
SHM_DIR = 06_shared_memory
MQ_DIR = 07_message_queues

# All targets
.PHONY: all clean processes threads mutex condvars threadpool shm mq

all: processes threads mutex condvars threadpool

# Note: shm and mq require Linux (use Docker on macOS)

# =============================================================================
# Module 01: Processes
# =============================================================================

processes: $(PROC_DIR)/fork_basics $(PROC_DIR)/exec_example $(PROC_DIR)/process_tree

$(PROC_DIR)/fork_basics: $(PROC_DIR)/fork_basics.c
	$(CC) $(CFLAGS) $< -o $@

$(PROC_DIR)/exec_example: $(PROC_DIR)/exec_example.c
	$(CC) $(CFLAGS) $< -o $@

$(PROC_DIR)/process_tree: $(PROC_DIR)/process_tree.c
	$(CC) $(CFLAGS) $< -o $@

# =============================================================================
# Module 02: Threads Basics
# =============================================================================

threads: $(THREAD_DIR)/thread_create $(THREAD_DIR)/thread_args $(THREAD_DIR)/thread_return

$(THREAD_DIR)/thread_create: $(THREAD_DIR)/thread_create.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(THREAD_DIR)/thread_args: $(THREAD_DIR)/thread_args.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(THREAD_DIR)/thread_return: $(THREAD_DIR)/thread_return.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# =============================================================================
# Module 03: Mutex
# =============================================================================

mutex: $(MUTEX_DIR)/race_condition $(MUTEX_DIR)/mutex_fix $(MUTEX_DIR)/deadlock $(MUTEX_DIR)/counter_benchmark

$(MUTEX_DIR)/race_condition: $(MUTEX_DIR)/race_condition.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(MUTEX_DIR)/mutex_fix: $(MUTEX_DIR)/mutex_fix.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(MUTEX_DIR)/deadlock: $(MUTEX_DIR)/deadlock.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(MUTEX_DIR)/counter_benchmark: $(MUTEX_DIR)/counter_benchmark.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# =============================================================================
# Module 04: Condition Variables
# =============================================================================

condvars: $(CONDVAR_DIR)/producer_consumer $(CONDVAR_DIR)/bounded_buffer $(CONDVAR_DIR)/barrier

$(CONDVAR_DIR)/producer_consumer: $(CONDVAR_DIR)/producer_consumer.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(CONDVAR_DIR)/bounded_buffer: $(CONDVAR_DIR)/bounded_buffer.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(CONDVAR_DIR)/barrier: $(CONDVAR_DIR)/barrier.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# =============================================================================
# Module 05: Thread Pool
# =============================================================================

POOL_SRCS = $(POOL_DIR)/thread_pool.c $(POOL_DIR)/work_queue.c $(POOL_DIR)/pool_test.c

threadpool: $(POOL_DIR)/pool_test

$(POOL_DIR)/pool_test: $(POOL_SRCS)
	$(CC) $(CFLAGS) $(POOL_SRCS) -o $@ $(LDFLAGS)

# =============================================================================
# Module 06: Shared Memory (Linux only - use Docker on macOS)
# =============================================================================

shm: $(SHM_DIR)/shm_writer $(SHM_DIR)/shm_reader $(SHM_DIR)/shm_sync

$(SHM_DIR)/shm_writer: $(SHM_DIR)/shm_writer.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS_IPC) $(LDFLAGS)

$(SHM_DIR)/shm_reader: $(SHM_DIR)/shm_reader.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS_IPC) $(LDFLAGS)

$(SHM_DIR)/shm_sync: $(SHM_DIR)/shm_sync.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS_IPC) $(LDFLAGS)

# =============================================================================
# Module 07: Message Queues (Linux only - use Docker on macOS)
# =============================================================================

mq: $(MQ_DIR)/mq_sender $(MQ_DIR)/mq_receiver $(MQ_DIR)/mq_priority

$(MQ_DIR)/mq_sender: $(MQ_DIR)/mq_sender.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS_IPC)

$(MQ_DIR)/mq_receiver: $(MQ_DIR)/mq_receiver.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS_IPC)

$(MQ_DIR)/mq_priority: $(MQ_DIR)/mq_priority.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS_IPC)

# =============================================================================
# Cleanup
# =============================================================================

clean:
	rm -f $(PROC_DIR)/fork_basics $(PROC_DIR)/exec_example $(PROC_DIR)/process_tree
	rm -f $(THREAD_DIR)/thread_create $(THREAD_DIR)/thread_args $(THREAD_DIR)/thread_return
	rm -f $(MUTEX_DIR)/race_condition $(MUTEX_DIR)/mutex_fix $(MUTEX_DIR)/deadlock $(MUTEX_DIR)/counter_benchmark
	rm -f $(CONDVAR_DIR)/producer_consumer $(CONDVAR_DIR)/bounded_buffer $(CONDVAR_DIR)/barrier
	rm -f $(POOL_DIR)/pool_test
	rm -f $(SHM_DIR)/shm_writer $(SHM_DIR)/shm_reader $(SHM_DIR)/shm_sync
	rm -f $(MQ_DIR)/mq_sender $(MQ_DIR)/mq_receiver $(MQ_DIR)/mq_priority

# =============================================================================
# Help
# =============================================================================

help:
	@echo "Project 3: Concurrency & IPC"
	@echo ""
	@echo "Targets:"
	@echo "  all        - Build modules 01-05 (macOS compatible)"
	@echo "  processes  - Build process exercises"
	@echo "  threads    - Build threading exercises"
	@echo "  mutex      - Build mutex exercises"
	@echo "  condvars   - Build condition variable exercises"
	@echo "  threadpool - Build thread pool exercises"
	@echo "  shm        - Build shared memory exercises (Docker/Linux)"
	@echo "  mq         - Build message queue exercises (Docker/Linux)"
	@echo "  clean      - Remove all built executables"
	@echo ""
	@echo "Note: Modules 06 and 07 require Linux. Use Docker on macOS."
