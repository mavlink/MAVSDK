<img src="./assets/site/dronecore_logo_full.png" align="right" title="DroneCore Logo" width="400px"/>

# DroneCore

[![Build Status](https://travis-ci.org/dronecore/DroneCore.svg?branch=master)](https://travis-ci.org/dronecore/DroneCore)

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

## API overview

- [dronecore](include/dronecore.h): set up connection, discover devices
- [device](include/device.h): a device providing access to modules below using ...
- [device_plugin_container.h.in](include/device_plugin_container.h.in) which is auto-generated on build.

- [info](plugins/info/info.h): general info about a device
- [telemetry](plugins/telemetry/telemetry.h): to receive telemetry data
- [action](plugins/action/action.h): to send commands such as arm, disarm, takeoff, land to drone
- [mission](plugins/mission/mission.h)/[mission_item](plugins/mission/mission_item.h): to upload a waypoint mission
- [offboard](plugins/offboard/offboard.h): for velocity or position control

## Build instructions, usage

Instructions how to build the library and how to write an example can be found in [docs/build.md](docs/build.md).

## FAQ

Find a FAQ in [docs/faq.md](docs/faq.md).

## License

The DroneCore project is licensed under the permissive BSD 3-clause, see [LICENSE.md](LICENSE.md).

## Authors

See [AUTHORS.md](AUTHORS.md).

