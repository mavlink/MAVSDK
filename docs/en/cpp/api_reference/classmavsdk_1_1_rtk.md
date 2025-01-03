# mavsdk::Rtk Class Reference
`#include: rtk.h`

----


Service to send RTK corrections to the vehicle. 


## Data Structures


struct [RtcmData](structmavsdk_1_1_rtk_1_1_rtcm_data.md)

## Public Types


Type | Description
--- | ---
enum [Result](#classmavsdk_1_1_rtk_1a7e310a6ab3cfc82efb46e238bc918a94) | Possible results returned for rtk requests.
std::function< void([Result](classmavsdk_1_1_rtk.md#classmavsdk_1_1_rtk_1a7e310a6ab3cfc82efb46e238bc918a94))> [ResultCallback](#classmavsdk_1_1_rtk_1a63f4e9e3fd880d2f1d88e61f7cda0fdb) | Callback type for asynchronous [Rtk](classmavsdk_1_1_rtk.md) calls.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [Rtk](#classmavsdk_1_1_rtk_1af3b0f558cdab23843d06d898cfe7cf34) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [Rtk](#classmavsdk_1_1_rtk_1a5b3d107b883e74885cc2c966cb6ebd10) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~Rtk](#classmavsdk_1_1_rtk_1a008724fb2e96cb9a96024f38130c6213) () override | Destructor (internal use only).
&nbsp; | [Rtk](#classmavsdk_1_1_rtk_1af9118d75e7ef85846bcf7a637d8df73c) (const [Rtk](classmavsdk_1_1_rtk.md) & other) | Copy constructor.
[Result](classmavsdk_1_1_rtk.md#classmavsdk_1_1_rtk_1a7e310a6ab3cfc82efb46e238bc918a94) | [send_rtcm_data](#classmavsdk_1_1_rtk_1ab0f183ba8e57944e6f9d383f51490d09) ([RtcmData](structmavsdk_1_1_rtk_1_1_rtcm_data.md) rtcm_data)const | Send RTCM data.
const [Rtk](classmavsdk_1_1_rtk.md) & | [operator=](#classmavsdk_1_1_rtk_1a8d4c39419257c0df20b55aa7b838ecb2) (const [Rtk](classmavsdk_1_1_rtk.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### Rtk() {#classmavsdk_1_1_rtk_1af3b0f558cdab23843d06d898cfe7cf34}
```cpp
mavsdk::Rtk::Rtk(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto rtk = Rtk(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### Rtk() {#classmavsdk_1_1_rtk_1a5b3d107b883e74885cc2c966cb6ebd10}
```cpp
mavsdk::Rtk::Rtk(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto rtk = Rtk(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~Rtk() {#classmavsdk_1_1_rtk_1a008724fb2e96cb9a96024f38130c6213}
```cpp
mavsdk::Rtk::~Rtk() override
```


Destructor (internal use only).


### Rtk() {#classmavsdk_1_1_rtk_1af9118d75e7ef85846bcf7a637d8df73c}
```cpp
mavsdk::Rtk::Rtk(const Rtk &other)
```


Copy constructor.


**Parameters**

* const [Rtk](classmavsdk_1_1_rtk.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_rtk_1a63f4e9e3fd880d2f1d88e61f7cda0fdb}

```cpp
using mavsdk::Rtk::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [Rtk](classmavsdk_1_1_rtk.md) calls.


## Member Enumeration Documentation


### enum Result {#classmavsdk_1_1_rtk_1a7e310a6ab3cfc82efb46e238bc918a94}


Possible results returned for rtk requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_rtk_1a7e310a6ab3cfc82efb46e238bc918a94a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_rtk_1a7e310a6ab3cfc82efb46e238bc918a94a505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Request succeeded. 
<span id="classmavsdk_1_1_rtk_1a7e310a6ab3cfc82efb46e238bc918a94a046afc1d5955fb46cbc55a8aa1f373dc"></span> `TooLong` | Passed data is too long. 
<span id="classmavsdk_1_1_rtk_1a7e310a6ab3cfc82efb46e238bc918a94a1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system connected. 
<span id="classmavsdk_1_1_rtk_1a7e310a6ab3cfc82efb46e238bc918a94a094a6f6b0868122a9dd008cb91c083e4"></span> `ConnectionError` | Connection error. 

## Member Function Documentation


### send_rtcm_data() {#classmavsdk_1_1_rtk_1ab0f183ba8e57944e6f9d383f51490d09}
```cpp
Result mavsdk::Rtk::send_rtcm_data(RtcmData rtcm_data) const
```


Send RTCM data.

This function is blocking.

**Parameters**

* [RtcmData](structmavsdk_1_1_rtk_1_1_rtcm_data.md) **rtcm_data** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_rtk.md#classmavsdk_1_1_rtk_1a7e310a6ab3cfc82efb46e238bc918a94) - Result of request.

### operator=() {#classmavsdk_1_1_rtk_1a8d4c39419257c0df20b55aa7b838ecb2}
```cpp
const Rtk& mavsdk::Rtk::operator=(const Rtk &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [Rtk](classmavsdk_1_1_rtk.md)&  - 

**Returns**

&emsp;const [Rtk](classmavsdk_1_1_rtk.md) & - 