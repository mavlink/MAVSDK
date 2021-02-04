[![prototool](https://github.com/mavlink/MAVSDK-Proto/workflows/prototool/badge.svg)](https://github.com/mavlink/MAVSDK-Proto/actions?query=branch%3Amain)

# MAVSDK-Proto

MAVSDK is made of a backend in C++ called mavsdk_server, exposing a protobuf API that can be used by different frontends, such as [MAVSDK-Python](https://github.com/dronecode/MAVSDK-Python), [MAVSDK-Swift](https://github.com/dronecode/MAVSDK-Swift) or [MAVSDK-Java](https://github.com/dronecode/MAVSDK-Java).

The mavsdk_server is made of different components responsible for different parts of the API. Each component defines a protobuf interface in the form of a directory containing `*.proto` files in this repository.

The proto files defining the components are located into [protos](protos), and the protobuf plugins generating the bindings in the different languages are located into [pb_plugins](pb_plugins).
