# mavsdk::Param Class Reference
`#include: param.h`

----


Provide raw access to get and set parameters. 


## Data Structures


struct [AllParams](structmavsdk_1_1_param_1_1_all_params.md)

struct [CustomParam](structmavsdk_1_1_param_1_1_custom_param.md)

struct [FloatParam](structmavsdk_1_1_param_1_1_float_param.md)

struct [IntParam](structmavsdk_1_1_param_1_1_int_param.md)

## Public Types


Type | Description
--- | ---
enum [ProtocolVersion](#classmavsdk_1_1_param_1a37807968ecb40a732b4fec83792bd5c8) | Parameter version.
enum [Result](#classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881df) | Possible results returned for param requests.
std::function< void([Result](classmavsdk_1_1_param.md#classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881df))> [ResultCallback](#classmavsdk_1_1_param_1a7047374c38d4220e8709c2b10275f860) | Callback type for asynchronous [Param](classmavsdk_1_1_param.md) calls.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [Param](#classmavsdk_1_1_param_1a3f15c8d0c238a68cd97a49ba5c3ea1ef) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [Param](#classmavsdk_1_1_param_1a08e40eaf4052555d28f2404cc7ede680) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~Param](#classmavsdk_1_1_param_1a33f67b5c3daea8ca3af8c573f4e07153) () override | Destructor (internal use only).
&nbsp; | [Param](#classmavsdk_1_1_param_1ab7a03a825118c944d31c562594826f72) (const [Param](classmavsdk_1_1_param.md) & other) | Copy constructor.
std::pair< [Result](classmavsdk_1_1_param.md#classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881df), int32_t > | [get_param_int](#classmavsdk_1_1_param_1a23e4fe22bcef677fe9bb291a8f7d56c5) (std::string name)const | Get an int parameter.
[Result](classmavsdk_1_1_param.md#classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881df) | [set_param_int](#classmavsdk_1_1_param_1af8124bae8b4649605a51fe2943ae8414) (std::string name, int32_t value)const | Set an int parameter.
std::pair< [Result](classmavsdk_1_1_param.md#classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881df), float > | [get_param_float](#classmavsdk_1_1_param_1a3258e5ceec1bfaa2b0228786f197f4d3) (std::string name)const | Get a float parameter.
[Result](classmavsdk_1_1_param.md#classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881df) | [set_param_float](#classmavsdk_1_1_param_1a58a2f14fbcda2bf73815dbc2a31528bf) (std::string name, float value)const | Set a float parameter.
std::pair< [Result](classmavsdk_1_1_param.md#classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881df), std::string > | [get_param_custom](#classmavsdk_1_1_param_1a0fdcf7aee3324843ddd1638aa409a962) (std::string name)const | Get a custom parameter.
[Result](classmavsdk_1_1_param.md#classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881df) | [set_param_custom](#classmavsdk_1_1_param_1abb9cc4e4e14d33a93b23295f836de39e) (std::string name, std::string value)const | Set a custom parameter.
[Param::AllParams](structmavsdk_1_1_param_1_1_all_params.md) | [get_all_params](#classmavsdk_1_1_param_1ab9259e1f91809aa574404131aa540fd8) () const | Get all parameters.
[Result](classmavsdk_1_1_param.md#classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881df) | [select_component](#classmavsdk_1_1_param_1a2ef2e607225d54c6fedd21a9b483937f) (int32_t component_id, [ProtocolVersion](classmavsdk_1_1_param.md#classmavsdk_1_1_param_1a37807968ecb40a732b4fec83792bd5c8) protocol_version)const | Select component ID of parameter component to talk to and param protocol version.
const [Param](classmavsdk_1_1_param.md) & | [operator=](#classmavsdk_1_1_param_1ac66cb2c623da03454e0cee033c3a1514) (const [Param](classmavsdk_1_1_param.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### Param() {#classmavsdk_1_1_param_1a3f15c8d0c238a68cd97a49ba5c3ea1ef}
```cpp
mavsdk::Param::Param(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto param = Param(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### Param() {#classmavsdk_1_1_param_1a08e40eaf4052555d28f2404cc7ede680}
```cpp
mavsdk::Param::Param(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto param = Param(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~Param() {#classmavsdk_1_1_param_1a33f67b5c3daea8ca3af8c573f4e07153}
```cpp
mavsdk::Param::~Param() override
```


Destructor (internal use only).


### Param() {#classmavsdk_1_1_param_1ab7a03a825118c944d31c562594826f72}
```cpp
mavsdk::Param::Param(const Param &other)
```


Copy constructor.


**Parameters**

* const [Param](classmavsdk_1_1_param.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_param_1a7047374c38d4220e8709c2b10275f860}

```cpp
using mavsdk::Param::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [Param](classmavsdk_1_1_param.md) calls.


## Member Enumeration Documentation


### enum ProtocolVersion {#classmavsdk_1_1_param_1a37807968ecb40a732b4fec83792bd5c8}


Parameter version.


Value | Description
--- | ---
<span id="classmavsdk_1_1_param_1a37807968ecb40a732b4fec83792bd5c8ab4daca084ad9eabfc8de231929477ed6"></span> `V1` | Original v1 version. 
<span id="classmavsdk_1_1_param_1a37807968ecb40a732b4fec83792bd5c8a84ab79217fad62904ab3f60586129611"></span> `Ext` | Extended param version. 

### enum Result {#classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881df}


Possible results returned for param requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881dfa88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881dfa505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Request succeeded. 
<span id="classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881dfac85a251cc457840f1e032f1b733e9398"></span> `Timeout` | Request timed out. 
<span id="classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881dfa094a6f6b0868122a9dd008cb91c083e4"></span> `ConnectionError` | Connection error. 
<span id="classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881dfafdf152936dcbf201445440856357f6ac"></span> `WrongType` | Wrong type. 
<span id="classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881dfaa2b5cfc4e45ca036892b3dadc483e655"></span> `ParamNameTooLong` | Parameter name too long (> 16). 
<span id="classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881dfa1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system connected. 
<span id="classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881dfa1fc93bc695e2e3e1903029eb77228234"></span> `ParamValueTooLong` | [Param](classmavsdk_1_1_param.md) value too long (> 128). 
<span id="classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881dfad7c8c85bf79bbe1b7188497c32c3b0ca"></span> `Failed` | Operation failed.. 
<span id="classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881dfac47c93cf49de211bba5d62d65225f128"></span> `DoesNotExist` | Parameter does not exist. 
<span id="classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881dfaf3bd760221d37f5f1a973f094ea02478"></span> `ValueOutOfRange` | Parameter value does not fit within accepted range. 
<span id="classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881dfa4b33a686be73bb172407d73b26356275"></span> `PermissionDenied` | Caller is not permitted to set the value of this parameter. 
<span id="classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881dfab43f027b5acb9fb70fafef6a3de8e646"></span> `ComponentNotFound` | Unknown component specified. 
<span id="classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881dfa131fb182a881796e7606ed6da27f1197"></span> `ReadOnly` | Parameter is read-only. 
<span id="classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881dfa849c937b23fd724a14c4597eef61ce29"></span> `TypeUnsupported` | Parameter data type is not supported by flight stack. 
<span id="classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881dfa2782c4707c245684aa95545098e1b70b"></span> `TypeMismatch` | Parameter type does not match expected type. 
<span id="classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881dfa61d8b4448684ca1db22e9894cf995e0a"></span> `ReadFail` | Parameter exists but reading failed. 

## Member Function Documentation


### get_param_int() {#classmavsdk_1_1_param_1a23e4fe22bcef677fe9bb291a8f7d56c5}
```cpp
std::pair< Result, int32_t > mavsdk::Param::get_param_int(std::string name) const
```


Get an int parameter.

If the type is wrong, the result will be `WRONG_TYPE`.


This function is blocking.

**Parameters**

* std::string **name** - 

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_param.md#classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881df), int32_t > - Result of request.

### set_param_int() {#classmavsdk_1_1_param_1af8124bae8b4649605a51fe2943ae8414}
```cpp
Result mavsdk::Param::set_param_int(std::string name, int32_t value) const
```


Set an int parameter.

If the type is wrong, the result will be `WRONG_TYPE`.


This function is blocking.

**Parameters**

* std::string **name** - 
* int32_t **value** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_param.md#classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881df) - Result of request.

### get_param_float() {#classmavsdk_1_1_param_1a3258e5ceec1bfaa2b0228786f197f4d3}
```cpp
std::pair< Result, float > mavsdk::Param::get_param_float(std::string name) const
```


Get a float parameter.

If the type is wrong, the result will be `WRONG_TYPE`.


This function is blocking.

**Parameters**

* std::string **name** - 

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_param.md#classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881df), float > - Result of request.

### set_param_float() {#classmavsdk_1_1_param_1a58a2f14fbcda2bf73815dbc2a31528bf}
```cpp
Result mavsdk::Param::set_param_float(std::string name, float value) const
```


Set a float parameter.

If the type is wrong, the result will be `WRONG_TYPE`.


This function is blocking.

**Parameters**

* std::string **name** - 
* float **value** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_param.md#classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881df) - Result of request.

### get_param_custom() {#classmavsdk_1_1_param_1a0fdcf7aee3324843ddd1638aa409a962}
```cpp
std::pair< Result, std::string > mavsdk::Param::get_param_custom(std::string name) const
```


Get a custom parameter.

If the type is wrong, the result will be `WRONG_TYPE`.


This function is blocking.

**Parameters**

* std::string **name** - 

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_param.md#classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881df), std::string > - Result of request.

### set_param_custom() {#classmavsdk_1_1_param_1abb9cc4e4e14d33a93b23295f836de39e}
```cpp
Result mavsdk::Param::set_param_custom(std::string name, std::string value) const
```


Set a custom parameter.

If the type is wrong, the result will be `WRONG_TYPE`.


This function is blocking.

**Parameters**

* std::string **name** - 
* std::string **value** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_param.md#classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881df) - Result of request.

### get_all_params() {#classmavsdk_1_1_param_1ab9259e1f91809aa574404131aa540fd8}
```cpp
Param::AllParams mavsdk::Param::get_all_params() const
```


Get all parameters.

This function is blocking.

**Returns**

&emsp;[Param::AllParams](structmavsdk_1_1_param_1_1_all_params.md) - Result of request.

### select_component() {#classmavsdk_1_1_param_1a2ef2e607225d54c6fedd21a9b483937f}
```cpp
Result mavsdk::Param::select_component(int32_t component_id, ProtocolVersion protocol_version) const
```


Select component ID of parameter component to talk to and param protocol version.

Default is the autopilot component (1), and Version (0).


This function is blocking.

**Parameters**

* int32_t **component_id** - 
* [ProtocolVersion](classmavsdk_1_1_param.md#classmavsdk_1_1_param_1a37807968ecb40a732b4fec83792bd5c8) **protocol_version** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_param.md#classmavsdk_1_1_param_1afde69c8b60c41e2f21db148d211881df) - Result of request.

### operator=() {#classmavsdk_1_1_param_1ac66cb2c623da03454e0cee033c3a1514}
```cpp
const Param & mavsdk::Param::operator=(const Param &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [Param](classmavsdk_1_1_param.md)&  - 

**Returns**

&emsp;const [Param](classmavsdk_1_1_param.md) & - 