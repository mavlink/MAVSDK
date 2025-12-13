# mavsdk::RemoteId::ArmStatus Struct Reference
`#include: UNKNOWN`

----


## Public Types


Type | Description
--- | ---
enum [Status](#structmavsdk_1_1_remote_id_1_1_arm_status_1a0a0d9bc466173fb1ea81f5c1ade165d4) | 

## Data Fields


[Status](structmavsdk_1_1_remote_id_1_1_arm_status.md#structmavsdk_1_1_remote_id_1_1_arm_status_1a0a0d9bc466173fb1ea81f5c1ade165d4) [status](#structmavsdk_1_1_remote_id_1_1_arm_status_1adc61cfb4da57757af09b399474e1fd14) {} - Status level indicating if arming is allowed.

std::string [error](#structmavsdk_1_1_remote_id_1_1_arm_status_1a9ce9979e248ae2e998a8a198c9916e2f) {} - Text error message, should be empty if status is good to arm. Fill with nulls in unused portion.


## Member Enumeration Documentation


### enum Status {#structmavsdk_1_1_remote_id_1_1_arm_status_1a0a0d9bc466173fb1ea81f5c1ade165d4}


Value | Description
--- | ---
<span id="structmavsdk_1_1_remote_id_1_1_arm_status_1a0a0d9bc466173fb1ea81f5c1ade165d4a6c0afce921633da24cf1e4c3fae5f36a"></span> `GoodToArm` | Passing arming checks.. 
<span id="structmavsdk_1_1_remote_id_1_1_arm_status_1a0a0d9bc466173fb1ea81f5c1ade165d4ae95689b3914af8ddd7d61c00993cf508"></span> `PreArmFailGeneric` | Generic arming failure, see error string for details.. 

## Field Documentation


### status {#structmavsdk_1_1_remote_id_1_1_arm_status_1adc61cfb4da57757af09b399474e1fd14}

```cpp
Status mavsdk::RemoteId::ArmStatus::status {}
```


Status level indicating if arming is allowed.


### error {#structmavsdk_1_1_remote_id_1_1_arm_status_1a9ce9979e248ae2e998a8a198c9916e2f}

```cpp
std::string mavsdk::RemoteId::ArmStatus::error {}
```


Text error message, should be empty if status is good to arm. Fill with nulls in unused portion.

