# mavsdk::Telemetry::Health Struct Reference
`#include: telemetry.h`

----


[Health](structmavsdk_1_1_telemetry_1_1_health.md) type. 


## Data Fields


bool [is_gyrometer_calibration_ok](#structmavsdk_1_1_telemetry_1_1_health_1a4ce524d8a5ca42e50cf615ddade4bac4) {false} - True if the gyrometer is calibrated.

bool [is_accelerometer_calibration_ok](#structmavsdk_1_1_telemetry_1_1_health_1a910417712b6960d371e79acdb5a19e35) { false} - True if the accelerometer is calibrated.

bool [is_magnetometer_calibration_ok](#structmavsdk_1_1_telemetry_1_1_health_1ac99ff7621f22a5148adfbb03004fccc8) { false} - True if the magnetometer is calibrated.

bool [is_local_position_ok](#structmavsdk_1_1_telemetry_1_1_health_1a86da005d0191a856b649d679e03b78b4) {false} - True if the local position estimate is good enough to fly in 'position control' mode.

bool [is_global_position_ok](#structmavsdk_1_1_telemetry_1_1_health_1ae85eb066716de1eef1a1bd3f6b375c6c) {false} - True if the global position estimate is good enough to fly in 'position control' mode.

bool [is_home_position_ok](#structmavsdk_1_1_telemetry_1_1_health_1ad67d5e7ef46fd34246011d31f95049fa) { false} - True if the home position has been initialized properly.

bool [is_armable](#structmavsdk_1_1_telemetry_1_1_health_1a7fbe53f1255bce1dcb30d4cd33c3d4f7) {false} - True if system can be armed.


## Field Documentation


### is_gyrometer_calibration_ok {#structmavsdk_1_1_telemetry_1_1_health_1a4ce524d8a5ca42e50cf615ddade4bac4}

```cpp
bool mavsdk::Telemetry::Health::is_gyrometer_calibration_ok {false}
```


True if the gyrometer is calibrated.


### is_accelerometer_calibration_ok {#structmavsdk_1_1_telemetry_1_1_health_1a910417712b6960d371e79acdb5a19e35}

```cpp
bool mavsdk::Telemetry::Health::is_accelerometer_calibration_ok { false}
```


True if the accelerometer is calibrated.


### is_magnetometer_calibration_ok {#structmavsdk_1_1_telemetry_1_1_health_1ac99ff7621f22a5148adfbb03004fccc8}

```cpp
bool mavsdk::Telemetry::Health::is_magnetometer_calibration_ok { false}
```


True if the magnetometer is calibrated.


### is_local_position_ok {#structmavsdk_1_1_telemetry_1_1_health_1a86da005d0191a856b649d679e03b78b4}

```cpp
bool mavsdk::Telemetry::Health::is_local_position_ok {false}
```


True if the local position estimate is good enough to fly in 'position control' mode.


### is_global_position_ok {#structmavsdk_1_1_telemetry_1_1_health_1ae85eb066716de1eef1a1bd3f6b375c6c}

```cpp
bool mavsdk::Telemetry::Health::is_global_position_ok {false}
```


True if the global position estimate is good enough to fly in 'position control' mode.


### is_home_position_ok {#structmavsdk_1_1_telemetry_1_1_health_1ad67d5e7ef46fd34246011d31f95049fa}

```cpp
bool mavsdk::Telemetry::Health::is_home_position_ok { false}
```


True if the home position has been initialized properly.


### is_armable {#structmavsdk_1_1_telemetry_1_1_health_1a7fbe53f1255bce1dcb30d4cd33c3d4f7}

```cpp
bool mavsdk::Telemetry::Health::is_armable {false}
```


True if system can be armed.

