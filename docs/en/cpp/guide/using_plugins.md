# Managing Systems/Vehicles (Using Plugins)

Access to drone information, telemetry and control objects are provided by a number of different *plugins*.
For example, the [Action](../api_reference/classmavsdk_1_1_action.md) plugin is used to arm, takeoff and land the vehicle,
while the [Telemetry](../guide/telemetry.md) plugin can be used to query the vehicle GPS status, flight mode, etc.
A separate plugin instance must be created for each system that needs it.

> **Note** All plugins are declared/used in the same way. This topic uses the `Action` plugin for the purposes of the demonstration.

> **Note** Plugins are named using the convention **mavsdk\__plugin\_name_.so**.
  For more information see [Building C++ Apps](../guide/toolchain.md)


In the application source code:
1. `#include` the header file for the plugin.
   ```cpp
   #include <mavsdk/plugins/action/action.h>
   ```
1. [Create a connection](../guide/connections.md) to a `System` object (below named: `system`).
1. Create the plugin instance, specifying the `System` it is to be used with:
   ```cpp
   auto action = Action{system};
   ```
1. The pointer can then be used to invoke actions on the specified system. For example, to takeoff you would call the API as shown:
   ```cpp
   action.takeoff();
   ```
