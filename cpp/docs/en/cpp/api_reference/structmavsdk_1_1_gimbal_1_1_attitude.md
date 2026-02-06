# mavsdk::Gimbal::Attitude Struct Reference
`#include: gimbal.h`

----


[Gimbal](classmavsdk_1_1_gimbal.md) attitude type. 


## Data Fields


int32_t [gimbal_id](#structmavsdk_1_1_gimbal_1_1_attitude_1afbcae93e94034d0e43ebb35a358cf098) {} - [Gimbal](classmavsdk_1_1_gimbal.md) ID.

[EulerAngle](structmavsdk_1_1_gimbal_1_1_euler_angle.md) [euler_angle_forward](#structmavsdk_1_1_gimbal_1_1_attitude_1af84d7adce79d0fb5c72151b00e6aaf4d) {} - Euler angle relative to forward.

[Quaternion](structmavsdk_1_1_gimbal_1_1_quaternion.md) [quaternion_forward](#structmavsdk_1_1_gimbal_1_1_attitude_1acd153d4d03bf2e780bf5073e49f9a1b3) {} - [Quaternion](structmavsdk_1_1_gimbal_1_1_quaternion.md) relative to forward.

[EulerAngle](structmavsdk_1_1_gimbal_1_1_euler_angle.md) [euler_angle_north](#structmavsdk_1_1_gimbal_1_1_attitude_1ade09fd2a0748b8fa310110f39e937365) {} - Euler angle relative to North.

[Quaternion](structmavsdk_1_1_gimbal_1_1_quaternion.md) [quaternion_north](#structmavsdk_1_1_gimbal_1_1_attitude_1a40ec2e7395692e824a855e5771c8a728) {} - [Quaternion](structmavsdk_1_1_gimbal_1_1_quaternion.md) relative to North.

[AngularVelocityBody](structmavsdk_1_1_gimbal_1_1_angular_velocity_body.md) [angular_velocity](#structmavsdk_1_1_gimbal_1_1_attitude_1a1746291596ff6b69dd9c28c2f37730f1) {} - The angular rate.

uint64_t [timestamp_us](#structmavsdk_1_1_gimbal_1_1_attitude_1a5c620e118ca06f8553918df38012677e) {} - Timestamp in microseconds.


## Field Documentation


### gimbal_id {#structmavsdk_1_1_gimbal_1_1_attitude_1afbcae93e94034d0e43ebb35a358cf098}

```cpp
int32_t mavsdk::Gimbal::Attitude::gimbal_id {}
```


[Gimbal](classmavsdk_1_1_gimbal.md) ID.


### euler_angle_forward {#structmavsdk_1_1_gimbal_1_1_attitude_1af84d7adce79d0fb5c72151b00e6aaf4d}

```cpp
EulerAngle mavsdk::Gimbal::Attitude::euler_angle_forward {}
```


Euler angle relative to forward.


### quaternion_forward {#structmavsdk_1_1_gimbal_1_1_attitude_1acd153d4d03bf2e780bf5073e49f9a1b3}

```cpp
Quaternion mavsdk::Gimbal::Attitude::quaternion_forward {}
```


[Quaternion](structmavsdk_1_1_gimbal_1_1_quaternion.md) relative to forward.


### euler_angle_north {#structmavsdk_1_1_gimbal_1_1_attitude_1ade09fd2a0748b8fa310110f39e937365}

```cpp
EulerAngle mavsdk::Gimbal::Attitude::euler_angle_north {}
```


Euler angle relative to North.


### quaternion_north {#structmavsdk_1_1_gimbal_1_1_attitude_1a40ec2e7395692e824a855e5771c8a728}

```cpp
Quaternion mavsdk::Gimbal::Attitude::quaternion_north {}
```


[Quaternion](structmavsdk_1_1_gimbal_1_1_quaternion.md) relative to North.


### angular_velocity {#structmavsdk_1_1_gimbal_1_1_attitude_1a1746291596ff6b69dd9c28c2f37730f1}

```cpp
AngularVelocityBody mavsdk::Gimbal::Attitude::angular_velocity {}
```


The angular rate.


### timestamp_us {#structmavsdk_1_1_gimbal_1_1_attitude_1a5c620e118ca06f8553918df38012677e}

```cpp
uint64_t mavsdk::Gimbal::Attitude::timestamp_us {}
```


Timestamp in microseconds.

