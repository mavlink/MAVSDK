# mavsdk::TelemetryServer::Quaternion Struct Reference
`#include: telemetry_server.h`

----


[Quaternion](structmavsdk_1_1_telemetry_server_1_1_quaternion.md) type. 


All rotations and axis systems follow the right-hand rule. The Hamilton quaternion product definition is used. A zero-rotation quaternion is represented by (1,0,0,0). The quaternion could also be written as w + xi + yj + zk.


For more info see: [https://en.wikipedia.org/wiki/Quaternion](https://en.wikipedia.org/wiki/Quaternion) 


## Data Fields


float [w](#structmavsdk_1_1_telemetry_server_1_1_quaternion_1aa7094ef6e3dd0ea32db4e5361b2226d9) {float(NAN)} - [Quaternion](structmavsdk_1_1_telemetry_server_1_1_quaternion.md) entry 0, also denoted as a.

float [x](#structmavsdk_1_1_telemetry_server_1_1_quaternion_1ab9faee12c2c0a13364726b0ea9f302d6) {float(NAN)} - [Quaternion](structmavsdk_1_1_telemetry_server_1_1_quaternion.md) entry 1, also denoted as b.

float [y](#structmavsdk_1_1_telemetry_server_1_1_quaternion_1addbbde28f94587875d848cb7680f485c) {float(NAN)} - [Quaternion](structmavsdk_1_1_telemetry_server_1_1_quaternion.md) entry 2, also denoted as c.

float [z](#structmavsdk_1_1_telemetry_server_1_1_quaternion_1a393e8285beb9f254276f605cfb13db05) {float(NAN)} - [Quaternion](structmavsdk_1_1_telemetry_server_1_1_quaternion.md) entry 3, also denoted as d.

uint64_t [timestamp_us](#structmavsdk_1_1_telemetry_server_1_1_quaternion_1a24aab4c790bb8a861c489b6d14771c87) {} - Timestamp in microseconds.


## Field Documentation


### w {#structmavsdk_1_1_telemetry_server_1_1_quaternion_1aa7094ef6e3dd0ea32db4e5361b2226d9}

```cpp
float mavsdk::TelemetryServer::Quaternion::w {float(NAN)}
```


[Quaternion](structmavsdk_1_1_telemetry_server_1_1_quaternion.md) entry 0, also denoted as a.


### x {#structmavsdk_1_1_telemetry_server_1_1_quaternion_1ab9faee12c2c0a13364726b0ea9f302d6}

```cpp
float mavsdk::TelemetryServer::Quaternion::x {float(NAN)}
```


[Quaternion](structmavsdk_1_1_telemetry_server_1_1_quaternion.md) entry 1, also denoted as b.


### y {#structmavsdk_1_1_telemetry_server_1_1_quaternion_1addbbde28f94587875d848cb7680f485c}

```cpp
float mavsdk::TelemetryServer::Quaternion::y {float(NAN)}
```


[Quaternion](structmavsdk_1_1_telemetry_server_1_1_quaternion.md) entry 2, also denoted as c.


### z {#structmavsdk_1_1_telemetry_server_1_1_quaternion_1a393e8285beb9f254276f605cfb13db05}

```cpp
float mavsdk::TelemetryServer::Quaternion::z {float(NAN)}
```


[Quaternion](structmavsdk_1_1_telemetry_server_1_1_quaternion.md) entry 3, also denoted as d.


### timestamp_us {#structmavsdk_1_1_telemetry_server_1_1_quaternion_1a24aab4c790bb8a861c489b6d14771c87}

```cpp
uint64_t mavsdk::TelemetryServer::Quaternion::timestamp_us {}
```


Timestamp in microseconds.

