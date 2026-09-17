# Python wrappers for MAVSDK

This folder contains the `mavsdk` Python package, which exposes two interfaces:

* `mavsdk` is an old school Python API (threads and callbacks, no asyncio).
* `mavsdk.asyncio` is an asyncio wrapper around `mavsdk` above.

Both ship in the same distribution, so there is nothing extra to install to use
either one.

Moreover, there is no longer a dependency on gRPC, unlike
[MAVSDK-Python](https://github.com/mavlink/mavsdk-python). The `mavsdk.asyncio`
API tries to be as close as possible to MAVSDK-Python in order to facilitate
transition. This is still new as compared to MAVSDK-Python, but the idea is to
eventually replace it. It has all the features of MAVSDK-Python, it just hasn't
been tested as much.
