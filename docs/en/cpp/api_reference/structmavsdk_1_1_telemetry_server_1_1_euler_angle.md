# mavsdk::TelemetryServer::EulerAngle Struct Reference
`#include: telemetry_server.h`

----


Euler angle type. 


All rotations and axis systems follow the right-hand rule. The Euler angles follow the convention of a 3-2-1 intrinsic Tait-Bryan rotation sequence.


For more info see [https://en.wikipedia.org/wiki/Euler_angles](https://en.wikipedia.org/wiki/Euler_angles) 


## Data Fields


float [roll_deg](#structmavsdk_1_1_telemetry_server_1_1_euler_angle_1a333bf87adf280813e0ad18649fb8f281) { float(NAN)} - Roll angle in degrees, positive is banking to the right.

float [pitch_deg](#structmavsdk_1_1_telemetry_server_1_1_euler_angle_1adb4b88d942bdae150ade9b458d61f216) { float(NAN)} - Pitch angle in degrees, positive is pitching nose up.

float [yaw_deg](#structmavsdk_1_1_telemetry_server_1_1_euler_angle_1a9e29561b9cf2159c02de5056c52b9e1a) { float(NAN)} - Yaw angle in degrees, positive is clock-wise seen from above.

uint64_t [timestamp_us](#structmavsdk_1_1_telemetry_server_1_1_euler_angle_1ae6261c65520d7b4622760b0be78d458b) {} - Timestamp in microseconds.


## Field Documentation


### roll_deg {#structmavsdk_1_1_telemetry_server_1_1_euler_angle_1a333bf87adf280813e0ad18649fb8f281}

```cpp
float mavsdk::TelemetryServer::EulerAngle::roll_deg { float(NAN)}
```


Roll angle in degrees, positive is banking to the right.


### pitch_deg {#structmavsdk_1_1_telemetry_server_1_1_euler_angle_1adb4b88d942bdae150ade9b458d61f216}

```cpp
float mavsdk::TelemetryServer::EulerAngle::pitch_deg { float(NAN)}
```


Pitch angle in degrees, positive is pitching nose up.


### yaw_deg {#structmavsdk_1_1_telemetry_server_1_1_euler_angle_1a9e29561b9cf2159c02de5056c52b9e1a}

```cpp
float mavsdk::TelemetryServer::EulerAngle::yaw_deg { float(NAN)}
```


Yaw angle in degrees, positive is clock-wise seen from above.


### timestamp_us {#structmavsdk_1_1_telemetry_server_1_1_euler_angle_1ae6261c65520d7b4622760b0be78d458b}

```cpp
uint64_t mavsdk::TelemetryServer::EulerAngle::timestamp_us {}
```


Timestamp in microseconds.

