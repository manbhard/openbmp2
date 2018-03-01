cmake_minimum_required(VERSION 2.8)
# find openssl
find_package( OpenSSL 1)
if (OPENSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIR})
    set (SSL_LIBS ${OPENSSL_LIBRARIES} sasl2)
else()
    set(SSL_LIBS )
endif()

# Protobuf
include(FindProtobuf)
find_package(Protobuf REQUIRED)

find_package(ZLIB REQUIRED)

# gRPC C++ plugin
get_target_property(gRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin
        LOCATION)

# Proto file
get_filename_component(openbmp_proto "../../proto/openbmp.proto" ABSOLUTE)
get_filename_component(openbmp_proto_path "${openbmp_proto}" PATH)

# Generated sources
protobuf_generate_cpp(openbmp_proto_srcs openbmp_proto_hdrs "${openbmp_proto}")
set(openbmp_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/openbmp.grpc.pb.cc")
set(openbmp_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/openbmp.grpc.pb.h")
#TODO: The grpc_cpp_plugin path should be relative
add_custom_command(
        OUTPUT "${openbmp_grpc_srcs}" "${openbmp_grpc_hdrs}"
        COMMAND protoc
        ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}" -I "${openbmp_proto_path}"
        --plugin=protoc-gen-grpc="/usr/local/bin/grpc_cpp_plugin"
        "${openbmp_proto}"
        DEPENDS "${openbmp_proto}")

set(HINT_ROOT_DIR
        "${HINT_ROOT_DIR}"
        CACHE
        PATH
        "Where to start looking for this component.")

find_path(LIBGRPC++_INCLUDE_DIR
        NAMES
        grpc++/grpc++.h
        HINTS
        ${HINT_ROOT_DIR}
        PATH_SUFFIXES
        include)

find_library(LIBGRPC++_LIBRARY
        NAMES
        libgrpc++.a grpc++
        HINTS
        ${HINT_ROOT_DIR}
        PATH_SUFFIXES
        lib64
        lib)

find_path(LIBGRPC_INCLUDE_DIR
        NAMES
        grpc/grpc.h
        HINTS
        ${HINT_ROOT_DIR}
        PATH_SUFFIXES
        include)

find_library(LIBGRPC_LIBRARY
        NAMES
        libgrpc.a grpc
        HINTS
        ${HINT_ROOT_DIR}
        PATH_SUFFIXES
        lib64
        lib)

if (NOT LIBGRPC++_INCLUDE_DIR OR NOT LIBGRPC++_LIBRARY)
    Message (FATAL_ERROR "Libgrpc++ was not found, cannot proceed.  Visit https://github.com/grpc/grpc/blob/master/INSTALL.md for install details.")
endif()

if (NOT LIBGRPC_INCLUDE_DIR OR NOT LIBGRPC_LIBRARY)
    Message (FATAL_ERROR "Libgrpc was not found, cannot proceed.  Visit https://github.com/grpc/grpc/blob/master/INSTALL.md for install details.")
endif()

# Update the include dir
include_directories(. ${PROTOBUF_INCLUDE_DIR} ${LIBGRPC++_INCLUDE_DIR} ${LIBGRPC_INCLUDE_DIR} ${ZLIB_INCLUDE_DIRS})
#link_directories(${LIBRDKAFKA_LIBRARY})


# Define the source files to compile
set (TEST_FILES
        Grpc_client.cpp
        )


# Disable warnings
add_definitions ("-Wno-unused-result")

# Add C++11
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR CMAKE_COMPILER_IS_GNUCXX)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(--std=c++11 SUPPORTS_STD_CXX11)
    check_cxx_compiler_flag(--std=c++0x SUPPORTS_STD_CXX01)
    if(SUPPORTS_STD_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --std=c++11")
    elseif(SUPPORTS_STD_CXX01)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++0x")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --std=c++0x")
    else()
        message(ERROR "Compiler does not support --std=c++11 or --std=c++0x. Upgrade gcc 4.7 or greater")
    endif()
endif()

# Set the libs to link
set (LIBS ${LIBGRPC++_LIBRARY} ${LIBGRPC_LIBRARY} ${PROTOBUF_LIBRARY} ${ZLIB_LIBRARIES} pthread rt dl)

# Set the binary
add_executable (openbmpd_client ${TEST_FILES} ${openbmp_proto_srcs} ${openbmp_grpc_srcs})

# Link the binary
target_link_libraries (openbmpd_client ${LIBS})

# Install the binary and configs
install(TARGETS openbmpd_client DESTINATION bin COMPONENT binaries)