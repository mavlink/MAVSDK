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

### Integration-tests

Start PX4 e.g. in SITL using `make posix gazebo` and run the integration tests use:

```
make run_integration_tests
```

To run a single integration test:
```
build/default/integration_tests_runner --gtest_filter="Telemetry.Simple"
```
or all telemetry tests:
```
build/default/integration_tests_runner --gtest_filter="Telemetry*"
```

### Code style

All cpp and h files should be formatted according to the astyle settings defined by astylerc.
To automatically fix the formatting, run this command:

```
make fix-style
```
