## FAQ

### Why was DroneCore written in C++

The aim was to have an API in a language which is cross-platform and has many language bindings. Additionally, the library needs to be lightweight and fast, so it does not fall over for onboard usage with high rate messaging.

### Does DroneCore support multiple vehicles?

Yes, DroneCore is designed to support multiple vehicles. A vehicle is called a device in DroneCore.

A device needs to have a specific mavlink system ID but can consist of multiple components with different component IDs. An example would be a drone with a gimbal and a camera talking mavlink with the same system ID but different component IDs.

The limit is in theory 255 vehicles for system IDs ranging from 1 to 255.

### Are serial connections supported?

The architecture has stubs for serial, TCP, and UDP connections. However, only UDP connections are implemented right now.

