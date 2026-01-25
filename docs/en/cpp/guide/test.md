# Testing

The MAVSDK C++ library has both unit and system tests, written using the [Google C++ Test Framework](https://google.github.io/googletest/primer.html) (`gtest`).
The unit tests are run every time new code is committed to the library, and must pass before the code can be merged.

This topic shows how to run the existing tests.

::: tip
For information on _writing_ tests see: [Writing Plugins > Test Code](../contributing/plugins.md#testing).
:::

## Running Unit Tests

To run all unit tests:

```
./build/debug/src/unit_tests/unit_tests_runner
```

## Running System Tests

System tests use two MAVSDK instances (a ground station and an autopilot) to test functionality without requiring an external simulator. To run all system tests:

```
./build/debug/src/system_tests/system_tests_runner
```

### Gtest Tricks

To list all system tests:

```
./build/debug/src/system_tests/system_tests_runner --gtest_list_tests
```

To run a single system test:

```
./build/debug/src/system_tests/system_tests_runner --gtest_filter="SystemTest.MissionRawUpload"
```

To run all mission tests:

```
./build/debug/src/system_tests/system_tests_runner --gtest_filter="*Mission*"
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

