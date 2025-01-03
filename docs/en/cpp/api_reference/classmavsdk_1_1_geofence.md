# mavsdk::Geofence Class Reference
`#include: geofence.h`

----


Enable setting a geofence. 


## Data Structures


struct [Circle](structmavsdk_1_1_geofence_1_1_circle.md)

struct [GeofenceData](structmavsdk_1_1_geofence_1_1_geofence_data.md)

struct [Point](structmavsdk_1_1_geofence_1_1_point.md)

struct [Polygon](structmavsdk_1_1_geofence_1_1_polygon.md)

## Public Types


Type | Description
--- | ---
enum [FenceType](#classmavsdk_1_1_geofence_1a51196ded958aa9d4b3f25cde95da48e5) | [Geofence](classmavsdk_1_1_geofence.md) types.
enum [Result](#classmavsdk_1_1_geofence_1ab64d6e3b9aeb9b6d5e45ae8a843a2642) | Possible results returned for geofence requests.
std::function< void([Result](classmavsdk_1_1_geofence.md#classmavsdk_1_1_geofence_1ab64d6e3b9aeb9b6d5e45ae8a843a2642))> [ResultCallback](#classmavsdk_1_1_geofence_1af9662e645781e4e64ed8b7c65d3d9309) | Callback type for asynchronous [Geofence](classmavsdk_1_1_geofence.md) calls.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [Geofence](#classmavsdk_1_1_geofence_1a115762872a12894270e758af250549f0) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [Geofence](#classmavsdk_1_1_geofence_1a7e921625517d772df4125b22841b78da) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~Geofence](#classmavsdk_1_1_geofence_1aa8362d021d182c807ddb5e95e40a9cbe) () override | Destructor (internal use only).
&nbsp; | [Geofence](#classmavsdk_1_1_geofence_1a60e1f1a3123050c73980cba61b4b4009) (const [Geofence](classmavsdk_1_1_geofence.md) & other) | Copy constructor.
void | [upload_geofence_async](#classmavsdk_1_1_geofence_1a1f0c6431b86b77c29f19eafb4e55dcdb) ([GeofenceData](structmavsdk_1_1_geofence_1_1_geofence_data.md) geofence_data, const [ResultCallback](classmavsdk_1_1_geofence.md#classmavsdk_1_1_geofence_1af9662e645781e4e64ed8b7c65d3d9309) callback) | Upload geofences.
[Result](classmavsdk_1_1_geofence.md#classmavsdk_1_1_geofence_1ab64d6e3b9aeb9b6d5e45ae8a843a2642) | [upload_geofence](#classmavsdk_1_1_geofence_1ab2e825f0955e7a320117a21d649bab09) ([GeofenceData](structmavsdk_1_1_geofence_1_1_geofence_data.md) geofence_data)const | Upload geofences.
void | [clear_geofence_async](#classmavsdk_1_1_geofence_1a6947151765b621a93d35885599812752) (const [ResultCallback](classmavsdk_1_1_geofence.md#classmavsdk_1_1_geofence_1af9662e645781e4e64ed8b7c65d3d9309) callback) | Clear all geofences saved on the vehicle.
[Result](classmavsdk_1_1_geofence.md#classmavsdk_1_1_geofence_1ab64d6e3b9aeb9b6d5e45ae8a843a2642) | [clear_geofence](#classmavsdk_1_1_geofence_1a54b2a696e8aebae6916116adb92c03c3) () const | Clear all geofences saved on the vehicle.
const [Geofence](classmavsdk_1_1_geofence.md) & | [operator=](#classmavsdk_1_1_geofence_1a2e8a69dddfa9b4937df117060fa2e0d7) (const [Geofence](classmavsdk_1_1_geofence.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### Geofence() {#classmavsdk_1_1_geofence_1a115762872a12894270e758af250549f0}
```cpp
mavsdk::Geofence::Geofence(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto geofence = Geofence(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### Geofence() {#classmavsdk_1_1_geofence_1a7e921625517d772df4125b22841b78da}
```cpp
mavsdk::Geofence::Geofence(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto geofence = Geofence(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~Geofence() {#classmavsdk_1_1_geofence_1aa8362d021d182c807ddb5e95e40a9cbe}
```cpp
mavsdk::Geofence::~Geofence() override
```


Destructor (internal use only).


### Geofence() {#classmavsdk_1_1_geofence_1a60e1f1a3123050c73980cba61b4b4009}
```cpp
mavsdk::Geofence::Geofence(const Geofence &other)
```


Copy constructor.


**Parameters**

* const [Geofence](classmavsdk_1_1_geofence.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_geofence_1af9662e645781e4e64ed8b7c65d3d9309}

```cpp
using mavsdk::Geofence::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [Geofence](classmavsdk_1_1_geofence.md) calls.


## Member Enumeration Documentation


### enum FenceType {#classmavsdk_1_1_geofence_1a51196ded958aa9d4b3f25cde95da48e5}


[Geofence](classmavsdk_1_1_geofence.md) types.


Value | Description
--- | ---
<span id="classmavsdk_1_1_geofence_1a51196ded958aa9d4b3f25cde95da48e5a9e3be7835607d7cc3ae456b20ce9a4f4"></span> `Inclusion` | Type representing an inclusion fence. 
<span id="classmavsdk_1_1_geofence_1a51196ded958aa9d4b3f25cde95da48e5a25218e43e67f1b1c1c833bc48c7ca7bb"></span> `Exclusion` | Type representing an exclusion fence. 

### enum Result {#classmavsdk_1_1_geofence_1ab64d6e3b9aeb9b6d5e45ae8a843a2642}


Possible results returned for geofence requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_geofence_1ab64d6e3b9aeb9b6d5e45ae8a843a2642a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_geofence_1ab64d6e3b9aeb9b6d5e45ae8a843a2642a505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Request succeeded. 
<span id="classmavsdk_1_1_geofence_1ab64d6e3b9aeb9b6d5e45ae8a843a2642a902b0d55fddef6f8d651fe1035b7d4bd"></span> `Error` | Error. 
<span id="classmavsdk_1_1_geofence_1ab64d6e3b9aeb9b6d5e45ae8a843a2642aa74add5fe6bda878df41d8c3f07d3ef8"></span> `TooManyGeofenceItems` | Too many objects in the geofence. 
<span id="classmavsdk_1_1_geofence_1ab64d6e3b9aeb9b6d5e45ae8a843a2642ad8a942ef2b04672adfafef0ad817a407"></span> `Busy` | Vehicle is busy. 
<span id="classmavsdk_1_1_geofence_1ab64d6e3b9aeb9b6d5e45ae8a843a2642ac85a251cc457840f1e032f1b733e9398"></span> `Timeout` | Request timed out. 
<span id="classmavsdk_1_1_geofence_1ab64d6e3b9aeb9b6d5e45ae8a843a2642a253ca7dd096ee0956cccee4d376cab8b"></span> `InvalidArgument` | Invalid argument. 
<span id="classmavsdk_1_1_geofence_1ab64d6e3b9aeb9b6d5e45ae8a843a2642a1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system connected. 

## Member Function Documentation


### upload_geofence_async() {#classmavsdk_1_1_geofence_1a1f0c6431b86b77c29f19eafb4e55dcdb}
```cpp
void mavsdk::Geofence::upload_geofence_async(GeofenceData geofence_data, const ResultCallback callback)
```


Upload geofences.

[Polygon](structmavsdk_1_1_geofence_1_1_polygon.md) and Circular geofences are uploaded to a drone. Once uploaded, the geofence will remain on the drone even if a connection is lost.


This function is non-blocking. See 'upload_geofence' for the blocking counterpart.

**Parameters**

* [GeofenceData](structmavsdk_1_1_geofence_1_1_geofence_data.md) **geofence_data** - 
* const [ResultCallback](classmavsdk_1_1_geofence.md#classmavsdk_1_1_geofence_1af9662e645781e4e64ed8b7c65d3d9309) **callback** - 

### upload_geofence() {#classmavsdk_1_1_geofence_1ab2e825f0955e7a320117a21d649bab09}
```cpp
Result mavsdk::Geofence::upload_geofence(GeofenceData geofence_data) const
```


Upload geofences.

[Polygon](structmavsdk_1_1_geofence_1_1_polygon.md) and Circular geofences are uploaded to a drone. Once uploaded, the geofence will remain on the drone even if a connection is lost.


This function is blocking. See 'upload_geofence_async' for the non-blocking counterpart.

**Parameters**

* [GeofenceData](structmavsdk_1_1_geofence_1_1_geofence_data.md) **geofence_data** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_geofence.md#classmavsdk_1_1_geofence_1ab64d6e3b9aeb9b6d5e45ae8a843a2642) - Result of request.

### clear_geofence_async() {#classmavsdk_1_1_geofence_1a6947151765b621a93d35885599812752}
```cpp
void mavsdk::Geofence::clear_geofence_async(const ResultCallback callback)
```


Clear all geofences saved on the vehicle.

This function is non-blocking. See 'clear_geofence' for the blocking counterpart.

**Parameters**

* const [ResultCallback](classmavsdk_1_1_geofence.md#classmavsdk_1_1_geofence_1af9662e645781e4e64ed8b7c65d3d9309) **callback** - 

### clear_geofence() {#classmavsdk_1_1_geofence_1a54b2a696e8aebae6916116adb92c03c3}
```cpp
Result mavsdk::Geofence::clear_geofence() const
```


Clear all geofences saved on the vehicle.

This function is blocking. See 'clear_geofence_async' for the non-blocking counterpart.

**Returns**

&emsp;[Result](classmavsdk_1_1_geofence.md#classmavsdk_1_1_geofence_1ab64d6e3b9aeb9b6d5e45ae8a843a2642) - Result of request.

### operator=() {#classmavsdk_1_1_geofence_1a2e8a69dddfa9b4937df117060fa2e0d7}
```cpp
const Geofence& mavsdk::Geofence::operator=(const Geofence &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [Geofence](classmavsdk_1_1_geofence.md)&  - 

**Returns**

&emsp;const [Geofence](classmavsdk_1_1_geofence.md) & - 