# mavsdk::ComponentMetadata::MetadataUpdate Struct Reference
`#include: component_metadata.h`

----


Metadata for a given component and type. 


## Data Fields


uint32_t [compid](#structmavsdk_1_1_component_metadata_1_1_metadata_update_1acf104bcc3e77947687de065436acbbda) {} - The component ID.

[MetadataType](classmavsdk_1_1_component_metadata.md#classmavsdk_1_1_component_metadata_1a0d85236afbbb3dc6b2dd46351a6b38bc) [type](#structmavsdk_1_1_component_metadata_1_1_metadata_update_1ab38bfcf6247c11c74f12b46e4e630eed) {} - The metadata type.

std::string [json_metadata](#structmavsdk_1_1_component_metadata_1_1_metadata_update_1a2fad2330a6ed1c2fd1e5e34d5a56b377) {} - The JSON metadata.


## Field Documentation


### compid {#structmavsdk_1_1_component_metadata_1_1_metadata_update_1acf104bcc3e77947687de065436acbbda}

```cpp
uint32_t mavsdk::ComponentMetadata::MetadataUpdate::compid {}
```


The component ID.


### type {#structmavsdk_1_1_component_metadata_1_1_metadata_update_1ab38bfcf6247c11c74f12b46e4e630eed}

```cpp
MetadataType mavsdk::ComponentMetadata::MetadataUpdate::type {}
```


The metadata type.


### json_metadata {#structmavsdk_1_1_component_metadata_1_1_metadata_update_1a2fad2330a6ed1c2fd1e5e34d5a56b377}

```cpp
std::string mavsdk::ComponentMetadata::MetadataUpdate::json_metadata {}
```


The JSON metadata.

