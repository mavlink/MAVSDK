# mavsdk::PluginBase Class Reference
`#include: plugin_base.h`

----


Base class for every plugin. 


## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [PluginBase](#classmavsdk_1_1_plugin_base_1afbb5a017df6856e58fb576d65a9fe207) ()=default | Default Constructor.
&nbsp; | [~PluginBase](#classmavsdk_1_1_plugin_base_1a038befc8f15d34e0be17ec7df8e9d092) ()=default | Default Destructor.
&nbsp; | [PluginBase](#classmavsdk_1_1_plugin_base_1a717e8eda4a615730256f4a707f00aa72) (const [PluginBase](classmavsdk_1_1_plugin_base.md) &)=delete | Copy constructor (object is not copyable).
const [PluginBase](classmavsdk_1_1_plugin_base.md) & | [operator=](#classmavsdk_1_1_plugin_base_1a4f60b8eb315861e418e265ceba9a7a9e) (const [PluginBase](classmavsdk_1_1_plugin_base.md) &)=delete | Assign operator (object is not copyable).


## Constructor & Destructor Documentation


### PluginBase() {#classmavsdk_1_1_plugin_base_1afbb5a017df6856e58fb576d65a9fe207}
```cpp
mavsdk::PluginBase::PluginBase()=default
```


Default Constructor.


### ~PluginBase() {#classmavsdk_1_1_plugin_base_1a038befc8f15d34e0be17ec7df8e9d092}
```cpp
virtual mavsdk::PluginBase::~PluginBase()=default
```


Default Destructor.


### PluginBase() {#classmavsdk_1_1_plugin_base_1a717e8eda4a615730256f4a707f00aa72}
```cpp
mavsdk::PluginBase::PluginBase(const PluginBase &)=delete
```


Copy constructor (object is not copyable).


**Parameters**

* const [PluginBase](classmavsdk_1_1_plugin_base.md)&  - 

## Member Function Documentation


### operator=() {#classmavsdk_1_1_plugin_base_1a4f60b8eb315861e418e265ceba9a7a9e}
```cpp
const PluginBase & mavsdk::PluginBase::operator=(const PluginBase &)=delete
```


Assign operator (object is not copyable).


**Parameters**

* const [PluginBase](classmavsdk_1_1_plugin_base.md)&  - 

**Returns**

&emsp;const [PluginBase](classmavsdk_1_1_plugin_base.md) & - 