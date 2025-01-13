# mavsdk::System Class Reference
`#include: system.h`

----


This class represents a system, made up of one or more components (e.g. autopilot, cameras, servos, gimbals, etc). 


[System](classmavsdk_1_1_system.md) objects are used to interact with UAVs (including their components) and standalone cameras. They are not created directly by application code, but are returned by the [Mavsdk](classmavsdk_1_1_mavsdk.md) class. 


## Public Types


Type | Description
--- | ---
enum [ComponentType](#classmavsdk_1_1_system_1af2a91929d9771ae0e59c98557027b1dc) | Component Types.
std::function< void(bool)> [IsConnectedCallback](#classmavsdk_1_1_system_1a0e56bb48498100fde0872a3ec376f282) | type for is connected callback.
[Handle](classmavsdk_1_1_handle.md)< bool > [IsConnectedHandle](#classmavsdk_1_1_system_1adedf1d76e922076dfd3ca3c726443efd) | handle type to unsubscribe from subscribe_is_connected.
std::function< void([ComponentType](classmavsdk_1_1_system.md#classmavsdk_1_1_system_1af2a91929d9771ae0e59c98557027b1dc))> [ComponentDiscoveredCallback](#classmavsdk_1_1_system_1a064172b17193bb9be448e2053c83627b) | type for component discovery callback
[Handle](classmavsdk_1_1_handle.md)< [ComponentType](classmavsdk_1_1_system.md#classmavsdk_1_1_system_1af2a91929d9771ae0e59c98557027b1dc) > [ComponentDiscoveredHandle](#classmavsdk_1_1_system_1adfb374a9eaaa765cf0813cfa6b40df39) | type for component discovery callback handle
std::function< void([ComponentType](classmavsdk_1_1_system.md#classmavsdk_1_1_system_1af2a91929d9771ae0e59c98557027b1dc), uint8_t)> [ComponentDiscoveredIdCallback](#classmavsdk_1_1_system_1a914c50b413b5bd61d334631096e614ca) | type for component discovery callback with component ID
[Handle](classmavsdk_1_1_handle.md)< [ComponentType](classmavsdk_1_1_system.md#classmavsdk_1_1_system_1af2a91929d9771ae0e59c98557027b1dc), uint8_t > [ComponentDiscoveredIdHandle](#classmavsdk_1_1_system_1abd573ae09348f33e7cd3a006fc26a708) | type for component discovery callback handle with component ID

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [~System](#classmavsdk_1_1_system_1abdc4208c07d776c628acdc037a881ea6) () | Destructor.
&nbsp; | [System](#classmavsdk_1_1_system_1ac0e97e92181683f6b31fe208165dc35c) (const [System](classmavsdk_1_1_system.md) &)=delete | Copy constructor (object is not copyable).
void | [init](#classmavsdk_1_1_system_1a5c4c30affa7abbe06cc809187f3ddcb3) (uint8_t system_id, uint8_t component_id)const | Initialize the system.
bool | [has_autopilot](#classmavsdk_1_1_system_1a0c3d766baa73f5b35e2950a6f0a38c02) () const | Checks whether the system has an autopilot.
bool | [is_standalone](#classmavsdk_1_1_system_1a7fb7ed01204498dcaa2ab7d9cc31acf2) () const | Checks whether the system is a standalone (non-autopilot).
bool | [has_camera](#classmavsdk_1_1_system_1a440fd601ed2120e1e41d9eab536a7da8) (int camera_id=-1)const | Checks whether the system has a camera with the given camera ID.
bool | [has_gimbal](#classmavsdk_1_1_system_1ad66c3ecc096970d40c34610e49dba929) () const | Checks whether the system has a gimbal.
bool | [is_connected](#classmavsdk_1_1_system_1ad07991ae044bc367e27f544db40d065b) () const | Checks if the system is connected.
uint8_t | [get_system_id](#classmavsdk_1_1_system_1a091d793db29719f4996040886ad951a6) () const | MAVLink [System](classmavsdk_1_1_system.md) ID of connected system.
std::vector< uint8_t > | [component_ids](#classmavsdk_1_1_system_1aa33f748f81cc512601451c80bd077888) () const | MAVLink component IDs of connected system.
[IsConnectedHandle](classmavsdk_1_1_system.md#classmavsdk_1_1_system_1adedf1d76e922076dfd3ca3c726443efd) | [subscribe_is_connected](#classmavsdk_1_1_system_1aae68747c23976fa7eb63ec0762493263) (const [IsConnectedCallback](classmavsdk_1_1_system.md#classmavsdk_1_1_system_1a0e56bb48498100fde0872a3ec376f282) & callback) | Subscribe to callback to be called when system connection state changes.
void | [unsubscribe_is_connected](#classmavsdk_1_1_system_1a2f1927d56c14a7ad44995bd56afb706f) ([IsConnectedHandle](classmavsdk_1_1_system.md#classmavsdk_1_1_system_1adedf1d76e922076dfd3ca3c726443efd) handle) | Unsubscribe from subscribe_is_connected.
[ComponentDiscoveredHandle](classmavsdk_1_1_system.md#classmavsdk_1_1_system_1adfb374a9eaaa765cf0813cfa6b40df39) | [subscribe_component_discovered](#classmavsdk_1_1_system_1a25ede402b74a9412334ff1cab521e7d3) (const [ComponentDiscoveredCallback](classmavsdk_1_1_system.md#classmavsdk_1_1_system_1a064172b17193bb9be448e2053c83627b) & callback) | Subscribe to be called when a component is discovered.
void | [unsubscribe_component_discovered](#classmavsdk_1_1_system_1a5d62d308534b5e1673779543d02037f3) ([ComponentDiscoveredHandle](classmavsdk_1_1_system.md#classmavsdk_1_1_system_1adfb374a9eaaa765cf0813cfa6b40df39) handle) | Unsubscribe from subscribe_component_discovered.
[ComponentDiscoveredIdHandle](classmavsdk_1_1_system.md#classmavsdk_1_1_system_1abd573ae09348f33e7cd3a006fc26a708) | [subscribe_component_discovered_id](#classmavsdk_1_1_system_1a1905176f3a6633a8a9fe655f2dcd2d23) (const [ComponentDiscoveredIdCallback](classmavsdk_1_1_system.md#classmavsdk_1_1_system_1a914c50b413b5bd61d334631096e614ca) & callback) | Subscribe to be called when a component is discovered.
void | [unsubscribe_component_discovered_id](#classmavsdk_1_1_system_1a37bca637341bba8b07e8f95c97d122db) ([ComponentDiscoveredIdHandle](classmavsdk_1_1_system.md#classmavsdk_1_1_system_1abd573ae09348f33e7cd3a006fc26a708) handle) | Unsubscribe from subscribe_component_discovered_id.
void | [enable_timesync](#classmavsdk_1_1_system_1a7c7177fb0789aefbfb375f4bb12ce824) () | Enable time synchronization using the TIMESYNC messages.
const [System](classmavsdk_1_1_system.md) & | [operator=](#classmavsdk_1_1_system_1a21284c27829fda2391ee27f5732f916d) (const [System](classmavsdk_1_1_system.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### ~System() {#classmavsdk_1_1_system_1abdc4208c07d776c628acdc037a881ea6}
```cpp
mavsdk::System::~System()
```


Destructor.


### System() {#classmavsdk_1_1_system_1ac0e97e92181683f6b31fe208165dc35c}
```cpp
mavsdk::System::System(const System &)=delete
```


Copy constructor (object is not copyable).


**Parameters**

* const [System](classmavsdk_1_1_system.md)&  - 

## Member Typdef Documentation


### typedef IsConnectedCallback {#classmavsdk_1_1_system_1a0e56bb48498100fde0872a3ec376f282}

```cpp
using mavsdk::System::IsConnectedCallback =  std::function<void(bool)>
```


type for is connected callback.


### typedef IsConnectedHandle {#classmavsdk_1_1_system_1adedf1d76e922076dfd3ca3c726443efd}

```cpp
using mavsdk::System::IsConnectedHandle =  Handle<bool>
```


handle type to unsubscribe from subscribe_is_connected.


### typedef ComponentDiscoveredCallback {#classmavsdk_1_1_system_1a064172b17193bb9be448e2053c83627b}

```cpp
using mavsdk::System::ComponentDiscoveredCallback =  std::function<void(ComponentType)>
```


type for component discovery callback


### typedef ComponentDiscoveredHandle {#classmavsdk_1_1_system_1adfb374a9eaaa765cf0813cfa6b40df39}

```cpp
using mavsdk::System::ComponentDiscoveredHandle =  Handle<ComponentType>
```


type for component discovery callback handle


### typedef ComponentDiscoveredIdCallback {#classmavsdk_1_1_system_1a914c50b413b5bd61d334631096e614ca}

```cpp
using mavsdk::System::ComponentDiscoveredIdCallback =  std::function<void(ComponentType, uint8_t)>
```


type for component discovery callback with component ID


### typedef ComponentDiscoveredIdHandle {#classmavsdk_1_1_system_1abd573ae09348f33e7cd3a006fc26a708}

```cpp
using mavsdk::System::ComponentDiscoveredIdHandle =  Handle<ComponentType, uint8_t>
```


type for component discovery callback handle with component ID


## Member Enumeration Documentation


### enum ComponentType {#classmavsdk_1_1_system_1af2a91929d9771ae0e59c98557027b1dc}


Component Types.


Value | Description
--- | ---
<span id="classmavsdk_1_1_system_1af2a91929d9771ae0e59c98557027b1dca696b031073e74bf2cb98e5ef201d4aa3"></span> `UNKNOWN` |  
<span id="classmavsdk_1_1_system_1af2a91929d9771ae0e59c98557027b1dca8797273a75c761c3b925c165511d653c"></span> `AUTOPILOT` |  
<span id="classmavsdk_1_1_system_1af2a91929d9771ae0e59c98557027b1dcaddf0d6b21537d984fea6544f58101fa8"></span> `CAMERA` |  
<span id="classmavsdk_1_1_system_1af2a91929d9771ae0e59c98557027b1dca0bad549bc68e5c4adb2ed793b8dcf8e3"></span> `GIMBAL` |  

## Member Function Documentation


### init() {#classmavsdk_1_1_system_1a5c4c30affa7abbe06cc809187f3ddcb3}
```cpp
void mavsdk::System::init(uint8_t system_id, uint8_t component_id) const
```


Initialize the system.

This is not (and should not be) directly called by application code.

**Parameters**

* uint8_t **system_id** - [System](classmavsdk_1_1_system.md) id.
* uint8_t **component_id** - Component id.

### has_autopilot() {#classmavsdk_1_1_system_1a0c3d766baa73f5b35e2950a6f0a38c02}
```cpp
bool mavsdk::System::has_autopilot() const
```


Checks whether the system has an autopilot.


**Returns**

&emsp;bool - `true` if it has an autopilot, `false` otherwise.

### is_standalone() {#classmavsdk_1_1_system_1a7fb7ed01204498dcaa2ab7d9cc31acf2}
```cpp
bool mavsdk::System::is_standalone() const
```


Checks whether the system is a standalone (non-autopilot).


**Returns**

&emsp;bool - `true` if stand alone, `false` otherwise.

### has_camera() {#classmavsdk_1_1_system_1a440fd601ed2120e1e41d9eab536a7da8}
```cpp
bool mavsdk::System::has_camera(int camera_id=-1) const
```


Checks whether the system has a camera with the given camera ID.

A [System](classmavsdk_1_1_system.md) may have several cameras, with IDs starting from 0. When called without passing a camera ID, it checks whether the system has any camera.

**Parameters**

* int **camera_id** - ID of the camera starting from 0 onwards.

**Returns**

&emsp;bool - `true` if camera with the given camera ID is found, `false` otherwise.

### has_gimbal() {#classmavsdk_1_1_system_1ad66c3ecc096970d40c34610e49dba929}
```cpp
bool mavsdk::System::has_gimbal() const
```


Checks whether the system has a gimbal.


**Returns**

&emsp;bool - `true` if the system has a gimbal, false otherwise.

### is_connected() {#classmavsdk_1_1_system_1ad07991ae044bc367e27f544db40d065b}
```cpp
bool mavsdk::System::is_connected() const
```


Checks if the system is connected.

A system is connected when heartbeats are arriving (discovered and not timed out).

**Returns**

&emsp;bool - `true` if the system is connected.

### get_system_id() {#classmavsdk_1_1_system_1a091d793db29719f4996040886ad951a6}
```cpp
uint8_t mavsdk::System::get_system_id() const
```


MAVLink [System](classmavsdk_1_1_system.md) ID of connected system.

> **Note** : this is 0 if nothing is connected yet.

**Returns**

&emsp;uint8_t - the system ID.

### component_ids() {#classmavsdk_1_1_system_1aa33f748f81cc512601451c80bd077888}
```cpp
std::vector<uint8_t> mavsdk::System::component_ids() const
```


MAVLink component IDs of connected system.

> **Note** : all components that have been seen at least once will be listed.

**Returns**

&emsp;std::vector< uint8_t > - a list of all component ids

### subscribe_is_connected() {#classmavsdk_1_1_system_1aae68747c23976fa7eb63ec0762493263}
```cpp
IsConnectedHandle mavsdk::System::subscribe_is_connected(const IsConnectedCallback &callback)
```


Subscribe to callback to be called when system connection state changes.


**Parameters**

* const [IsConnectedCallback](classmavsdk_1_1_system.md#classmavsdk_1_1_system_1a0e56bb48498100fde0872a3ec376f282)& **callback** - Callback which will be called.

**Returns**

&emsp;[IsConnectedHandle](classmavsdk_1_1_system.md#classmavsdk_1_1_system_1adedf1d76e922076dfd3ca3c726443efd) - 

### unsubscribe_is_connected() {#classmavsdk_1_1_system_1a2f1927d56c14a7ad44995bd56afb706f}
```cpp
void mavsdk::System::unsubscribe_is_connected(IsConnectedHandle handle)
```


Unsubscribe from subscribe_is_connected.


**Parameters**

* [IsConnectedHandle](classmavsdk_1_1_system.md#classmavsdk_1_1_system_1adedf1d76e922076dfd3ca3c726443efd) **handle** - 

### subscribe_component_discovered() {#classmavsdk_1_1_system_1a25ede402b74a9412334ff1cab521e7d3}
```cpp
ComponentDiscoveredHandle mavsdk::System::subscribe_component_discovered(const ComponentDiscoveredCallback &callback)
```


Subscribe to be called when a component is discovered.


**Parameters**

* const [ComponentDiscoveredCallback](classmavsdk_1_1_system.md#classmavsdk_1_1_system_1a064172b17193bb9be448e2053c83627b)& **callback** - a function of type void(ComponentType) which will be called with the component type of the new component.

**Returns**

&emsp;[ComponentDiscoveredHandle](classmavsdk_1_1_system.md#classmavsdk_1_1_system_1adfb374a9eaaa765cf0813cfa6b40df39) - 

### unsubscribe_component_discovered() {#classmavsdk_1_1_system_1a5d62d308534b5e1673779543d02037f3}
```cpp
void mavsdk::System::unsubscribe_component_discovered(ComponentDiscoveredHandle handle)
```


Unsubscribe from subscribe_component_discovered.


**Parameters**

* [ComponentDiscoveredHandle](classmavsdk_1_1_system.md#classmavsdk_1_1_system_1adfb374a9eaaa765cf0813cfa6b40df39) **handle** - 

### subscribe_component_discovered_id() {#classmavsdk_1_1_system_1a1905176f3a6633a8a9fe655f2dcd2d23}
```cpp
ComponentDiscoveredIdHandle mavsdk::System::subscribe_component_discovered_id(const ComponentDiscoveredIdCallback &callback)
```


Subscribe to be called when a component is discovered.


**Parameters**

* const [ComponentDiscoveredIdCallback](classmavsdk_1_1_system.md#classmavsdk_1_1_system_1a914c50b413b5bd61d334631096e614ca)& **callback** - a function of type void(ComponentType) which will be called with the component type and the component id of the new component.

**Returns**

&emsp;[ComponentDiscoveredIdHandle](classmavsdk_1_1_system.md#classmavsdk_1_1_system_1abd573ae09348f33e7cd3a006fc26a708) - 

### unsubscribe_component_discovered_id() {#classmavsdk_1_1_system_1a37bca637341bba8b07e8f95c97d122db}
```cpp
void mavsdk::System::unsubscribe_component_discovered_id(ComponentDiscoveredIdHandle handle)
```


Unsubscribe from subscribe_component_discovered_id.


**Parameters**

* [ComponentDiscoveredIdHandle](classmavsdk_1_1_system.md#classmavsdk_1_1_system_1abd573ae09348f33e7cd3a006fc26a708) **handle** - 

### enable_timesync() {#classmavsdk_1_1_system_1a7c7177fb0789aefbfb375f4bb12ce824}
```cpp
void mavsdk::System::enable_timesync()
```


Enable time synchronization using the TIMESYNC messages.


### operator=() {#classmavsdk_1_1_system_1a21284c27829fda2391ee27f5732f916d}
```cpp
const System& mavsdk::System::operator=(const System &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [System](classmavsdk_1_1_system.md)&  - 

**Returns**

&emsp;const [System](classmavsdk_1_1_system.md) & - 