# Testing

The MAVSDK C++ library has both unit and integration tests, written using the [Google C++ Test Framework](https://google.github.io/googletest/primer.html) (`gtest`).
The unit tests are run every time new code is committed to the library, and must pass before the code can be merged.

This topic shows how to run the existing tests.

::: tip
For information on _writing_ tests see: [Writing Plugins &gt; Test Code](../contributing/plugins.md#testing).
:::

## Running Unit Tests

To run all unit tests:

```
./build/default/src/unit_tests_runner
```

## Running Integration Tests

Tests can be run against the simulator (either manually starting PX4 SITL or letting the tests start it automatically) or against a real vehicle.

::: tip
To run SITL you will need to install the _Gazebo_ simulator.
This is included as part of the standard PX4 installation for [macOS](https://docs.px4.io/master/en/dev_setup/dev_env_mac.html) and [Linux](https://docs.px4.io/master/en/dev_setup/dev_env_linux_ubuntu.html).
It does not run on Windows.
:::

### Autostart PX4 SITL

Make sure that the [PX4 Gazebo simulation](https://docs.px4.io/master/en/simulation/gazebo.html) is built and works:

```
cd path/to/Firmware/
make px4_sitl gazebo
```

Then press **Ctrl+C** to stop the simulation and run the integration tests:

```
cd path/to/MAVSDK/
AUTOSTART_SITL=1 ./build/debug/src/integration_tests/integration_tests_runner
```

To run the tests without the 3D viewer (_gzclient_), use:

```
AUTOSTART_SITL=1 HEADLESS=1 ./build/debug/src/integration_tests/integration_tests_runner
```

### Run PX4 SITL Manually

Build and run the PX4 simulation manually:

```
cd path/to/Firmware/
make px4_sitl gazebo
```

Then run the tests as shown:

```
cd path/to/MAVSDK/
./build/debug/src/integration_tests/integration_tests_runner
```

::: tip
The tests are designed to run in simulation, and may not be safe if run on a real vehicle.
:::

### Gtest Tricks

To list all integration tests:

```
./build/default/src/integration_tests/integration_tests_runner --gtest_list_tests
```

To run a single integration test:

```
./build/default/src/integration_tests/integration_tests_runner --gtest_filter="SitlTest.TelemetryAsync"
```

To run all telemetry tests:

```
./build/default/src/integration_tests/integration_tests_runner --gtest_filter="SitlTest.Telemetry*"
```

## Fuzz Testing

MAVSDK includes a MAVLink fuzzer using libfuzzer to find bugs through randomized input testing. Fuzzing requires the Clang compiler.

### Building the Fuzzer

```bash
cmake -Bbuild-fuzz -S. -GNinja -DBUILD_FUZZ_TESTS=ON -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
cmake --build build-fuzz
```

### Running the Fuzzer

```bash
# Run for 60 seconds
./build-fuzz/src/fuzz_tests/mavlink_fuzzer fuzz_corpus -max_total_time=60

# Run with more options
./build-fuzz/src/fuzz_tests/mavlink_fuzzer -help
```

The `fuzz_corpus/` directory stores test cases that the fuzzer discovers. This corpus grows over time and improves fuzzing effectiveness.

