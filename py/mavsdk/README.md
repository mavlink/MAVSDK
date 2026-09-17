# Python wrappers

This defines Python wrappers around the C implementation of MAVSDK.

It is still new, but the idea is to eventually replace
[MAVSDK-Python](https://github.com/mavlink/mavsdk-python). The difference is
that this does *not* depend on gRPC; instead it calls the C wrapper directly and
the resulting Python package has no dependency.

The package exposes two interfaces:

* `mavsdk` — old school threads and callbacks, no asyncio.
* `mavsdk.asyncio` — an asyncio API, closer to what MAVSDK-Python offers.

Both are generated from the same protos and ship in the same distribution.

## Examples

Find examples for the synchronous API in [`examples/`](examples), and for the
asyncio one in [`examples/asyncio/`](examples/asyncio).

## Generate code

In order to generate the plugins from the proto submodule, run:

```
./tools/generate_from_protos.bash
```

This generates both interfaces: the synchronous plugins into `mavsdk/plugins`
from [`templates/sync`](templates/sync), and the asyncio ones into
`mavsdk/asyncio/plugins` from [`templates/asyncio`](templates/asyncio).

## Running it

This project relies on the C MAVSDK library, which it wraps. The library must be
built separately from the [C wrapper](../../c) and copied into `mavsdk/lib/libcmavsdk.so`.
