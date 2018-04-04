![DroneCore](https://raw.githubusercontent.com/dronecore/docs/master/assets/site/dronecore_logo_full.png)

[![travis-ci build status](https://travis-ci.org/dronecore/DroneCore.svg?branch=develop)](https://travis-ci.org/dronecore/DroneCore)
[![appveyor build status](https://ci.appveyor.com/api/projects/status/1ntjvooywpxmoir8/branch/develop?svg=true)](https://ci.appveyor.com/project/julianoes/dronecore/branch/develop)
[![Coverage Status](https://coveralls.io/repos/github/dronecore/DroneCore/badge.svg?branch=develop)](https://coveralls.io/github/dronecore/DroneCore?branch=develop)

## Description

DroneCore is an API and library for the [PX4 flight stack](http://github.com/PX4/Firmware) using [Mavlink](http://mavlink.org).

It is written in C++ and aiming to be:

- Easy to use with a simple API.
- Fast and lightweight.
- Cross-platform (Linux, Mac, Windows, iOS, Android).
- Extensible (using compile-time plugins).

## Roadmap

The next steps will be:

- Add Camera settings and actions interface.
- Add language bindings for Android, iOS, and Python

## Interfacing

DroneCore currently takes care of the mavlink messaging using a UDP network connection to the drone. Connecting via TCP, or serial is planned but not implemeted yet.

The library provides both synchronous (blocking) API calls, as well as asynchronous API calls using callbacks.

## API Overview

API consumers use the `DroneCore` class to discover and manage vehicles (`Device` objects), which in turn provide access to all other drone information and control objects (e.g. `Telemetry`, `Mission` etc.).

The links below take you to the respective header files:

- [dronecore](include/dronecore.h): set up connection, discover devices
- [device](include/device.h): a device providing access to modules below using ...
- [device_plugin_container.h.in](include/device_plugin_container.h.in) which is auto-generated on build.
- [info](plugins/info/info.h): general info about a device
- [telemetry](plugins/telemetry/telemetry.h): to receive telemetry data
- [action](plugins/action/action.h): to send commands such as arm, disarm, takeoff, land to drone
- [mission](plugins/mission/mission.h)/[mission_item](plugins/mission/mission_item.h): to upload a waypoint mission
- [offboard](plugins/offboard/offboard.h): for velocity control
- [gimbal](plugins/gimbal/gimbal.h): control a gimbal
- [follow_me](plugins/follow_me/follow_me.h): drone tracks a position supplied by DroneCore.
- [logging](plugins/logging/logging.h): (not implemented) data logging and streaming from the vehicle.

For more information see the [API Overview](https://docs.dronecore.io/en/getting_started/#api-overview) in the DroneCore Guide.


## Guide Docs (Build instructions etc.)

Instructions for how to use the library can be found in the [DroneCore Guide](https://docs.dronecore.io/en/).

Quick Links:

- [QuickStart](https://docs.dronecore.io/en/getting_started/)
- [Building the Library](https://docs.dronecore.io/en/contributing/build.html)
- [Examples](https://docs.dronecore.io/en/examples/)
- [API Reference](https://docs.dronecore.io/en/api_reference/)
- [FAQ](https://docs.dronecore.io/en/getting_started/faq.html)


## License

The DroneCore project is licensed under the permissive BSD 3-clause, see [LICENSE.md](LICENSE.md).

## Authors

See [AUTHORS.md](AUTHORS.md).


