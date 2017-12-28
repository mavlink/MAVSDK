# DroneCore-Proto

DroneCore is made of a backend in C++, exposing a protobuf API that can be used by different frontends, such as [DroneCore-Python](https://github.com/dronecore/DroneCore-Python) or [DroneCore-Java](https://github.com/dronecore/DroneCore-Java).

The backend is made of different components responsible for different parts of the API. Each component defines a protobuf interface in the form of a directory containing `*.proto` files in this repository.
