# mavsdk::Offboard::AttitudeRate Struct Reference
`#include: offboard.h`

----


Type for attitude rate commands in body coordinates (roll, pitch, yaw angular rate and thrust) 


## Data Fields


float [roll_deg_s](#structmavsdk_1_1_offboard_1_1_attitude_rate_1a1486686af45d621418c3a4d8cbe3509b) {} - Roll angular rate (in degrees/second, positive for clock-wise looking from front)

float [pitch_deg_s](#structmavsdk_1_1_offboard_1_1_attitude_rate_1a431a2639410ca4658266fbb1017a2689) {} - Pitch angular rate (in degrees/second, positive for head/front moving up)

float [yaw_deg_s](#structmavsdk_1_1_offboard_1_1_attitude_rate_1abac41def2fefa38cd37de6d55b5f0808) {} - Yaw angular rate (in degrees/second, positive for clock-wise looking from above)

float [thrust_value](#structmavsdk_1_1_offboard_1_1_attitude_rate_1af7e3ae65fb3739684ab88a533eba14e2) {} - Thrust (range: 0 to 1)


## Field Documentation


### roll_deg_s {#structmavsdk_1_1_offboard_1_1_attitude_rate_1a1486686af45d621418c3a4d8cbe3509b}

```cpp
float mavsdk::Offboard::AttitudeRate::roll_deg_s {}
```


Roll angular rate (in degrees/second, positive for clock-wise looking from front)


### pitch_deg_s {#structmavsdk_1_1_offboard_1_1_attitude_rate_1a431a2639410ca4658266fbb1017a2689}

```cpp
float mavsdk::Offboard::AttitudeRate::pitch_deg_s {}
```


Pitch angular rate (in degrees/second, positive for head/front moving up)


### yaw_deg_s {#structmavsdk_1_1_offboard_1_1_attitude_rate_1abac41def2fefa38cd37de6d55b5f0808}

```cpp
float mavsdk::Offboard::AttitudeRate::yaw_deg_s {}
```


Yaw angular rate (in degrees/second, positive for clock-wise looking from above)


### thrust_value {#structmavsdk_1_1_offboard_1_1_attitude_rate_1af7e3ae65fb3739684ab88a533eba14e2}

```cpp
float mavsdk::Offboard::AttitudeRate::thrust_value {}
```


Thrust (range: 0 to 1)

