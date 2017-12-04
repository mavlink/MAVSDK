
find_package(GRPC)
if(GRPC_FOUND)

    # If protobuf is installed to e.g. /opt/protobuf, you need to specifiy this via environment variable:
    # `export PROTOBUF_DIR=/opt/protobuf`
    #
    if (DEFINED ENV{PROTOBUF_DIR})
        message(STATUS "PROTOBUF_DIR manually set, using protobuf from $ENV{PROTOBUF_DIR}")
        set(Protobuf_INCLUDE_DIRS "$ENV{PROTOBUF_DIR}/include")
        set(Protobuf_PROTOC_EXECUTABLE "$ENV{PROTOBUF_DIR}/bin/protoc")
        set(Protobuf_PROTOC_LIBRARY_DIR "$ENV{PROTOBUF_DIR}/lib")
        set(Protobuf_PROTOC_LIBRARIES "$ENV{PROTOBUF_DIR}/lib/libprotoc.so")
        set(Protobuf_LIBRARIES "$ENV{PROTOBUF_DIR}/lib/libprotobuf.so")
    else()
        message(STATUS "PROTOBUF_DIR not set, checking automatically for Protobuf")
        find_package(Protobuf REQUIRED)
    endif()

    add_subdirectory(server)
    add_subdirectory(python_client)
else()
    message(STATUS "gRPC not found, skipping gRPC build")
endif()
