# MavlinkDirect

The [MavlinkDirect](../api_reference/classmavsdk_1_1_mavlink_direct.md) plugin enables direct MAVLink communication with runtime message parsing and JSON field representation.

::: warning
The MavlinkDirect API is still quite new.
The functionality is here to stay, but the specifics and types may still change in a future major release.
:::

::: tip
MavlinkDirect is the recommended replacement for [MavlinkPassthrough](../api_reference/classmavsdk_1_1_mavlink_passthrough.md).
Since MAVSDK v4, MavlinkPassthrough is deprecated and only available if `MAVSDK_ENABLE_MAVLINK_C_API` is defined, see [API changes](../api_changes.md#mavlink-c-headers-are-opt-in).
:::

::: info
The MavlinkDirect plugin is built on top of the work by Thomas Debrunner in [libmav](https://github.com/Auterion/libmav).
:::

## Overview

MavlinkDirect allows you to:

- Send and receive any MAVLink message
- Load custom MAVLink message definitions from XML

## Quickstart

A complete program that connects to a vehicle and prints every GLOBAL_POSITION_INT it receives:

```cpp
#include <mavsdk/mavsdk.hpp>
#include <mavsdk/plugins/mavlink_direct/mavlink_direct.hpp>
#include <chrono>
#include <iostream>
#include <thread>

using namespace mavsdk;

int main()
{
    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};

    if (mavsdk.add_any_connection("udpin://0.0.0.0:14540") != ConnectionResult::Success) {
        std::cerr << "Connection failed\n";
        return 1;
    }

    auto system = mavsdk.first_autopilot(3.0);
    if (!system) {
        std::cerr << "No autopilot found\n";
        return 1;
    }

    auto mavlink_direct = MavlinkDirect{system.value()};

    auto handle = mavlink_direct.subscribe_message(
        "GLOBAL_POSITION_INT", [](MavlinkDirect::MavlinkMessage message) {
            std::cout << "Got " << message.message_name << ": " << message.fields_json << '\n';
        });

    std::this_thread::sleep_for(std::chrono::seconds(3));

    mavlink_direct.unsubscribe_message(handle);
    return 0;
}
```

Against a vehicle or simulator on `udpin://0.0.0.0:14540`, this prints lines like:

```
Got GLOBAL_POSITION_INT: {"message_id":33,"message_name":"GLOBAL_POSITION_INT","alt":500000,"hdg":9000,"lat":473977420,"lon":85455940,"relative_alt":100000,"time_boot_ms":1234,"vx":0,"vy":0,"vz":0}
```

See [Building C++ Apps](toolchain.md) for how to build it, and the [C++ QuickStart](../quickstart.md) for setting up a simulator.

### Getting at the fields

`fields_json` is just a JSON string, so you can pick it apart with the JSON library of your choice. Using [nlohmann/json](https://github.com/nlohmann/json), the subscription above becomes:

```cpp
#include <nlohmann/json.hpp>

// ...

    auto handle = mavlink_direct.subscribe_message(
        "GLOBAL_POSITION_INT", [](MavlinkDirect::MavlinkMessage message) {
            const auto fields = nlohmann::json::parse(message.fields_json);

            // The units are the ones from the MAVLink message definition:
            // degrees * 1e7, and millimeters above the home position.
            const double latitude_deg = fields["lat"].get<int32_t>() / 1e7;
            const double longitude_deg = fields["lon"].get<int32_t>() / 1e7;
            const double altitude_m = fields["relative_alt"].get<int32_t>() / 1e3;

            std::cout << "Position: " << latitude_deg << ", " << longitude_deg << " at "
                      << altitude_m << " m\n";
        });
```

which prints:

```
Position: 47.3977, 8.54559 at 100 m
```

MAVSDK does not expose a JSON library in its headers, so add the one you want to your own build, e.g.:

```cmake
find_package(nlohmann_json REQUIRED)

target_link_libraries(your_executable_name
    PRIVATE
    MAVSDK::mavsdk
    nlohmann_json::nlohmann_json
)
```

The sections below go through this in more detail. The complete examples are in the repository:

| Example | Shows |
|---|---|
| [mavlink_direct](https://github.com/mavlink/MAVSDK/tree/main/cpp/examples/mavlink_direct) | Subscribing to a message (GPS_RAW_INT), and stats about all arriving messages |
| [mavlink_direct_sender](https://github.com/mavlink/MAVSDK/tree/main/cpp/examples/mavlink_direct_sender) | Sending a message (OBSTACLE_DISTANCE) |
| [mavlink_direct_sender_custom](https://github.com/mavlink/MAVSDK/tree/main/cpp/examples/mavlink_direct_sender_custom) | Sending a message that MAVSDK does not know yet, by loading its XML definition |

## Runtime vs compile-time considerations

MavlinkDirect works at runtime instead of compile-time, so instead of having the C types and functions to rely on, everything has to be done using string methods. There are trade-offs with that:

**Runtime pros:**

- Allows extension with custom messages/dialects without re-compiling MAVSDK.
- Allows usage in language wrappers such as Python.
- MAVSDK no longer needs to carry and install the MAVLink C headers.

**Runtime cons:**

- API users need to look up MAVLink message definitions and don't get auto-complete.
- Enum values or flags/bits need to be assembled manually.
- The API is likely slower although that's likely negligible in C++ as long as messages are not sent/received at very high rates (benchmarks outstanding).

## Load custom MAVLink XML/ MAVLink dialects

By default all messages from the [common.xml](https://mavlink.io/en/messages/common.html) MAVLink dialect are available, loaded in.

MavlinkDirect allows to load your own/custom MAVLink messages or other dialects.

In the example below we load in the AIRSPEED message as it is drafted in development.xml but not yet moved to common.xml, and hence not available by default in MAVSDK yet:

```cpp
    std::string custom_xml = R"(
<mavlink>
    <messages>
        <message id="295" name="AIRSPEED">
            <description>Airspeed sensor data</description>
            <field type="uint8_t" name="id">Sensor ID</field>
            <field type="float" name="airspeed">Calibrated airspeed in m/s</field>
            <field type="int16_t" name="temperature">Temperature in centidegrees</field>
            <field type="float" name="raw_press">Raw differential pressure</field>
            <field type="uint8_t" name="flags">Airspeed sensor flags</field>
        </message>
    </messages>
</mavlink>)";

    // Load custom XML
    auto xml_result = mavlink_direct.load_custom_xml(custom_xml);
    if (xml_result != MavlinkDirect::Result::Success) {
        std::cerr << "Failed to load custom XML: " << xml_result << std::endl;
        return 1;
    }
```

Check out the [full example on GitHub](https://github.com/mavlink/MAVSDK/tree/main/cpp/examples/mavlink_direct_sender_custom)


## Sending messages

Let's assume we are a companion computer that detects obstacles and therefore needs to send the message [OBSTACLE_DISTANCE](https://mavlink.io/en/messages/common.html#OBSTACLE_DISTANCE) to the autopilot.

```cpp
// Set up as companion computer
auto config = Mavsdk::Configuration{ComponentType::CompanionComputer};

// Connect ...

// Create MavlinkDirect plugin instance
auto mavlink_direct = MavlinkDirect{system.value()};

// Create a HEARTBEAT message
MavlinkDirect::MavlinkMessage obstacle_distance{};
obstacle_distance.message_name = "OBSTACLE_DISTANCE";
obstacle_distance.system_id = config.get_system_id(); // Your component's system ID
obstacle_distance.component_id = config.get_component_id(); // Your component's component ID
obstacle_distance.target_system_id = 0; // Does not apply for this message
obstacle_distance.target_component_id = 0; // Does not apply for this message

// For now we just hard-code values for demonstration purposes.
// We assume the sensor has is looking forward and has 8 segments with 10 degrees per segment,
// so 80 degrees field of view.
// On 3 segments we see an obstacle, the closest is at 7m.
//
// In a real application this JSON message content would of course be
// assembled manually, or using a json library like nlohmann/json.
obstacle_distance.fields_json = R"({
    "time_usec": 12345678,
    "sensor_type": 3,
    "distances": [
        2000,   2000,  2000,  1000,   800,   700,  2000,  2000,
        65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536,
        65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536,
        65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536,
        65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536,
        65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536,
        65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536,
        65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536,
        65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536
    ],
    "increment": 0,
    "min_distance": 000,
    "max_distance": 2000,
    "increment_f": 10.0,
    "angle_offset": -40.0,
    "frame": 12
})";

while (true) {
    // Send the message
    auto result = mavlink_direct.send_message(obstacle_distance);
    if (result == MavlinkDirect::Result::Success) {
        std::cout << "OBSTACLE_DISTANCE message sent successfully" << std::endl;
    } else {
        std::cerr << "OBSTACLE_DISTANCE message could not be sent: " << result << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}
```

Check the full example on [GitHub](https://github.com/mavlink/MAVSDK/tree/main/cpp/examples/mavlink_direct_sender).

## Receiving messages

You can subscribe to specific message types:

```cpp
// Subscribe to GLOBAL_POSITION_INT messages
auto handle = mavlink_direct.subscribe_message(
    "GLOBAL_POSITION_INT",
    [](MavlinkDirect::MavlinkMessage message) {
        std::cout << "Received position: " << message.fields_json << std::endl;
    }
);
```

See [Getting at the fields](#getting-at-the-fields) for how to parse `fields_json`.

Or all messages using the empty string `""`:

```cpp
// Subscribe to all incoming messages
auto handle = mavlink_direct.subscribe_message(
    "",  // Empty string = all messages
    [](MavlinkDirect::MavlinkMessage message) {
        std::cout << "Received " << message.message_name
                  << " from system " << message.system_id
                  << " component " << message.component_id << std::endl;
    }
);
```

And to unsubscribe again, just use the handle from the subscription, as usual.

```cpp
// Clean up subscription
mavlink_direct.unsubscribe_message(handle);
```

## Sniffing all messages

There is also [an example that demonstrates a generic MAVLink message sniffer](https://github.com/mavlink/MAVSDK/tree/main/cpp/examples/sniffer).

::: info
: This example uses the mavsdk.h intercept API rather than the MavlinkDirect plugin API, however, they are very similar.
:::

## JSON field format

MavlinkDirect represents MAVLink message fields as JSON objects:

```json
{
  "time_boot_ms": 12345,
  "lat": 473977418,
  "lon": -1223974560,
  "alt": 100500,
  "relative_alt": 50250,
  "vx": 100,
  "vy": -50,
  "vz": 25,
  "hdg": 18000
}
```

**Key points:**

- All field names match the MAVLink message definition
- Numeric values preserve original MAVLink types and scaling
- Arrays are represented as JSON arrays: `[1, 2, 3, 0, 0]`
- NaN and infinity values are represented as `null`
- Extended fields are automatically included

## API reference

See [API Reference](../api_reference/classmavsdk_1_1_mavlink_direct.md) for full details.

## Troubleshooting

### Can I get more debug information?

Yes, set the environment variable `MAVSDK_MAVLINK_DIRECT_DEBUGGING=1` before running your binary.
