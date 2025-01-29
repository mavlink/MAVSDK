# mavsdk::ServerComponent Class Reference
`#include: server_component.h`

----


This class represents a component, used to initialize a server plugin. 


## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [~ServerComponent](#classmavsdk_1_1_server_component_1aa93dd133c0c5476bfd9269365e4afcec) ()=default | Destructor.
uint8_t | [component_id](#classmavsdk_1_1_server_component_1af05efb79ba6c1ed9992e13671d3268ba) () const | MAVLink component ID of this component.


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