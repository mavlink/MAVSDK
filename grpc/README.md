# DroneCore with GRPC

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
./build/default/grpc/server/src/dronecore_server
```

## Notes

- `unset http_proxy` if set before starting server
