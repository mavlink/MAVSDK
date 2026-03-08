# Python wrappers for MAVSDK

This folder contains two different Python wrappers for MAVSDK:

* `mavsdk` is an old school Python API (threads and callbacks, no asyncio).
* `aiomavsdk` is an asyncio wrapper around `mavsdk` above.

Moreover, there is no longer a dependency on gRPC, unlike
[MAVSDK-Python](https://github.com/mavlink/mavsdk-python). The `aiomavsdk` API
tries to be as close as possible to MAVSDK-Python in order to facilitate
transition. This is still new as compared to MAVSDK-Python, but the idea is to
eventually replace it. It has all the features of MAVSDK-Python, it just hasn't
been tested as much.
