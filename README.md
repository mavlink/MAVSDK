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
cd googletest/
mkdir build
cd build
cmake ..
make -j4
cd ..
sudo cp -r googletest/include/gtest /usr/local/include
sudo cp googlemock/gtest/libgtest.a /usr/local/lib
sudo cp googlemock/gtest/libgtest_main.a /usr/local/lib
```

### Building

```
mkdir build
cd build
```

For releasing:
```
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

For debugging:
```
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

### Unit-tests

To run tests without output:
```
make test
```

And with output:
```
make check
```

### Integration-tests

Start PX4 e.g. in SITL using `make posix gazebo` and run the integration tests using the commands
below:

```
make simple_connect
integration_tests/simple_connect
```

```
make simple_telemetry
integration_tests/simple_telemetry
```

```
make simple_hover
integration_tests/simple_hover
```
