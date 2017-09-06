![DroneCore](https://raw.githubusercontent.com/dronecore/docs/master/assets/site/dronecore_logo_full.png)

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

## API Overview

- [dronecore](include/dronecore.h): set up connection, discover devices
- [device](include/device.h): a device providing access to modules below using ...
- [device_plugin_container.h.in](include/device_plugin_container.h.in) which is auto-generated on build.

- [info](plugins/info/info.h): general info about a device
- [telemetry](plugins/telemetry/telemetry.h): to receive telemetry data
- [action](plugins/action/action.h): to send commands such as arm, disarm, takeoff, land to drone
- [mission](plugins/mission/mission.h)/[mission_item](plugins/mission/mission_item.h): to upload a waypoint mission
- [offboard](plugins/offboard/offboard.h): for velocity or position control

See also: [Guide > API Overview](https://dronecore.gitbooks.io/dronecore-guide/content/en/getting_started/#api-overview).

## Guide Docs (Build instructions etc.)

Instructions for how to use the library can be found in the [DroneCore Guide](https://dronecore.gitbooks.io/dronecore-guide/content/en/ ). 

Quick Links:

- [QuickStart](https://dronecore.gitbooks.io/dronecore-guide/content/en/getting_started/).
- [Building the Library](https://dronecore.gitbooks.io/dronecore-guide/content/en/contributing/build.html)
- [Examples](https://dronecore.gitbooks.io/dronecore-guide/content/en/examples/).
- [API Reference](https://dronecore.gitbooks.io/dronecore-guide/content/en/api_reference/).
- [FAQ](https://dronecore.gitbooks.io/dronecore-guide/content/en/getting_started/faq.html).


## License

The DroneCore project is licensed under the permissive BSD 3-clause, see [LICENSE.md](LICENSE.md).

## Authors

See [AUTHORS.md](AUTHORS.md).

