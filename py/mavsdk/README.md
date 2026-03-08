# Python wrappers

This defines Python wrappers around the C implementation of MAVSDK.

It is still new, but the idea is to eventually replace
[MAVSDK-Python](https://github.com/mavlink/mavsdk-python). The difference is
that this does *not* depend on gRPC; instead it calls the C wrapper directly and
the resulting Python package has no dependency.

Another difference with MAVSDK-Python is that it does not expose an asyncio
API. It's old school threads and callbacks here! We also have a new
wrapper called [aiomavsdk](../aiomavsdk) that wraps this library with an asyncio API.

## Examples

Find examples in [`examples/`](examples).

## Generate code

In order to generate the plugins from the proto submodule, run:

```
./tools/generate_from_protos.bash
```

## Running it

This project relies on the C MAVSDK library, which it wraps. The library must be
built separately from the [C wrapper](../../c) and copied into [mavsdk/lib/libcmavsdk.so](mavsdk/lib).
