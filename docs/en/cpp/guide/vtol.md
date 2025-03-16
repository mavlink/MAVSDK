# VTOL Support

MAVSDK has only basic support for VTOL vehicles.
VTOL waypoint missions are only supported using the [MissionRaw plugin](../api_reference/classmavsdk_1_1_mission_raw.md).

::: info
Most design and test effort has gone into multicopter support.
[Get in touch](../../index.md#getting-help) if you would like to help enhance the VTOL/Fixed-wing experience!
:::

## Supported Functionality

Much of the functionality in MAVSDK is generic, and useful on all vehicle types.
At time of writing the only *VTOL-specific* functionality are the actions to
[transition between fixed wing and copter modes](../guide/taking_off_landing.md#transition_vtol).

MAVSDK can be used to command VTOL vehicles to:

- Take off and land in *multicopter* mode.
- Fly in multicopter mode and transition to fly in fixed wing mode.
- Use the camera/gimbal and other generic features.


## Remaining Work

Known omissions:

- You can't include the transition to/from fixed wing in the mission itself, but have to use the transition actions.
- VTOL takeoff in fixed wing mode is not supported.
- VTOL specific testing needs to be considered and implemented.


## Further Information

Additional information/examples are linked below:

* [Guide: Taking Off and Landing > Transition to fixed wing mode](../guide/taking_off_landing.md#transition_vtol)
* [Example: VTOL Transitions](../examples/transition_vtol_fixed_wing.md)
* Integration tests:
  * [action_transition_multicopter_fixedwing.cpp](https://github.com/mavlink/MAVSDK/blob/main/src/integration_tests/action_transition_multicopter_fixedwing.cpp)

