# mavsdk::Mavsdk::Configuration Class Reference
`#include: mavsdk.h`

----


Possible configurations. 


## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [Configuration](#classmavsdk_1_1_mavsdk_1_1_configuration_1a10477130d041107e76efd1f94e65b503) (uint8_t system_id, uint8_t component_id, bool always_send_heartbeats) | Create new [Configuration](classmavsdk_1_1_mavsdk_1_1_configuration.md) via manually configured system and component ID.
&nbsp; | [Configuration](#classmavsdk_1_1_mavsdk_1_1_configuration_1abcc015ee6be8abc2da2c967bbbda9fed) ([ComponentType](namespacemavsdk.md#namespacemavsdk_1a20fe7f7c8312779a187017111bf33d12) component_type) | Create new [Configuration](classmavsdk_1_1_mavsdk_1_1_configuration.md) using a component type. In this mode, the system and component ID will be automatically chosen.
&nbsp; | [Configuration](#classmavsdk_1_1_mavsdk_1_1_configuration_1a1a65e2e31f06bec1f6692a933c95b03c) ()=delete |
&nbsp; | [~Configuration](#classmavsdk_1_1_mavsdk_1_1_configuration_1a31cad2329ee14898752638d9c3759da9) ()=default |
uint8_t | [get_system_id](#classmavsdk_1_1_mavsdk_1_1_configuration_1a0497bdda816b674b1418ab07889ca781) () const | Get the system id of this configuration.
void | [set_system_id](#classmavsdk_1_1_mavsdk_1_1_configuration_1a28f495bce2cf89ca3c52bdfa938b1761) (uint8_t system_id) | Set the system id of this configuration.
uint8_t | [get_component_id](#classmavsdk_1_1_mavsdk_1_1_configuration_1adfcae3d5b6f047ad24d9c24983375e97) () const | Get the component id of this configuration.
void | [set_component_id](#classmavsdk_1_1_mavsdk_1_1_configuration_1aa590fbafa8ca104e1a004ca537f5798e) (uint8_t component_id) | Set the component id of this configuration.
bool | [get_always_send_heartbeats](#classmavsdk_1_1_mavsdk_1_1_configuration_1a0aa9008fe5a7498f374dbd2adad5f137) () const | Get whether to send heartbeats by default.
void | [set_always_send_heartbeats](#classmavsdk_1_1_mavsdk_1_1_configuration_1a0ad68b52763e205012b34faa5120a792) (bool always_send_heartbeats) | Set whether to send heartbeats by default.
[ComponentType](namespacemavsdk.md#namespacemavsdk_1a20fe7f7c8312779a187017111bf33d12) | [get_component_type](#classmavsdk_1_1_mavsdk_1_1_configuration_1a81d3645816f8a3072044498c3f539d12) () const | Component type of this configuration, used for automatic ID set.
void | [set_component_type](#classmavsdk_1_1_mavsdk_1_1_configuration_1a06461b86734eaa9544e80a4a907c9754) ([ComponentType](namespacemavsdk.md#namespacemavsdk_1a20fe7f7c8312779a187017111bf33d12) component_type) | Set the component type of this configuration.
uint8_t | [get_mav_type](#classmavsdk_1_1_mavsdk_1_1_configuration_1aafe9e8fc11dd0b688a836c123357e9ba) () const | Get the mav type (vehicle type) of this configuration.
void | [set_mav_type](#classmavsdk_1_1_mavsdk_1_1_configuration_1a16db98d8802c3427b1be10f0b72b977b) (uint8_t mav_type) | Set the mav type (vehicle type) of this configuration.

## Static Public Member Functions


Type | Name | Description
---: | --- | ---
uint8_t | [mav_type_for_component_type](#classmavsdk_1_1_mavsdk_1_1_configuration_1a9c5b385d4097b9f486acfbfb1191adc8) ([ComponentType](namespacemavsdk.md#namespacemavsdk_1a20fe7f7c8312779a187017111bf33d12) component_type) | Get the mav type for a given component type.


## Constructor & Destructor Documentation


### Configuration() {#classmavsdk_1_1_mavsdk_1_1_configuration_1a10477130d041107e76efd1f94e65b503}
```cpp
mavsdk::Mavsdk::Configuration::Configuration(uint8_t system_id, uint8_t component_id, bool always_send_heartbeats)
```


Create new [Configuration](classmavsdk_1_1_mavsdk_1_1_configuration.md) via manually configured system and component ID.


**Parameters**

* uint8_t **system_id** - the system id to store in this configuration
* uint8_t **component_id** - the component id to store in this configuration
* bool **always_send_heartbeats** - send heartbeats by default even without a system connected

### Configuration() {#classmavsdk_1_1_mavsdk_1_1_configuration_1abcc015ee6be8abc2da2c967bbbda9fed}
```cpp
mavsdk::Mavsdk::Configuration::Configuration(ComponentType component_type)
```


Create new [Configuration](classmavsdk_1_1_mavsdk_1_1_configuration.md) using a component type. In this mode, the system and component ID will be automatically chosen.


**Parameters**

* [ComponentType](namespacemavsdk.md#namespacemavsdk_1a20fe7f7c8312779a187017111bf33d12) **component_type** - the component type, used for automatically choosing ids.

### Configuration() {#classmavsdk_1_1_mavsdk_1_1_configuration_1a1a65e2e31f06bec1f6692a933c95b03c}
```cpp
mavsdk::Mavsdk::Configuration::Configuration()=delete
```


### ~Configuration() {#classmavsdk_1_1_mavsdk_1_1_configuration_1a31cad2329ee14898752638d9c3759da9}
```cpp
mavsdk::Mavsdk::Configuration::~Configuration()=default
```


## Member Function Documentation


### get_system_id() {#classmavsdk_1_1_mavsdk_1_1_configuration_1a0497bdda816b674b1418ab07889ca781}
```cpp
uint8_t mavsdk::Mavsdk::Configuration::get_system_id() const
```


Get the system id of this configuration.


**Returns**

&emsp;uint8_t - `uint8_t` the system id stored in this configuration, from 1-255

### set_system_id() {#classmavsdk_1_1_mavsdk_1_1_configuration_1a28f495bce2cf89ca3c52bdfa938b1761}
```cpp
void mavsdk::Mavsdk::Configuration::set_system_id(uint8_t system_id)
```


Set the system id of this configuration.


**Parameters**

* uint8_t **system_id** - 

### get_component_id() {#classmavsdk_1_1_mavsdk_1_1_configuration_1adfcae3d5b6f047ad24d9c24983375e97}
```cpp
uint8_t mavsdk::Mavsdk::Configuration::get_component_id() const
```


Get the component id of this configuration.


**Returns**

&emsp;uint8_t - `uint8_t` the component id stored in this configuration,from 1-255

### set_component_id() {#classmavsdk_1_1_mavsdk_1_1_configuration_1aa590fbafa8ca104e1a004ca537f5798e}
```cpp
void mavsdk::Mavsdk::Configuration::set_component_id(uint8_t component_id)
```


Set the component id of this configuration.


**Parameters**

* uint8_t **component_id** - 

### get_always_send_heartbeats() {#classmavsdk_1_1_mavsdk_1_1_configuration_1a0aa9008fe5a7498f374dbd2adad5f137}
```cpp
bool mavsdk::Mavsdk::Configuration::get_always_send_heartbeats() const
```


Get whether to send heartbeats by default.


**Returns**

&emsp;bool - whether to always send heartbeats

### set_always_send_heartbeats() {#classmavsdk_1_1_mavsdk_1_1_configuration_1a0ad68b52763e205012b34faa5120a792}
```cpp
void mavsdk::Mavsdk::Configuration::set_always_send_heartbeats(bool always_send_heartbeats)
```


Set whether to send heartbeats by default.


**Parameters**

* bool **always_send_heartbeats** - 

### get_component_type() {#classmavsdk_1_1_mavsdk_1_1_configuration_1a81d3645816f8a3072044498c3f539d12}
```cpp
ComponentType mavsdk::Mavsdk::Configuration::get_component_type() const
```


Component type of this configuration, used for automatic ID set.


**Returns**

&emsp;[ComponentType](namespacemavsdk.md#namespacemavsdk_1a20fe7f7c8312779a187017111bf33d12) - 

### set_component_type() {#classmavsdk_1_1_mavsdk_1_1_configuration_1a06461b86734eaa9544e80a4a907c9754}
```cpp
void mavsdk::Mavsdk::Configuration::set_component_type(ComponentType component_type)
```


Set the component type of this configuration.


**Parameters**

* [ComponentType](namespacemavsdk.md#namespacemavsdk_1a20fe7f7c8312779a187017111bf33d12) **component_type** - 

### get_mav_type() {#classmavsdk_1_1_mavsdk_1_1_configuration_1aafe9e8fc11dd0b688a836c123357e9ba}
```cpp
uint8_t mavsdk::Mavsdk::Configuration::get_mav_type() const
```


Get the mav type (vehicle type) of this configuration.


**Returns**

&emsp;uint8_t - `uint8_t` the mav type stored in this configuration

### set_mav_type() {#classmavsdk_1_1_mavsdk_1_1_configuration_1a16db98d8802c3427b1be10f0b72b977b}
```cpp
void mavsdk::Mavsdk::Configuration::set_mav_type(uint8_t mav_type)
```


Set the mav type (vehicle type) of this configuration.


**Parameters**

* uint8_t **mav_type** - 

### mav_type_for_component_type() {#classmavsdk_1_1_mavsdk_1_1_configuration_1a9c5b385d4097b9f486acfbfb1191adc8}
```cpp
static uint8_t mavsdk::Mavsdk::Configuration::mav_type_for_component_type(ComponentType component_type)
```


Get the mav type for a given component type.


**Parameters**

* [ComponentType](namespacemavsdk.md#namespacemavsdk_1a20fe7f7c8312779a187017111bf33d12) **component_type** - The component type

**Returns**

&emsp;uint8_t - The corresponding mav type