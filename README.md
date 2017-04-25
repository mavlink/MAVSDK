# DroneLink

## Description

Messaging library for PX4 using mavlink.

## License

BSD 3-clause, see [LICENSE.md](LICENSE.md).

## Authors

See [AUTHORS.md](AUTHORS.md).

## Usage

### gtest

Ubuntu:
[Ubuntu instructions](https://www.eriksmistad.no/getting-started-with-google-test-on-ubuntu/)

Mac:

```
cd wherever
git clone git@github.com:google/googletest.git
git checkout release-1.8.0
cd googletest/
mkdir build
cd build
cmake ..
make -j4
cd ..
sudo cp -r googletest/include/gtest /usr/local/include
sudo cp build/googlemock/gtest/libgtest.a /usr/local/lib
sudo cp build/googlemock/gtest/libgtest_main.a /usr/local/lib
```

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

To build for Android devices or simulators, you first need to install the Android ndk and point the environment variable `ANDROID_TOOLCHAIN_CMAKE` to `build/cmake/android.toolchain.cmake`, e.g. adding the line below to the .bashrc:

```
export ANDROID_TOOLCHAIN_CMAKE=$HOME/Android/android-ndk-r13/build/cmake/android.toolchain.cmake
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

The external directory needs to contain the folders `integration_tests` and `plugins`,
and should be organized like the `external_example`.

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

Make sure that the PX4 Gazebo simulation is built and works:

```
cd wherever/Firmware/
make posix gazebo
```

Then press Ctrl+C to stop the simulation and run the integration tests:

```
cd wherever/DroneLink/
AUTOSTART_SITL=1 make run_integration_tests
```

To prevent the 3D viewer (gzclient) from being started, use:

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
make
build/default/integration_tests_runner --gtest_filter="SitlTest.TelemetrySimple"
```

Note that some of the tests might not be suited for real vehicles, especially the takeoff and kill test.

### Gtest tricks

To list all possible tests:
```
build/default/integration_tests_runner --gtest_list_tests
```

To run a single integration test:
```
build/default/integration_tests_runner --gtest_filter="SitlTest.TelemetrySimple"
```

To run all telemetry tests:
```
build/default/integration_tests_runner --gtest_filter="SitlTest.Telemetry*"
```

### Code style

All cpp and h files should be formatted according to the astyle settings defined by astylerc.
To automatically fix the formatting, run this command:

```
make fix-style
```
