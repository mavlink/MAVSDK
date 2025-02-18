# mavsdk::Gimbal::GimbalItem Struct Reference
`#include: gimbal.h`

----


[Gimbal](classmavsdk_1_1_gimbal.md) list item. 


## Data Fields


int32_t [gimbal_id](#structmavsdk_1_1_gimbal_1_1_gimbal_item_1a9424d6672c7ce15ba1e76f7dba72c398) {} - ID to address it, starting at 1 (0 means all gimbals)

std::string [vendor_name](#structmavsdk_1_1_gimbal_1_1_gimbal_item_1a4f242d74e738cf807770c8d45340edcb) {} - Vendor name.

std::string [model_name](#structmavsdk_1_1_gimbal_1_1_gimbal_item_1a7429465ade39f000358ba6597dd21c07) {} - Model name.

std::string [custom_name](#structmavsdk_1_1_gimbal_1_1_gimbal_item_1a495511db3d00aa170512dbe3f862a919) {} - Custom name name.

int32_t [gimbal_manager_component_id](#structmavsdk_1_1_gimbal_1_1_gimbal_item_1a9b378bec4973e834803917c341b6832d) {} - MAVLink component of gimbal manager, for debugging purposes.

int32_t [gimbal_device_id](#structmavsdk_1_1_gimbal_1_1_gimbal_item_1a6c6ed15759db38636774ca244367dafd) {} - MAVLink component of gimbal device.


## Field Documentation


### gimbal_id {#structmavsdk_1_1_gimbal_1_1_gimbal_item_1a9424d6672c7ce15ba1e76f7dba72c398}

```cpp
int32_t mavsdk::Gimbal::GimbalItem::gimbal_id {}
```


ID to address it, starting at 1 (0 means all gimbals)


### vendor_name {#structmavsdk_1_1_gimbal_1_1_gimbal_item_1a4f242d74e738cf807770c8d45340edcb}

```cpp
std::string mavsdk::Gimbal::GimbalItem::vendor_name {}
```


Vendor name.


### model_name {#structmavsdk_1_1_gimbal_1_1_gimbal_item_1a7429465ade39f000358ba6597dd21c07}

```cpp
std::string mavsdk::Gimbal::GimbalItem::model_name {}
```


Model name.


### custom_name {#structmavsdk_1_1_gimbal_1_1_gimbal_item_1a495511db3d00aa170512dbe3f862a919}

```cpp
std::string mavsdk::Gimbal::GimbalItem::custom_name {}
```


Custom name name.


### gimbal_manager_component_id {#structmavsdk_1_1_gimbal_1_1_gimbal_item_1a9b378bec4973e834803917c341b6832d}

```cpp
int32_t mavsdk::Gimbal::GimbalItem::gimbal_manager_component_id {}
```


MAVLink component of gimbal manager, for debugging purposes.


### gimbal_device_id {#structmavsdk_1_1_gimbal_1_1_gimbal_item_1a6c6ed15759db38636774ca244367dafd}

```cpp
int32_t mavsdk::Gimbal::GimbalItem::gimbal_device_id {}
```


MAVLink component of gimbal device.

