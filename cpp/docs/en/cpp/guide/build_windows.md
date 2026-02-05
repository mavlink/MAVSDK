# Building MAVSDK on Windows

This guide explains how to build MAVSDK from source on Windows systems.

::: warning
Building using Cygwin or MinGW is not supported. These instructions are for building with MSVC (Microsoft Visual C++).
:::

## Requirements

The build requirements are:
- Build Tools for Visual Studio 2022 (including Ninja)
- CMake
- Git
- Perl (for OpenSSL unless building without CURL)
- jom (optional) to parallelize the OpenSSL build

### Install Build Tools

1. Install [Build Tools for Visual Studio 2022](https://www.visualstudio.com/downloads/)
   - Only the "Visual C++ Build Tools" are needed from the installer

2. Install [CMake](https://cmake.org/download/)
   - Download and run the installer
   - Make sure to tick "add to PATH" during installation

3. Install [Git](https://git-scm.com/download/win)
   - Choose any Git client you prefer

4. Install [Strawberry Perl](https://strawberryperl.com/) (required for OpenSSL unless building without CURL and not mavsdk_server):
   - After installation, you may want to modify the PATH to only include the Perl binary path, otherwise it will pick up cmake and libraries from the Strawberry Perl installation.
   - To edit Windows paths:
     1. Go to System -> Advanced system settings -> Environment Variables
     2. Edit the System variable `Path`
     3. Remove `C:\Strawberry\c\bin` and `C:\Strawberry\perl\site\bin`
     4. Keep only `C:\Strawberry\perl\bin`

5. (Optional) Install [jom](https://download.qt.io/official_releases/jom/) and add it to the path.

## Getting the Source

Download the source using git:
```bash
git clone https://github.com/mavlink/MAVSDK.git
cd MAVSDK
git submodule update --init --recursive
```

## Building the MAVSDK library only
::: Tip
Make sure to run this within the Visual Studio environment to access Ninja. You can either run the "x64 Native Tools Command Prompt for VS 2022" program or call the vcvarsall.bat script located in your Visual Studio 2022 installation directory:
`C:\Program Files\Microsoft Visual Studio\2022\EDITION\VC\Auxiliary\Build\vcvarsall.bat`
:::
### Debug Build

For development, use the debug build:
```bash
cmake -GNinja -DCMAKE_BUILD_TYPE=Debug -Bbuild -S.
cmake --build build --config Debug
```

### Release Build

For production use, build with optimizations enabled:

```bash
cmake -GNinja -DCMAKE_BUILD_TYPE=Release -S. -Bbuild
cmake --build build --config Release
```

::: tip
It is not usual to use CMAKE_BUILD_TYPE on Windows (with MSVC), however, our build requires it for the dependencies which are built at configure time.
:::

### Shared vs. static library build

There are (at least) two ways to build the MAVSDK library:
- As a shared library (.dll) including all dependencies statically.
- As a static library (.lib) **not** including the dependencies.

By default a shared library (.dll) is built which is easier to handle because it can be used standalone.

If a static library needs to be built, this can be done using `-DCMAKE_BUILD_SHARED_LIBS=ON`.
Note that a `mavsdk.lib` built this way does not "contain" the dependencies it was built with, so the CMAKE_PREFIX_PATH used to build the library needs to be passed to any binary which is built on top of it.

For instance, to build the examples, it looks like this:

```bash
cmake -GNinja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=install -DBUILD_MAVSDK_SERVER=ON -DBUILD_SHARED_LIBS=OFF -DWERROR=ON -Bbuild -S.
cmake --build build --config RelWithDebInfo --target install
```

And now the examples need both dependencies: mavsdk.lib but also its transitive dependencies:

```bash
cd examples
cmake -G"Ninja" -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_PREFIX_PATH="../install;../build/third_party/install" -DCMAKE_INSTALL_PREFIX=install -Bbuild -S.
cmake --build build
```

## Installation

### System-wide Installation

To install system-wide (requires admin privileges):
```bash
# Run in command prompt with admin privileges
cmake --build build --target install
```

### Local Installation

To install to a custom location:
```bash
cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install -Bbuild -S.
cmake --build build --target install
```

## Build mavsdk_server binary on Windows

Language wrappers for MAVSDK other than C++ connect to the MAVSDK C++ core using gRPC. This gRPC server around the MAVSDK C++ library is called mavsdk_server (in the past it was referred to as the backend).

For more information about the architecture, also see [how the auto-generation works](../contributing/autogen.md).

In order to include the mavsdk_server in the build, add `-DBUILD_MAVSDK_SERVER=ON`. In order to avoid a linking issue on Windows (where more than 65535 objects are present, we need to build all static libraries like so:

```bash
cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DBUILD_MAVSDK_SERVER=ON -Bbuild -S.
cmake --build build
```

## Build Options

During the configure step you can set various flags using `-DFLAG=Value`:

- `CMAKE_BUILD_TYPE`: Choose between `Debug`, `Release`, or `RelWithDebInfo` build.
- `CMAKE_INSTALL_PREFIX`: Specify directory to install library artifacts
- `BUILD_SHARED_LIBS`: Set to `ON` for dynamic libraries (.dll), `OFF` for static libraries (.lib)
- `SUPERBUILD`: Set to `OFF` to use system dependencies instead of third party dependencies
- `CMAKE_PREFIX_PATH`: Set path where dependencies can be found if `SUPERBUILD` is `OFF`
- `BUILD_MAVSDK_SERVER`: Set to `ON` to build mavsdk_server
- `BUILD_WITHOUT_CURL`: Set to `ON` to build without CURL support (avoids OpenSSL/Perl requirement)
- `WERROR`: Set to `ON` to treat warnings as errors

## Troubleshooting

### Git Submodules Out of Date

If you encounter build issues, try updating the submodules:
```bash
git submodule update --recursive
rm -rf build
```

### Build Without CURL

If you don't need http/https downloads for camera definition files or other component definition files, and you're not building the MAVSDK server (`BUILD_MAVSDK_SERVER=OFF`), you can exclude CURL and OpenSSL using:
```bash
cmake -GNinja -DBUILD_WITHOUT_CURL=ON -Bbuild -S.
```

This eliminates the need for Perl/OpenSSL setup.

### Strange errors such as "Invalid connection URL"

When mixing build types such as linking a Debug application against a Release MAVSDK library (or vice versa), you may encounter runtime crashes or strange error messages. This is due to ABI (Application Binary Interface) incompatibility between Debug and Release builds in MSVC.

**Common symptoms:**
- Runtime crashes when passing `std::string` objects between your application and MAVSDK
- "Invalid connection URL" errors with valid connection strings
- Memory corruption or access violations
- Seemingly random crashes when calling MAVSDK functions

**Root cause:**
MSVC uses different memory layouts and allocators for `std::string` in Debug vs Release builds. When a Debug application passes a `std::string` to a Release MAVSDK library, the internal structure mismatch causes corruption.

**Solution:**
Ensure your application build configuration matches the MAVSDK library configuration:

1. **Using pre-built packages**: The dual-configuration packages automatically handle this by selecting the matching configuration based on your `CMAKE_BUILD_TYPE`

2. **When building from source**: Use the same `CMAKE_BUILD_TYPE` for both MAVSDK and your application:
   ```bash
   # For Debug applications
   cmake -GNinja -DCMAKE_BUILD_TYPE=Debug -Bbuild -S.

   # For Release applications
   cmake -GNinja -DCMAKE_BUILD_TYPE=Release -Bbuild -S.
   ```

3. **Mixed configurations are not supported**: Never link a Debug application against a Release MAVSDK library or vice versa on Windows.
