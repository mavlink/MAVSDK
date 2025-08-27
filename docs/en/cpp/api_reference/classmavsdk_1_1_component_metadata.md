# mavsdk::ComponentMetadata Class Reference
`#include: component_metadata.h`

----


Access component metadata json definitions, such as parameters. 


## Data Structures


struct [MetadataData](structmavsdk_1_1_component_metadata_1_1_metadata_data.md)

struct [MetadataUpdate](structmavsdk_1_1_component_metadata_1_1_metadata_update.md)

## Public Types


Type | Description
--- | ---
enum [MetadataType](#classmavsdk_1_1_component_metadata_1a0d85236afbbb3dc6b2dd46351a6b38bc) | The metadata type.
enum [Result](#classmavsdk_1_1_component_metadata_1af7b6ff8c58634fac8d2670f2b882ef44) | Possible results returned.
std::function< void([Result](classmavsdk_1_1_component_metadata.md#classmavsdk_1_1_component_metadata_1af7b6ff8c58634fac8d2670f2b882ef44))> [ResultCallback](#classmavsdk_1_1_component_metadata_1aca3d0432b9711666398eb1e3acdab327) | Callback type for asynchronous [ComponentMetadata](classmavsdk_1_1_component_metadata.md) calls.
std::function< void([MetadataUpdate](structmavsdk_1_1_component_metadata_1_1_metadata_update.md))> [MetadataAvailableCallback](#classmavsdk_1_1_component_metadata_1aba4946a5e2219b6b24a9dd3ce4f9ace1) | Callback type for subscribe_metadata_available.
[Handle](classmavsdk_1_1_handle.md)< [MetadataUpdate](structmavsdk_1_1_component_metadata_1_1_metadata_update.md) > [MetadataAvailableHandle](#classmavsdk_1_1_component_metadata_1a374204a0542af5f4a73bf36e25513910) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_metadata_available.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [ComponentMetadata](#classmavsdk_1_1_component_metadata_1a1d5efd4730ac6cc96131195675e76dd9) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [ComponentMetadata](#classmavsdk_1_1_component_metadata_1aa6ecff15e5832fbcd023d140a5583341) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~ComponentMetadata](#classmavsdk_1_1_component_metadata_1aac93ac109e9327015fd199646df75a73) () override | Destructor (internal use only).
&nbsp; | [ComponentMetadata](#classmavsdk_1_1_component_metadata_1a3b145559b5d92068e1bc9f7689223a48) (const [ComponentMetadata](classmavsdk_1_1_component_metadata.md) & other) | Copy constructor.
void | [request_component](#classmavsdk_1_1_component_metadata_1a1237d1aad4d8ab70470cf3390370625a) (uint32_t compid)const | Request metadata from a specific component. This is used to start requesting metadata from a component. The metadata can later be accessed via subscription (see below) or GetMetadata.
void | [request_autopilot_component](#classmavsdk_1_1_component_metadata_1a7620b5ef1adb417b6fea0222c44d2c33) () const | Request metadata from the autopilot component. This is used to start requesting metadata from the autopilot. The metadata can later be accessed via subscription (see below) or GetMetadata.
[MetadataAvailableHandle](classmavsdk_1_1_component_metadata.md#classmavsdk_1_1_component_metadata_1a374204a0542af5f4a73bf36e25513910) | [subscribe_metadata_available](#classmavsdk_1_1_component_metadata_1a622e41c77a19b137ae8dfd40bdb44ee5) (const [MetadataAvailableCallback](classmavsdk_1_1_component_metadata.md#classmavsdk_1_1_component_metadata_1aba4946a5e2219b6b24a9dd3ce4f9ace1) & callback) | Register a callback that gets called when metadata is available.
void | [unsubscribe_metadata_available](#classmavsdk_1_1_component_metadata_1a1245bcbb6d6edc650fe7e0b7035d90f4) ([MetadataAvailableHandle](classmavsdk_1_1_component_metadata.md#classmavsdk_1_1_component_metadata_1a374204a0542af5f4a73bf36e25513910) handle) | Unsubscribe from subscribe_metadata_available.
std::pair< [Result](classmavsdk_1_1_component_metadata.md#classmavsdk_1_1_component_metadata_1af7b6ff8c58634fac8d2670f2b882ef44), [ComponentMetadata::MetadataData](structmavsdk_1_1_component_metadata_1_1_metadata_data.md) > | [get_metadata](#classmavsdk_1_1_component_metadata_1a2a268428596fe70a97b6be3ec0d4046d) (uint32_t compid, [MetadataType](classmavsdk_1_1_component_metadata.md#classmavsdk_1_1_component_metadata_1a0d85236afbbb3dc6b2dd46351a6b38bc) metadata_type)const | Access metadata. This can be used if you know the metadata is available already, otherwise use the subscription to get notified when it becomes available.
const [ComponentMetadata](classmavsdk_1_1_component_metadata.md) & | [operator=](#classmavsdk_1_1_component_metadata_1a2be7fadaa2dad61f558430278ecf51bb) (const [ComponentMetadata](classmavsdk_1_1_component_metadata.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### ComponentMetadata() {#classmavsdk_1_1_component_metadata_1a1d5efd4730ac6cc96131195675e76dd9}
```cpp
mavsdk::ComponentMetadata::ComponentMetadata(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto component_metadata = ComponentMetadata(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### ComponentMetadata() {#classmavsdk_1_1_component_metadata_1aa6ecff15e5832fbcd023d140a5583341}
```cpp
mavsdk::ComponentMetadata::ComponentMetadata(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto component_metadata = ComponentMetadata(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~ComponentMetadata() {#classmavsdk_1_1_component_metadata_1aac93ac109e9327015fd199646df75a73}
```cpp
mavsdk::ComponentMetadata::~ComponentMetadata() override
```


Destructor (internal use only).


### ComponentMetadata() {#classmavsdk_1_1_component_metadata_1a3b145559b5d92068e1bc9f7689223a48}
```cpp
mavsdk::ComponentMetadata::ComponentMetadata(const ComponentMetadata &other)
```


Copy constructor.


**Parameters**

* const [ComponentMetadata](classmavsdk_1_1_component_metadata.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_component_metadata_1aca3d0432b9711666398eb1e3acdab327}

```cpp
using mavsdk::ComponentMetadata::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [ComponentMetadata](classmavsdk_1_1_component_metadata.md) calls.


### typedef MetadataAvailableCallback {#classmavsdk_1_1_component_metadata_1aba4946a5e2219b6b24a9dd3ce4f9ace1}

```cpp
using mavsdk::ComponentMetadata::MetadataAvailableCallback =  std::function<void(MetadataUpdate)>
```


Callback type for subscribe_metadata_available.


### typedef MetadataAvailableHandle {#classmavsdk_1_1_component_metadata_1a374204a0542af5f4a73bf36e25513910}

```cpp
using mavsdk::ComponentMetadata::MetadataAvailableHandle =  Handle<MetadataUpdate>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_metadata_available.


## Member Enumeration Documentation


### enum MetadataType {#classmavsdk_1_1_component_metadata_1a0d85236afbbb3dc6b2dd46351a6b38bc}


The metadata type.


Value | Description
--- | ---
<span id="classmavsdk_1_1_component_metadata_1a0d85236afbbb3dc6b2dd46351a6b38bcaddd135693fb2c479c7a756317837042b"></span> `AllCompleted` | This is set in the subscription callback when all metadata types completed for a given component ID. 
<span id="classmavsdk_1_1_component_metadata_1a0d85236afbbb3dc6b2dd46351a6b38bca83f499a540b1323009c200d6f8cc9396"></span> `Parameter` | Parameter metadata. 
<span id="classmavsdk_1_1_component_metadata_1a0d85236afbbb3dc6b2dd46351a6b38bca87f9f735a1d36793ceaecd4e47124b63"></span> `Events` | Event definitions. 
<span id="classmavsdk_1_1_component_metadata_1a0d85236afbbb3dc6b2dd46351a6b38bcabc26da9b4558294f17ead5c532d12b80"></span> `Actuators` | Actuator definitions. 

### enum Result {#classmavsdk_1_1_component_metadata_1af7b6ff8c58634fac8d2670f2b882ef44}


Possible results returned.


Value | Description
--- | ---
<span id="classmavsdk_1_1_component_metadata_1af7b6ff8c58634fac8d2670f2b882ef44a505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Success. 
<span id="classmavsdk_1_1_component_metadata_1af7b6ff8c58634fac8d2670f2b882ef44a534ceac854da4ba59c4dc41b7ab732dc"></span> `NotAvailable` | Not available. 
<span id="classmavsdk_1_1_component_metadata_1af7b6ff8c58634fac8d2670f2b882ef44a094a6f6b0868122a9dd008cb91c083e4"></span> `ConnectionError` | Connection error. 
<span id="classmavsdk_1_1_component_metadata_1af7b6ff8c58634fac8d2670f2b882ef44ab4080bdf74febf04d578ff105cce9d3f"></span> `Unsupported` | Unsupported. 
<span id="classmavsdk_1_1_component_metadata_1af7b6ff8c58634fac8d2670f2b882ef44a58d036b9b7f0e7eb38cfb90f1cc70a73"></span> `Denied` | Denied. 
<span id="classmavsdk_1_1_component_metadata_1af7b6ff8c58634fac8d2670f2b882ef44ad7c8c85bf79bbe1b7188497c32c3b0ca"></span> `Failed` | Failed. 
<span id="classmavsdk_1_1_component_metadata_1af7b6ff8c58634fac8d2670f2b882ef44ac85a251cc457840f1e032f1b733e9398"></span> `Timeout` | Timeout. 
<span id="classmavsdk_1_1_component_metadata_1af7b6ff8c58634fac8d2670f2b882ef44a1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system. 
<span id="classmavsdk_1_1_component_metadata_1af7b6ff8c58634fac8d2670f2b882ef44ac534a66e591207038fe31a2a9e0164b7"></span> `NotRequested` | Not requested. 

## Member Function Documentation


### request_component() {#classmavsdk_1_1_component_metadata_1a1237d1aad4d8ab70470cf3390370625a}
```cpp
void mavsdk::ComponentMetadata::request_component(uint32_t compid) const
```


Request metadata from a specific component. This is used to start requesting metadata from a component. The metadata can later be accessed via subscription (see below) or GetMetadata.

This function is blocking.

**Parameters**

* uint32_t **compid** - 

### request_autopilot_component() {#classmavsdk_1_1_component_metadata_1a7620b5ef1adb417b6fea0222c44d2c33}
```cpp
void mavsdk::ComponentMetadata::request_autopilot_component() const
```


Request metadata from the autopilot component. This is used to start requesting metadata from the autopilot. The metadata can later be accessed via subscription (see below) or GetMetadata.

This function is blocking.

### subscribe_metadata_available() {#classmavsdk_1_1_component_metadata_1a622e41c77a19b137ae8dfd40bdb44ee5}
```cpp
MetadataAvailableHandle mavsdk::ComponentMetadata::subscribe_metadata_available(const MetadataAvailableCallback &callback)
```


Register a callback that gets called when metadata is available.


**Parameters**

* const [MetadataAvailableCallback](classmavsdk_1_1_component_metadata.md#classmavsdk_1_1_component_metadata_1aba4946a5e2219b6b24a9dd3ce4f9ace1)& **callback** - 

**Returns**

&emsp;[MetadataAvailableHandle](classmavsdk_1_1_component_metadata.md#classmavsdk_1_1_component_metadata_1a374204a0542af5f4a73bf36e25513910) - 

### unsubscribe_metadata_available() {#classmavsdk_1_1_component_metadata_1a1245bcbb6d6edc650fe7e0b7035d90f4}
```cpp
void mavsdk::ComponentMetadata::unsubscribe_metadata_available(MetadataAvailableHandle handle)
```


Unsubscribe from subscribe_metadata_available.


**Parameters**

* [MetadataAvailableHandle](classmavsdk_1_1_component_metadata.md#classmavsdk_1_1_component_metadata_1a374204a0542af5f4a73bf36e25513910) **handle** - 

### get_metadata() {#classmavsdk_1_1_component_metadata_1a2a268428596fe70a97b6be3ec0d4046d}
```cpp
std::pair< Result, ComponentMetadata::MetadataData > mavsdk::ComponentMetadata::get_metadata(uint32_t compid, MetadataType metadata_type) const
```


Access metadata. This can be used if you know the metadata is available already, otherwise use the subscription to get notified when it becomes available.

This function is blocking.

**Parameters**

* uint32_t **compid** - 
* [MetadataType](classmavsdk_1_1_component_metadata.md#classmavsdk_1_1_component_metadata_1a0d85236afbbb3dc6b2dd46351a6b38bc) **metadata_type** - 

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_component_metadata.md#classmavsdk_1_1_component_metadata_1af7b6ff8c58634fac8d2670f2b882ef44), [ComponentMetadata::MetadataData](structmavsdk_1_1_component_metadata_1_1_metadata_data.md) > - Result of request.

### operator=() {#classmavsdk_1_1_component_metadata_1a2be7fadaa2dad61f558430278ecf51bb}
```cpp
const ComponentMetadata & mavsdk::ComponentMetadata::operator=(const ComponentMetadata &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [ComponentMetadata](classmavsdk_1_1_component_metadata.md)&  - 

**Returns**

&emsp;const [ComponentMetadata](classmavsdk_1_1_component_metadata.md) & - 