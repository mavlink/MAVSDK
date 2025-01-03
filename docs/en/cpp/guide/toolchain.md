# Building C++ Apps

The MAVSDK core C++ library is written in C++17 and exposes C++11 interfaces such as `std::function`. Therefore, applications using the library need to be C++11 or later as well.

Applications can be built using your preferred build system, compiler and linker toolchain. The only requirement is that the build system must be able to locate the MAVSDK C++ headers and libraries (installed as described [here](installation.md)).

MAVSDK itself uses the [cmake](https://cmake.org/) build system, and we recommend that you do too.
*CMake* is an open-source, cross-platform toolchain that allows you to build your examples on macOS, Linux and Windows using the same build file definition.

Below we explain how to set up a minimal build setup (**CMakeLists.txt**) file for your application.


## Build Definition File - CMakeLists.txt

*Cmake* uses a definition file named **CMakeLists.txt** to define the project. This specifies the name of the project, compiler flags for different platforms and targets, where to find dependencies (libraries and header files), source files to build, and the name of the generated binary. **CMakeLists.txt** is typically stored in the root directory of your app project.

The sections below show how you can set up the file for when the SDK is [installed system wide](installation.md) (the default) or [locally](build.md#sdk_local_install).

> **Warning** On Windows use the [local installation](#sdk_local_install).

A "template" **CMakeLists.txt** is shown below.
Most of file is boilerplate - the main things you need to change are *your_project_name*, *your_executable_name* and *your_source_file*. You should also make sure that any plugins used by your app are listed in the `target_link_libraries` section.

```cmake
cmake_minimum_required(VERSION 3.15)

# Specify your project's name
project(your_project_name)

# Specify at least C++11, better C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable strict handling of warnings
add_definitions("-Wall -Wextra -Werror")

# Finds MAVSDK when installed system wide.
find_package(MAVSDK REQUIRED)

# Specify your app's executable name, and list of source files used to create it.
add_executable(your_executable_name
    your_source_file.cpp
    # ... any other source files
)

# Specify your app's executable name and a list of linked libraries
target_link_libraries(your_executable_name
    MAVSDK::mavsdk             #All apps link against mavsdk library
    MAVSDK::mavsdk_action      # If action plugin used by app ...
    MAVSDK::mavsdk_telemetry   #If telemetry plugin used by app ...
    # ... Any other linked libraries
)
```

> **Note** The file format and required modifications are self-explanatory.
> If additional information is required see the [cmake documentation](https://cmake.org/cmake/help/latest/manual/cmake-commands.7.html).


## Building the App

In order to build the application, the MAVSDK library and header files need to be installed.

### MAVSDK Installed System-wide {#sdk_installed_system_wide}

If MAVSDK has been installed using the **.deb** or **.rpm** file from [MAVSDK releases](https://github.com/mavlink/MAVSDK/releases) it is installed in `/usr/`, so system-wide.
If MAVSDK has been built from source and installed in `/usr/local`, we also consider that system-wide.

For this case `find_package(MAVSDK REQUIRED)` as used in `CMakeLists.txt` above should find the library automatically.

To build the example we would do:

```bash
   cmake -Bbuild -H.
   cmake --build build -j8
```

### MAVSDK Installed Locally {#sdk_local_install}

For Windows we need to install the library and header files locally by extracting **mavsdk-windows-x64-release.zip** file from [MAVSDK releases](https://github.com/mavlink/MAVSDK/releases).

On macOS/Linux, you can also use locally installed library artefacts by [building from source](build.md) if that is required (e.g. during development in order not to conflict with a system-wide installed library).

For the case where the library is installed in some local directory, you need to specify this path using the `CMAKE_PREFIX_PATH`.

E.g. here there would be a directory called mavsdk just one directory up:

```bash
cmake -Bbuild -DCMAKE_PREFIX_PATH=../mavsdk -H.
cmake --build build -j8
```

## Run the App

To Execute the file (in your build directory):

```bash
build/your_executable_name  # Run your new executable
```

## Release Build

In order to build in Release mode where the file size is smaller and optimizations are turned on, do:

**macOS/Linux:**
```
cmake -Bbuild -DCMAKE_BUILD_TYPE=Release -H.
cmake --build build -j8
```

**Windows:**

```
cmake -Bbuild -H.
cmake --build build -j8 --config Release
```

## Useful Links

We only show a small fraction of what *Cmake* is capable of! Check out the links below for more information and examples.

* [cmake resources page](https://cmake.org/documentation/)
* [cmake documentation](https://cmake.org/cmake/help/latest/) (latest)
* [cmake commands](https://cmake.org/cmake/help/latest/manual/cmake-commands.7.html)
