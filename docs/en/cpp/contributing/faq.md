## FAQ

### Why was the SDK written in C++

The aim was to have an API in a language which is cross-platform and has many language bindings.
Additionally, the library needs to be lightweight and fast, so it does not fall over for onboard usage with high rate messaging.

### Does the SDK support multiple vehicles?

The SDK is designed to support multiple vehicles ("systems") but at time of writing there are no tests for this.

A system needs to have a specific MAVLink system ID, and can consist of multiple components with different component IDs. 
An example would be a drone with a gimbal and a camera talking MAVLink with the same system ID but different component IDs.

The limit is in theory 255 vehicles, with system IDs ranging from 1 to 255.

### Are serial connections supported?

The architecture supports serial, TCP, and UDP connections. 

### Why is libCURL a dependency?

*libCURL* will be required to download the camera definition file which is referenced in [CAMERA_INFORMATION](https://mavlink.io/en/messages/common.html#CAMERA_INFORMATION). 
It might also come in handy whenever any other REST resources are required.

### How are plugins added?

Plugins need to have the correct structure and be placed in the [plugins directory](../plugins). 
It is also possible to add external plugins which are outside of the SDK source tree. 
For instructions see the [build docs](build.md#build-with-external-directory-for-plugins-and-custom-integration_tests).

### Can a plugin depend on another one?

Yes it can but it should not! 
The idea is that plugins don't have any coupling between each other, which allows for any plugin to be changed or removed without breaking the rest. 
This will lead to some duplication at places, that's an acceptable trade-off. 
If the same code is used in many places, it can always be moved to core and get re-used from there.


### What is the difference between unit and integration test?

The unit tests are only concerned with one plugin or less. 
The integration tests however might need PX4 SITL running and combine multiple plugins.
