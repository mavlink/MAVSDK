# mavsdk::Telemetry::Wind Struct Reference
`#include: telemetry.h`

----


[Wind](structmavsdk_1_1_telemetry_1_1_wind.md) message type. 


## Data Fields


float [wind_x_ned_m_s](#structmavsdk_1_1_telemetry_1_1_wind_1ad7fac56cc9f087ac0e0103ed9429e93d) {float(NAN)} - [Wind](structmavsdk_1_1_telemetry_1_1_wind.md) in North (NED) direction.

float [wind_y_ned_m_s](#structmavsdk_1_1_telemetry_1_1_wind_1a8276ae7d34b15d0f1b6c56b3ba84af62) {float(NAN)} - [Wind](structmavsdk_1_1_telemetry_1_1_wind.md) in East (NED) direction.

float [wind_z_ned_m_s](#structmavsdk_1_1_telemetry_1_1_wind_1a787a81d7ebc11bc1fc07542b8d017e9e) {float(NAN)} - [Wind](structmavsdk_1_1_telemetry_1_1_wind.md) in down (NED) direction.

float [horizontal_variability_stddev_m_s](#structmavsdk_1_1_telemetry_1_1_wind_1a2b5884a2b904f0f9f4ea3a1a612eca3a) { float(NAN)} - Variability of wind in XY, 1-STD estimated from a 1 Hz lowpassed wind estimate.

float [vertical_variability_stddev_m_s](#structmavsdk_1_1_telemetry_1_1_wind_1a2ddcff29c842878eb57d9f96b97e8e5f) { float(NAN)} - Variability of wind in Z, 1-STD estimated from a 1 Hz lowpassed wind estimate.

float [wind_altitude_msl_m](#structmavsdk_1_1_telemetry_1_1_wind_1ae249fb71b66b19e7684c719a1c30310e) { float(NAN)} - [Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md) (MSL) that this measurement was taken at.

float [horizontal_wind_speed_accuracy_m_s](#structmavsdk_1_1_telemetry_1_1_wind_1a37b1d942085c0a53ec41dc742cb14b89) { float(NAN)} - Horizontal speed 1-STD accuracy.

float [vertical_wind_speed_accuracy_m_s](#structmavsdk_1_1_telemetry_1_1_wind_1a06c326ddddbe4f71ee9a31d85b8f06e9) { float(NAN)} - Vertical speed 1-STD accuracy.


## Field Documentation


### wind_x_ned_m_s {#structmavsdk_1_1_telemetry_1_1_wind_1ad7fac56cc9f087ac0e0103ed9429e93d}

```cpp
float mavsdk::Telemetry::Wind::wind_x_ned_m_s {float(NAN)}
```


[Wind](structmavsdk_1_1_telemetry_1_1_wind.md) in North (NED) direction.


### wind_y_ned_m_s {#structmavsdk_1_1_telemetry_1_1_wind_1a8276ae7d34b15d0f1b6c56b3ba84af62}

```cpp
float mavsdk::Telemetry::Wind::wind_y_ned_m_s {float(NAN)}
```


[Wind](structmavsdk_1_1_telemetry_1_1_wind.md) in East (NED) direction.


### wind_z_ned_m_s {#structmavsdk_1_1_telemetry_1_1_wind_1a787a81d7ebc11bc1fc07542b8d017e9e}

```cpp
float mavsdk::Telemetry::Wind::wind_z_ned_m_s {float(NAN)}
```


[Wind](structmavsdk_1_1_telemetry_1_1_wind.md) in down (NED) direction.


### horizontal_variability_stddev_m_s {#structmavsdk_1_1_telemetry_1_1_wind_1a2b5884a2b904f0f9f4ea3a1a612eca3a}

```cpp
float mavsdk::Telemetry::Wind::horizontal_variability_stddev_m_s { float(NAN)}
```


Variability of wind in XY, 1-STD estimated from a 1 Hz lowpassed wind estimate.


### vertical_variability_stddev_m_s {#structmavsdk_1_1_telemetry_1_1_wind_1a2ddcff29c842878eb57d9f96b97e8e5f}

```cpp
float mavsdk::Telemetry::Wind::vertical_variability_stddev_m_s { float(NAN)}
```


Variability of wind in Z, 1-STD estimated from a 1 Hz lowpassed wind estimate.


### wind_altitude_msl_m {#structmavsdk_1_1_telemetry_1_1_wind_1ae249fb71b66b19e7684c719a1c30310e}

```cpp
float mavsdk::Telemetry::Wind::wind_altitude_msl_m { float(NAN)}
```


[Altitude](structmavsdk_1_1_telemetry_1_1_altitude.md) (MSL) that this measurement was taken at.


### horizontal_wind_speed_accuracy_m_s {#structmavsdk_1_1_telemetry_1_1_wind_1a37b1d942085c0a53ec41dc742cb14b89}

```cpp
float mavsdk::Telemetry::Wind::horizontal_wind_speed_accuracy_m_s { float(NAN)}
```


Horizontal speed 1-STD accuracy.


### vertical_wind_speed_accuracy_m_s {#structmavsdk_1_1_telemetry_1_1_wind_1a06c326ddddbe4f71ee9a31d85b8f06e9}

```cpp
float mavsdk::Telemetry::Wind::vertical_wind_speed_accuracy_m_s { float(NAN)}
```


Vertical speed 1-STD accuracy.

