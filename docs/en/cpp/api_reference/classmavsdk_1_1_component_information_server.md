# mavsdk::ComponentInformationServer Class Reference
`#include: component_information_server.h`

----


Provide component information such as parameters. 


## Data Structures


struct [FloatParam](structmavsdk_1_1_component_information_server_1_1_float_param.md)

struct [FloatParamUpdate](structmavsdk_1_1_component_information_server_1_1_float_param_update.md)

## Public Types


Type | Description
--- | ---
enum [Result](#classmavsdk_1_1_component_information_server_1aca86e47230e256d3f812269dcbaa5cad) | Possible results returned for param requests.
std::function< void([Result](classmavsdk_1_1_component_information_server.md#classmavsdk_1_1_component_information_server_1aca86e47230e256d3f812269dcbaa5cad))> [ResultCallback](#classmavsdk_1_1_component_information_server_1a5f65b34949a1954c85f3f02e64dec35f) | Callback type for asynchronous [ComponentInformationServer](classmavsdk_1_1_component_information_server.md) calls.
std::function< void([FloatParamUpdate](structmavsdk_1_1_component_information_server_1_1_float_param_update.md))> [FloatParamCallback](#classmavsdk_1_1_component_information_server_1a6174e3eebb5a10c619c57723623696cf) | Callback type for subscribe_float_param.
[Handle](classmavsdk_1_1_handle.md)< [FloatParamUpdate](structmavsdk_1_1_component_information_server_1_1_float_param_update.md) > [FloatParamHandle](#classmavsdk_1_1_component_information_server_1a0843521587e27f0d630280309712bddb) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_float_param.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [ComponentInformationServer](#classmavsdk_1_1_component_information_server_1a89266c1e143f0cffb6596897b92f7b62) (std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > server_component) | Constructor. Creates the plugin for a [ServerComponent](classmavsdk_1_1_server_component.md) instance.
&nbsp; | [~ComponentInformationServer](#classmavsdk_1_1_component_information_server_1a64a224210755136e130b349db3a8fd10) () override | Destructor (internal use only).
&nbsp; | [ComponentInformationServer](#classmavsdk_1_1_component_information_server_1a7f8fc33e21e00a390da14596465c800d) (const [ComponentInformationServer](classmavsdk_1_1_component_information_server.md) & other) | Copy constructor.
[Result](classmavsdk_1_1_component_information_server.md#classmavsdk_1_1_component_information_server_1aca86e47230e256d3f812269dcbaa5cad) | [provide_float_param](#classmavsdk_1_1_component_information_server_1ac4f9a480ef052b792e65b82c3c08b225) ([FloatParam](structmavsdk_1_1_component_information_server_1_1_float_param.md) param)const | Provide a param of type float.
[FloatParamHandle](classmavsdk_1_1_component_information_server.md#classmavsdk_1_1_component_information_server_1a0843521587e27f0d630280309712bddb) | [subscribe_float_param](#classmavsdk_1_1_component_information_server_1afb87c280501c677a8f4eaa33394d24e7) (const [FloatParamCallback](classmavsdk_1_1_component_information_server.md#classmavsdk_1_1_component_information_server_1a6174e3eebb5a10c619c57723623696cf) & callback) | Subscribe to float param updates.
void | [unsubscribe_float_param](#classmavsdk_1_1_component_information_server_1a56650131d743e441232c5edf844acaeb) ([FloatParamHandle](classmavsdk_1_1_component_information_server.md#classmavsdk_1_1_component_information_server_1a0843521587e27f0d630280309712bddb) handle) | Unsubscribe from subscribe_float_param.
const [ComponentInformationServer](classmavsdk_1_1_component_information_server.md) & | [operator=](#classmavsdk_1_1_component_information_server_1a850a10c9d195da5f52807984e72d21fa) (const [ComponentInformationServer](classmavsdk_1_1_component_information_server.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### ComponentInformationServer() {#classmavsdk_1_1_component_information_server_1a89266c1e143f0cffb6596897b92f7b62}
```cpp
mavsdk::ComponentInformationServer::ComponentInformationServer(std::shared_ptr< ServerComponent > server_component)
```


Constructor. Creates the plugin for a [ServerComponent](classmavsdk_1_1_server_component.md) instance.

The plugin is typically created as shown below: 

```cpp
auto component_information_server = ComponentInformationServer(server_component);
```

**Parameters**

* std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > **server_component** - The [ServerComponent](classmavsdk_1_1_server_component.md) instance associated with this server plugin.

### ~ComponentInformationServer() {#classmavsdk_1_1_component_information_server_1a64a224210755136e130b349db3a8fd10}
```cpp
mavsdk::ComponentInformationServer::~ComponentInformationServer() override
```


Destructor (internal use only).


### ComponentInformationServer() {#classmavsdk_1_1_component_information_server_1a7f8fc33e21e00a390da14596465c800d}
```cpp
mavsdk::ComponentInformationServer::ComponentInformationServer(const ComponentInformationServer &other)
```


Copy constructor.


**Parameters**

* const [ComponentInformationServer](classmavsdk_1_1_component_information_server.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_component_information_server_1a5f65b34949a1954c85f3f02e64dec35f}

```cpp
using mavsdk::ComponentInformationServer::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [ComponentInformationServer](classmavsdk_1_1_component_information_server.md) calls.


### typedef FloatParamCallback {#classmavsdk_1_1_component_information_server_1a6174e3eebb5a10c619c57723623696cf}

```cpp
using mavsdk::ComponentInformationServer::FloatParamCallback =  std::function<void(FloatParamUpdate)>
```


Callback type for subscribe_float_param.


### typedef FloatParamHandle {#classmavsdk_1_1_component_information_server_1a0843521587e27f0d630280309712bddb}

```cpp
using mavsdk::ComponentInformationServer::FloatParamHandle =  Handle<FloatParamUpdate>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_float_param.


## Member Enumeration Documentation


### enum Result {#classmavsdk_1_1_component_information_server_1aca86e47230e256d3f812269dcbaa5cad}


Possible results returned for param requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_component_information_server_1aca86e47230e256d3f812269dcbaa5cada88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_component_information_server_1aca86e47230e256d3f812269dcbaa5cada505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Request succeeded. 
<span id="classmavsdk_1_1_component_information_server_1aca86e47230e256d3f812269dcbaa5cada2e665af7b9e17282f7301a1dcc5e6365"></span> `DuplicateParam` | Duplicate param. 
<span id="classmavsdk_1_1_component_information_server_1aca86e47230e256d3f812269dcbaa5cada6d9ebfdcd8bc404f5d64d3cec5a190cc"></span> `InvalidParamStartValue` | Invalid start param value. 
<span id="classmavsdk_1_1_component_information_server_1aca86e47230e256d3f812269dcbaa5cada6f12b0597e5013f1211f1e1af2eb79c9"></span> `InvalidParamDefaultValue` | Invalid default param value. 
<span id="classmavsdk_1_1_component_information_server_1aca86e47230e256d3f812269dcbaa5cada297a4ad19203eb48dcc016ac3a2d3149"></span> `InvalidParamName` | Invalid param name. 
<span id="classmavsdk_1_1_component_information_server_1aca86e47230e256d3f812269dcbaa5cada1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system is connected. 

## Member Function Documentation


### provide_float_param() {#classmavsdk_1_1_component_information_server_1ac4f9a480ef052b792e65b82c3c08b225}
```cpp
Result mavsdk::ComponentInformationServer::provide_float_param(FloatParam param) const
```


Provide a param of type float.

This function is blocking.

**Parameters**

* [FloatParam](structmavsdk_1_1_component_information_server_1_1_float_param.md) **param** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_component_information_server.md#classmavsdk_1_1_component_information_server_1aca86e47230e256d3f812269dcbaa5cad) - Result of request.

### subscribe_float_param() {#classmavsdk_1_1_component_information_server_1afb87c280501c677a8f4eaa33394d24e7}
```cpp
FloatParamHandle mavsdk::ComponentInformationServer::subscribe_float_param(const FloatParamCallback &callback)
```


Subscribe to float param updates.


**Parameters**

* const [FloatParamCallback](classmavsdk_1_1_component_information_server.md#classmavsdk_1_1_component_information_server_1a6174e3eebb5a10c619c57723623696cf)& **callback** - 

**Returns**

&emsp;[FloatParamHandle](classmavsdk_1_1_component_information_server.md#classmavsdk_1_1_component_information_server_1a0843521587e27f0d630280309712bddb) - 

### unsubscribe_float_param() {#classmavsdk_1_1_component_information_server_1a56650131d743e441232c5edf844acaeb}
```cpp
void mavsdk::ComponentInformationServer::unsubscribe_float_param(FloatParamHandle handle)
```


Unsubscribe from subscribe_float_param.


**Parameters**

* [FloatParamHandle](classmavsdk_1_1_component_information_server.md#classmavsdk_1_1_component_information_server_1a0843521587e27f0d630280309712bddb) **handle** - 

### operator=() {#classmavsdk_1_1_component_information_server_1a850a10c9d195da5f52807984e72d21fa}
```cpp
const ComponentInformationServer& mavsdk::ComponentInformationServer::operator=(const ComponentInformationServer &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [ComponentInformationServer](classmavsdk_1_1_component_information_server.md)&  - 

**Returns**

&emsp;const [ComponentInformationServer](classmavsdk_1_1_component_information_server.md) & - 