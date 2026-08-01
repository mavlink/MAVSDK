# C wrappers

This defines C wrappers around the C++ implementation of MAVSDK.

## Generate code

In order to generate the plugins from the proto submodule, run:

```
./tools/generate_from_protos.bash
```

## Build

This is a normal CMake project. Note that it depends on the C++ MAVSDK library and,
if MAVSDK is built as a static library, the MAVSDK dependencies.

An example configuring it is as follows:

```
cmake -DCMAKE_INSTALL_PREFIX=build/install \
-DCMAKE_PREFIX_PATH="/path/to/mavsdk/build/install;/path/to/mavsdk/build/third_party/install" \
-DBUILD_SHARED_LIBS=ON \
-Bbuild -S.
```

Then build with the standard CMake build command, for instance:

```
cmake --build build --target install
```

## Memory ownership

**The receiver owns everything this API produces, including values delivered to
callbacks.**

Structs, strings, and arrays handed out by this library are freshly allocated —
strings with `strdup`, arrays with `new[]`, and struct string fields with
`strdup` inside the `translate_*_to_c` helpers. Nothing is freed on your behalf,
and nothing is reclaimed when a callback returns. Every such value must be
released with its matching destructor:

| What you received | How to release it |
| --- | --- |
| `mavsdk_<plugin>_<struct>_t` | `mavsdk_<plugin>_<struct>_destroy(&value)` |
| `mavsdk_<plugin>_<struct>_t*` + count | `mavsdk_<plugin>_<struct>_array_destroy(&values, count)` |
| `<primitive>*` | `mavsdk_<plugin>_<primitive>_array_destroy(&values)` |
| `char*` | `mavsdk_<plugin>_string_destroy(&str)` |
| `uint8_t*` | `mavsdk_<plugin>_byte_buffer_destroy(&buffer)` |

Call the destructor even when the struct currently holds no dynamic
allocations — whether it does is an implementation detail that can change when
the protos change.

This applies equally on both paths, and the callback path is the easy one to get
wrong: a subscription that fires at telemetry rates leaks on every message. In
C++ wrappers, release the value with an RAII guard declared before any early
return, so it cannot be skipped.
