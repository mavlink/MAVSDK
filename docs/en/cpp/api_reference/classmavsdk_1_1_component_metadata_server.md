# mavsdk::ComponentMetadataServer Class Reference
`#include: component_metadata_server.h`

----


Provide component metadata json definitions, such as parameters. 


## Data Structures


struct [Metadata](structmavsdk_1_1_component_metadata_server_1_1_metadata.md)

## Public Types


Type | Description
--- | ---
enum [MetadataType](#classmavsdk_1_1_component_metadata_server_1abaa555f8d1e2ae73f2275b18271537d6) | The metadata type.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [ComponentMetadataServer](#classmavsdk_1_1_component_metadata_server_1a83ef20f6459a983e6c499ec4a40232f0) (std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > server_component) | Constructor. Creates the plugin for a [ServerComponent](classmavsdk_1_1_server_component.md) instance.
&nbsp; | [~ComponentMetadataServer](#classmavsdk_1_1_component_metadata_server_1a7597c973992ce3cab32c8270996f9900) () override | Destructor (internal use only).
&nbsp; | [ComponentMetadataServer](#classmavsdk_1_1_component_metadata_server_1a9f56317067d68551868680af34ac1124) (const [ComponentMetadataServer](classmavsdk_1_1_component_metadata_server.md) & other) | Copy constructor.
void | [set_metadata](#classmavsdk_1_1_component_metadata_server_1a441be303b7421b527d3ff3bc1dc8ac9d) (std::vector< [Metadata](structmavsdk_1_1_component_metadata_server_1_1_metadata.md) > metadata)const | Provide metadata (can only be called once)
const [ComponentMetadataServer](classmavsdk_1_1_component_metadata_server.md) & | [operator=](#classmavsdk_1_1_component_metadata_server_1ae873337d2e144e0cc1fbdcfd2881dfee) (const [ComponentMetadataServer](classmavsdk_1_1_component_metadata_server.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### ComponentMetadataServer() {#classmavsdk_1_1_component_metadata_server_1a83ef20f6459a983e6c499ec4a40232f0}
```cpp
mavsdk::ComponentMetadataServer::ComponentMetadataServer(std::shared_ptr< ServerComponent > server_component)
```


Constructor. Creates the plugin for a [ServerComponent](classmavsdk_1_1_server_component.md) instance.

The plugin is typically created as shown below: 

```cpp
auto component_metadata_server = ComponentMetadataServer(server_component);
```

**Parameters**

* std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > **server_component** - The [ServerComponent](classmavsdk_1_1_server_component.md) instance associated with this server plugin.

### ~ComponentMetadataServer() {#classmavsdk_1_1_component_metadata_server_1a7597c973992ce3cab32c8270996f9900}
```cpp
mavsdk::ComponentMetadataServer::~ComponentMetadataServer() override
```


Destructor (internal use only).


### ComponentMetadataServer() {#classmavsdk_1_1_component_metadata_server_1a9f56317067d68551868680af34ac1124}
```cpp
mavsdk::ComponentMetadataServer::ComponentMetadataServer(const ComponentMetadataServer &other)
```


Copy constructor.


**Parameters**

* const [ComponentMetadataServer](classmavsdk_1_1_component_metadata_server.md)& **other** - 

## Member Enumeration Documentation


### enum MetadataType {#classmavsdk_1_1_component_metadata_server_1abaa555f8d1e2ae73f2275b18271537d6}


The metadata type.


Value | Description
--- | ---
<span id="classmavsdk_1_1_component_metadata_server_1abaa555f8d1e2ae73f2275b18271537d6a83f499a540b1323009c200d6f8cc9396"></span> `Parameter` | Parameter metadata. 
<span id="classmavsdk_1_1_component_metadata_server_1abaa555f8d1e2ae73f2275b18271537d6a87f9f735a1d36793ceaecd4e47124b63"></span> `Events` | Event definitions. 
<span id="classmavsdk_1_1_component_metadata_server_1abaa555f8d1e2ae73f2275b18271537d6abc26da9b4558294f17ead5c532d12b80"></span> `Actuators` | Actuator definitions. 

## Member Function Documentation


### set_metadata() {#classmavsdk_1_1_component_metadata_server_1a441be303b7421b527d3ff3bc1dc8ac9d}
```cpp
void mavsdk::ComponentMetadataServer::set_metadata(std::vector< Metadata > metadata) const
```


Provide metadata (can only be called once)

This function is blocking.

**Parameters**

* std::vector< [Metadata](structmavsdk_1_1_component_metadata_server_1_1_metadata.md) > **metadata** - 

### operator=() {#classmavsdk_1_1_component_metadata_server_1ae873337d2e144e0cc1fbdcfd2881dfee}
```cpp
const ComponentMetadataServer & mavsdk::ComponentMetadataServer::operator=(const ComponentMetadataServer &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [ComponentMetadataServer](classmavsdk_1_1_component_metadata_server.md)&  - 

**Returns**

&emsp;const [ComponentMetadataServer](classmavsdk_1_1_component_metadata_server.md) & - 