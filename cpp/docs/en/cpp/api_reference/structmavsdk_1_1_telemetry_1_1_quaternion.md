# mavsdk::Telemetry::Quaternion Struct Reference
`#include: telemetry.h`

----


[Quaternion](structmavsdk_1_1_telemetry_1_1_quaternion.md) type. 


All rotations and axis systems follow the right-hand rule. The Hamilton quaternion product definition is used. A zero-rotation quaternion is represented by (1,0,0,0). The quaternion could also be written as w + xi + yj + zk.


For more info see: [https://en.wikipedia.org/wiki/Quaternion](https://en.wikipedia.org/wiki/Quaternion) 


## Data Fields


float [w](#structmavsdk_1_1_telemetry_1_1_quaternion_1a0294a4f526fcdfbe0436dea3cdb10b53) {float(NAN)} - [Quaternion](structmavsdk_1_1_telemetry_1_1_quaternion.md) entry 0, also denoted as a.

float [x](#structmavsdk_1_1_telemetry_1_1_quaternion_1a66d67f70947be2578297fa2438c44900) {float(NAN)} - [Quaternion](structmavsdk_1_1_telemetry_1_1_quaternion.md) entry 1, also denoted as b.

float [y](#structmavsdk_1_1_telemetry_1_1_quaternion_1a774a00985baa458cf0da144b72c51dad) {float(NAN)} - [Quaternion](structmavsdk_1_1_telemetry_1_1_quaternion.md) entry 2, also denoted as c.

float [z](#structmavsdk_1_1_telemetry_1_1_quaternion_1a3888a29be1425239efc637edf7a265c4) {float(NAN)} - [Quaternion](structmavsdk_1_1_telemetry_1_1_quaternion.md) entry 3, also denoted as d.

uint64_t [timestamp_us](#structmavsdk_1_1_telemetry_1_1_quaternion_1a611fa203ff433802868dd42f834e08e4) {} - Timestamp in microseconds.


## Field Documentation


### w {#structmavsdk_1_1_telemetry_1_1_quaternion_1a0294a4f526fcdfbe0436dea3cdb10b53}

```cpp
float mavsdk::Telemetry::Quaternion::w {float(NAN)}
```


[Quaternion](structmavsdk_1_1_telemetry_1_1_quaternion.md) entry 0, also denoted as a.


### x {#structmavsdk_1_1_telemetry_1_1_quaternion_1a66d67f70947be2578297fa2438c44900}

```cpp
float mavsdk::Telemetry::Quaternion::x {float(NAN)}
```


[Quaternion](structmavsdk_1_1_telemetry_1_1_quaternion.md) entry 1, also denoted as b.


### y {#structmavsdk_1_1_telemetry_1_1_quaternion_1a774a00985baa458cf0da144b72c51dad}

```cpp
float mavsdk::Telemetry::Quaternion::y {float(NAN)}
```


[Quaternion](structmavsdk_1_1_telemetry_1_1_quaternion.md) entry 2, also denoted as c.


### z {#structmavsdk_1_1_telemetry_1_1_quaternion_1a3888a29be1425239efc637edf7a265c4}

```cpp
float mavsdk::Telemetry::Quaternion::z {float(NAN)}
```


[Quaternion](structmavsdk_1_1_telemetry_1_1_quaternion.md) entry 3, also denoted as d.


### timestamp_us {#structmavsdk_1_1_telemetry_1_1_quaternion_1a611fa203ff433802868dd42f834e08e4}

```cpp
uint64_t mavsdk::Telemetry::Quaternion::timestamp_us {}
```


Timestamp in microseconds.

