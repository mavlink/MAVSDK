# Build mavsdk_server from Source

Language wrappers for MAVSDK other than C++ connect to the MAVSDK C++ core using gRPC. This gRPC server around the MAVSDK C++ library is called mavsdk_server (in the past it was referred to as the backend).

For more information about the architecture, also see [how the auto-generation works](../contributing/autogen.md).

> **Note** It is generally recommended to use pre-built binaries of mavsdk_server from the [releases page](https://github.com/mavlink/MAVSDK/releases), however, if needed, it can also be built manually from source as described below.

## Requirements

The requirements are the same as described in the [library build instructions](build.md#requirements)

## Build on Linux

```
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DBUILD_MAVSDK_SERVER=ON -Bbuild/default -H.
cmake --build build/default -j8
```

## Build on macOS

```
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DBUILD_MAVSDK_SERVER=ON -Bbuild/default -H.
cmake --build build/default -j8
```

> **Note:** there is no proper support for the Apple M1 chip yet. One blocker is that there is currently no M1 hardware in GitHub Actions CI available.

## Build on Windows

Open the "x64 Native Tools Command Prompt for VS 2022", cd into the MAVSDK directory, and do:

```
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_MAVSDK_SERVER=ON -Bbuild/default -H.
cmake --build build/default -j8
```

## Build for Android {#build_cpp_android}

Build for Android using the dockcross cross compiler, as described in the [cross-compilation](#cross_compilation_dockcross) section below (e.g. with the `dockcross/android-arm64` image).

## Build for iOS {#build_cpp_iOS}

To build for real iOS devices on macOS:

```sh
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_MAVSDK_SERVER=ON -DBUILD_SHARED_LIBS=OFF -DCMAKE_TOOLCHAIN_FILE=tools/ios.toolchain.cmake -DPLATFORM=OS -Bbuild/ios -H.
cmake --build build/ios
```

Build for the iOS simulator on macOS:

```sh
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_MAVSDK_SERVER=ON -DBUILD_SHARED_LIBS=OFF -DCMAKE_TOOLCHAIN_FILE=tools/ios.toolchain.cmake -DPLATFORM=SIMULATOR64 -Bbuild/ios_simulator -H.
```

## Cross compilation using dockcross {#cross_compilation_dockcross}

Cross compilation is usually the fastest way to compile for "embedded" platforms like the Raspberry Pi, BeagleBone Blue or Nvidia Jetson (i.e. typically faster than native compilation on device itself).

We recommend using [dockcross](https://github.com/dockcross/dockcross), which is a very convenient tool for cross compilation based on docker (and which supports many platforms).

For example, use the commands below to build for `armv7`:

1. Make sure docker is installed on your system.
2. Navigate into the SDK directory, and use the commands below:
   ```
   cd MAVSDK
   docker run --rm dockcross/linux-armv7 > ./dockcross-linux-armv7
   chmod +x ./dockcross-linux-armv7
   ./dockcross-linux-armv7 cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_MAVSDK_SERVER=ON -DBUILD_SHARED_LIBS=OFF -Bbuild/linux-armv7 -S.
   ./dockcross-linux-armv7 cmake --build build/linux-armv7 -j 8
   ```
3. If all goes well, `mavsdk_server` is built. You can check the file with:
   ```
   ./dockcross-linux-armv7 file build/linux-armv7-release/src/backend/src/mavsdk_server
   build/linux-armv7-release/src/backend/src/mavsdk_server: ELF 32-bit LSB executable, ARM, EABI5 version 1 (GNU/Linux), dynamically linked, interpreter /lib/ld-linux-armhf.so.3, for GNU/Linux 4.10.8, not stripped
   ```
4. The newly built binary can now be copied to the device:
   ```
   cp build/linux-armv7/src/backend/src/mavsdk_server somewhere/else
   ```
