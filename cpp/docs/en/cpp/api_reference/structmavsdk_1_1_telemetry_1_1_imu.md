# mavsdk::Telemetry::Imu Struct Reference
`#include: telemetry.h`

----


[Imu](structmavsdk_1_1_telemetry_1_1_imu.md) message type. 


## Data Fields


[AccelerationFrd](structmavsdk_1_1_telemetry_1_1_acceleration_frd.md) [acceleration_frd](#structmavsdk_1_1_telemetry_1_1_imu_1a600a740b55b0f5fd6fe24ab58e178e85) {} - Acceleration.

[AngularVelocityFrd](structmavsdk_1_1_telemetry_1_1_angular_velocity_frd.md) [angular_velocity_frd](#structmavsdk_1_1_telemetry_1_1_imu_1ace8ac6faa1404700c2771b7cd6befe5e) {} - Angular velocity.

[MagneticFieldFrd](structmavsdk_1_1_telemetry_1_1_magnetic_field_frd.md) [magnetic_field_frd](#structmavsdk_1_1_telemetry_1_1_imu_1a8b3ffac7ac05485441574cc4e59443dc) {} - Magnetic field.

float [temperature_degc](#structmavsdk_1_1_telemetry_1_1_imu_1a25504448c25602ab87cc773c28f58caa) {float(NAN)} - Temperature.

uint64_t [timestamp_us](#structmavsdk_1_1_telemetry_1_1_imu_1a3466ec3148942040add5d5bab2c69f56) {} - Timestamp in microseconds.


## Field Documentation


### acceleration_frd {#structmavsdk_1_1_telemetry_1_1_imu_1a600a740b55b0f5fd6fe24ab58e178e85}

```cpp
AccelerationFrd mavsdk::Telemetry::Imu::acceleration_frd {}
```


Acceleration.


### angular_velocity_frd {#structmavsdk_1_1_telemetry_1_1_imu_1ace8ac6faa1404700c2771b7cd6befe5e}

```cpp
AngularVelocityFrd mavsdk::Telemetry::Imu::angular_velocity_frd {}
```


Angular velocity.


### magnetic_field_frd {#structmavsdk_1_1_telemetry_1_1_imu_1a8b3ffac7ac05485441574cc4e59443dc}

```cpp
MagneticFieldFrd mavsdk::Telemetry::Imu::magnetic_field_frd {}
```


Magnetic field.


### temperature_degc {#structmavsdk_1_1_telemetry_1_1_imu_1a25504448c25602ab87cc773c28f58caa}

```cpp
float mavsdk::Telemetry::Imu::temperature_degc {float(NAN)}
```


Temperature.


### timestamp_us {#structmavsdk_1_1_telemetry_1_1_imu_1a3466ec3148942040add5d5bab2c69f56}

```cpp
uint64_t mavsdk::Telemetry::Imu::timestamp_us {}
```


Timestamp in microseconds.

