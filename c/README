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
