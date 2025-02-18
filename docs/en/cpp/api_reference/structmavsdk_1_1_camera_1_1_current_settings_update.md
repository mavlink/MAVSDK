# mavsdk::Camera::CurrentSettingsUpdate Struct Reference
`#include: camera.h`

----


An update about a current setting. 


## Data Fields


int32_t [component_id](#structmavsdk_1_1_camera_1_1_current_settings_update_1ac558277e13980dbebac288334741d163) {} - Component ID.

std::vector< [Setting](structmavsdk_1_1_camera_1_1_setting.md) > [current_settings](#structmavsdk_1_1_camera_1_1_current_settings_update_1a459873efe09aec15c83fd78eab046cc7) {} - List of current settings.


## Field Documentation


### component_id {#structmavsdk_1_1_camera_1_1_current_settings_update_1ac558277e13980dbebac288334741d163}

```cpp
int32_t mavsdk::Camera::CurrentSettingsUpdate::component_id {}
```


Component ID.


### current_settings {#structmavsdk_1_1_camera_1_1_current_settings_update_1a459873efe09aec15c83fd78eab046cc7}

```cpp
std::vector<Setting> mavsdk::Camera::CurrentSettingsUpdate::current_settings {}
```


List of current settings.

