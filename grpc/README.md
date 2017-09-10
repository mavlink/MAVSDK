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
./grpc/python_client/dronecore_client.py
```

## Notes

- `unset http_proxy` if set before starting server

