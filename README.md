<img alt="MAVSDK" src="https://mavsdk.mavlink.io/main/assets/site/sdk_logo_full.png" width="400">

[![GitHub Actions Status](https://github.com/mavlink/MAVSDK/workflows/Build%20and%20Test/badge.svg?branch=main)](https://github.com/mavlink/MAVSDK/actions?query=branch%3Amain)
[![Coverage Status](https://coveralls.io/repos/github/mavlink/MAVSDK/badge.svg?branch=main)](https://coveralls.io/github/mavlink/MAVSDK?branch=main)

## Description

[MAVSDK](https://mavsdk.mavlink.io/main/en/) is a set of libraries providing a high-level API to [MAVLink](https://mavlink.io/en/).
It aims to be:
- Easy to use with a simple API supporting both synchronous (blocking) API calls and asynchronous API calls using callbacks.
- Fast and lightweight.
- Cross-platform (Linux, macOS, Windows, iOS, Android).
- Extensible (using the MavlinkPassthrough plugin).
- Fully compliant with the MAVLink standard/definitions.

In order to support multiple programming languages, MAVSDK implements a gRPC server in C++ which allows clients in different programming languages to connect to. The API is defined by the proto IDL ([proto files](https://github.com/mavlink/MAVSDK-Proto/tree/master/protos)).
This architecture allows the clients to be implemented in idiomatic patterns, so using the tooling and syntax expected by end users. For example, the Python library can be installed from PyPi using `pip`.

The MAVSDK C++ part consists of:
- The [core library](https://github.com/mavlink/MAVSDK/tree/main/src/mavsdk/core) implementing the basic MAVLink communication.
- The [plugin libraries](https://github.com/mavlink/MAVSDK/tree/main/src/mavsdk/plugins) implementing the MAVLink communication specific to a feature.
- The [mavsdk_server](https://github.com/mavlink/MAVSDK/tree/main/src/mavsdk_server) implementing the gRPC server for the language clients.

## Repos

- [MAVSDK](https://github.com/mavlink/MAVSDK) - this repo containing the source code for the C++ core.
- [MAVSDK-Proto](https://github.com/mavlink/MAVSDK-Proto) - Common interface definitions for API specified as proto files used by gRPC between language clients and mavsdk_server.
- [MAVSDK-Python](https://github.com/mavlink/MAVSDK-Python) - MAVSDK client for Python (first released on Pypi 2019).
- [MAVSDK-Swift](https://github.com/mavlink/MAVSDK-Swift) - MAVSDK client for Swift (used in production, first released 2018).
- [MAVSDK-Java](https://github.com/mavlink/MAVSDK-Java) - MAVSDK client for Java (first released on MavenCentral in 2019).
- [MAVSDK-JavaScript](https://github.com/mavlink/MAVSDK-JavaScript) - MAVSDK client in JavaScript (proof of concept, 2019).
- [MAVSDK-Rust](https://github.com/mavlink/MAVSDK-Rust) - MAVSDK client for Rust (proof of concept, 2019).
- [MAVSDK-CSharp](https://github.com/mavlink/MAVSDK-CSharp) - MAVSDK client for CSharp (proof of concept, 2019).
- [Docs](https://github.com/mavlink/MAVSDK-docs) - MAVSDK [docs](https://mavsdk.mavlink.io/main/en/) source.

## Docs

Instructions for how to use the C++ library can be found in the [MAVSDK docs](https://mavsdk.mavlink.io/main/en/) (links to other programming languages can be found from the documentation sidebar).

Quick Links:

- [Getting started](https://mavsdk.mavlink.io/main/en/cpp/#getting-started)
- [C++ API Overview](https://mavsdk.mavlink.io/main/en/cpp/#api-overview)
- [API Reference](https://mavsdk.mavlink.io/main/en/cpp/api_reference/)
- [Installing the Library](https://mavsdk.mavlink.io/main/en/cpp/guide/installation.html)
- [Building the Library](https://mavsdk.mavlink.io/main/en/cpp/guide/build.html)
- [Examples](https://mavsdk.mavlink.io/main/en/cpp/examples/)
- [FAQ](https://mavsdk.mavlink.io/main/en/faq.html)

## License

This project is licensed under the permissive BSD 3-clause, see [LICENSE.md](LICENSE.md).

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
