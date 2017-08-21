## Building / Usage

### Build the Library

Make sure the submodules are updated first.

```
git submodule update --init --recursive
```

#### Desktop

Build for the desktop to test and debug:

```
make default
```

#### Android

To build for Android devices or simulators, you first need to install:
- [Android NDK](https://developer.android.com/ndk/downloads/index.html)
- [Android SDK](https://developer.android.com/studio/index.html)


Also, you need to set these three environment variables:

- `NDK_ROOT` to `<your-android-ndk>`
- `ANDROID_TOOLCHAIN_CMAKE` to `<your-android-ndk>/build/cmake/android.toolchain.cmake`
- `ANDROID_CMAKE_BIN` to `<your-android-sdk>/cmake/<version>/bin/cmake`

E.g. you can add the lines below to your .bashrc, (or .zshrc for zshell users, or .profile):

```
export NDK_ROOT=$HOME/Android/android-ndk-r13
export ANDROID_TOOLCHAIN_CMAKE=$HOME/Android/android-ndk-r13/build/cmake/android.toolchain.cmake
export ANDROID_CMAKE_BIN=$HOME/Android/Sdk/cmake/3.6.3155560/bin/cmake
```

Then you build for all Android architectures:
```
make android install
```


#### iOS

To build for real iOS devices on macOS:

```
make ios install
```

Build for the iOS simulator on macOS:

```
make ios_simulator install
```

#### Windows

First download the curl source, extract and build it:
```
cd C:\curl-7.54.1\winbuild
nmake /f Makefile.vc mode=static VC=15 MACHINE=x64 DEBUG=no
```


To build in Windows, open the VS2015 x64 Native Tools Command Prompt, go to the source directory and do:
```
mkdir build && cd build
cmake -DWIN_CURL_INCLUDE_DIR:STRING=C:\\curl-7.54.1\\include -DWIN_CURL_LIB:STRING="C:\curl-7.54.1\builds\libcurl-vc15-x64-release-static-ipv6-sspi-winssl\lib\libcurl_a.lib" -G "Visual Studio 14 2015 Win64" ..

cmake --build .
```

Note that you need to download the curl source and provide the directory of the header files.

### Build with external directory for plugins and custom integration_tests

The SDK is split into a [core](../core/) and [plugins](../plugins/). The plugins are included at compile time.
The cmake script [autogenerate_plugin_container.cmake](../autogenerate_plugin_container.cmake) takes care of including the plugin folders and integrations tests.

The architecture goal is that the plugins do not depend on each other but only to the core source. This means you can swap out plugins as needed, however, it will lead to some duplicate functionality across the plugin modules.

You can add modules by copying the [external_example](../external_example/) and adapting it:

The external directory needs to contain the folders `integration_tests` and `plugins`.

```
external_example
├── integration_tests
│   ├── CMakeLists.txt
│   └── hello_world.cpp
└── plugins
    └── example
        ├── CMakeLists.txt
        ├── example.cpp
        ├── example.h
        ├── example_impl.cpp
        ├── example_impl.h
        └── example_impl_test.cpp
```

To add an additional library, the library name can be added to the variable `additional_libs` as follows:
```
set(additional_libs "library_name" PARENT_SCOPE)
```

And the required includes as `additional_includes`.
```
set(additional_includes "include_dir" PARENT_SCOPE)
```

To include the external folder in the build, add the folder name to the make command:

```
make EXTERNAL_DIR=external_example
```

To run the external hello world integration test, do:
```
build/default/integration_tests_runner --gtest_filter="ExternalExampleHello"
```

### Unit Tests

To run the unit tests:

```
make run_unit_tests
```

### Integration Tests

There are three ways to run the integration tests:

1. automatically with SITL autostart.
2. manually without SITL autostart.
3. on a real vehicle.

#### 1. Autostart PX4 SITL

Make sure that the [PX4 Gazebo simulation](https://dev.px4.io/en/simulation/gazebo.html) is built and works:

```
cd wherever/Firmware/
make posix gazebo
```

Then press Ctrl+C to stop the simulation and run the integration tests:

```
cd wherever/DroneCore/
AUTOSTART_SITL=1 make run_integration_tests
```

To run the tests without 3D viewer (gzclient), use:

```
AUTOSTART_SITL=1 HEADLESS=1 make run_integration_tests
```

#### 2. Run PX4 SITL Manually

Build and run the PX4 simulation manually:

```
cd wherever/Firmware/
make posix gazebo
```

```
make run_integration_tests
```

#### 3. On a Real Vehicle

Make sure you are connected to a vehicle and check the connection using e.g.

```
make && build/default/integration_tests_runner --gtest_filter="SitlTest.TelemetryAsync"
```

Note that some of the tests might not be suited for real vehicles, especially the takeoff and kill test.

### Gtest tricks

To list all possible tests:
```
make && build/default/integration_tests_runner --gtest_list_tests
```

To run a single integration test:
```
make && build/default/integration_tests_runner --gtest_filter="SitlTest.TelemetryAsync"
```

To run all telemetry tests:
```
make && build/default/integration_tests_runner --gtest_filter="SitlTest.Telemetry*"
```

### Code Style

All cpp and h files should be formatted according to the astyle settings defined by astylerc.
To automatically fix the formatting, run this command:

```
make fix_style
```

### Building in Docker

If you want to develop in docker, you can use the [Dockerfile](../Dockerfile) based on Ubuntu 16.04.

Build the image:
```
docker build . -t dronecore
```

To compile in it:
```
docker run -it -v $HOME/<wherever>/DroneCore:/home/docker1000/src/DroneCore:rw dronecore make
```

Or run the code style check:
```
docker run -it -v $HOME/<wherever>/DroneCore:/home/docker1000/src/DroneCore:rw dronecore make fix_style
```

### Install Artefacts

The build artefacts (headers and static library file) can be installed locally into the folder `./install/` using:

```
make default install
```

Or, to install the files system-wide, using:

```
INSTALL_PREFIX=/usr/local make default install
```

Note that when using the library `libdronecore.a`, you need to link to a thread library such as pthread on a POSIX system. (pthread is not included in the static library because it is included in glibc.)

### Build Example

Check out the [example](../example/). It sets up a UDP connection, waits for a device appearing, and commands it to takeoff and land again, while receiving some telemetry.

To build and try it, start PX4 in SITL and build and run the example as follows:

Build and install the DroneCore library first:

```
make default install
```

Then build the example:

```
cd example/
mkdir build && cd build
cmake ..
make && ./takeoff_and_land
```

Note that the example needs to be linked to a thread library (see [CMakeLists.txt](example/CMakeLists.txt))

