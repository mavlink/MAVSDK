# MAVSDK for Python

Python binding for [MAVSDK](https://mavsdk.mavlink.io), a library to communicate with MAVLink systems such as drones, cameras or ground stations.

It calls the MAVSDK C++ library directly (through its C wrapper), so there is no gRPC dependency and no `mavsdk_server` process to run.

The package exposes two interfaces:

* `mavsdk`: a synchronous API using threads and callbacks.
* `mavsdk.asyncio`: an asyncio API.

Both are generated from the same protos and ship in the same distribution.

```sh
pip install mavsdk
```

> **Coming from MAVSDK-Python (`mavsdk` 3.x and earlier)?**
>
> Up to 3.x, `mavsdk` was the gRPC-based [MAVSDK-Python](https://github.com/mavlink/MAVSDK-Python), which has a different API. It continues as [`mavsdk-grpc`](https://pypi.org/project/mavsdk-grpc/), imported as `mavsdk_grpc`. To keep using that API, install `mavsdk-grpc`, or pin `mavsdk<4`. To move to this package, see the [migration guide](https://mavsdk.mavlink.io/main/en/python/migration.html).

## Examples

Find examples for the synchronous API in [`examples/`](https://github.com/mavlink/MAVSDK/tree/main/py/mavsdk/examples), and for the asyncio one in [`examples/asyncio/`](https://github.com/mavlink/MAVSDK/tree/main/py/mavsdk/examples/asyncio).

## Development

### Generate code

In order to generate the plugins from the proto submodule, run:

```
./tools/generate_from_protos.bash
```

This generates both interfaces: the synchronous plugins into `mavsdk/plugins` from [`templates/sync`](https://github.com/mavlink/MAVSDK/tree/main/py/mavsdk/templates/sync), and the asyncio ones into `mavsdk/asyncio/plugins` from [`templates/asyncio`](https://github.com/mavlink/MAVSDK/tree/main/py/mavsdk/templates/asyncio).

### Running it from source

This project relies on the C MAVSDK library, which it wraps. The library must be built separately from the [C wrapper](https://github.com/mavlink/MAVSDK/tree/main/c) and copied into `mavsdk/lib/libcmavsdk.so`.
