# Missions

The Mission API (plugin) allows you to create, upload, download, run, pause, restart, jump to item in, and track missions.
Missions can have multiple "mission items", each which may specify a position, altitude, fly-through behaviour, camera action, gimbal position, and the speed to use when traveling to the next position.

Missions are *managed* though the [Mission](../api_reference/classmavsdk_1_1_mission.md) class, which communicates with the vehicle to upload mission information and run, pause, track the mission progress etc.
The mission that is uploaded to the vehicle is defined as a vector of [MissionItem](../api_reference/structmavsdk_1_1_mission_1_1_mission_item.md) objects.

::: tip
The [Mission plugin](../api_reference/classmavsdk_1_1_mission.md) described here only supports a small subset of the full functionality of MAVLink missions. If you require the full mission item spec as MAVLink provides it, you might be better off using the [MissionRaw plugin](../api_reference/classmavsdk_1_1_mission_raw.md).
Furthermore `MissionRaw` allows importing mission from QGroundControl.
:::

## Supported Mission Commands {#supported_mission_commands}

The [MissionItem](../api_reference/structmavsdk_1_1_mission_1_1_mission_item.md) class abstracts a small but useful subset of the mission commands supported by PX4 (and the MAVLink specification):

The supported set is:
* [MAV_CMD_NAV_WAYPOINT](https://mavlink.io/en/messages/common.html#MAV_CMD_NAV_WAYPOINT)
* [MAV_CMD_DO_CHANGE_SPEED](https://mavlink.io/en/messages/common.html#MAV_CMD_DO_CHANGE_SPEED)
* [MAV_CMD_DO_MOUNT_CONTROL](https://mavlink.io/en/messages/common.html#MAV_CMD_DO_MOUNT_CONTROL)
* [MAV_CMD_IMAGE_START_CAPTURE](https://mavlink.io/en/messages/common.html#MAV_CMD_IMAGE_START_CAPTURE)
* [MAV_CMD_IMAGE_STOP_CAPTURE](https://mavlink.io/en/messages/common.html#MAV_CMD_IMAGE_STOP_CAPTURE)
* [MAV_CMD_VIDEO_START_CAPTURE](https://mavlink.io/en/messages/common.html#MAV_CMD_VIDEO_START_CAPTURE)
* [MAV_CMD_VIDEO_STOP_CAPTURE](https://mavlink.io/en/messages/common.html#MAV_CMD_VIDEO_STOP_CAPTURE)
* [MAV_CMD_NAV_LOITER_TIME](https://mavlink.io/en/messages/common.html#MAV_CMD_NAV_LOITER_TIME)

Additionally, the following commands are supported only for mission import/download (there are no corresponding `MissionItem` methods):
* [MAV_CMD_NAV_LAND](https://mavlink.io/en/messages/common.html#MAV_CMD_NAV_LAND)
* [MAV_CMD_NAV_TAKEOFF](https://mavlink.io/en/messages/common.html#MAV_CMD_NAV_TAKEOFF)

::: tip
The Mission API does not (at time of writing) provide explicit functionality to "repeat", takeoff, return to land etc.
The SDK provides some omitted functionality through the [Action](../guide/taking_off_landing.md) API.
:::


## Create the Plugin

::: tip
`Mission` objects are created in the same way as other MAVSDK plugins.
General instructions are provided in the topic: [Using Plugins](../guide/using_plugins.md).
:::

The main steps are:

1. [Create a connection](../guide/connections.md) to a `system`.
   For example (basic code without error checking):
   ```
   #include <mavsdk/mavsdk.h>
   Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
   ConnectionResult conn_result = mavsdk.add_udp_connection();
   // Wait for the system to connect via heartbeat
   while (mavsdk.system().size() == 0) {
      sleep_for(seconds(1));
   }
   // System got discovered.
   System system = mavsdk.systems()[0];
   ```
1. Create an instance of `Mission` with the `system`:
   ```
   #include <mavsdk/plugins/mission/mission.h>
   auto mission = Mission{system};
   ```

The `mission` object can then used to access the plugin API (as shown in the following sections).


## Defining a Mission

A mission must be defined as a vector of [MissionItem](../api_reference/structmavsdk_1_1_mission_1_1_mission_item.md) objects as shown below:
```cpp
std::vector<std::shared_ptr<MissionItem>> mission_items;
```

You can create as many `MissionItem` objects as you like and use `std_vector::push_back()` to add them to the back of the mission item vector.
The example below shows how to create and add a `MissionItem` that sets the target position.
```cpp
// Create MissionItem and set its position
std::shared_ptr<MissionItem> new_item(new MissionItem());
new_item->latitude_deg = 47.40;
new_item->longitude_deg = 8.5455360114574432;

// Add new_item to the vector
mission_items.push_back(new_item);
```

The example below shows how you might set the other options on a second `MissionItem` (and add it to the mission).

```cpp
std::shared_ptr<MissionItem> new_item2(new MissionItem());
new_item2->latitude_deg = 47.40;
new_item2->longitude_deg = 8.5455360114574432);
new_item2->relative_altitude_m = 2.0f;
new_item2->speed_m_s = 5.0f;
new_item2->is_fly_through = true;
new_item2->gimbal_pitch_deg = 20.0f;
new_item2->gimbal_pitch_deg = 20.0f;
new_item2->gimbal_yaw_deg = 60.0f;
new_item2->camera_action = MissionItem::CameraAction::TakePhoto;
new_item2->loiter_time_s = 1.0f;
new_item2->camera_photo_interval_s =  1.0f;

//Add new_item2 to the vector
mission_items.push_back(new_item2);
```

::: info
The autopilot has sensible default values for the attributes.
If you do set a value (e.g. the desired speed) then it will be the default for the remainder of the mission.
:::

::: info
There are also getter methods for querying the current value of `MissionItem` attributes.
The default values of most fields are `NaN` (which means they are ignored/not sent).
:::

The mission (`mission_items`) can then be uploaded as shown in the section [Uploading a Mission](#uploading_mission) below.

### Convenience Function

The [Fly Mission](../examples/fly_mission.md) uses a convenience function to create `MissionItem` objects.
Using this approach you have to specify every attribute for every mission item, whether or not the value is actually used.

The definition and use of this function is shown below:

```cpp
static Mission::MissionItem make_mission_item(
    double latitude_deg,
    double longitude_deg,
    float relative_altitude_m,
    float speed_m_s,
    bool is_fly_through,
    float gimbal_pitch_deg,
    float gimbal_yaw_deg,
    Mission::MissionItem::CameraAction camera_action);


mission_items.push_back(
    make_mission_item(47.398170327054473,
                      8.5456490218639658,
                      10.0f, 5.0f, false,
                      20.0f, 60.0f,
                      MissionItem::CameraAction::NONE));
```

## Uploading a Mission {#uploading_mission}

Use [Mission::upload_mission()](../api_reference/classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1a38274b1c1509375a182c44711ee9f7b1) to upload the mission defined in the previous section.

The example below shows how this is done.

```cpp
// ... declare and populate the mission vector: mission_items

{
    std::cout << "Uploading mission..." << '\n';
    Mission::MissionPlan mission_plan{};
    mission_plan.mission_items = mission_items;
    const Mission::Result result = mission.upload_mission(
        mission_plan);

    if (result != Mission::Result::Success) {
        std::cout << "Mission upload failed (" << result << "), exiting." << '\n';
        return 1;
    }
    std::cout << "Mission uploaded." << '\n';
}
```

## Starting/Pausing Missions

Start or resume a paused mission using [Mission::start_mission](../api_reference/classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1a927fecc7734810d01cc218153780e6e3).
The vehicle must already have a mission (the mission need not have been uploaded using the SDK).

The code fragment below shows how this is done.

```cpp
{
    const Mission::Result result = mission.start_mission();

    if (result != Mission::Result::Success) {
        std::cout << "Mission start failed (" << result << "), exiting." << '\n';
        return 1;
    }
    std::cout << "Started mission." << '\n';
}
```

To pause a mission use [Mission::pause_mission()](../api_reference/classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1ab569500d992d6d859c1c35052db07315).
The code is almost exactly the same as for starting a mission:

```cpp
{
    std::cout << "Pausing mission..." << '\n';

    const Mission::Result result = mission.pause_mission();
    if (result != Mission::Result::Success) {
        std::cout << "Failed to pause mission (" << result << ")" << '\n';
    } else {
        std::cout << "Mission paused." << '\n';
    }
}
```

## Monitoring Progress

Asynchronously monitor progress using [Mission::subscribe_mission_progress()](../api_reference/classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1a6dd32b92e593c1a692fe59e5bfb670fb),
which receives a regular callback with the current `MissionItem` number and the total number of items.

The code fragment just takes a lambda function that reports the current status.

```cpp
mission.subscribe_mission_progress( [](Mission::MissionProgress mission_progress)) {
       std::cout << "Mission status update: " << mission_progress.current << " / " << mission_progress.total << '\n';
    });
```

::: info
The mission is complete when `current == total`.
:::

The following synchronous methods is also available for checking mission progress:
* [is_mission_finished()](../api_reference/classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1a1ecf4f8798ab9ae96882dfbd34f23466) - Checks if mission has been finished.



## Taking Off, Landing, Returning

If using a copter or VTOL vehicle then PX4 will automatically takeoff when it is armed and a mission is started (even without a takeoff mission item).
For Fixed Wing vehicles the vehicle must be launched before starting a mission.

At time of writing the Mission API does not provide takeoff, land or "return to launch" `MissionItems`.
If required you can instead use the appropriate commands in the [Action](../guide/taking_off_landing.md) class.

<!-- Update if we get new mission items -->

## Downloading Missions

Use [Mission::download_mission()](../api_reference/classmavsdk_1_1_mission.md#classmavsdk_1_1_mission_1a23e9f7da32f42bcce7ef16ea8044fe53) to download a mission from the vehicle.
The mission is downloaded as a vector of [MissionItem](../api_reference/structmavsdk_1_1_mission_1_1_mission_item.md) objects, that you can then view or manipulate as required.

::: info
Mission download will fail if the mission contains a command that is outside the [supported set](#supported_mission_commands).
Missions created using *QGroundControl* are not guaranteed to successfully download!
Again, for that case [MissionRaw](../api_reference/classmavsdk_1_1_mission_raw.md) might be a better fit.
:::

The code fragment below shows how to download a mission:

```cpp
{
    std::cout << "Downloading mission." << '\n';
    std::pair<Result, Mission::MissionPlan> result = mission.download_mission();

    if (result.first != Mission::Result::Success) {
        std::cout << "Mission download failed (" << result.first << "), exiting." << '\n';
        return 1;
    }

    std::cout << "Mission downloaded (MissionItems: "
        << result.second.mission_items.size()
        << ")" << '\n';
}
```


## Further Information

* [Mission Flight Mode](https://docs.px4.io/master/en/flight_modes/mission.html) (PX4 User Guide)
* [Example:Fly Mission](../examples/fly_mission.md)
* Integration tests:
  * [mission.cpp](https://github.com/mavlink/MAVSDK/blob/main/src/integration_tests/mission.cpp)
  * [mission_cancellation.cpp](https://github.com/mavlink/MAVSDK/blob/main/src/integration_tests/mission_cancellation.cpp)
  * [mission_change_speed.cpp](https://github.com/mavlink/MAVSDK/blob/main/src/integration_tests/mission_change_speed.cpp)
  * [mission_raw_mission_changed.cpp](https://github.com/mavlink/MAVSDK/blob/main/src/integration_tests/mission_raw_mission_changed.cpp)
  * [mission_rtl.cpp](https://github.com/mavlink/MAVSDK/blob/main/src/integration_tests/mission_rtl.cpp)
  * [mission_transfer_lossy.cpp](https://github.com/mavlink/MAVSDK/blob/main/src/integration_tests/mission_transfer_lossy.cpp)
