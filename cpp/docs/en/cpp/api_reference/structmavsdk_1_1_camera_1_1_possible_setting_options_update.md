# mavsdk::Camera::PossibleSettingOptionsUpdate Struct Reference
`#include: camera.h`

----


An update about possible setting options. 


## Data Fields


int32_t [component_id](#structmavsdk_1_1_camera_1_1_possible_setting_options_update_1a2e9c2092ebd74c24bfbca3640fdd717b) {} - Component ID.

std::vector< [SettingOptions](structmavsdk_1_1_camera_1_1_setting_options.md) > [setting_options](#structmavsdk_1_1_camera_1_1_possible_setting_options_update_1a215577526139988181681a0d6eb5c2f0) {} - List of settings that can be changed.


## Field Documentation


### component_id {#structmavsdk_1_1_camera_1_1_possible_setting_options_update_1a2e9c2092ebd74c24bfbca3640fdd717b}

```cpp
int32_t mavsdk::Camera::PossibleSettingOptionsUpdate::component_id {}
```


Component ID.


### setting_options {#structmavsdk_1_1_camera_1_1_possible_setting_options_update_1a215577526139988181681a0d6eb5c2f0}

```cpp
std::vector<SettingOptions> mavsdk::Camera::PossibleSettingOptionsUpdate::setting_options {}
```


List of settings that can be changed.

