# mavsdk::TelemetryServer::Imu Struct Reference
`#include: telemetry_server.h`

----


[Imu](structmavsdk_1_1_telemetry_server_1_1_imu.md) message type. 


## Data Fields


[AccelerationFrd](structmavsdk_1_1_telemetry_server_1_1_acceleration_frd.md) [acceleration_frd](#structmavsdk_1_1_telemetry_server_1_1_imu_1af7240dd075dee884831aef5fc0679ca4) {} - Acceleration.

[AngularVelocityFrd](structmavsdk_1_1_telemetry_server_1_1_angular_velocity_frd.md) [angular_velocity_frd](#structmavsdk_1_1_telemetry_server_1_1_imu_1a640c064f865730cef14bb0dc97392bbb) {} - Angular velocity.

[MagneticFieldFrd](structmavsdk_1_1_telemetry_server_1_1_magnetic_field_frd.md) [magnetic_field_frd](#structmavsdk_1_1_telemetry_server_1_1_imu_1aaf9025ebc65dbdb903c5bfe896a371df) {} - Magnetic field.

float [temperature_degc](#structmavsdk_1_1_telemetry_server_1_1_imu_1aa0c804fa763136c1d20d35eb770a4bde) {float(NAN)} - Temperature.

uint64_t [timestamp_us](#structmavsdk_1_1_telemetry_server_1_1_imu_1acc62eb7fb6e986a5facfd570dd2fd4f5) {} - Timestamp in microseconds.


## Field Documentation


### acceleration_frd {#structmavsdk_1_1_telemetry_server_1_1_imu_1af7240dd075dee884831aef5fc0679ca4}

```cpp
AccelerationFrd mavsdk::TelemetryServer::Imu::acceleration_frd {}
```


Acceleration.


### angular_velocity_frd {#structmavsdk_1_1_telemetry_server_1_1_imu_1a640c064f865730cef14bb0dc97392bbb}

```cpp
AngularVelocityFrd mavsdk::TelemetryServer::Imu::angular_velocity_frd {}
```


Angular velocity.


### magnetic_field_frd {#structmavsdk_1_1_telemetry_server_1_1_imu_1aaf9025ebc65dbdb903c5bfe896a371df}

```cpp
MagneticFieldFrd mavsdk::TelemetryServer::Imu::magnetic_field_frd {}
```


Magnetic field.


### temperature_degc {#structmavsdk_1_1_telemetry_server_1_1_imu_1aa0c804fa763136c1d20d35eb770a4bde}

```cpp
float mavsdk::TelemetryServer::Imu::temperature_degc {float(NAN)}
```


Temperature.


### timestamp_us {#structmavsdk_1_1_telemetry_server_1_1_imu_1acc62eb7fb6e986a5facfd570dd2fd4f5}

```cpp
uint64_t mavsdk::TelemetryServer::Imu::timestamp_us {}
```


Timestamp in microseconds.

