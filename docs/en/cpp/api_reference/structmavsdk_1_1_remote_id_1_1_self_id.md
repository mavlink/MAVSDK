# mavsdk::RemoteId::SelfId Struct Reference
`#include: UNKNOWN`

----


## Public Types


Type | Description
--- | ---
enum [DescType](#structmavsdk_1_1_remote_id_1_1_self_id_1a5ac107a55de48e823aaea962ab056c82) | 

## Data Fields


[DescType](structmavsdk_1_1_remote_id_1_1_self_id.md#structmavsdk_1_1_remote_id_1_1_self_id_1a5ac107a55de48e823aaea962ab056c82) [description_type](#structmavsdk_1_1_remote_id_1_1_self_id_1ae43a7128f2b1dfec50b51ae8a07dd0aa) {} - Indicates the type of the operator_id field.

std::string [description](#structmavsdk_1_1_remote_id_1_1_self_id_1a1d3c4ba302f7dd8d0d2502e25f4a1b65) {} - Text description or numeric value expressed as ASCII characters.


## Member Enumeration Documentation


### enum DescType {#structmavsdk_1_1_remote_id_1_1_self_id_1a5ac107a55de48e823aaea962ab056c82}


Value | Description
--- | ---
<span id="structmavsdk_1_1_remote_id_1_1_self_id_1a5ac107a55de48e823aaea962ab056c82a9dffbf69ffba8bc38bc4e01abf4b1675"></span> `Text` | Optional free-form text description of the purpose of the flight.. 
<span id="structmavsdk_1_1_remote_id_1_1_self_id_1a5ac107a55de48e823aaea962ab056c82aa3fa706f20bc0b7858b7ae6932261940"></span> `Emergency` | Optional additional clarification when status == MAV_ODID_STATUS_EMERGENCY.. 
<span id="structmavsdk_1_1_remote_id_1_1_self_id_1a5ac107a55de48e823aaea962ab056c82a802ec171785b37272c728ea9e10bdef7"></span> `ExtendedStatus` | Optional additional clarification when status != MAV_ODID_STATUS_EMERGENCY.. 

## Field Documentation


### description_type {#structmavsdk_1_1_remote_id_1_1_self_id_1ae43a7128f2b1dfec50b51ae8a07dd0aa}

```cpp
DescType mavsdk::RemoteId::SelfId::description_type {}
```


Indicates the type of the operator_id field.


### description {#structmavsdk_1_1_remote_id_1_1_self_id_1a1d3c4ba302f7dd8d0d2502e25f4a1b65}

```cpp
std::string mavsdk::RemoteId::SelfId::description {}
```


Text description or numeric value expressed as ASCII characters.

