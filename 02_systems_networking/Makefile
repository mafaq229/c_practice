# Project 2: Systems & Networking - Makefile

CC = clang
CFLAGS = -Wall -Wextra -g -std=c11

.PHONY: all clean test

all: file_io sockets protocol file_transfer

# Module 02: File I/O
.PHONY: file_io
file_io:
	@echo "Building Module 02: File I/O"
	$(CC) $(CFLAGS) 02_file_io/file_copy.c -o 02_file_io/file_copy
	$(CC) $(CFLAGS) 02_file_io/binary_file.c -o 02_file_io/binary_file
	$(CC) $(CFLAGS) 02_file_io/buffered_io.c -o 02_file_io/buffered_io

# Module 03: Sockets
.PHONY: sockets
sockets:
	@echo "Building Module 03: Socket Basics"
	$(CC) $(CFLAGS) 03_sockets_basics/tcp_server.c -o 03_sockets_basics/tcp_server
	$(CC) $(CFLAGS) 03_sockets_basics/tcp_client.c -o 03_sockets_basics/tcp_client

# Module 04: Protocol
.PHONY: protocol
protocol:
	@echo "Building Module 04: Protocol Design"
	$(CC) $(CFLAGS) 04_protocol_design/protocol_parser.c -o 04_protocol_design/protocol_parser

# Module 05: File Transfer
.PHONY: file_transfer
file_transfer:
	@echo "Building Module 05: File Transfer"
	$(CC) $(CFLAGS) 05_file_transfer/ft_server.c -o 05_file_transfer/ft_server
	$(CC) $(CFLAGS) 05_file_transfer/ft_client.c -o 05_file_transfer/ft_client

# Test
.PHONY: test
test:
	@echo "Running Project 2 tests..."
	@echo "File I/O tests:"
	@cd 02_file_io && ./file_copy 2>/dev/null || echo "  file_copy: TODO"
	@echo ""
	@echo "Socket tests require manual testing."
	@echo "Run: ./tcp_server & ./tcp_client"

# Clean
.PHONY: clean
clean:
	rm -f 02_file_io/file_copy 02_file_io/binary_file 02_file_io/buffered_io
	rm -f 03_sockets_basics/tcp_server 03_sockets_basics/tcp_client
	rm -f 04_protocol_design/protocol_parser
	rm -f 05_file_transfer/ft_server 05_file_transfer/ft_client
	rm -f 05_file_transfer/test_files/*.received
