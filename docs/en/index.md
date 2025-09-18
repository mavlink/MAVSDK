<div style="float:right; padding:10px; margin-right:20px;"><img src="../assets/site/sdk_logo_full.png" title="MAVSDK Logo" width="400px"/></div>

# MAVSDK (main / v3)

*MAVSDK* is a collection of libraries for various programming languages to interface with [MAVLink](https://mavlink.io/en/) systems such as drones, cameras or ground systems.

The libraries provides a simple API for managing one or more vehicles, providing programmatic access to vehicle information and telemetry, and control over missions, movement and other operations.

The libraries can be used onboard a drone on a companion computer or on the ground for a  ground station or mobile device.

MAVSDK is cross-platform: Linux, macOS, Windows, Android and iOS.

## Programming Languages

MAVSDK is primarily written in C++ with wrappers available for several programming languages:

- [MAVSDK-C++](https://github.com/mavlink/MAVSDK) (2016): Used in production.
- [MAVSDK-Swift](https://github.com/mavlink/MAVSDK-Swift) (2018): Used in production.
- [MAVSDK-Python](https://github.com/mavlink/MAVSDK-Python) (2019): Used in production.
- [MAVSDK-Java](https://github.com/mavlink/MAVSDK-Java) (2019): Used in production.
- [MAVSDK-Go](https://github.com/mavlink/MAVSDK-Go) (2020): Proof of concept.
- [MAVSDK-JavaScript](https://github.com/mavlink/MAVSDK-JavaScript) (2019): Proof of concept.
- [MAVSDK-CSharp](https://github.com/mavlink/MAVSDK-CSharp) (2019). Proof of concept.
- [MAVSDK-Rust](https://github.com/mavlink/MAVSDK-Rust) (2019): Proof of concept.

## Getting Started

Check out the quickstart guide for [C++](cpp/quickstart.md) and [Python](python/quickstart.md).
And no matter which language you are using, use the [C++ Guide](cpp/guide) to learn how to
perform common tasks and use the library in general.

## Getting Help {#getting-help}

This guide contains information and examples showing how to use MAVSDK.
If you have specific questions that are not answered by the documentation, these can be raised on:

* [Discuss board](https://discuss.px4.io/c/mavsdk)
* [Discord (#mavsdk)](https://discord.gg/dronecode) 

Use Github for bug reports/enhancement requests:

* [C++ API](https://github.com/mavlink/MAVSDK/issues)
* [C++ Documentation](https://github.com/mavlink/MAVSDK-docs/issues)
* [Swift API and docs](https://github.com/mavlink/MAVSDK-Swift/issues)
* [Python API and docs](https://github.com/mavlink/MAVSDK-Python/issues)
<!-- Add info about where other API issues are reported). -->


## Contributing

We welcome contributions! If you want to help or have suggestions/bug reports [please get in touch with the development team](#getting-help).

The [Contributing](cpp/contributing/index.md) (C++) section contains more information on how and what to contribute, including topics about building MAVSDK from source code, running our integration and unit tests, and all other aspects of core development.

## Maintenance

This project is maintained by volunteers:
- [Julian Oes](https://github.com/julianoes) ([sponsoring](https://github.com/sponsors/julianoes), [consulting](https://julianoes.com)).
- [Jonas Vautherin](https://github.com/JonasVautherin)

Maintenance is not sponsored by any company, however, hosting of the [docs](https://mavsdk.mavlink.io/main/en/) and the [forum](https://discuss.px4.io/c/mavsdk/) is provided by the [Dronecode Foundation](https://dronecode.org).

## Support and issues

If you just have a question, consider asking in the [forum](https://discuss.px4.io/c/mavsdk/).

If you have run into an issue, discovered a bug, or want to request a feature, create an [issue](https://github.com/mavlink/MAVSDK/issues). If it is important or urgent to you, consider sponsoring any of the maintainers to move the issue up on their todo list.

If you need private support, consider paid consulting:
- [Julian Oes consulting](https://julianoes.com)

(Create a pull request if you wish to be listed here.)

## License

* The *MAVSDK* is licensed under the permissive [BSD 3-clause](https://github.com/mavlink/MAVSDK/blob/main/LICENSE.md).
* This documentation is licensed under [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/) license.

## Governance

The MAVSDK project is hosted under the governance of the [Dronecode Foundation](https://www.dronecode.org/).

<a href="https://www.dronecode.org/" style="padding:20px" ><img src="../assets/site/logo_dronecode.png" alt="Dronecode Logo" width="110px"/></a>
<a href="https://www.linuxfoundation.org/projects" style="padding:20px;"><img src="../assets/site/logo_linux_foundation.png" alt="Linux Foundation Logo" width="80px" /></a>
<div style="padding:10px">&nbsp;</div>
