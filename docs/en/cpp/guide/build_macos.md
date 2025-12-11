# Building MAVSDK on macOS

This guide explains how to build MAVSDK from source on macOS systems.

## Requirements

The build requirements are:
- XCode Command Line Tools
- CMake
- Git

### Install Build Tools

First install XCode Command line tools:
```bash
xcode-select --install
```

Then install [Homebrew](https://brew.sh/) and use it to install CMake:
```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
brew install cmake
```

## Getting the Source

Download the source using git:
```bash
git clone https://github.com/mavlink/MAVSDK.git
cd MAVSDK
git submodule update --init --recursive
```

## Building the MAVSDK library on macOS

### Debug Build

For development, use the debug build:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug -Bbuild -S.
cmake --build build -j8
```

### Release Build

For production use, build with optimizations enabled:
```bash
cmake -DCMAKE_BUILD_TYPE=Release -Bbuild -S.
cmake --build build -j8
```

## Installation

### System-wide Installation

To install system-wide in `/usr/local`:
```bash
sudo cmake --build build --target install
```

### Local Installation

To install to a local folder, e.g. `install`:
```bash
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install -Bbuild -S.
cmake --build build --target install
```

## Build for iOS {#build_cpp_iOS}

To build for real iOS devices on macOS:

```bash
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_MAVSDK_SERVER=ON -DBUILD_SHARED_LIBS=OFF -DCMAKE_TOOLCHAIN_FILE=tools/ios.toolchain.cmake -DPLATFORM=OS -Bbuild/ios -S.
cmake --build build/ios
```

Build for the iOS simulator on macOS:

```bash
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_MAVSDK_SERVER=ON -DBUILD_SHARED_LIBS=OFF -DCMAKE_TOOLCHAIN_FILE=tools/ios.toolchain.cmake -DPLATFORM=SIMULATOR64 -Bbuild/ios_simulator -S.
```

## Build Options

During the configure step you can set various flags using `-DFLAG=Value`:

- `CMAKE_BUILD_TYPE`: Choose between `Debug` and `Release` build
- `CMAKE_INSTALL_PREFIX`: Specify directory to install library artifacts
- `BUILD_SHARED_LIBS`: Set to `ON` for dynamic libraries (.dylib), `OFF` for static libraries (.a)
- `SUPERBUILD`: Set to `OFF` to use system dependencies instead of third party dependencies
- `CMAKE_PREFIX_PATH`: Set path where dependencies can be found if `SUPERBUILD` is `OFF`
- `BUILD_MAVSDK_SERVER`: Set to `ON` to build mavsdk_server
- `BUILD_WITHOUT_CURL`: Set to `ON` to build without CURL support
- `ASAN`: Set to `ON` to enable address sanitizer
- `UBSAN`: Set to `ON` to enable undefined behavior sanitizer
- `LSAN`: Set to `ON` to enable leak sanitizer
- `WERROR`: Set to `ON` to treat warnings as errors

## Troubleshooting

### Git Submodules Out of Date

If you encounter build issues, try updating the submodules:
```bash
git submodule update --recursive
``` 