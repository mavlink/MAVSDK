# Build mavsdk_server from Source

Language wrappers for MAVSDK other than C++ connect to the MAVSDK C++ core using gRPC. This gRPC server around the MAVSDK C++ library is called mavsdk_server (in the past it was referred to as the backend).

For more information about the architecture, also see [how the auto-generation works](../contributing/autogen.md).

::: info
It is generally recommended to use pre-built binaries of mavsdk_server from the [releases page](https://github.com/mavlink/MAVSDK/releases), however, if needed, it can also be built manually from source as described below.
:::


This topic has been split into:
- [Build on Linux](build_linux.md)
- [Build on macOS](build_macos.md)
- [Build on Windows](build_windows.md)
- [Build with dockcross](build_dockcross.md) (for cross compilation and Android)
