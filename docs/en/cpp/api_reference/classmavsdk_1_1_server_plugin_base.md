# mavsdk::ServerPluginBase Class Reference
`#include: server_plugin_base.h`

----


Base class for every server plugin. 


## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [ServerPluginBase](#classmavsdk_1_1_server_plugin_base_1a77b7c74e4e2a10cf55071e95b53eedae) ()=default | Default Constructor.
&nbsp; | [~ServerPluginBase](#classmavsdk_1_1_server_plugin_base_1a97e1d14c2b72253d53f9284621b1d248) ()=default | Default Destructor.
&nbsp; | [ServerPluginBase](#classmavsdk_1_1_server_plugin_base_1a86ef071b10d0afc7d287069ac8d11575) (const [ServerPluginBase](classmavsdk_1_1_server_plugin_base.md) &)=delete | Copy constructor (object is not copyable).
const [ServerPluginBase](classmavsdk_1_1_server_plugin_base.md) & | [operator=](#classmavsdk_1_1_server_plugin_base_1ab424dcb3a08fcb7159ca326361548301) (const [ServerPluginBase](classmavsdk_1_1_server_plugin_base.md) &)=delete | Assign operator (object is not copyable).


## Constructor & Destructor Documentation


### ServerPluginBase() {#classmavsdk_1_1_server_plugin_base_1a77b7c74e4e2a10cf55071e95b53eedae}
```cpp
mavsdk::ServerPluginBase::ServerPluginBase()=default
```


Default Constructor.


### ~ServerPluginBase() {#classmavsdk_1_1_server_plugin_base_1a97e1d14c2b72253d53f9284621b1d248}
```cpp
virtual mavsdk::ServerPluginBase::~ServerPluginBase()=default
```


Default Destructor.


### ServerPluginBase() {#classmavsdk_1_1_server_plugin_base_1a86ef071b10d0afc7d287069ac8d11575}
```cpp
mavsdk::ServerPluginBase::ServerPluginBase(const ServerPluginBase &)=delete
```


Copy constructor (object is not copyable).


**Parameters**

* const [ServerPluginBase](classmavsdk_1_1_server_plugin_base.md)&  - 

## Member Function Documentation


### operator=() {#classmavsdk_1_1_server_plugin_base_1ab424dcb3a08fcb7159ca326361548301}
```cpp
const ServerPluginBase& mavsdk::ServerPluginBase::operator=(const ServerPluginBase &)=delete
```


Assign operator (object is not copyable).


**Parameters**

* const [ServerPluginBase](classmavsdk_1_1_server_plugin_base.md)&  - 

**Returns**

&emsp;const [ServerPluginBase](classmavsdk_1_1_server_plugin_base.md) & - 