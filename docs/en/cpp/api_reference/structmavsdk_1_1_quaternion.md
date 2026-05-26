# mavsdk::Quaternion Struct Reference
`#include: math_utils.hpp`

----


[Quaternion](structmavsdk_1_1_quaternion.md) (Hamilton convention) representing a rotation. 


Components are ordered (w, x, y, z), where w is the scalar part. By MAVSDK convention this represents the rotation from local NED earth frame to body FRD frame. 


## Data Fields


float [w](#structmavsdk_1_1_quaternion_1a9d883fc07e96ebcdfd45c2ef28200049)  - Scalar component.

float [x](#structmavsdk_1_1_quaternion_1aa20aaf3f91854c3dc0a94be227e3aba2)  - Vector component along the x-axis.

float [y](#structmavsdk_1_1_quaternion_1a620720816c4b743b3e90a57baf0fbbe1)  - Vector component along the y-axis.

float [z](#structmavsdk_1_1_quaternion_1a155dd2dcb7708e0b829bb1950a009fe6)  - Vector component along the z-axis.


## Field Documentation


### w {#structmavsdk_1_1_quaternion_1a9d883fc07e96ebcdfd45c2ef28200049}

```cpp
float mavsdk::Quaternion::w
```


Scalar component.


### x {#structmavsdk_1_1_quaternion_1aa20aaf3f91854c3dc0a94be227e3aba2}

```cpp
float mavsdk::Quaternion::x
```


Vector component along the x-axis.


### y {#structmavsdk_1_1_quaternion_1a620720816c4b743b3e90a57baf0fbbe1}

```cpp
float mavsdk::Quaternion::y
```


Vector component along the y-axis.


### z {#structmavsdk_1_1_quaternion_1a155dd2dcb7708e0b829bb1950a009fe6}

```cpp
float mavsdk::Quaternion::z
```


Vector component along the z-axis.

