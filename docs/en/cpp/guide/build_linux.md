# Building MAVSDK on Linux

This guide explains how to build MAVSDK from source on Linux systems.

## Requirements

The build requirements are git, cmake, and a C++ compiler (GCC or Clang), and python.

### Ubuntu
```bash
sudo apt-get update
sudo apt-get install build-essential cmake git python3 python3-pip
```

### Fedora
```bash
sudo dnf update
sudo dnf groupinstall "Development Tools" "Development Libraries"
sudo dnf install cmake git
```

## Getting the Source

Download the source using git:
```bash
git clone https://github.com/mavlink/MAVSDK.git
cd MAVSDK
git submodule update --init --recursive
cd cpp
```

## Building the MAVSDK library on Linux

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
sudo ldconfig  # Update linker cache
```

### Local Installation

To install to a custom location, e.g. `install`

```bash
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install -Bbuild -S.
cmake --build build --target install
```

## Build mavsdk_server binary on Linux

Language wrappers for MAVSDK other than C++ connect to the MAVSDK C++ core using gRPC. This gRPC server around the MAVSDK C++ library is called mavsdk_server (in the past it was referred to as the backend).

For more information about the architecture, also see [how the auto-generation works](../contributing/autogen.md).

In order to include the mavsdk_server in the build, add `-DBUILD_MAVSDK_SERVER=ON`:

```bash
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_MAVSDK_SERVER=ON -Bbuild -S.
cmake --build build -j8
```

## Build Options

During the configure step you can set various flags using `-DFLAG=Value`:

- `CMAKE_BUILD_TYPE`: Choose between `Debug`, `Release`, or `RelWithDebInfo` (optimizations and debug symbols)
- `CMAKE_INSTALL_PREFIX`: Specify directory to install library artifacts
- `BUILD_SHARED_LIBS`: Set to `ON` for dynamic libraries (.so), `OFF` for static libraries (.a)
- `SUPERBUILD`: Set to `OFF` to use system dependencies instead of third party dependencies (see [Building without Superbuild](#building-without-superbuild))
- `CMAKE_PREFIX_PATH`: Set path where dependencies can be found if `SUPERBUILD` is `OFF`
- `BUILD_MAVSDK_SERVER`: Set to `ON` to build mavsdk_server
- `BUILD_WITHOUT_CURL`: Set to `ON` to build without CURL support
- `ASAN`: Set to `ON` to enable address sanitizer
- `UBSAN`: Set to `ON` to enable undefined behavior sanitizer
- `LSAN`: Set to `ON` to enable leak sanitizer
- `WERROR`: Set to `ON` to treat warnings as errors

### MAVLink Configuration

MAVSDK supports configuring the MAVLink dialect and repository:

- `MAVLINK_DIALECT`: MAVLink dialect to use (default: `ardupilotmega`)
- `MAVLINK_XML_PATH`: Path to local MAVLink XML definition files. If not set, definitions are fetched automatically.
- `MAVLINK_URL`: URL for the MAVLink repository (default: `https://github.com/mavlink/mavlink`)
- `MAVLINK_HASH`: Git commit hash to use from the MAVLink repository

> **Note:** You can also load custom MAVLink message definitions at runtime using the [MavlinkDirect](../api_reference/classmavsdk_1_1_mavlink_direct.md) plugin, without needing to rebuild MAVSDK.

Examples:

```bash
# Build with development dialect
cmake -DCMAKE_BUILD_TYPE=Debug -DMAVLINK_DIALECT=development -Bbuild -S.

# Use a custom MAVLink repository fork
cmake -DCMAKE_BUILD_TYPE=Debug \
    -DMAVLINK_URL=https://github.com/yourfork/mavlink \
    -DMAVLINK_HASH=abc123def456 \
    -Bbuild -S.

# Use local MAVLink XML files
cmake -DCMAKE_BUILD_TYPE=Debug \
    -DMAVLINK_XML_PATH=/path/to/mavlink/message_definitions/v1.0 \
    -Bbuild -S.
```

## Building without Superbuild

By default, MAVSDK uses a "superbuild" that automatically downloads and builds all required dependencies. If you prefer to provide dependencies yourself (e.g., from system packages or custom builds), you can disable this with `SUPERBUILD=OFF`.

A script is provided that demonstrates how to build all dependencies and MAVSDK:

```bash
tools/build-with-system-deps.sh
```

This script:
1. Clones and builds the required dependencies (MAVLink, libevents, PicoSHA2, libmav) into a local `deps/` directory
2. Installs them to `deps-install/`
3. Builds MAVSDK with `SUPERBUILD=OFF` using these dependencies

Prerequisites (install before running the script):
```bash
sudo apt install build-essential cmake git python3 python3-pip \
                 liblzma-dev libtinyxml2-dev libjsoncpp-dev \
                 libcurl4-openssl-dev libssl-dev
```

## Troubleshooting

### Git Submodules Out of Date

If you encounter build issues, try updating the submodules:
```bash
git submodule update --recursive
```

### Undefined References

If you get undefined reference errors after installation, update the linker cache:
```bash
sudo ldconfig
``` 

And try a clean build by wiping the build directory:
```bash
rm -r build
```
