# mavsdk::System::AutopilotVersion Struct Reference
`#include: system.h`

----


This struct represents Autopilot version information. This is only used when MAVSDK is configured as an autopilot. 


Other MAVLink systems can use this to identify and match software and capabilities. 


## Data Fields


uint64_t [capabilities](#structmavsdk_1_1_system_1_1_autopilot_version_1a3c4e4f2fb21b67f9d676adbe0ec32ce6) {} - MAVLink autopilot_version capabilities.

uint32_t [flight_sw_version](#structmavsdk_1_1_system_1_1_autopilot_version_1a50dccbc4c01e07a65dda21ab7bb49e7f) {0} - MAVLink autopilot_version flight_sw_version.

uint32_t [middleware_sw_version](#structmavsdk_1_1_system_1_1_autopilot_version_1a737e740ddc844306a086e0c779568951) {0} - MAVLink autopilot_version middleware_sw_version.

uint32_t [os_sw_version](#structmavsdk_1_1_system_1_1_autopilot_version_1aa120013fe26560952d19f5d2bee1df01) {0} - MAVLink autopilot_version os_sw_version.

uint32_t [board_version](#structmavsdk_1_1_system_1_1_autopilot_version_1ab5fa9c33f5493e68fbe4b8407f07ba8d) {0} - MAVLink autopilot_version board_version.

uint16_t [vendor_id](#structmavsdk_1_1_system_1_1_autopilot_version_1a04b6ff2e1add2247a8c510e018efb81a) {0} - MAVLink autopilot_version vendor_id.

uint16_t [product_id](#structmavsdk_1_1_system_1_1_autopilot_version_1a0c09156f2c75545f3895023fe22f80ec) {0} - MAVLink autopilot_version product_id.

std::array< uint8_t, 18 > [uid2](#structmavsdk_1_1_system_1_1_autopilot_version_1a7f0bd696e9200a6e3c802fd9b05adca9) {0} - MAVLink autopilot_version uid2.


## Field Documentation


### capabilities {#structmavsdk_1_1_system_1_1_autopilot_version_1a3c4e4f2fb21b67f9d676adbe0ec32ce6}

```cpp
uint64_t mavsdk::System::AutopilotVersion::capabilities {}
```


MAVLink autopilot_version capabilities.


### flight_sw_version {#structmavsdk_1_1_system_1_1_autopilot_version_1a50dccbc4c01e07a65dda21ab7bb49e7f}

```cpp
uint32_t mavsdk::System::AutopilotVersion::flight_sw_version {0}
```


MAVLink autopilot_version flight_sw_version.


### middleware_sw_version {#structmavsdk_1_1_system_1_1_autopilot_version_1a737e740ddc844306a086e0c779568951}

```cpp
uint32_t mavsdk::System::AutopilotVersion::middleware_sw_version {0}
```


MAVLink autopilot_version middleware_sw_version.


### os_sw_version {#structmavsdk_1_1_system_1_1_autopilot_version_1aa120013fe26560952d19f5d2bee1df01}

```cpp
uint32_t mavsdk::System::AutopilotVersion::os_sw_version {0}
```


MAVLink autopilot_version os_sw_version.


### board_version {#structmavsdk_1_1_system_1_1_autopilot_version_1ab5fa9c33f5493e68fbe4b8407f07ba8d}

```cpp
uint32_t mavsdk::System::AutopilotVersion::board_version {0}
```


MAVLink autopilot_version board_version.


### vendor_id {#structmavsdk_1_1_system_1_1_autopilot_version_1a04b6ff2e1add2247a8c510e018efb81a}

```cpp
uint16_t mavsdk::System::AutopilotVersion::vendor_id {0}
```


MAVLink autopilot_version vendor_id.


### product_id {#structmavsdk_1_1_system_1_1_autopilot_version_1a0c09156f2c75545f3895023fe22f80ec}

```cpp
uint16_t mavsdk::System::AutopilotVersion::product_id {0}
```


MAVLink autopilot_version product_id.


### uid2 {#structmavsdk_1_1_system_1_1_autopilot_version_1a7f0bd696e9200a6e3c802fd9b05adca9}

```cpp
std::array<uint8_t, 18> mavsdk::System::AutopilotVersion::uid2 {0}
```


MAVLink autopilot_version uid2.

