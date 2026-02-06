# mavsdk::Telemetry::EulerAngle Struct Reference
`#include: telemetry.h`

----


Euler angle type. 


All rotations and axis systems follow the right-hand rule. The Euler angles follow the convention of a 3-2-1 intrinsic Tait-Bryan rotation sequence.


For more info see [https://en.wikipedia.org/wiki/Euler_angles](https://en.wikipedia.org/wiki/Euler_angles) 


## Data Fields


float [roll_deg](#structmavsdk_1_1_telemetry_1_1_euler_angle_1a8278764834e15734ad51391b280bf179) { float(NAN)} - Roll angle in degrees, positive is banking to the right.

float [pitch_deg](#structmavsdk_1_1_telemetry_1_1_euler_angle_1ad1d95bc36e1993eba9636c7d2649e9a6) { float(NAN)} - Pitch angle in degrees, positive is pitching nose up.

float [yaw_deg](#structmavsdk_1_1_telemetry_1_1_euler_angle_1a1607da7e9063a1dd3f3a9a0624bc53fc) { float(NAN)} - Yaw angle in degrees, positive is clock-wise seen from above.

uint64_t [timestamp_us](#structmavsdk_1_1_telemetry_1_1_euler_angle_1a97a20080f84000960ad23f15cc9035e4) {} - Timestamp in microseconds.


## Field Documentation


### roll_deg {#structmavsdk_1_1_telemetry_1_1_euler_angle_1a8278764834e15734ad51391b280bf179}

```cpp
float mavsdk::Telemetry::EulerAngle::roll_deg { float(NAN)}
```


Roll angle in degrees, positive is banking to the right.


### pitch_deg {#structmavsdk_1_1_telemetry_1_1_euler_angle_1ad1d95bc36e1993eba9636c7d2649e9a6}

```cpp
float mavsdk::Telemetry::EulerAngle::pitch_deg { float(NAN)}
```


Pitch angle in degrees, positive is pitching nose up.


### yaw_deg {#structmavsdk_1_1_telemetry_1_1_euler_angle_1a1607da7e9063a1dd3f3a9a0624bc53fc}

```cpp
float mavsdk::Telemetry::EulerAngle::yaw_deg { float(NAN)}
```


Yaw angle in degrees, positive is clock-wise seen from above.


### timestamp_us {#structmavsdk_1_1_telemetry_1_1_euler_angle_1a97a20080f84000960ad23f15cc9035e4}

```cpp
uint64_t mavsdk::Telemetry::EulerAngle::timestamp_us {}
```


Timestamp in microseconds.

