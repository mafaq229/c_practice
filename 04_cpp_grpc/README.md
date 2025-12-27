# Project 4: C++ & gRPC Primer

## Overview

This project prepares you for GIOS Project 4, which uses gRPC for distributed file systems. While Projects 1-3 use pure C, Project 4 requires C++ and Protocol Buffers.

**Estimated Time:** 5 hours
**Difficulty:** Medium-Hard
**When to Complete:** Can be done during the course (before Project 4)

## Why This Matters for GIOS

GIOS Project 4 requires:
- Basic C++ knowledge (classes, STL containers)
- Protocol Buffers for data serialization
- gRPC for client-server communication
- Implementing a distributed file system

## Module Overview

| Module | Topic | Time | Key Concepts |
|--------|-------|------|--------------|
| 01 | C++ for C Programmers | 2 hrs | Classes, RAII, STL |
| 02 | Protocol Buffers | 1.5 hrs | .proto files, serialization |
| 03 | gRPC Basics | 1.5 hrs | Services, stubs, streaming |

## Building

This project uses CMake (required by gRPC):

```bash
# Install dependencies (Ubuntu/Docker)
apt-get install -y cmake protobuf-compiler libprotobuf-dev libgrpc++-dev

# macOS (Homebrew)
brew install cmake protobuf grpc

# Build
mkdir build && cd build
cmake ..
make
```

## Module Details

### Module 01: C++ for C Programmers (2 hours)

Learn the C++ features you'll need.

**Files:**
- `01_cpp_basics/classes.cpp` - Classes and objects
- `01_cpp_basics/stl_containers.cpp` - std::string, std::vector, std::map
- `01_cpp_basics/smart_pointers.cpp` - unique_ptr, shared_ptr (RAII)

**Key Differences from C:**
- `std::string` instead of `char*` - no manual memory management
- `std::vector` instead of arrays - dynamic sizing
- Classes with constructors/destructors
- References (`&`) as safer pointers
- RAII: Resource Acquisition Is Initialization

### Module 02: Protocol Buffers (1.5 hours)

Define message formats for gRPC.

**Files:**
- `02_protobuf/messages.proto` - Message definitions
- `02_protobuf/proto_example.cpp` - Using generated code
- `02_protobuf/README.md` - Setup instructions

**Key Concepts:**
- `.proto` files define message structure
- `protoc` generates C++ code
- Serialization/deserialization is automatic
- More efficient than JSON/XML

### Module 03: gRPC Basics (1.5 hours)

Build client-server applications with gRPC.

**Files:**
- `03_grpc/keyvalue.proto` - Service definition
- `03_grpc/kv_server.cpp` - Server implementation
- `03_grpc/kv_client.cpp` - Client implementation
- `03_grpc/README.md` - Setup and testing

**Key Concepts:**
- Service = collection of RPC methods
- Server implements service interface
- Client uses generated stub
- Four RPC types: unary, server streaming, client streaming, bidirectional

## C++ vs C Quick Reference

| C | C++ |
|---|-----|
| `char str[100]` | `std::string str` |
| `malloc/free` | `new/delete` or smart pointers |
| `struct` | `class` |
| Function pointers | Virtual methods |
| `printf` | `std::cout` |
| `#include <string.h>` | `#include <string>` |

## Docker Setup for gRPC

gRPC setup can be complex. Use Docker:

```bash
# From project root
docker-compose run --rm dev bash
cd 04_cpp_grpc
mkdir build && cd build
cmake ..
make
```

## Common Pitfalls

### 1. Forgetting to link libraries
```cmake
# CMakeLists.txt
target_link_libraries(server grpc++ protobuf)
```

### 2. Not generating proto files
```bash
# Generate C++ from .proto
protoc --cpp_out=. --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` messages.proto
```

### 3. Mixing C and C++ string types
```cpp
// C++ string to C string
std::string cpp_str = "hello";
const char* c_str = cpp_str.c_str();

// C string to C++ string
const char* c_str = "hello";
std::string cpp_str(c_str);
```

## Resources

- [gRPC C++ Tutorial](https://grpc.io/docs/languages/cpp/basics/)
- [Protocol Buffers Guide](https://protobuf.dev/programming-guides/proto3/)
- [C++ Reference](https://en.cppreference.com/)
- [Learn C++ in Y Minutes](https://learnxinyminutes.com/docs/c++/)

## Progress Checklist

- [ ] Module 01: C++ basics (classes, STL)
- [ ] Module 02: Protocol Buffers
- [ ] Module 03: gRPC key-value store

## What's Next?

After completing this project:
1. You have all the skills for GIOS Project 4
2. Complete Project 5 (OS Concepts) for exam prep
3. Check the resources folder for tips
