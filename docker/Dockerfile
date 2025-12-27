# CS-6200 Preparation Development Environment
#
# This container provides a full Linux development environment with all tools
# needed for the curriculum, including those that don't work on Apple Silicon.
#
# Build: docker build -t gios-prep .
# Run:   docker run -it --rm -v $(pwd):/code -w /code gios-prep bash

FROM ubuntu:22.04

# Avoid interactive prompts during installation
ENV DEBIAN_FRONTEND=noninteractive

# Install development tools
RUN apt-get update && apt-get install -y \
    # Compilers
    gcc \
    g++ \
    clang \
    # Build tools
    make \
    cmake \
    autoconf \
    automake \
    libtool \
    pkg-config \
    # Debugging tools
    gdb \
    valgrind \
    strace \
    ltrace \
    # Networking tools
    netcat-openbsd \
    curl \
    wget \
    iproute2 \
    net-tools \
    # Libraries
    libpthread-stubs0-dev \
    libssl-dev \
    zlib1g-dev \
    # gRPC dependencies
    libgrpc-dev \
    libgrpc++-dev \
    protobuf-compiler \
    protobuf-compiler-grpc \
    libprotobuf-dev \
    # Editor and utilities
    vim \
    nano \
    less \
    tree \
    git \
    # Man pages
    man-db \
    manpages-dev \
    manpages-posix-dev \
    # Cleanup
    && rm -rf /var/lib/apt/lists/*

# Create a non-root user for development
RUN useradd -ms /bin/bash developer
USER developer
WORKDIR /code

# Default command
CMD ["/bin/bash"]
