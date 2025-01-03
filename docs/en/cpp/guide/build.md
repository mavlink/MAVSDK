# Building MAVSDK Library from Source

This section explains how to build and install the MAVSDK C++ library from source for all our target platforms.

Note that there are separate instructions to [build the mavsdk_server](build_mavsdk_server.md).

## Build the C++ Library {#build_mavsdk_cpp_library}

This section explains how to build the library along with its unit and integration tests.

### Requirements {#requirements}

The build requirements are git, cmake, and a compiler such as GCC, Clang, or MSVC.

#### Linux

**Ubuntu**:
```bash
sudo apt-get update
sudo apt-get install build-essential cmake git
```

**Fedora**:
```bash
sudo dnf update
sudo dnf groupinstall "Development Tools" "Development Libraries"
sudo dnf install cmake git
```

**Arch Linux**:
```bash
sudo pacman -Sy base-devel cmake git
```

#### macOS

First install XCode Command line tools:
```
xcode-select --install
```

And [Homebrew](https://brew.sh/) (for cmake).
Once you have installed brew, you can install `cmake` using `brew` in the terminal:
```
brew install cmake
```

#### Windows

To build the library in Windows, you need:

- [Build Tools for Visual Studio 2022](https://www.visualstudio.com/downloads/): Download and install (only the "Visual C+ Build Tools" are needed from installer).
- [cmake](https://cmake.org/download/): Download the installer and run it.
  Make sure to tick "add to PATH" during the installation.
- [git](https://git-scm.com/download/win) or any other tool to work with git.

> Note: Building using Cygwin or MingW is not supported.

### Getting the source

Download the source using git:
```bash
git clone https://github.com/mavlink/MAVSDK.git
```

Make sure to get all the submodules as well:
```
git submodule update --init --recursive
```

### Building Fundamentals

Building the MAVSDK with CMake is broken down into two steps: **Configuration** and **Build**.

#### Build Configuration Step {#configuration_step}
For configuration, you specify the type of build you want to execute in the [build step](#build_step). You can checkout the [CMake Documentation on Build Configuration](https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#build-configurations) for more context.

A typical configuration command example would be:

`cmake -DCMAKE_BUILD_TYPE=Debug -Bbuild/default -H.`

- Build type is set to `Debug`
- Build directory is set to `build/default`
- Chose the directory using the `H.` flag

During the configure step you can set more flags using `-DFLAG=Value`:

- `CMAKE_BUILD_TYPE`: as documented above, to chose between `Debug` and `Release` build.
- `CMAKE_INSTALL_PREFIX`: as documented above, to specify directory to install library artifacts.
- `BUILD_SHARED_LIBS`: set to `ON` to build dynamic libraries (such as .so on Linux, .dylib on macOS, .dll on Windows). Set to `OFF` to build static libraries (such as .a on Linux and macOS, .lib on Windows).
- `SUPERBUILD`: set to `OFF` to use system dependencies instead of [third party dependencies](https://github.com/mavlink/MAVSDK/tree/main/third_party) downloaded and built using cmake.
- `CMAKE_PREFIX_PATH`: can be used to set the path where the dependencies can be found if `SUPERBUILD` is set to `OFF`.
- `BUILD_MAVSDK_SERVER`: set to `ON` to build mavsdk_server, see instruction to [build mavsdk_server](build_mavsdk_server.md).
- `ASAN`: set to `ON` to enable address sanitizer.
- `UBSAN`: set to `ON` to enable undefined behavior sanitizer.
- `LSAN`: set to `ON` to enable leak sanitizer.
- `WERROR`: set to `ON` to error on warnings, mostly used for CI.

After the configuration step, everything that will be build in the [build step](#build_step) have been specified, and if you want to change your build configuration (e.g. If you want to build `Debug` build instead of `Release` build), you must execute the [configuration step](#configuration_step) again!

#### Build Step {#build_step}
In the build step, we finally build the library and binaries and link them.

The stripped down version of the build command would be:

`cmake --build build/default`

- The `--build` signals the CMake to execute the build
- It uses the build configuration built from the [configuration step](#configuration_step), located at `build/default` folder

Additionally, you can install it in the system with the `--target install` command added as well, which is explained in detail in the [Installing the C++ Library](#install_mavsdk_cpp_library) section below.

### Building

#### Debug

To build the MAVSDK C++ Library for development, use the debug build.

There are 2 steps in building a library: Configure and build.
```bash
cmake -DCMAKE_BUILD_TYPE=Debug -Bbuild/default -H.
cmake --build build/default -j8
```

####  Release

Once you ship software, make sure to use the release build with optimizations turned on:

**Linux/macOS:**

 ```bash
 cmake -Bbuild/default -DCMAKE_BUILD_TYPE=Release -H.
 cmake --build build/default -j8
 ```

**Windows:**

```bash
cmake -Bbuild/default -H. -DCMAKE_BUILD_TYPE=Release
cmake --build build/default -j8 --config Release
```

> Note: It is not usual to use CMAKE_BUILD_TYPE on Windows (with MSVC), however, our build requires it for the dependencies which are built at configure time.

## Installing the C++ Library {#install_mavsdk_cpp_library}

*Installing* builds the SDK **and** copies the libraries and header files into a "public" location so that they can be referenced by C++ applications (see [Building C++ Apps](../guide/toolchain.md)).


### System-wide Install {#sdk_system_wide_install}

You can configure to install system wide by not setting the `CMAKE_INSTALL_PREFIX` in the [configuration step](#configuration_step), since CMake tries to install system wide by default. For Linux/macOS that's `/usr/local`, for Windows it is somewhere in `C:\Program Files`.

To install system-wide the command needs to be run with sudo on Linux/macOS:

```bash
sudo cmake --build build/default --target install
```

or run in a command prompt with admin privileges on Windows, or using `runas`:

```bash
runas cmake --build build/default --target install
```

> **Warning** Make sure to prevent conflicts between libraries installed in your system.
> The mavsdk library installed via a .deb or .rpm file will be installed in `/usr/` while the built library will be installed in `/usr/local`.
> It's recommended to only have one installed at any time to prevent conflicts.

### Local Install {#sdk_local_install}

The install path can be set in the [configuration step](#configuration_step) using `CMAKE_INSTALL_PREFIX`:

For example, to install into the `MAVSDK/install/` folder you would set the `CMAKE_INSTALL_PREFIX` variable to specify a path relative to the folder from which you call `cmake` (or an absolute path).
```bash
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install -Bbuild/default -H.
cmake --build build/default --target install
```

> **Tip** If you already have run *cmake* without setting `CMAKE_INSTALL_PREFIX`, you may need to clean the build first:
  ```sh
  rm -rf build/default
  ```

## Troubleshooting

### Git submodules out of date

Some common build issues can be resolved by updating submodules and cleaning the distribution:
```
cd MAVSDK
git submodule update --recursive
rm -rf build
```

Then attempt to build again.

### MSVC vs. MingW

We only try to support building using MSVC, MingW is not working, mostly because the defines/flags for it are missing.

### Undefined reference to mavsdk...

If you only just built the library and installed it system-wide may also need to [update the linker cache](http://tldp.org/HOWTO/Program-Library-HOWTO/shared-libraries.html).

On Linux this is done with `sudo ldconfig`.
