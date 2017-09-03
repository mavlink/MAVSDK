# DroneCore with GRPC

## Prerequisites

### Install grpc

Follow the instructions on https://github.com/grpc/grpc/blob/v1.6.x/INSTALL.md.

### Install grpcio_tools for Python

```
sudo pip install grpcio_tools
```

## Build

### DroneCore

```
make default install
```

### Server

```
cd grpc/server/
protoc -I ../proto --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ../proto/dronecore.proto
protoc -I ../proto --cpp_out=. ../proto/dronecore.proto
make
cd -
```

### Python client

```
cd grpc/python_client/
python -m grpc_tools.protoc -I../proto --python_out=. --grpc_python_out=. ../proto/dronecore.proto
cd -
```

## Test

To test Python client, start PX4 SITL, and run Python test script:

```
./grpc/python_client/dronecore_client.py
```

## Notes

- `unset http_proxy` if set

