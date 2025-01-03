# mavsdk::ParamServer Class Reference
`#include: param_server.h`

----


Provide raw access to retrieve and provide server parameters. 


## Data Structures


struct [AllParams](structmavsdk_1_1_param_server_1_1_all_params.md)

struct [CustomParam](structmavsdk_1_1_param_server_1_1_custom_param.md)

struct [FloatParam](structmavsdk_1_1_param_server_1_1_float_param.md)

struct [IntParam](structmavsdk_1_1_param_server_1_1_int_param.md)

## Public Types


Type | Description
--- | ---
enum [Result](#classmavsdk_1_1_param_server_1a6f7fcc017f43dcf68837dbc35ee4f469) | Possible results returned for param requests.
std::function< void([Result](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1a6f7fcc017f43dcf68837dbc35ee4f469))> [ResultCallback](#classmavsdk_1_1_param_server_1a669845877bb8e14482fb46542825c625) | Callback type for asynchronous [ParamServer](classmavsdk_1_1_param_server.md) calls.
std::function< void([IntParam](structmavsdk_1_1_param_server_1_1_int_param.md))> [ChangedParamIntCallback](#classmavsdk_1_1_param_server_1a7674183da6d76416b34df9ce51c59358) | Callback type for subscribe_changed_param_int.
[Handle](classmavsdk_1_1_handle.md)< [IntParam](structmavsdk_1_1_param_server_1_1_int_param.md) > [ChangedParamIntHandle](#classmavsdk_1_1_param_server_1a923f3ae88ca5614690f3b1b6e4eb259a) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_changed_param_int.
std::function< void([FloatParam](structmavsdk_1_1_param_server_1_1_float_param.md))> [ChangedParamFloatCallback](#classmavsdk_1_1_param_server_1a11b86b8482f80f95130647dfa98ebaa3) | Callback type for subscribe_changed_param_float.
[Handle](classmavsdk_1_1_handle.md)< [FloatParam](structmavsdk_1_1_param_server_1_1_float_param.md) > [ChangedParamFloatHandle](#classmavsdk_1_1_param_server_1a86b8aa9c92d3d6d7dd47fc9b8f0bf881) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_changed_param_float.
std::function< void([CustomParam](structmavsdk_1_1_param_server_1_1_custom_param.md))> [ChangedParamCustomCallback](#classmavsdk_1_1_param_server_1ada550af744c9125178f7ad74d3c1041a) | Callback type for subscribe_changed_param_custom.
[Handle](classmavsdk_1_1_handle.md)< [CustomParam](structmavsdk_1_1_param_server_1_1_custom_param.md) > [ChangedParamCustomHandle](#classmavsdk_1_1_param_server_1a27cf0998ffff4bbdb3f00743b30c903f) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_changed_param_custom.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [ParamServer](#classmavsdk_1_1_param_server_1ae86996ca7c1cf57ae1d011ca5279d231) (std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > server_component) | Constructor. Creates the plugin for a [ServerComponent](classmavsdk_1_1_server_component.md) instance.
&nbsp; | [~ParamServer](#classmavsdk_1_1_param_server_1a0b221c28148f0278f063232059d372b5) () override | Destructor (internal use only).
&nbsp; | [ParamServer](#classmavsdk_1_1_param_server_1a4cffcb488093838f72414c94e6c40fd0) (const [ParamServer](classmavsdk_1_1_param_server.md) & other) | Copy constructor.
std::pair< [Result](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1a6f7fcc017f43dcf68837dbc35ee4f469), int32_t > | [retrieve_param_int](#classmavsdk_1_1_param_server_1a95c445dbdd2b764248c811da0230b0b4) (std::string name)const | Retrieve an int parameter.
[Result](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1a6f7fcc017f43dcf68837dbc35ee4f469) | [provide_param_int](#classmavsdk_1_1_param_server_1a9de5dade4020eda7fb1cc07c6868dad1) (std::string name, int32_t value)const | Provide an int parameter.
std::pair< [Result](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1a6f7fcc017f43dcf68837dbc35ee4f469), float > | [retrieve_param_float](#classmavsdk_1_1_param_server_1a2845916c07a7e47e7444a49f88b23320) (std::string name)const | Retrieve a float parameter.
[Result](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1a6f7fcc017f43dcf68837dbc35ee4f469) | [provide_param_float](#classmavsdk_1_1_param_server_1a7893e4b00609eb0826835b3d8930db1f) (std::string name, float value)const | Provide a float parameter.
std::pair< [Result](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1a6f7fcc017f43dcf68837dbc35ee4f469), std::string > | [retrieve_param_custom](#classmavsdk_1_1_param_server_1aa6564b8138bc66519f425a350265b50d) (std::string name)const | Retrieve a custom parameter.
[Result](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1a6f7fcc017f43dcf68837dbc35ee4f469) | [provide_param_custom](#classmavsdk_1_1_param_server_1a60487de3470b9b1c39b403d4c9053d73) (std::string name, std::string value)const | Provide a custom parameter.
[ParamServer::AllParams](structmavsdk_1_1_param_server_1_1_all_params.md) | [retrieve_all_params](#classmavsdk_1_1_param_server_1aaf6b3862213d415ff26730afad95565f) () const | Retrieve all parameters.
[ChangedParamIntHandle](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1a923f3ae88ca5614690f3b1b6e4eb259a) | [subscribe_changed_param_int](#classmavsdk_1_1_param_server_1ad022db97fe1c040d255ce8eeb98dcbf9) (const [ChangedParamIntCallback](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1a7674183da6d76416b34df9ce51c59358) & callback) | Subscribe to changed int param.
void | [unsubscribe_changed_param_int](#classmavsdk_1_1_param_server_1adea6bfacaf4808d572d0559279d0d3a4) ([ChangedParamIntHandle](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1a923f3ae88ca5614690f3b1b6e4eb259a) handle) | Unsubscribe from subscribe_changed_param_int.
[ChangedParamFloatHandle](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1a86b8aa9c92d3d6d7dd47fc9b8f0bf881) | [subscribe_changed_param_float](#classmavsdk_1_1_param_server_1aed4a104c9e99603002e16843360c5a18) (const [ChangedParamFloatCallback](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1a11b86b8482f80f95130647dfa98ebaa3) & callback) | Subscribe to changed float param.
void | [unsubscribe_changed_param_float](#classmavsdk_1_1_param_server_1a33796a9ecc396b905b51ad4d2ce7989b) ([ChangedParamFloatHandle](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1a86b8aa9c92d3d6d7dd47fc9b8f0bf881) handle) | Unsubscribe from subscribe_changed_param_float.
[ChangedParamCustomHandle](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1a27cf0998ffff4bbdb3f00743b30c903f) | [subscribe_changed_param_custom](#classmavsdk_1_1_param_server_1a91d4aa280e7e0c43621ea87088781ccb) (const [ChangedParamCustomCallback](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1ada550af744c9125178f7ad74d3c1041a) & callback) | Subscribe to changed custom param.
void | [unsubscribe_changed_param_custom](#classmavsdk_1_1_param_server_1aa78c8df43a9d4c2dbdb6516a04d5f2de) ([ChangedParamCustomHandle](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1a27cf0998ffff4bbdb3f00743b30c903f) handle) | Unsubscribe from subscribe_changed_param_custom.
const [ParamServer](classmavsdk_1_1_param_server.md) & | [operator=](#classmavsdk_1_1_param_server_1a29ce1d2c4a2b80fbe4a0b7e7470e14af) (const [ParamServer](classmavsdk_1_1_param_server.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### ParamServer() {#classmavsdk_1_1_param_server_1ae86996ca7c1cf57ae1d011ca5279d231}
```cpp
mavsdk::ParamServer::ParamServer(std::shared_ptr< ServerComponent > server_component)
```


Constructor. Creates the plugin for a [ServerComponent](classmavsdk_1_1_server_component.md) instance.

The plugin is typically created as shown below: 

```cpp
auto param_server = ParamServer(server_component);
```

**Parameters**

* std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > **server_component** - The [ServerComponent](classmavsdk_1_1_server_component.md) instance associated with this server plugin.

### ~ParamServer() {#classmavsdk_1_1_param_server_1a0b221c28148f0278f063232059d372b5}
```cpp
mavsdk::ParamServer::~ParamServer() override
```


Destructor (internal use only).


### ParamServer() {#classmavsdk_1_1_param_server_1a4cffcb488093838f72414c94e6c40fd0}
```cpp
mavsdk::ParamServer::ParamServer(const ParamServer &other)
```


Copy constructor.


**Parameters**

* const [ParamServer](classmavsdk_1_1_param_server.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_param_server_1a669845877bb8e14482fb46542825c625}

```cpp
using mavsdk::ParamServer::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [ParamServer](classmavsdk_1_1_param_server.md) calls.


### typedef ChangedParamIntCallback {#classmavsdk_1_1_param_server_1a7674183da6d76416b34df9ce51c59358}

```cpp
using mavsdk::ParamServer::ChangedParamIntCallback =  std::function<void(IntParam)>
```


Callback type for subscribe_changed_param_int.


### typedef ChangedParamIntHandle {#classmavsdk_1_1_param_server_1a923f3ae88ca5614690f3b1b6e4eb259a}

```cpp
using mavsdk::ParamServer::ChangedParamIntHandle =  Handle<IntParam>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_changed_param_int.


### typedef ChangedParamFloatCallback {#classmavsdk_1_1_param_server_1a11b86b8482f80f95130647dfa98ebaa3}

```cpp
using mavsdk::ParamServer::ChangedParamFloatCallback =  std::function<void(FloatParam)>
```


Callback type for subscribe_changed_param_float.


### typedef ChangedParamFloatHandle {#classmavsdk_1_1_param_server_1a86b8aa9c92d3d6d7dd47fc9b8f0bf881}

```cpp
using mavsdk::ParamServer::ChangedParamFloatHandle =  Handle<FloatParam>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_changed_param_float.


### typedef ChangedParamCustomCallback {#classmavsdk_1_1_param_server_1ada550af744c9125178f7ad74d3c1041a}

```cpp
using mavsdk::ParamServer::ChangedParamCustomCallback =  std::function<void(CustomParam)>
```


Callback type for subscribe_changed_param_custom.


### typedef ChangedParamCustomHandle {#classmavsdk_1_1_param_server_1a27cf0998ffff4bbdb3f00743b30c903f}

```cpp
using mavsdk::ParamServer::ChangedParamCustomHandle =  Handle<CustomParam>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_changed_param_custom.


## Member Enumeration Documentation


### enum Result {#classmavsdk_1_1_param_server_1a6f7fcc017f43dcf68837dbc35ee4f469}


Possible results returned for param requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_param_server_1a6f7fcc017f43dcf68837dbc35ee4f469a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_param_server_1a6f7fcc017f43dcf68837dbc35ee4f469a505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Request succeeded. 
<span id="classmavsdk_1_1_param_server_1a6f7fcc017f43dcf68837dbc35ee4f469a38c300f4fc9ce8a77aad4a30de05cad8"></span> `NotFound` | Not Found. 
<span id="classmavsdk_1_1_param_server_1a6f7fcc017f43dcf68837dbc35ee4f469afdf152936dcbf201445440856357f6ac"></span> `WrongType` | Wrong type. 
<span id="classmavsdk_1_1_param_server_1a6f7fcc017f43dcf68837dbc35ee4f469aa2b5cfc4e45ca036892b3dadc483e655"></span> `ParamNameTooLong` | Parameter name too long (> 16). 
<span id="classmavsdk_1_1_param_server_1a6f7fcc017f43dcf68837dbc35ee4f469a1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system available. 
<span id="classmavsdk_1_1_param_server_1a6f7fcc017f43dcf68837dbc35ee4f469a1fc93bc695e2e3e1903029eb77228234"></span> `ParamValueTooLong` | Parameter name too long (> 128). 

## Member Function Documentation


### retrieve_param_int() {#classmavsdk_1_1_param_server_1a95c445dbdd2b764248c811da0230b0b4}
```cpp
std::pair<Result, int32_t> mavsdk::ParamServer::retrieve_param_int(std::string name) const
```


Retrieve an int parameter.

If the type is wrong, the result will be `WRONG_TYPE`.


This function is blocking.

**Parameters**

* std::string **name** - 

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1a6f7fcc017f43dcf68837dbc35ee4f469), int32_t > - Result of request.

### provide_param_int() {#classmavsdk_1_1_param_server_1a9de5dade4020eda7fb1cc07c6868dad1}
```cpp
Result mavsdk::ParamServer::provide_param_int(std::string name, int32_t value) const
```


Provide an int parameter.

If the type is wrong, the result will be `WRONG_TYPE`.


This function is blocking.

**Parameters**

* std::string **name** - 
* int32_t **value** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1a6f7fcc017f43dcf68837dbc35ee4f469) - Result of request.

### retrieve_param_float() {#classmavsdk_1_1_param_server_1a2845916c07a7e47e7444a49f88b23320}
```cpp
std::pair<Result, float> mavsdk::ParamServer::retrieve_param_float(std::string name) const
```


Retrieve a float parameter.

If the type is wrong, the result will be `WRONG_TYPE`.


This function is blocking.

**Parameters**

* std::string **name** - 

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1a6f7fcc017f43dcf68837dbc35ee4f469), float > - Result of request.

### provide_param_float() {#classmavsdk_1_1_param_server_1a7893e4b00609eb0826835b3d8930db1f}
```cpp
Result mavsdk::ParamServer::provide_param_float(std::string name, float value) const
```


Provide a float parameter.

If the type is wrong, the result will be `WRONG_TYPE`.


This function is blocking.

**Parameters**

* std::string **name** - 
* float **value** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1a6f7fcc017f43dcf68837dbc35ee4f469) - Result of request.

### retrieve_param_custom() {#classmavsdk_1_1_param_server_1aa6564b8138bc66519f425a350265b50d}
```cpp
std::pair<Result, std::string> mavsdk::ParamServer::retrieve_param_custom(std::string name) const
```


Retrieve a custom parameter.

If the type is wrong, the result will be `WRONG_TYPE`.


This function is blocking.

**Parameters**

* std::string **name** - 

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1a6f7fcc017f43dcf68837dbc35ee4f469), std::string > - Result of request.

### provide_param_custom() {#classmavsdk_1_1_param_server_1a60487de3470b9b1c39b403d4c9053d73}
```cpp
Result mavsdk::ParamServer::provide_param_custom(std::string name, std::string value) const
```


Provide a custom parameter.

If the type is wrong, the result will be `WRONG_TYPE`.


This function is blocking.

**Parameters**

* std::string **name** - 
* std::string **value** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1a6f7fcc017f43dcf68837dbc35ee4f469) - Result of request.

### retrieve_all_params() {#classmavsdk_1_1_param_server_1aaf6b3862213d415ff26730afad95565f}
```cpp
ParamServer::AllParams mavsdk::ParamServer::retrieve_all_params() const
```


Retrieve all parameters.

This function is blocking.

**Returns**

&emsp;[ParamServer::AllParams](structmavsdk_1_1_param_server_1_1_all_params.md) - Result of request.

### subscribe_changed_param_int() {#classmavsdk_1_1_param_server_1ad022db97fe1c040d255ce8eeb98dcbf9}
```cpp
ChangedParamIntHandle mavsdk::ParamServer::subscribe_changed_param_int(const ChangedParamIntCallback &callback)
```


Subscribe to changed int param.


**Parameters**

* const [ChangedParamIntCallback](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1a7674183da6d76416b34df9ce51c59358)& **callback** - 

**Returns**

&emsp;[ChangedParamIntHandle](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1a923f3ae88ca5614690f3b1b6e4eb259a) - 

### unsubscribe_changed_param_int() {#classmavsdk_1_1_param_server_1adea6bfacaf4808d572d0559279d0d3a4}
```cpp
void mavsdk::ParamServer::unsubscribe_changed_param_int(ChangedParamIntHandle handle)
```


Unsubscribe from subscribe_changed_param_int.


**Parameters**

* [ChangedParamIntHandle](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1a923f3ae88ca5614690f3b1b6e4eb259a) **handle** - 

### subscribe_changed_param_float() {#classmavsdk_1_1_param_server_1aed4a104c9e99603002e16843360c5a18}
```cpp
ChangedParamFloatHandle mavsdk::ParamServer::subscribe_changed_param_float(const ChangedParamFloatCallback &callback)
```


Subscribe to changed float param.


**Parameters**

* const [ChangedParamFloatCallback](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1a11b86b8482f80f95130647dfa98ebaa3)& **callback** - 

**Returns**

&emsp;[ChangedParamFloatHandle](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1a86b8aa9c92d3d6d7dd47fc9b8f0bf881) - 

### unsubscribe_changed_param_float() {#classmavsdk_1_1_param_server_1a33796a9ecc396b905b51ad4d2ce7989b}
```cpp
void mavsdk::ParamServer::unsubscribe_changed_param_float(ChangedParamFloatHandle handle)
```


Unsubscribe from subscribe_changed_param_float.


**Parameters**

* [ChangedParamFloatHandle](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1a86b8aa9c92d3d6d7dd47fc9b8f0bf881) **handle** - 

### subscribe_changed_param_custom() {#classmavsdk_1_1_param_server_1a91d4aa280e7e0c43621ea87088781ccb}
```cpp
ChangedParamCustomHandle mavsdk::ParamServer::subscribe_changed_param_custom(const ChangedParamCustomCallback &callback)
```


Subscribe to changed custom param.


**Parameters**

* const [ChangedParamCustomCallback](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1ada550af744c9125178f7ad74d3c1041a)& **callback** - 

**Returns**

&emsp;[ChangedParamCustomHandle](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1a27cf0998ffff4bbdb3f00743b30c903f) - 

### unsubscribe_changed_param_custom() {#classmavsdk_1_1_param_server_1aa78c8df43a9d4c2dbdb6516a04d5f2de}
```cpp
void mavsdk::ParamServer::unsubscribe_changed_param_custom(ChangedParamCustomHandle handle)
```


Unsubscribe from subscribe_changed_param_custom.


**Parameters**

* [ChangedParamCustomHandle](classmavsdk_1_1_param_server.md#classmavsdk_1_1_param_server_1a27cf0998ffff4bbdb3f00743b30c903f) **handle** - 

### operator=() {#classmavsdk_1_1_param_server_1a29ce1d2c4a2b80fbe4a0b7e7470e14af}
```cpp
const ParamServer& mavsdk::ParamServer::operator=(const ParamServer &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [ParamServer](classmavsdk_1_1_param_server.md)&  - 

**Returns**

&emsp;const [ParamServer](classmavsdk_1_1_param_server.md) & - 