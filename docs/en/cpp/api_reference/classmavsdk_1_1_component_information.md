# mavsdk::ComponentInformation Class Reference
`#include: component_information.h`

----


Access component information such as parameters. 


## Data Structures


struct [FloatParam](structmavsdk_1_1_component_information_1_1_float_param.md)

struct [FloatParamUpdate](structmavsdk_1_1_component_information_1_1_float_param_update.md)

## Public Types


Type | Description
--- | ---
enum [Result](#classmavsdk_1_1_component_information_1aa91c6fe2a0335b2b86a140e37359341f) | Possible results returned for param requests.
std::function< void([Result](classmavsdk_1_1_component_information.md#classmavsdk_1_1_component_information_1aa91c6fe2a0335b2b86a140e37359341f))> [ResultCallback](#classmavsdk_1_1_component_information_1a37506981d2b48162f3304947e5520ea8) | Callback type for asynchronous [ComponentInformation](classmavsdk_1_1_component_information.md) calls.
std::function< void([FloatParamUpdate](structmavsdk_1_1_component_information_1_1_float_param_update.md))> [FloatParamCallback](#classmavsdk_1_1_component_information_1a346dfd9dc8de60f6cdeca0a4ddb20f78) | Callback type for subscribe_float_param.
[Handle](classmavsdk_1_1_handle.md)< [FloatParamUpdate](structmavsdk_1_1_component_information_1_1_float_param_update.md) > [FloatParamHandle](#classmavsdk_1_1_component_information_1ab22a0bb9704930d17c2e2c82ffa9a977) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_float_param.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [ComponentInformation](#classmavsdk_1_1_component_information_1afd3a1f8282548a895ff3bf3f08da20ac) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [ComponentInformation](#classmavsdk_1_1_component_information_1a3c6bc3ad6d571a944407e148e2496f13) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~ComponentInformation](#classmavsdk_1_1_component_information_1adc9f86087470dcf0b1eb888c236918ec) () override | Destructor (internal use only).
&nbsp; | [ComponentInformation](#classmavsdk_1_1_component_information_1a146e2a8d181c75ae1c7c98501c4b205a) (const [ComponentInformation](classmavsdk_1_1_component_information.md) & other) | Copy constructor.
std::pair< [Result](classmavsdk_1_1_component_information.md#classmavsdk_1_1_component_information_1aa91c6fe2a0335b2b86a140e37359341f), std::vector< [ComponentInformation::FloatParam](structmavsdk_1_1_component_information_1_1_float_param.md) > > | [access_float_params](#classmavsdk_1_1_component_information_1ababf4e7ff24e6fabfd3ee52b58e66002) () const | List available float params.
[FloatParamHandle](classmavsdk_1_1_component_information.md#classmavsdk_1_1_component_information_1ab22a0bb9704930d17c2e2c82ffa9a977) | [subscribe_float_param](#classmavsdk_1_1_component_information_1a41df5c7f9340d3e01b24a96d3215754d) (const [FloatParamCallback](classmavsdk_1_1_component_information.md#classmavsdk_1_1_component_information_1a346dfd9dc8de60f6cdeca0a4ddb20f78) & callback) | Subscribe to float param changes/updates.
void | [unsubscribe_float_param](#classmavsdk_1_1_component_information_1a877d2a620d6646bf4063863297c0b666) ([FloatParamHandle](classmavsdk_1_1_component_information.md#classmavsdk_1_1_component_information_1ab22a0bb9704930d17c2e2c82ffa9a977) handle) | Unsubscribe from subscribe_float_param.
const [ComponentInformation](classmavsdk_1_1_component_information.md) & | [operator=](#classmavsdk_1_1_component_information_1ada12fb6199ef988219281900ad2d9b79) (const [ComponentInformation](classmavsdk_1_1_component_information.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### ComponentInformation() {#classmavsdk_1_1_component_information_1afd3a1f8282548a895ff3bf3f08da20ac}
```cpp
mavsdk::ComponentInformation::ComponentInformation(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto component_information = ComponentInformation(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### ComponentInformation() {#classmavsdk_1_1_component_information_1a3c6bc3ad6d571a944407e148e2496f13}
```cpp
mavsdk::ComponentInformation::ComponentInformation(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto component_information = ComponentInformation(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~ComponentInformation() {#classmavsdk_1_1_component_information_1adc9f86087470dcf0b1eb888c236918ec}
```cpp
mavsdk::ComponentInformation::~ComponentInformation() override
```


Destructor (internal use only).


### ComponentInformation() {#classmavsdk_1_1_component_information_1a146e2a8d181c75ae1c7c98501c4b205a}
```cpp
mavsdk::ComponentInformation::ComponentInformation(const ComponentInformation &other)
```


Copy constructor.


**Parameters**

* const [ComponentInformation](classmavsdk_1_1_component_information.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_component_information_1a37506981d2b48162f3304947e5520ea8}

```cpp
using mavsdk::ComponentInformation::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [ComponentInformation](classmavsdk_1_1_component_information.md) calls.


### typedef FloatParamCallback {#classmavsdk_1_1_component_information_1a346dfd9dc8de60f6cdeca0a4ddb20f78}

```cpp
using mavsdk::ComponentInformation::FloatParamCallback =  std::function<void(FloatParamUpdate)>
```


Callback type for subscribe_float_param.


### typedef FloatParamHandle {#classmavsdk_1_1_component_information_1ab22a0bb9704930d17c2e2c82ffa9a977}

```cpp
using mavsdk::ComponentInformation::FloatParamHandle =  Handle<FloatParamUpdate>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_float_param.


## Member Enumeration Documentation


### enum Result {#classmavsdk_1_1_component_information_1aa91c6fe2a0335b2b86a140e37359341f}


Possible results returned for param requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_component_information_1aa91c6fe2a0335b2b86a140e37359341fa88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_component_information_1aa91c6fe2a0335b2b86a140e37359341fa505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Request succeeded. 
<span id="classmavsdk_1_1_component_information_1aa91c6fe2a0335b2b86a140e37359341fa1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system is connected. 

## Member Function Documentation


### access_float_params() {#classmavsdk_1_1_component_information_1ababf4e7ff24e6fabfd3ee52b58e66002}
```cpp
std::pair<Result, std::vector<ComponentInformation::FloatParam> > mavsdk::ComponentInformation::access_float_params() const
```


List available float params.

This function is blocking.

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_component_information.md#classmavsdk_1_1_component_information_1aa91c6fe2a0335b2b86a140e37359341f), std::vector< [ComponentInformation::FloatParam](structmavsdk_1_1_component_information_1_1_float_param.md) > > - Result of request.

### subscribe_float_param() {#classmavsdk_1_1_component_information_1a41df5c7f9340d3e01b24a96d3215754d}
```cpp
FloatParamHandle mavsdk::ComponentInformation::subscribe_float_param(const FloatParamCallback &callback)
```


Subscribe to float param changes/updates.


**Parameters**

* const [FloatParamCallback](classmavsdk_1_1_component_information.md#classmavsdk_1_1_component_information_1a346dfd9dc8de60f6cdeca0a4ddb20f78)& **callback** - 

**Returns**

&emsp;[FloatParamHandle](classmavsdk_1_1_component_information.md#classmavsdk_1_1_component_information_1ab22a0bb9704930d17c2e2c82ffa9a977) - 

### unsubscribe_float_param() {#classmavsdk_1_1_component_information_1a877d2a620d6646bf4063863297c0b666}
```cpp
void mavsdk::ComponentInformation::unsubscribe_float_param(FloatParamHandle handle)
```


Unsubscribe from subscribe_float_param.


**Parameters**

* [FloatParamHandle](classmavsdk_1_1_component_information.md#classmavsdk_1_1_component_information_1ab22a0bb9704930d17c2e2c82ffa9a977) **handle** - 

### operator=() {#classmavsdk_1_1_component_information_1ada12fb6199ef988219281900ad2d9b79}
```cpp
const ComponentInformation& mavsdk::ComponentInformation::operator=(const ComponentInformation &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [ComponentInformation](classmavsdk_1_1_component_information.md)&  - 

**Returns**

&emsp;const [ComponentInformation](classmavsdk_1_1_component_information.md) & - 