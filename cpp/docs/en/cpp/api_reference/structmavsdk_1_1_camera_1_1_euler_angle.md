# mavsdk::Camera::EulerAngle Struct Reference
`#include: camera.h`

----


Euler angle type. 


All rotations and axis systems follow the right-hand rule. The Euler angles follow the convention of a 3-2-1 intrinsic Tait-Bryan rotation sequence.


For more info see [https://en.wikipedia.org/wiki/Euler_angles](https://en.wikipedia.org/wiki/Euler_angles) 


## Data Fields


float [roll_deg](#structmavsdk_1_1_camera_1_1_euler_angle_1aa63fe9f7e9c4346686d554f37d47a178) {} - Roll angle in degrees, positive is banking to the right.

float [pitch_deg](#structmavsdk_1_1_camera_1_1_euler_angle_1a682e6baf3c8051afefec07d620b510f2) {} - Pitch angle in degrees, positive is pitching nose up.

float [yaw_deg](#structmavsdk_1_1_camera_1_1_euler_angle_1ad828494c6937d685ab33e02c04f529dd) {} - Yaw angle in degrees, positive is clock-wise seen from above.


## Field Documentation


### roll_deg {#structmavsdk_1_1_camera_1_1_euler_angle_1aa63fe9f7e9c4346686d554f37d47a178}

```cpp
float mavsdk::Camera::EulerAngle::roll_deg {}
```


Roll angle in degrees, positive is banking to the right.


### pitch_deg {#structmavsdk_1_1_camera_1_1_euler_angle_1a682e6baf3c8051afefec07d620b510f2}

```cpp
float mavsdk::Camera::EulerAngle::pitch_deg {}
```


Pitch angle in degrees, positive is pitching nose up.


### yaw_deg {#structmavsdk_1_1_camera_1_1_euler_angle_1ad828494c6937d685ab33e02c04f529dd}

```cpp
float mavsdk::Camera::EulerAngle::yaw_deg {}
```


Yaw angle in degrees, positive is clock-wise seen from above.

