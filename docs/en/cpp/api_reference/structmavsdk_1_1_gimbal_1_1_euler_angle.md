# mavsdk::Gimbal::EulerAngle Struct Reference
`#include: gimbal.h`

----


Euler angle type. 


All rotations and axis systems follow the right-hand rule. The Euler angles are converted using the 3-1-2 sequence instead of standard 3-2-1 in order to avoid the gimbal lock at 90 degrees down.


For more info see [https://en.wikipedia.org/wiki/Euler_angles](https://en.wikipedia.org/wiki/Euler_angles) 


## Data Fields


float [roll_deg](#structmavsdk_1_1_gimbal_1_1_euler_angle_1a8597dbca1cc6c0f9595b8299827164c4) { float(NAN)} - Roll angle in degrees, positive is banking to the right.

float [pitch_deg](#structmavsdk_1_1_gimbal_1_1_euler_angle_1aafe2a84a6a9001962190b115ea6b94e4) { float(NAN)} - Pitch angle in degrees, positive is pitching nose up.

float [yaw_deg](#structmavsdk_1_1_gimbal_1_1_euler_angle_1ac09ef8ac782fbaa72812f53985ff312e) { float(NAN)} - Yaw angle in degrees, positive is clock-wise seen from above.


## Field Documentation


### roll_deg {#structmavsdk_1_1_gimbal_1_1_euler_angle_1a8597dbca1cc6c0f9595b8299827164c4}

```cpp
float mavsdk::Gimbal::EulerAngle::roll_deg { float(NAN)}
```


Roll angle in degrees, positive is banking to the right.


### pitch_deg {#structmavsdk_1_1_gimbal_1_1_euler_angle_1aafe2a84a6a9001962190b115ea6b94e4}

```cpp
float mavsdk::Gimbal::EulerAngle::pitch_deg { float(NAN)}
```


Pitch angle in degrees, positive is pitching nose up.


### yaw_deg {#structmavsdk_1_1_gimbal_1_1_euler_angle_1ac09ef8ac782fbaa72812f53985ff312e}

```cpp
float mavsdk::Gimbal::EulerAngle::yaw_deg { float(NAN)}
```


Yaw angle in degrees, positive is clock-wise seen from above.

