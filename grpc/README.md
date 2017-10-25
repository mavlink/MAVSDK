# DroneCore with GRPC

## Prerequisites

### Install grpc

Instructions can be found on https://github.com/grpc/grpc/blob/v1.6.x/INSTALL.md.

We suggest the following steps:

1. Create folders for custom grpc and protobuf install that we do not want to interfere with the rest of the system:

    ```
    sudo mkdir /opt/grpc
    sudo chown $USER:$USER /opt/grpc
    sudo mkdir /opt/protobuf
    sudo chown $USER:$USER /opt/protobuf
    ```

2. Build and install grpc:

    ```
    git clone -b v1.6.x https://github.com/grpc/grpc
    cd grpc
    git submodule update --init
    git apply <wherever>/DroneCore/grpc/0001-Makefile-fix-wrong-SHARED-version-suffix.patch
    make HAS_SYSTEM_PROTOBUF=false -j8
    make prefix=/opt/grpc install
    cd ..
    ```

3. Build and install protobuf:

    ```
    cd grpc/third_party/protobuf
    make clean
    ./configure --prefix=/opt/protobuf
    make -j8
    make install
    cd ../../../
    ```

4. To use these installed libraries, set these environment variables before building:
    ```
    export GRPC_DIR=/opt/grpc
    export PROTOBUF_DIR=/opt/protobuf
    ```


### Install grpcio_tools for Python

```
sudo pip install grpcio_tools
```

## Build

### DroneCore

All that is required to build server and Python client is:

```
make default
```

## Test

To test start PX4 SITL first.

```
cd wherever/Firmware
make posix jmavsim
```

### Server

Then start the server:

```
build/default/grpc/server/dronecore_server
```

### Python client

To use the Python

```
touch build/default/grpc/python_client/__init__.py
export PYTHONPATH="$PYTHONPATH:`pwd`/build/default/grpc/python_client"
./grpc/python_client/sync_client.py
```

## Notes

- `unset http_proxy` if set before starting server


If you run into linking errors on dronecore_server start like:
```
error while loading shared libraries: libgrpc++.so.1: cannot open shared object file: No such file or directory
```

Make sure to apply the patch 0001-Makefile-fix-wrong-SHARED-version-suffix.patch before building gRPC.

If the gRPC build fails because of warnings, you need to remove all `-Werror` flags in the gRPC `Makefile`.
