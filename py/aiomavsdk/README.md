# Python wrappers

This defines asyncio wrappers around the [`py/mavsdk`](../mavsdk) wrapper of MAVSDK.

It is still new, but the idea is to eventually replace
[MAVSDK-Python](https://github.com/mavlink/mavsdk-python). The difference is
that this does *not* depend on gRPC; instead it calls the C wrapper directly and
the resulting Python package has no dependency.

This exposes an asyncio API, trying to be similar to
[MAVSDK-Python](https://github.com/mavlink/mavsdk-python), hopefully
facilitating transition. It wraps [`py/mavsdk`](../mavsdk) which is more of an
"old school" Python API (no asyncio: just threads and callbacks).

## Examples

Find examples in [`examples/`](examples).

## Generate code

In order to generate the plugins from the proto submodule, run:

```
./tools/generate_from_protos.bash
```

## Running it

This project depends on [`py/mavsdk`](../mavsdk), which must therefore be
installed.
