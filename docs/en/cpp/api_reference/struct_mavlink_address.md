# MavlinkAddress Struct Reference
`#include: mavlink_address.hpp`

----


Struct to represent a MAVLink address. 


## Data Fields


uint32_t [system_id](#struct_mavlink_address_1a0013895a69562425639dd93b1ffc5ce3)  - System ID, also called sysid.

uint8_t [component_id](#struct_mavlink_address_1a666949f3b25592649b66a96bebf1d82b)  - Component ID, also called compid.


## Field Documentation


### system_id {#struct_mavlink_address_1a0013895a69562425639dd93b1ffc5ce3}

```cpp
uint32_t MavlinkAddress::system_id
```


System ID, also called sysid.

32 bits wide to accommodate MAVLink's extended system IDs. Values above 255 are only carried by peers that understand them.

### component_id {#struct_mavlink_address_1a666949f3b25592649b66a96bebf1d82b}

```cpp
uint8_t MavlinkAddress::component_id
```


Component ID, also called compid.

