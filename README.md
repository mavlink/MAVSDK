# DroneLink

## Description

Messaging library/SDK for [PX4 flight stack](http://github.com/PX4/Firmware) using [Mavlink](http://mavlink.org).

## License

BSD 3-clause, see [LICENSE.md](LICENSE.md).

## Authors

See [AUTHORS.md](AUTHORS.md).

## Usage

### Building

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


Also, you need to set these two environment variables:

- `ANDROID_TOOLCHAIN_CMAKE` to `<your-android-ndk>/build/cmake/android.toolchain.cmake`
- `ANDROID_CMAKE_BIN` to `<your-android-sdk>/cmake/<version>/bin/cmake`

E.g. you can add the lines below to your .bashrc (or .profile):

```
export ANDROID_TOOLCHAIN_CMAKE=$HOME/Android/android-ndk-r13/build/cmake/android.toolchain.cmake
export ANDROID_CMAKE_BIN=$HOME/Android/Sdk/cmake/3.6.3155560/bin/cmake
```

Then you build for all the architectures:
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

To build in Windows, open the VS2015 x64 Native Tools Command Prompt, go to the directory and do:
```
mkdir build && cd build
cmake ..
cmake --build .
```

### Build with external directory for plugins/integration_tests

The SDK is split into a [core](src/) and [plugins](plugins/). The plugins are included at compile time.
The cmake script [autogenerate_plugin_container.cmake](autogenerate_plugin_container.cmake) takes care of including the plugin folders and integrations tests.

The architecture goal is that the plugins do not depend on each other but only to the core source. This means you can swap out plugins as needed, however, it will lead to some duplicate functionality acroos the plugin modules.

You can add modules by copying the [external_example](external_example/) and adapting it:

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

To include the external folder in the build, add the folder name to the make command:

```
make EXTERNAL_DIR=external_example
```

To run the external hello world integration test, do:
```
build/default/integration_tests_runner --gtest_filter="HelloWorld*"
```

### Unit-tests

To run the unit tests:

```
make run_unit_tests
```

### Integration tests

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
cd wherever/DroneLink/
AUTOSTART_SITL=1 make run_integration_tests
```

To run the tests without 3D viewer (gzclient), use:

```
AUTOSTART_SITL=1 HEADLESS=1 make run_integration_tests
```

#### 2. Run PX4 SITL manually

Build and run the PX4 simulation manually:

```
cd wherever/Firmware/
make posix gazebo
```

```
make run_integration_tests
```

#### 3. On a real vehicle

Make sure you are connected to a vehicle and check the connection using e.g.

```
make && build/default/integration_tests_runner --gtest_filter="SitlTest.TelemetrySimple"
```

Note that some of the tests might not be suited for real vehicles, especially the takeoff and kill test.

### Gtest tricks

To list all possible tests:
```
make &&build/default/integration_tests_runner --gtest_list_tests
```

To run a single integration test:
```
make &&build/default/integration_tests_runner --gtest_filter="SitlTest.TelemetrySimple"
```

To run all telemetry tests:
```
make &&build/default/integration_tests_runner --gtest_filter="SitlTest.Telemetry*"
```

### Code style

All cpp and h files should be formatted according to the astyle settings defined by astylerc.
To automatically fix the formatting, run this command:

```
make fix_style
```

### Building in Docker

If you want to develop in docker, you can use the [Dockerfile](Dockerfile) based on Ubuntu 16.04.

Build the image:
```
docker build . -t dronelink
```

To compile in it:
```
docker run -it -v $HOME/<wherever>/DroneLink:/home/docker1000/src/DroneLink:rw dronelink make
```

Or run the code style check:
```
docker run -it -v $HOME/<wherever>/DroneLink:/home/docker1000/src/DroneLink:rw dronelink make fix_style
```
