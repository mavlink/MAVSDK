# mavsdk::Info Class Reference
`#include: info.h`

----


Provide information about the hardware and/or software of a system. 


## Data Structures


struct [FlightInfo](structmavsdk_1_1_info_1_1_flight_info.md)

struct [Identification](structmavsdk_1_1_info_1_1_identification.md)

struct [Product](structmavsdk_1_1_info_1_1_product.md)

struct [Version](structmavsdk_1_1_info_1_1_version.md)

## Public Types


Type | Description
--- | ---
enum [Result](#classmavsdk_1_1_info_1ab1798ed39271915800b25aaa05d1d45a) | Possible results returned for info requests.
std::function< void([Result](classmavsdk_1_1_info.md#classmavsdk_1_1_info_1ab1798ed39271915800b25aaa05d1d45a))> [ResultCallback](#classmavsdk_1_1_info_1a649bf4a0936dea3168d40eb9b9dcdd57) | Callback type for asynchronous [Info](classmavsdk_1_1_info.md) calls.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [Info](#classmavsdk_1_1_info_1a01a29a579c8ecb75ea73d48f8f6adf17) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [Info](#classmavsdk_1_1_info_1ae67e006f16f1e1aa12efe94120ef83ec) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~Info](#classmavsdk_1_1_info_1abbf48bc4b9aa5b9fdbdb54ec3e398f65) () override | Destructor (internal use only).
&nbsp; | [Info](#classmavsdk_1_1_info_1a0f6e0851757046c540fe7ce920eb3fa2) (const [Info](classmavsdk_1_1_info.md) & other) | Copy constructor.
std::pair< [Result](classmavsdk_1_1_info.md#classmavsdk_1_1_info_1ab1798ed39271915800b25aaa05d1d45a), [Info::FlightInfo](structmavsdk_1_1_info_1_1_flight_info.md) > | [get_flight_information](#classmavsdk_1_1_info_1a915e0833d8c3c61fed9b9ad687caef77) () const | Get flight information of the system.
std::pair< [Result](classmavsdk_1_1_info.md#classmavsdk_1_1_info_1ab1798ed39271915800b25aaa05d1d45a), [Info::Identification](structmavsdk_1_1_info_1_1_identification.md) > | [get_identification](#classmavsdk_1_1_info_1a812ed66265b7427bc781faec3f0fa89e) () const | Get the identification of the system.
std::pair< [Result](classmavsdk_1_1_info.md#classmavsdk_1_1_info_1ab1798ed39271915800b25aaa05d1d45a), [Info::Product](structmavsdk_1_1_info_1_1_product.md) > | [get_product](#classmavsdk_1_1_info_1ae60d71a6dcd7546d1bdc06a9c26e629f) () const | Get product information of the system.
std::pair< [Result](classmavsdk_1_1_info.md#classmavsdk_1_1_info_1ab1798ed39271915800b25aaa05d1d45a), [Info::Version](structmavsdk_1_1_info_1_1_version.md) > | [get_version](#classmavsdk_1_1_info_1a2403b975a73b2b52a455b96cdc899af3) () const | Get the version information of the system.
std::pair< [Result](classmavsdk_1_1_info.md#classmavsdk_1_1_info_1ab1798ed39271915800b25aaa05d1d45a), double > | [get_speed_factor](#classmavsdk_1_1_info_1a3b372b3f6e4b167cb272ae68d3d172c6) () const | Get the speed factor of a simulation (with lockstep a simulation can run faster or slower than realtime).
const [Info](classmavsdk_1_1_info.md) & | [operator=](#classmavsdk_1_1_info_1a586eb91fd65d602bad1d016dca42b435) (const [Info](classmavsdk_1_1_info.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### Info() {#classmavsdk_1_1_info_1a01a29a579c8ecb75ea73d48f8f6adf17}
```cpp
mavsdk::Info::Info(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto info = Info(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### Info() {#classmavsdk_1_1_info_1ae67e006f16f1e1aa12efe94120ef83ec}
```cpp
mavsdk::Info::Info(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto info = Info(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~Info() {#classmavsdk_1_1_info_1abbf48bc4b9aa5b9fdbdb54ec3e398f65}
```cpp
mavsdk::Info::~Info() override
```


Destructor (internal use only).


### Info() {#classmavsdk_1_1_info_1a0f6e0851757046c540fe7ce920eb3fa2}
```cpp
mavsdk::Info::Info(const Info &other)
```


Copy constructor.


**Parameters**

* const [Info](classmavsdk_1_1_info.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_info_1a649bf4a0936dea3168d40eb9b9dcdd57}

```cpp
using mavsdk::Info::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [Info](classmavsdk_1_1_info.md) calls.


## Member Enumeration Documentation


### enum Result {#classmavsdk_1_1_info_1ab1798ed39271915800b25aaa05d1d45a}


Possible results returned for info requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_info_1ab1798ed39271915800b25aaa05d1d45aa88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_info_1ab1798ed39271915800b25aaa05d1d45aa505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Request succeeded. 
<span id="classmavsdk_1_1_info_1ab1798ed39271915800b25aaa05d1d45aa56c1e4870d2594d9fd4d91ffaf1e3e70"></span> `InformationNotReceivedYet` | Information has not been received yet. 
<span id="classmavsdk_1_1_info_1ab1798ed39271915800b25aaa05d1d45aa1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system is connected. 

## Member Function Documentation


### get_flight_information() {#classmavsdk_1_1_info_1a915e0833d8c3c61fed9b9ad687caef77}
```cpp
std::pair<Result, Info::FlightInfo> mavsdk::Info::get_flight_information() const
```


Get flight information of the system.

This function is blocking.

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_info.md#classmavsdk_1_1_info_1ab1798ed39271915800b25aaa05d1d45a), [Info::FlightInfo](structmavsdk_1_1_info_1_1_flight_info.md) > - Result of request.

### get_identification() {#classmavsdk_1_1_info_1a812ed66265b7427bc781faec3f0fa89e}
```cpp
std::pair<Result, Info::Identification> mavsdk::Info::get_identification() const
```


Get the identification of the system.

This function is blocking.

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_info.md#classmavsdk_1_1_info_1ab1798ed39271915800b25aaa05d1d45a), [Info::Identification](structmavsdk_1_1_info_1_1_identification.md) > - Result of request.

### get_product() {#classmavsdk_1_1_info_1ae60d71a6dcd7546d1bdc06a9c26e629f}
```cpp
std::pair<Result, Info::Product> mavsdk::Info::get_product() const
```


Get product information of the system.

This function is blocking.

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_info.md#classmavsdk_1_1_info_1ab1798ed39271915800b25aaa05d1d45a), [Info::Product](structmavsdk_1_1_info_1_1_product.md) > - Result of request.

### get_version() {#classmavsdk_1_1_info_1a2403b975a73b2b52a455b96cdc899af3}
```cpp
std::pair<Result, Info::Version> mavsdk::Info::get_version() const
```


Get the version information of the system.

This function is blocking.

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_info.md#classmavsdk_1_1_info_1ab1798ed39271915800b25aaa05d1d45a), [Info::Version](structmavsdk_1_1_info_1_1_version.md) > - Result of request.

### get_speed_factor() {#classmavsdk_1_1_info_1a3b372b3f6e4b167cb272ae68d3d172c6}
```cpp
std::pair<Result, double> mavsdk::Info::get_speed_factor() const
```


Get the speed factor of a simulation (with lockstep a simulation can run faster or slower than realtime).

This function is blocking.

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_info.md#classmavsdk_1_1_info_1ab1798ed39271915800b25aaa05d1d45a), double > - Result of request.

### operator=() {#classmavsdk_1_1_info_1a586eb91fd65d602bad1d016dca42b435}
```cpp
const Info& mavsdk::Info::operator=(const Info &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [Info](classmavsdk_1_1_info.md)&  - 

**Returns**

&emsp;const [Info](classmavsdk_1_1_info.md) & - 