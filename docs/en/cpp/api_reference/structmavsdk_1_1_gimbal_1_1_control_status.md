# mavsdk::Gimbal::ControlStatus Struct Reference
`#include: gimbal.h`

----


Control status. 


## Data Fields


[ControlMode](classmavsdk_1_1_gimbal.md#classmavsdk_1_1_gimbal_1a01b721086d7de6089aefdeb0fda4cff3) [control_mode](#structmavsdk_1_1_gimbal_1_1_control_status_1a58ab8c223f8dd71268bbcfdc019fae8a) {} - Control mode (none, primary or secondary)

int32_t [sysid_primary_control](#structmavsdk_1_1_gimbal_1_1_control_status_1a99fdfdc94af7663c03bd88ac4bde82c3) {} - Sysid of the component that has primary control over the gimbal (0 if no one is in control)

int32_t [compid_primary_control](#structmavsdk_1_1_gimbal_1_1_control_status_1afe0b88101acc98f412553316a34d12fe) {} - Compid of the component that has primary control over the gimbal (0 if no one is in control)

int32_t [sysid_secondary_control](#structmavsdk_1_1_gimbal_1_1_control_status_1a539dc995ffa028a7b31051f6077ffe65) {} - Sysid of the component that has secondary control over the gimbal (0 if no one is in control)

int32_t [compid_secondary_control](#structmavsdk_1_1_gimbal_1_1_control_status_1a6d2db416b726ccb3cb21ce0a382bd733) {} - Compid of the component that has secondary control over the gimbal (0 if no one is in control)


## Field Documentation


### control_mode {#structmavsdk_1_1_gimbal_1_1_control_status_1a58ab8c223f8dd71268bbcfdc019fae8a}

```cpp
ControlMode mavsdk::Gimbal::ControlStatus::control_mode {}
```


Control mode (none, primary or secondary)


### sysid_primary_control {#structmavsdk_1_1_gimbal_1_1_control_status_1a99fdfdc94af7663c03bd88ac4bde82c3}

```cpp
int32_t mavsdk::Gimbal::ControlStatus::sysid_primary_control {}
```


Sysid of the component that has primary control over the gimbal (0 if no one is in control)


### compid_primary_control {#structmavsdk_1_1_gimbal_1_1_control_status_1afe0b88101acc98f412553316a34d12fe}

```cpp
int32_t mavsdk::Gimbal::ControlStatus::compid_primary_control {}
```


Compid of the component that has primary control over the gimbal (0 if no one is in control)


### sysid_secondary_control {#structmavsdk_1_1_gimbal_1_1_control_status_1a539dc995ffa028a7b31051f6077ffe65}

```cpp
int32_t mavsdk::Gimbal::ControlStatus::sysid_secondary_control {}
```


Sysid of the component that has secondary control over the gimbal (0 if no one is in control)


### compid_secondary_control {#structmavsdk_1_1_gimbal_1_1_control_status_1a6d2db416b726ccb3cb21ce0a382bd733}

```cpp
int32_t mavsdk::Gimbal::ControlStatus::compid_secondary_control {}
```


Compid of the component that has secondary control over the gimbal (0 if no one is in control)

