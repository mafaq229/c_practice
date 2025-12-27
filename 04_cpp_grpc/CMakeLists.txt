cmake_minimum_required(VERSION 3.10)
project(gios_cpp_grpc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG)

# If gRPC not found via config, try pkg-config
if(NOT gRPC_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GRPC REQUIRED grpc++)
endif()

# Include directories
include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# ============================================================================
# Module 01: C++ Basics (no special dependencies)
# ============================================================================

add_executable(classes 01_cpp_basics/classes.cpp)
add_executable(stl_containers 01_cpp_basics/stl_containers.cpp)
add_executable(smart_pointers 01_cpp_basics/smart_pointers.cpp)

# ============================================================================
# Module 02: Protocol Buffers
# ============================================================================

# Generate protobuf files
set(PROTO_FILES 02_protobuf/messages.proto)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

add_executable(proto_example
    02_protobuf/proto_example.cpp
    ${PROTO_SRCS}
    ${PROTO_HDRS}
)
target_link_libraries(proto_example ${PROTOBUF_LIBRARIES})
target_include_directories(proto_example PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/02_protobuf)

# ============================================================================
# Module 03: gRPC (if available)
# ============================================================================

if(gRPC_FOUND OR GRPC_FOUND)
    message(STATUS "gRPC found, building Module 03")

    # Find gRPC plugin
    find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin)

    # Generate gRPC files
    set(GRPC_PROTO 03_grpc/keyvalue.proto)

    get_filename_component(GRPC_PROTO_PATH "${GRPC_PROTO}" ABSOLUTE)
    get_filename_component(GRPC_PROTO_DIR "${GRPC_PROTO_PATH}" DIRECTORY)

    set(GRPC_PROTO_SRCS "${CMAKE_CURRENT_BINARY_DIR}/keyvalue.pb.cc")
    set(GRPC_PROTO_HDRS "${CMAKE_CURRENT_BINARY_DIR}/keyvalue.pb.h")
    set(GRPC_SRCS "${CMAKE_CURRENT_BINARY_DIR}/keyvalue.grpc.pb.cc")
    set(GRPC_HDRS "${CMAKE_CURRENT_BINARY_DIR}/keyvalue.grpc.pb.h")

    add_custom_command(
        OUTPUT "${GRPC_PROTO_SRCS}" "${GRPC_PROTO_HDRS}" "${GRPC_SRCS}" "${GRPC_HDRS}"
        COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}
        ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
             --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
             -I "${GRPC_PROTO_DIR}"
             --plugin=protoc-gen-grpc="${GRPC_CPP_PLUGIN}"
             "${GRPC_PROTO_PATH}"
        DEPENDS "${GRPC_PROTO}"
    )

    # Server
    add_executable(kv_server
        03_grpc/kv_server.cpp
        ${GRPC_PROTO_SRCS}
        ${GRPC_SRCS}
    )

    # Client
    add_executable(kv_client
        03_grpc/kv_client.cpp
        ${GRPC_PROTO_SRCS}
        ${GRPC_SRCS}
    )

    if(gRPC_FOUND)
        target_link_libraries(kv_server gRPC::grpc++ protobuf::libprotobuf)
        target_link_libraries(kv_client gRPC::grpc++ protobuf::libprotobuf)
    else()
        target_link_libraries(kv_server ${GRPC_LIBRARIES} ${PROTOBUF_LIBRARIES})
        target_link_libraries(kv_client ${GRPC_LIBRARIES} ${PROTOBUF_LIBRARIES})
    endif()
else()
    message(WARNING "gRPC not found, skipping Module 03")
    message(STATUS "Install gRPC: apt-get install libgrpc++-dev (Ubuntu)")
    message(STATUS "           or: brew install grpc (macOS)")
endif()
