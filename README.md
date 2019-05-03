![Dronecode SDK](https://github.com/dronecore/sdk_docs/raw/develop/assets/site/sdk_logo_full.jpg)

[![travis-ci build status](https://travis-ci.org/Dronecode/DronecodeSDK.svg?branch=develop)](https://travis-ci.org/Dronecode/DronecodeSDK)
[![Appveyor Build status](https://ci.appveyor.com/api/projects/status/1ntjvooywpxmoir8/branch/develop?svg=true)](https://ci.appveyor.com/project/Dronecode/dronecore/branch/develop)
[![Coverage Status](https://coveralls.io/repos/github/Dronecode/DronecodeSDK/badge.svg?branch=develop)](https://coveralls.io/github/Dronecode/DronecodeSDK?branch=develop)

## Description

The [Dronecode SDK](https://www.dronecode.org/sdk/) (previously known as "DroneCore") is an API and library for the [PX4 flight stack](http://github.com/PX4/Firmware) using [MAVLink](https://mavlink.io/en/).

It is written in C++11 and aiming to be:

- Easy to use with a simple API.
- Fast and lightweight.
- Cross-platform (Linux, Mac, Windows, iOS, Android).
- Extensible (using compile-time plugins).

## Interfacing

The SDK currently takes care of the MAVLink messaging. Connections over serial, UDP, and TCP are supported on Linux, macOS, and Windows.

The library provides both synchronous (blocking) API calls, as well as asynchronous API calls using callbacks.

## API Overview

API consumers use the `DronecodeSDK` class to discover and manage vehicles (`System` objects). Using the `System` object plugins such as e.g. `Action`, `Telemetry`, or `Mission` can be instantiated which provide information about the state of the drone and allow to interact with it.

The links below take you to the respective header files:

- [dronecode_sdk](core/include/dronecode_sdk/dronecode_sdk.h): set up connection, discover devices
- [system](core/include/dronecode_sdk/system.h): an class representing one drone which can consist of multiple components
- [info](plugins/info/include/plugins/info/info.h): general info about a device
- [telemetry](plugins/telemetry/include/plugins/telemetry/telemetry.h): to receive telemetry data
- [action](plugins/action/include/plugins/action/action.h): to send commands such as arm, disarm, takeoff, land to drone
- [mission](plugins/mission/include/plugins/mission/mission.h)/[mission_item](plugins/mission/include/plugins/mission/mission_item.h): to upload a waypoint mission
- [offboard](plugins/offboard/include/plugins/offboard/offboard.h): for velocity control
- [gimbal](plugins/gimbal/include/plugins/gimbal/gimbal.h): control a gimbal
- [camera](plugins/camera/include/plugins/camera/camera.h): capture images, videos, and set camera settings
- [follow_me](plugins/follow_me/include/plugins/follow_me/follow_me.h): drone tracks a position supplied by the Dronecode SDK.
- [calibration](plugins/calibration/include/plugins/calibration/calibration.h): calibrate drone sensors including gyro, accelerometer, and magnetometer.
- [logging](plugins/logging/include/plugins/logging/logging.h): (not implemented) data logging and streaming from the vehicle.

For more information see the [API Overview](https://sdk.dronecode.org/en/#api-overview) in the Dronecode SDK Guide.


## Docs (Build instructions etc.)

Instructions for how to use the library can be found in the [SDK Guide](https://sdk.dronecode.org/en).

Quick Links:

- [QuickStart](https://sdk.dronecode.org/en/#getting-started)
- [Building the Library](https://sdk.dronecode.org/en/contributing/build.html)
- [Examples](https://sdk.dronecode.org/en/examples/)
- [API Reference](https://sdk.dronecode.org/en/api_reference/)
- [FAQ](https://sdk.dronecode.org/en/getting_started/faq.html)


## License

This project is licensed under the permissive BSD 3-clause, see [LICENSE.md](LICENSE.md).
