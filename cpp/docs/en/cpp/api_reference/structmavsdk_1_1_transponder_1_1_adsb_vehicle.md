# mavsdk::Transponder::AdsbVehicle Struct Reference
`#include: transponder.h`

----


ADSB Vehicle type. 


## Data Fields


uint32_t [icao_address](#structmavsdk_1_1_transponder_1_1_adsb_vehicle_1adb4e2ab15053b2b73541dfc42bf6614c) {} - ICAO (International Civil Aviation Organization) unique worldwide identifier.

double [latitude_deg](#structmavsdk_1_1_transponder_1_1_adsb_vehicle_1aa538799571c3984a9875ea2dba66020c) {} - Latitude in degrees (range: -90 to +90)

double [longitude_deg](#structmavsdk_1_1_transponder_1_1_adsb_vehicle_1a7a4f84ff4e7b773b86c9518d555bda1e) {} - Longitude in degrees (range: -180 to +180).

[AdsbAltitudeType](classmavsdk_1_1_transponder.md#classmavsdk_1_1_transponder_1a64af76be070a2496e0db1c201ece9313) [altitude_type](#structmavsdk_1_1_transponder_1_1_adsb_vehicle_1a85f9833840e59e4db1a99fc9192b410a) {} - ADSB altitude type.

float [absolute_altitude_m](#structmavsdk_1_1_transponder_1_1_adsb_vehicle_1af33bf54b2bd961157a44781e4c6b48fa) {} - Altitude in metres according to altitude_type.

float [heading_deg](#structmavsdk_1_1_transponder_1_1_adsb_vehicle_1aaf318675876c499566760bfa69ee279c) {} - Course over ground, in degrees.

float [horizontal_velocity_m_s](#structmavsdk_1_1_transponder_1_1_adsb_vehicle_1ac5e37567200b40ba817dd6aafc660747) {} - The horizontal velocity in metres/second.

float [vertical_velocity_m_s](#structmavsdk_1_1_transponder_1_1_adsb_vehicle_1a88014a51ef34ac1fdd17c9598c9e1d66) {} - The vertical velocity in metres/second. Positive is up.

std::string [callsign](#structmavsdk_1_1_transponder_1_1_adsb_vehicle_1a4289413ecb9cbe0de24cc32b0335e54e) {} - The callsign.

[AdsbEmitterType](classmavsdk_1_1_transponder.md#classmavsdk_1_1_transponder_1a0b16f74131c28940c6604bc55776c039) [emitter_type](#structmavsdk_1_1_transponder_1_1_adsb_vehicle_1aa2d27a9dbd59ea92abbe970592c94b22) {} - ADSB emitter type.

uint32_t [squawk](#structmavsdk_1_1_transponder_1_1_adsb_vehicle_1a424c6cf5a90d9296f43f78d173b72c94) {} - Squawk code.

uint32_t [tslc_s](#structmavsdk_1_1_transponder_1_1_adsb_vehicle_1a61d1625effb91af73cc763b92c55425b) {} - Time Since Last Communication in seconds.


## Field Documentation


### icao_address {#structmavsdk_1_1_transponder_1_1_adsb_vehicle_1adb4e2ab15053b2b73541dfc42bf6614c}

```cpp
uint32_t mavsdk::Transponder::AdsbVehicle::icao_address {}
```


ICAO (International Civil Aviation Organization) unique worldwide identifier.


### latitude_deg {#structmavsdk_1_1_transponder_1_1_adsb_vehicle_1aa538799571c3984a9875ea2dba66020c}

```cpp
double mavsdk::Transponder::AdsbVehicle::latitude_deg {}
```


Latitude in degrees (range: -90 to +90)


### longitude_deg {#structmavsdk_1_1_transponder_1_1_adsb_vehicle_1a7a4f84ff4e7b773b86c9518d555bda1e}

```cpp
double mavsdk::Transponder::AdsbVehicle::longitude_deg {}
```


Longitude in degrees (range: -180 to +180).


### altitude_type {#structmavsdk_1_1_transponder_1_1_adsb_vehicle_1a85f9833840e59e4db1a99fc9192b410a}

```cpp
AdsbAltitudeType mavsdk::Transponder::AdsbVehicle::altitude_type {}
```


ADSB altitude type.


### absolute_altitude_m {#structmavsdk_1_1_transponder_1_1_adsb_vehicle_1af33bf54b2bd961157a44781e4c6b48fa}

```cpp
float mavsdk::Transponder::AdsbVehicle::absolute_altitude_m {}
```


Altitude in metres according to altitude_type.


### heading_deg {#structmavsdk_1_1_transponder_1_1_adsb_vehicle_1aaf318675876c499566760bfa69ee279c}

```cpp
float mavsdk::Transponder::AdsbVehicle::heading_deg {}
```


Course over ground, in degrees.


### horizontal_velocity_m_s {#structmavsdk_1_1_transponder_1_1_adsb_vehicle_1ac5e37567200b40ba817dd6aafc660747}

```cpp
float mavsdk::Transponder::AdsbVehicle::horizontal_velocity_m_s {}
```


The horizontal velocity in metres/second.


### vertical_velocity_m_s {#structmavsdk_1_1_transponder_1_1_adsb_vehicle_1a88014a51ef34ac1fdd17c9598c9e1d66}

```cpp
float mavsdk::Transponder::AdsbVehicle::vertical_velocity_m_s {}
```


The vertical velocity in metres/second. Positive is up.


### callsign {#structmavsdk_1_1_transponder_1_1_adsb_vehicle_1a4289413ecb9cbe0de24cc32b0335e54e}

```cpp
std::string mavsdk::Transponder::AdsbVehicle::callsign {}
```


The callsign.


### emitter_type {#structmavsdk_1_1_transponder_1_1_adsb_vehicle_1aa2d27a9dbd59ea92abbe970592c94b22}

```cpp
AdsbEmitterType mavsdk::Transponder::AdsbVehicle::emitter_type {}
```


ADSB emitter type.


### squawk {#structmavsdk_1_1_transponder_1_1_adsb_vehicle_1a424c6cf5a90d9296f43f78d173b72c94}

```cpp
uint32_t mavsdk::Transponder::AdsbVehicle::squawk {}
```


Squawk code.


### tslc_s {#structmavsdk_1_1_transponder_1_1_adsb_vehicle_1a61d1625effb91af73cc763b92c55425b}

```cpp
uint32_t mavsdk::Transponder::AdsbVehicle::tslc_s {}
```


Time Since Last Communication in seconds.

