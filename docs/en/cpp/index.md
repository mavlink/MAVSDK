# MAVSDK C++ Library <div style="float:right; padding:10px; margin-right:20px;"><img src="../../assets/site/sdk_logo_full.png" title="MAVSDK Logo" width="180px"/></div>
[![Slack](https://px4-slack.herokuapp.com/badge.svg)](http://slack.px4.io)&nbsp;[![Discuss](https://img.shields.io/badge/discuss-MAVSDK-ff69b4.svg)](https://discuss.px4.io/c/sdk) [![GitHub Actions Status](https://github.com/mavlink/MAVSDK/workflows/Build%20and%20Test/badge.svg?branch=main)](https://github.com/mavlink/MAVSDK/actions?query=branch%3Amain) [![Coverage Status](https://coveralls.io/repos/github/mavlink/MAVSDK/badge.svg?branch=main)](https://coveralls.io/github/mavlink/MAVSDK?branch=main)

The MAVSDK C++ library provides a simple C++ API for managing one or more vehicles via MAVLink.
It enables programmatic access to vehicle information and telemetry, and control over missions, movement and other operations.
The C++ library is designed to be performant, and can be used to enable tasks like computer vision, obstacle avoidance, and route planning.


## Project Status

The MAVSDK C++ library is a robust and well-tested library that is in use in production environments.
Note however that the API is still being evolved and the project does not provide future compatibility guarantees.

## Getting Started

Check out the [QuickStart guide](quickstart.md) to get started.

A simple complete example can be found in [Takeoff and Land](examples/takeoff_and_land.md).

## Updating / API changes

Information about API changes between versions: [API changes](api_changes.md).

## API Overview

[Mavsdk](api_reference/classmavsdk_1_1_mavsdk.md) is the main library class.
API consumers use [Mavsdk](api_reference/classmavsdk_1_1_mavsdk.md) to discover and access vehicles ([System](api_reference/classmavsdk_1_1_system.md) objects), which in turn provide access to all other drone information and control objects (e.g. [Telemetry](api_reference/classmavsdk_1_1_telemetry.md), [Mission](api_reference/classmavsdk_1_1_mission.md) etc.).

The most important classes are:

- [Mavsdk](api_reference/classmavsdk_1_1_mavsdk.md): Discover and connect to vehicles ([System](api_reference/classmavsdk_1_1_system.md)).
- [System](api_reference/classmavsdk_1_1_system.md): Represents a connected vehicle (e.g. a copter or VTOL drone).
  It provides access to vehicle information and control through the classes listed below.
- [Info](api_reference/classmavsdk_1_1_info.md): Basic version information about the hardware and/or software of a system.
- [Telemetry](api_reference/classmavsdk_1_1_telemetry.md): Get vehicle telemetry and state information and set telemetry update rates.
- [Action](api_reference/classmavsdk_1_1_action.md): Simple drone actions including arming, taking off, and landing.
- [Mission](api_reference/classmavsdk_1_1_mission.md): Waypoint mission creation and upload/download.
  Missions are created from [MissionItem](api_reference/structmavsdk_1_1_mission_1_1_mission_item.md) objects.
- [Offboard](api_reference/classmavsdk_1_1_offboard.md): Control a drone with velocity commands.
- [Geofence](api_reference/classmavsdk_1_1_geofence.md): Specify a geofence.
- [Gimbal](api_reference/classmavsdk_1_1_gimbal.md): Control a gimbal.
- [Camera](api_reference/classmavsdk_1_1_camera.md): Control a camera.
- [FollowMe](api_reference/classmavsdk_1_1_follow_me.md): Drone tracks a position supplied by MAVSDK.
- [Calibration](api_reference/classmavsdk_1_1_calibration.md):  Calibrate sensors (e.g.: gyro, accelerometer, and magnetometer).
- [LogFiles](api_reference/classmavsdk_1_1_log_files.md): Download log files from the vehicle.


In addition, "[Server Plugins](server_plugins.md)" can be used to implement vehicle-side MAVLink functionality.
The might be used to expose a MAVLink interface from non-MAVLink autopilots or components.


The following APIs provide more direct access to underlying MAVLink messages/types.
They should only be used where features are missing from the main APIs above.
* [Param](api_reference/classmavsdk_1_1_param.md): Raw access to get and set parameters.
* [MissionRaw](api_reference/classmavsdk_1_1_mission_raw.md): Direct access to MAVLink mission items.
* [MavlinkDirect](api_reference/classmavsdk_1_1_mavlink_direct.md): Modern MAVLink access with JSON field representation and runtime message parsing
* [MavlinkPassthrough](api_reference/classmavsdk_1_1_mavlink_passthrough.md): **DEPRECATED** (use [MavlinkDirect](api_reference/classmavsdk_1_1_mavlink_direct.md) instead)


## Contributing/Extending

The [Contributing](contributing/index.md) section contains everything you need to contribute to project, including topics about building MAVSDK from source code, running our integration and unit tests, and all other aspects of core development.

## Troubleshooting

If something doesn't work as expected, make sure to check [Troubleshooting](troubleshooting.md).
