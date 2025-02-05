# Writing Plugins

MAVSDK-C++ is split into a [core](https://github.com/mavlink/MAVSDK/tree/main/src/core) and multiple independent [plugins](https://github.com/mavlink/MAVSDK/tree/main/src/mavsdk/plugins).

Plugins that are located in the *correct location* (a subfolder of **/plugins**) and have the *correct structure* are built at compile time.
The [CMakeLists.txt](https://github.com/mavlink/MAVSDK/blob/main/CMakeLists.txt) takes care of including the plugin folders and integration tests.

## Plugin Architecture

Plugins should be written so that they are independent of each other (they will still need to be dependent on the core source).
This allows plugins to be removed/replaced as needed at the cost of some duplicated functionality across the plugin modules.

The code for each plugin (and its unit test if one has been defined) is stored in a sub-folder of the **plugins** directory.
Integration tests for all plugins in the library are stored in **integration_tests**.

A simplified view of the folder structure is shown below:

```
├── MAVSDK
│   └── src
│       ├── system_tests
│       ├── mavsdk_server
│       └── mavsdk
│           ├── core
│           └── plugins
│               ├── action
│               ├── ...
│               └── tune
```

Each plugin must have the same files/structure, as shown for the "example" plugin below.
```
└── plugins
    └── example
        ├── CMakeLists.txt
        ├── example.cpp
        ├── example.h
        ├── example_impl.cpp
        ├── example_impl.h
        └── example_foo_test.cpp  ##optional
```

## Auto-generation

In order to support various language wrappers around MAVSDK without having to write the same things multiple times, once for every language, we opted to use auto-generation as much as possible.
The APIs are defined as [proto definitions](https://github.com/mavlink/MAVSDK-Proto).

From that, several parts are auto-generated, such as:
- Language wrappers based on gRPC client (formerly called frontend)
- gRPC mavsdk_server in C++ (formerly called backend)
- Plugin C++ headers defining the API.

Looking at the plugin structure again, this means that some of the files are auto-generated:

```
└── plugins
    └── example
        ├── CMakeLists.txt       # auto-generated
        ├── example.cpp          # auto-generated
        ├── example.h            # auto-generated
        ├── example_impl.cpp     # hand-written (can initially be generated)
        ├── example_impl.h       # hand-written (can initially be generated)
        └── example_foo_test.cpp  # optional
```

## Create a Plugin

To create a new plugin **do not copy an existing one**, instead follow the steps below:

### Think about public API

Before writing the API, take a step back and think what a user of it needs and expect, rather than what MAVLink already provides.

Generally, MAVSDK APIs ought to be:
- Simple and easy to use.
- Reduced to the essentials; no functionality that is not actually implemented/supported should be exposed.
- Clearly named and if possible without too much drone jargon and acronyms.
- Abstracted from the MAVLink implementation and therefore to provide specific functionality instead of just forwarding direct MAVLink.

This advice is important if you are planning to contribute the new plugin back and would like it to get accepted and merged.
We are convinced it is also applicable for internal development but - of course - that's up to you.

### About proto structure

There are a couple of different API types supported for MAVSDK plugins.

#### Requests:

A request is a simple one time call with a response. An example would be the takeoff command of the action plugin:

```
service ActionService {
    rpc Takeoff(TakeoffRequest) returns(TakeoffResponse) {}
    // all other services
}

message TakeoffRequest {}
message TakeoffResponse {
    ActionResult action_result = 1;
}
```

In this case the request has no argument and no return value except the result but this can vary, e.g. for getters and setters:
```
service ActionService {
    rpc GetReturnToLaunchAltitude(GetReturnToLaunchAltitudeRequest) returns(GetReturnToLaunchAltitudeResponse) {}
    rpc SetReturnToLaunchAltitude(SetReturnToLaunchAltitudeRequest) returns(SetReturnToLaunchAltitudeResponse) {}
    // all other services
}

message GetReturnToLaunchAltitudeRequest {}
message GetReturnToLaunchAltitudeResponse {
    ActionResult action_result = 1;
    float relative_altitude_m = 2;
}

message SetReturnToLaunchAltitudeRequest {
    float relative_altitude_m = 1;
}
message SetReturnToLaunchAltitudeResponse {
    ActionResult action_result = 1;
}
```

::: info
Requests can defined SYNC, ASYNC, or BOTH using `option (mavsdk.options.async_type) = ...;`.
The choice depends on the functionality that is being implemented and how it would generally be used.
There are no hard rules, it's something that makes sense to be discussed one by one in a pull request.
The default implementation is `BOTH`.
:::

#### Subscriptions:

A subscription is triggered once and will then continuously send responses as a stream. An example would be a the position information of the telemetry plugin:

```
service TelemetryService {
    rpc SubscribePosition(SubscribePositionRequest) returns(stream PositionResponse) {}
    // all other services
}


message SubscribePositionRequest {}
message PositionResponse {
    Position position = 1;
}
```

::: info
Subscriptions also can defined SYNC, ASYNC, or BOTH using `option (mavsdk.options.async_type) = ...;`.
The sync implementation of a subscription is just a getter for the last received value.
:::

::: info
Subscriptions can be defined finite using `option (mavsdk.options.is_finite) = true;`.
This means that the stream of messages will end at some point instead of continuing indefinitely. An example would be progress updates about a calibration which eventually finishes.
:::

### Add API to proto

The first step should be to define the user API in the [proto repository](https://github.com/mavlink/MAVSDK-Proto).
This repository is part of the MAVSDK as a submodule in the `proto/` directory.

You usually want to work from master in the `proto/` directory, and then create a feature branch with your additions:

```
cd proto
git switch main
git pull
git switch -c my-new-plugin
cd ../
```

Now you can add a folder with your proto file (or copy an existing one and rename it) and draft the API.

Once the API is defined, it makes sense to commit the changes, push them and make a pull request to [MAVSDK-Proto](https://github.com/mavlink/MAVSDK-Proto) to get feedback from the MAVSDK maintainers.


### Generate .h and .cpp files

Once the proto file has been created, you can generate all files required for the new plugin.

1. Run the configure step to prepare the tools required:
   ```
   cmake -DBUILD_MAVSDK_SERVER=ON -Bbuild/default -S.
   ```
1. Install `protoc_gen_mavsdk` which is required for the auto-generation:
   ```
   pip3 install --user protoc_gen_mavsdk  # Or however you install pip packages
   ```
1. Run the auto-generation:
   ```
   tools/generate_from_protos.sh
   ```
1. Fix style after auto-generation:
   ```
   tools/fix_style.sh .
   ```

::: info
The files `my_new_plugin.h` and `my_new_plugin.cpp` are generated and overwritten every time the script is run.
However, the files `my_new_plugin_impl.h` and `my_new_plugin_impl.cpp` are only generated once.
To re-generate them, delete them and run the script again.
This approach is used to prevent the script from overwriting your local changes.
:::

### Actually implement MAVLink messages

You can now add the actual "business logic" which is usually sending and receiving MAVLink messages, waiting for timeouts, etc.
All implementation goes into the files `my_new_plugin_impl.h` and `my_new_plugin_impl.cpp` or additional files for separate classes required.

You can also add unit tests with `unittest_source_files`, as [discussed below](#adding_unit_tests).

The [standard plugins](https://github.com/mavlink/MAVSDK/tree/main/src/plugins) can be reviewed for guidance on
how to write plugin code, including how to send and process MAVLink messages.

## Plugin Code

### Plugin Base Class

All plugins should derive their implementation from `PluginImplBase` (**core/plugin_impl_base.h**) and override virtual methods as needed.

### Plugin Enable/Disable

The SDK provides virtual methods that a plugin should implement to allow the core to better manage resources.
For example, to prevent callback being created before the `System` is instantiated, or messages being sent when a vehicle is not connected.

Plugin authors should provide an implementation of the following `PluginImplBase` pure virtual methods:
* [init()](#init)/[deinit()](#deinit): These are called when a system is created and just before it is destroyed. These should be used for setting up and cleaning everything that depends on having the `System` instantiated. This includes calls that set up callbacks.
* [enable()](#enable)/[disable()](#disable): These are called when a vehicle is discovered or has timed out. They should be used for managing resources needed to access a connected system/vehicle (e.g. getting a parameter or changing a setting).

Additional detail is provided for methods below.


##### init() {#init}
```cpp
virtual void init() = 0
```

The `init()` method is called when a plugin is instantiated.
This happens when a `System` is constructed (this does not mean that the system actually exists and is connected - it might just be an empty dummy system).

Plugins should do initialization steps with other parts of the SDK at this state, e.g. set up callbacks with `_parent` (`DeviceImpl`).

##### deinit() {#deinit}
```cpp
virtual void deinit() = 0
```
The `deinit()` method is called before a plugin is destroyed. This usually happens only at the very end, when a `MAVSDK` instance is destroyed.

Plugins should cleanup anything that was set up during `init()`.

##### enable() {#enable}
```cpp
virtual void enable() = 0
```
The `enable()` method is called when a system is discovered (connected).
Plugins should do all initialization/configuration steps that require a system to be connected.
For example, setting/getting parameters.

If any threads, call_every or timeouts are needed, they can be started in this method.

##### disable() {#disable}
```cpp
virtual void disable() = 0
```
The `disable()` method is called when a system has timed out.
The method is also called before `deinit()` is called to stop any systems with active plugins from communicating (in order to prevent warnings and errors because communication to the system no longer works).

If any threads, call_every, or timeouts are running, they should be stopped in this method.


## Test Code {#testing}

Tests must be created for all new and updated plugin code.
The tests should be exhaustive, and cover all aspects of using the plugin API.

The [Google Test Primer](https://google.github.io/googletest/primer.html)
provides an excellent overview of how tests are written and used.

### System Tests

Most of the existing plugins do not have unit tests as they mostly just translate between MAVSDK APIs and MAVLink message logic.
That sort of logic can be covered by system_tests which basically tests the MAVLink message logic by implementing both "client" and "server" parts, the server being exposed in the server plugin.

### Unit Tests

::: tip
Unit tests are a good way to test more intricate functionality as required in a plugin or in the core. They typically cover only one class.
:::
Unit test files are stored in the same directory as their associated source code.

In order to include a test in `unit_tests_runner`, it must be added to the `UNIT_TEST_SOURCES` variable in the plugin **CMakeLists.txt** file.

For example, to add the **example_foo_test.cpp** unit test you would
append the following lines to its **CMakeLists.txt**:

```cmake
list(APPEND UNIT_TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/src/plugins/mission/example_foo_test.cpp
)
set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
```

Unit tests typically include the header file of the class to be tested and `<gtest/gtest.h>`.

### Integration tests {#integration_tests}

We have opted to slowly fade out integration tests and use a few examples instead.

The rationale is as follows: the integration tests were used to test functionality against SITL (sofware in the loop) simulation. However, we learnt that it was slow to start up the simulatoin and then run a test all the way through. It was also cumbersome to keep track of the potential test matrix including both MAVSDK and PX4 as well as ArduPilot versions. In reality it's only really possible to test a few full integrations like this, so only a small part of the functionality/code coverage.

At this point, the integration tests ended up quite similar to the examples. For contributors it was then often confusing whether they should provide an integration test or an example, or both. Given the integration tests weren't useful as we had to remove them from CI, we decided to move away from them and instead focus on readable examples instead.

### Examples {#examples}

## Example Code

A simple example should be written that demonstrates basic usage of its API by 3rd parties.
The example need not cover all functionality, but should demonstrate enough that developers can see how it is used and how the example might be extended.

Where possible examples should demonstrate realistic use cases such that the code can usefully be copied and reused by external developers.


## Documentation

### In-Source Comments

The public API must be fully documented using the proto files.

::: tip
The in-source comments will be compiled to markdown and included in the [API Reference](../api_reference/index.md).
The process is outlined in [Documentation > API Reference](documentation.md#api-reference).
:::

Internal/implementation classes need not be documented, but should be written using expressive naming of variables and functions to help the reader.
Anything unexpected or special however warrants an explanation as a comment.

### Example Code Documentation

The plugin example should be documented in markdown following the same pattern as the existing [examples](../examples/index.md).

Generally this involves explaining what the example does and displaying the source.
The explanation of how the code works is usually deferred to [guide documentation](#guide).


### Guide Documentation {#guide}

Ideally, [guide](../guide/index.md) documentation should be created. This should be based on example code.

The purpose of the guide is to:
* Show how different parts of the API can be used together
* Highlight usage patterns and limitations that may not be obvious from API reference
* Provide code fragments that can easily be reused

