# Gimbal

The [Gimbal](../api_reference/classmavsdk_1_1_gimbal.md) plugin controls gimbals that implement [MAVLink Gimbal Protocol v2](https://mavlink.io/en/services/gimbal_v2.html).

::: tip
This page covers real-time gimbal control from a ground station or companion computer. For gimbal actions within a mission (e.g. camera angle at a waypoint), see the [Mission](missions.md) guide.
:::

## Create the Plugin

```cpp
#include <mavsdk/plugins/gimbal/gimbal.h>
auto gimbal = Gimbal{system};
```

## Discover Gimbals

Gimbals are identified by a gimbal id. Subscribe to the gimbal list to be notified when one connects:

```cpp
gimbal.subscribe_gimbal_list([](Gimbal::GimbalList list) {
    for (auto& item : list.gimbals) {
        std::cout << "Gimbal id: " << item.gimbal_id << '\n';
    }
});
```

## Take Control

Claim primary control before sending commands; release it when done:

```cpp
gimbal.take_control(gimbal_id, Gimbal::ControlMode::Primary);
// ... control gimbal ...
gimbal.release_control(gimbal_id);
```

## Set Angles

Point the gimbal at a fixed roll/pitch/yaw (in degrees):

```cpp
gimbal.set_angles(gimbal_id, 0.0f, -30.0f, 0.0f,
                  Gimbal::GimbalMode::YawFollow,
                  Gimbal::SendMode::Once);
```

`YawFollow` keeps the gimbal yaw locked to the vehicle body; `YawLock` holds a fixed world heading.

## Point at a Location

```cpp
gimbal.set_roi_location(gimbal_id, latitude_deg, longitude_deg, altitude_m);
```

## Monitor Attitude

```cpp
gimbal.subscribe_attitude([](Gimbal::Attitude attitude) {
    std::cout << "Pitch: " << attitude.euler_angle_forward.pitch_deg << '\n';
});
```

## Further Information

- [API Reference: Gimbal](../api_reference/classmavsdk_1_1_gimbal.md)
- [Example: Gimbal](https://github.com/mavlink/MAVSDK/tree/main/cpp/examples/gimbal)
- [Example: Gimbal full control](https://github.com/mavlink/MAVSDK/tree/main/cpp/examples/gimbal_full_control)
- [Example: Gimbal device tester](https://github.com/mavlink/MAVSDK/tree/main/cpp/examples/gimbal_device_tester)
- [MAVLink Gimbal Protocol v2](https://mavlink.io/en/services/gimbal_v2.html)
