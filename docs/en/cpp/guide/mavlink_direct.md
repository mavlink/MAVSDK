# MavlinkDirect

The [MavlinkDirect](../cpp/api_reference/classmavsdk_1_1_mavlink_direct.md) plugin enables direct MAVLink communication with runtime message parsing and JSON field representation.

::: warning
The MavlinkDirect API is quite new and is not necessarily stabilized yet.
The functionality is here to stay but the specifics and types might still change before MAVSDK v4.
:::

::: tip
MavlinkDirect is the recommended replacement for [MavlinkPassthrough](../cpp/api_reference/classmavsdk_1_1_mavlink_passthrough.md).
In MAVSDK v4, MavlinkPassthrough will only be available as a compile-time option.
:::

::: info
The MavlinkDirect plugin is built on top of the work by Thomas Debrunner in [libmav](https://github.com/Auterion/libmav).
:::

## Overview

MavlinkDirect allows you to:

- Send and receive any MAVLink message
- Load custom MAVLink message definitions from XML

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

By default all messsages from the [common.xml](https://mavlink.io/en/messages/common.html) MAVLink dialect are available, loaded in.

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

Check out the [full example on GitHub](https://github.com/mavlink/MAVSDK/tree/main/examples/mavlink_direct_sender_custom)


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
// assembled manually, or using a json library like jsoncpp or nlohmann/json.
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

Check the full example on [GitHub](https://github.com/mavlink/MAVSDK/tree/main/examples/mavlink_direct_sender).

## Receiving messages

You can subscribe to specific message types:

```cpp
// Subscribe to GLOBAL_POSITION_INT messages
auto handle = mavlink_direct.subscribe_message(
    "GLOBAL_POSITION_INT",
    [](MavlinkDirect::MavlinkMessage message) {
        std::cout << "Received position: " << message.fields_json << std::endl;

        // And this could now be parsed by jsoncpp or nlohmann/json:
        // Json::Value json;
        // Json::Reader reader;
        // if (reader.parse(message.fields_json, json)) {
        //     auto lat = json["lat"].asInt() / 1e7;  // Convert from degrees * 1e7
        //     auto lon = json["lon"].asInt() / 1e7;
        //     std::cout << "Position: " << lat << ", " << lon << std::endl;
        // }
    }
);
```

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

There is also [an example that demonstrates a generic MAVLink message sniffer](https://github.com/mavlink/MAVSDK/tree/main/examples/sniffer).

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
