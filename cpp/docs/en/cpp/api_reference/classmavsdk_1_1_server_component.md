# mavsdk::ServerComponent Class Reference
`#include: server_component.h`

----


This class represents a component, used to initialize a server plugin. 


## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [~ServerComponent](#classmavsdk_1_1_server_component_1aa93dd133c0c5476bfd9269365e4afcec) ()=default | Destructor.
uint8_t | [component_id](#classmavsdk_1_1_server_component_1af05efb79ba6c1ed9992e13671d3268ba) () const | MAVLink component ID of this component.
void | [set_system_status](#classmavsdk_1_1_server_component_1a61a231624bbeacc4b7f5f98eb052b7e6) (uint8_t system_status) | Set system status of this MAVLink entity.


## Constructor & Destructor Documentation


### ~ServerComponent() {#classmavsdk_1_1_server_component_1aa93dd133c0c5476bfd9269365e4afcec}
```cpp
mavsdk::ServerComponent::~ServerComponent()=default
```


Destructor.


## Member Function Documentation


### component_id() {#classmavsdk_1_1_server_component_1af05efb79ba6c1ed9992e13671d3268ba}
```cpp
uint8_t mavsdk::ServerComponent::component_id() const
```


MAVLink component ID of this component.


**Returns**

&emsp;uint8_t - 

### set_system_status() {#classmavsdk_1_1_server_component_1a61a231624bbeacc4b7f5f98eb052b7e6}
```cpp
void mavsdk::ServerComponent::set_system_status(uint8_t system_status)
```


Set system status of this MAVLink entity.

The default system status is MAV_STATE_UNINIT.

**Parameters**

* uint8_t **system_status** - system status.