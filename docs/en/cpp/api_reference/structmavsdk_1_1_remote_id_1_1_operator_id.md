# mavsdk::RemoteId::OperatorId Struct Reference
`#include: UNKNOWN`

----


## Public Types


Type | Description
--- | ---
enum [OperatorIdType](#structmavsdk_1_1_remote_id_1_1_operator_id_1aa76c126f360e9808d1163b7ff00c02b9) | 

## Data Fields


[OperatorIdType](structmavsdk_1_1_remote_id_1_1_operator_id.md#structmavsdk_1_1_remote_id_1_1_operator_id_1aa76c126f360e9808d1163b7ff00c02b9) [operator_id_type](#structmavsdk_1_1_remote_id_1_1_operator_id_1aaa76865d7a65872be99677f3642fa4ca) {} - Indicates the type of the description field.

std::string [operator_id](#structmavsdk_1_1_remote_id_1_1_operator_id_1a7c7b1b6a9d948e1433a7b1071f1e4410) {} - Text description or numeric value expressed as ASCII characters.


## Member Enumeration Documentation


### enum OperatorIdType {#structmavsdk_1_1_remote_id_1_1_operator_id_1aa76c126f360e9808d1163b7ff00c02b9}


Value | Description
--- | ---
<span id="structmavsdk_1_1_remote_id_1_1_operator_id_1aa76c126f360e9808d1163b7ff00c02b9a1483d40f72f6777472485333f40794b9"></span> `Caa` | CAA (Civil Aviation Authority) registered operator ID.. 

## Field Documentation


### operator_id_type {#structmavsdk_1_1_remote_id_1_1_operator_id_1aaa76865d7a65872be99677f3642fa4ca}

```cpp
OperatorIdType mavsdk::RemoteId::OperatorId::operator_id_type {}
```


Indicates the type of the description field.


### operator_id {#structmavsdk_1_1_remote_id_1_1_operator_id_1a7c7b1b6a9d948e1433a7b1071f1e4410}

```cpp
std::string mavsdk::RemoteId::OperatorId::operator_id {}
```


Text description or numeric value expressed as ASCII characters.

