# mavsdk::Calibration Class Reference
`#include: calibration.h`

----


Enable to calibrate sensors of a drone such as gyro, accelerometer, and magnetometer. 


## Data Structures


struct [ProgressData](structmavsdk_1_1_calibration_1_1_progress_data.md)

## Public Types


Type | Description
--- | ---
enum [Result](#classmavsdk_1_1_calibration_1a6e1ce7a3a07eb098edc06821d23a8ec1) | Possible results returned for calibration commands.
std::function< void([Result](classmavsdk_1_1_calibration.md#classmavsdk_1_1_calibration_1a6e1ce7a3a07eb098edc06821d23a8ec1))> [ResultCallback](#classmavsdk_1_1_calibration_1a566676d5803020cfda7a4a9d9f4a0161) | Callback type for asynchronous [Calibration](classmavsdk_1_1_calibration.md) calls.
std::function< void([Result](classmavsdk_1_1_calibration.md#classmavsdk_1_1_calibration_1a6e1ce7a3a07eb098edc06821d23a8ec1), [ProgressData](structmavsdk_1_1_calibration_1_1_progress_data.md))> [CalibrateGyroCallback](#classmavsdk_1_1_calibration_1a79bbabba5b60188b0afd8b3667c182b8) | Callback type for calibrate_gyro_async.
std::function< void([Result](classmavsdk_1_1_calibration.md#classmavsdk_1_1_calibration_1a6e1ce7a3a07eb098edc06821d23a8ec1), [ProgressData](structmavsdk_1_1_calibration_1_1_progress_data.md))> [CalibrateAccelerometerCallback](#classmavsdk_1_1_calibration_1a46f81dd310e70251240c619b5299d026) | Callback type for calibrate_accelerometer_async.
std::function< void([Result](classmavsdk_1_1_calibration.md#classmavsdk_1_1_calibration_1a6e1ce7a3a07eb098edc06821d23a8ec1), [ProgressData](structmavsdk_1_1_calibration_1_1_progress_data.md))> [CalibrateMagnetometerCallback](#classmavsdk_1_1_calibration_1a0b61d29092a8fc8407b0c0c13c825438) | Callback type for calibrate_magnetometer_async.
std::function< void([Result](classmavsdk_1_1_calibration.md#classmavsdk_1_1_calibration_1a6e1ce7a3a07eb098edc06821d23a8ec1), [ProgressData](structmavsdk_1_1_calibration_1_1_progress_data.md))> [CalibrateLevelHorizonCallback](#classmavsdk_1_1_calibration_1a397475f5df5fd0e1a181437d4e146aa4) | Callback type for calibrate_level_horizon_async.
std::function< void([Result](classmavsdk_1_1_calibration.md#classmavsdk_1_1_calibration_1a6e1ce7a3a07eb098edc06821d23a8ec1), [ProgressData](structmavsdk_1_1_calibration_1_1_progress_data.md))> [CalibrateGimbalAccelerometerCallback](#classmavsdk_1_1_calibration_1aa254e3a18042794e182fc7f3685aad01) | Callback type for calibrate_gimbal_accelerometer_async.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [Calibration](#classmavsdk_1_1_calibration_1ac86c794a9ca1043e21b501218346f2b1) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [Calibration](#classmavsdk_1_1_calibration_1a22b833023e256511495918cf9e78cb87) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~Calibration](#classmavsdk_1_1_calibration_1a10bc64254ae667f563484020ed5f7861) () override | Destructor (internal use only).
&nbsp; | [Calibration](#classmavsdk_1_1_calibration_1a90c300742cbb2e29594c294b2ca6ef0e) (const [Calibration](classmavsdk_1_1_calibration.md) & other) | Copy constructor.
void | [calibrate_gyro_async](#classmavsdk_1_1_calibration_1aa270d72a3dd0126b9c712f2467d105cb) (const [CalibrateGyroCallback](classmavsdk_1_1_calibration.md#classmavsdk_1_1_calibration_1a79bbabba5b60188b0afd8b3667c182b8) & callback) | Perform gyro calibration.
void | [calibrate_accelerometer_async](#classmavsdk_1_1_calibration_1a45fb6549dc486110359b532177c9bddc) (const [CalibrateAccelerometerCallback](classmavsdk_1_1_calibration.md#classmavsdk_1_1_calibration_1a46f81dd310e70251240c619b5299d026) & callback) | Perform accelerometer calibration.
void | [calibrate_magnetometer_async](#classmavsdk_1_1_calibration_1aa8a04e0f687a9d2aa7fe9a2346a233fa) (const [CalibrateMagnetometerCallback](classmavsdk_1_1_calibration.md#classmavsdk_1_1_calibration_1a0b61d29092a8fc8407b0c0c13c825438) & callback) | Perform magnetometer calibration.
void | [calibrate_level_horizon_async](#classmavsdk_1_1_calibration_1a77dd4e73da90801d63da973e1736628d) (const [CalibrateLevelHorizonCallback](classmavsdk_1_1_calibration.md#classmavsdk_1_1_calibration_1a397475f5df5fd0e1a181437d4e146aa4) & callback) | Perform board level horizon calibration.
void | [calibrate_gimbal_accelerometer_async](#classmavsdk_1_1_calibration_1a9b95e383527253c17cd0990653682cd6) (const [CalibrateGimbalAccelerometerCallback](classmavsdk_1_1_calibration.md#classmavsdk_1_1_calibration_1aa254e3a18042794e182fc7f3685aad01) & callback) | Perform gimbal accelerometer calibration.
[Result](classmavsdk_1_1_calibration.md#classmavsdk_1_1_calibration_1a6e1ce7a3a07eb098edc06821d23a8ec1) | [cancel](#classmavsdk_1_1_calibration_1ae26f45164d36576d56a186ee69e32ffb) () const | Cancel ongoing calibration process.
const [Calibration](classmavsdk_1_1_calibration.md) & | [operator=](#classmavsdk_1_1_calibration_1ada12d974bb516745ea67f94c72abf59b) (const [Calibration](classmavsdk_1_1_calibration.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### Calibration() {#classmavsdk_1_1_calibration_1ac86c794a9ca1043e21b501218346f2b1}
```cpp
mavsdk::Calibration::Calibration(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto calibration = Calibration(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### Calibration() {#classmavsdk_1_1_calibration_1a22b833023e256511495918cf9e78cb87}
```cpp
mavsdk::Calibration::Calibration(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto calibration = Calibration(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~Calibration() {#classmavsdk_1_1_calibration_1a10bc64254ae667f563484020ed5f7861}
```cpp
mavsdk::Calibration::~Calibration() override
```


Destructor (internal use only).


### Calibration() {#classmavsdk_1_1_calibration_1a90c300742cbb2e29594c294b2ca6ef0e}
```cpp
mavsdk::Calibration::Calibration(const Calibration &other)
```


Copy constructor.


**Parameters**

* const [Calibration](classmavsdk_1_1_calibration.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_calibration_1a566676d5803020cfda7a4a9d9f4a0161}

```cpp
using mavsdk::Calibration::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [Calibration](classmavsdk_1_1_calibration.md) calls.


### typedef CalibrateGyroCallback {#classmavsdk_1_1_calibration_1a79bbabba5b60188b0afd8b3667c182b8}

```cpp
using mavsdk::Calibration::CalibrateGyroCallback =  std::function<void(Result, ProgressData)>
```


Callback type for calibrate_gyro_async.


### typedef CalibrateAccelerometerCallback {#classmavsdk_1_1_calibration_1a46f81dd310e70251240c619b5299d026}

```cpp
using mavsdk::Calibration::CalibrateAccelerometerCallback =  std::function<void(Result, ProgressData)>
```


Callback type for calibrate_accelerometer_async.


### typedef CalibrateMagnetometerCallback {#classmavsdk_1_1_calibration_1a0b61d29092a8fc8407b0c0c13c825438}

```cpp
using mavsdk::Calibration::CalibrateMagnetometerCallback =  std::function<void(Result, ProgressData)>
```


Callback type for calibrate_magnetometer_async.


### typedef CalibrateLevelHorizonCallback {#classmavsdk_1_1_calibration_1a397475f5df5fd0e1a181437d4e146aa4}

```cpp
using mavsdk::Calibration::CalibrateLevelHorizonCallback =  std::function<void(Result, ProgressData)>
```


Callback type for calibrate_level_horizon_async.


### typedef CalibrateGimbalAccelerometerCallback {#classmavsdk_1_1_calibration_1aa254e3a18042794e182fc7f3685aad01}

```cpp
using mavsdk::Calibration::CalibrateGimbalAccelerometerCallback =  std::function<void(Result, ProgressData)>
```


Callback type for calibrate_gimbal_accelerometer_async.


## Member Enumeration Documentation


### enum Result {#classmavsdk_1_1_calibration_1a6e1ce7a3a07eb098edc06821d23a8ec1}


Possible results returned for calibration commands.


Value | Description
--- | ---
<span id="classmavsdk_1_1_calibration_1a6e1ce7a3a07eb098edc06821d23a8ec1a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_calibration_1a6e1ce7a3a07eb098edc06821d23a8ec1a505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | The calibration succeeded. 
<span id="classmavsdk_1_1_calibration_1a6e1ce7a3a07eb098edc06821d23a8ec1a10ac3d04253ef7e1ddc73e6091c0cd55"></span> `Next` | Intermediate message showing progress or instructions on the next steps. 
<span id="classmavsdk_1_1_calibration_1a6e1ce7a3a07eb098edc06821d23a8ec1ad7c8c85bf79bbe1b7188497c32c3b0ca"></span> `Failed` | [Calibration](classmavsdk_1_1_calibration.md) failed. 
<span id="classmavsdk_1_1_calibration_1a6e1ce7a3a07eb098edc06821d23a8ec1a1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system is connected. 
<span id="classmavsdk_1_1_calibration_1a6e1ce7a3a07eb098edc06821d23a8ec1a094a6f6b0868122a9dd008cb91c083e4"></span> `ConnectionError` | Connection error. 
<span id="classmavsdk_1_1_calibration_1a6e1ce7a3a07eb098edc06821d23a8ec1ad8a942ef2b04672adfafef0ad817a407"></span> `Busy` | Vehicle is busy. 
<span id="classmavsdk_1_1_calibration_1a6e1ce7a3a07eb098edc06821d23a8ec1a3398e12855176d55f43d53e04f472c8a"></span> `CommandDenied` | Command refused by vehicle. 
<span id="classmavsdk_1_1_calibration_1a6e1ce7a3a07eb098edc06821d23a8ec1ac85a251cc457840f1e032f1b733e9398"></span> `Timeout` | Command timed out. 
<span id="classmavsdk_1_1_calibration_1a6e1ce7a3a07eb098edc06821d23a8ec1aa149e85a44aeec9140e92733d9ed694e"></span> `Cancelled` | [Calibration](classmavsdk_1_1_calibration.md) process was cancelled. 
<span id="classmavsdk_1_1_calibration_1a6e1ce7a3a07eb098edc06821d23a8ec1a6d653ef81fcbd6909f17f5f91778b159"></span> `FailedArmed` | [Calibration](classmavsdk_1_1_calibration.md) process failed since the vehicle is armed. 
<span id="classmavsdk_1_1_calibration_1a6e1ce7a3a07eb098edc06821d23a8ec1ab4080bdf74febf04d578ff105cce9d3f"></span> `Unsupported` | Functionality not supported. 

## Member Function Documentation


### calibrate_gyro_async() {#classmavsdk_1_1_calibration_1aa270d72a3dd0126b9c712f2467d105cb}
```cpp
void mavsdk::Calibration::calibrate_gyro_async(const CalibrateGyroCallback &callback)
```


Perform gyro calibration.


**Parameters**

* const [CalibrateGyroCallback](classmavsdk_1_1_calibration.md#classmavsdk_1_1_calibration_1a79bbabba5b60188b0afd8b3667c182b8)& **callback** - 

### calibrate_accelerometer_async() {#classmavsdk_1_1_calibration_1a45fb6549dc486110359b532177c9bddc}
```cpp
void mavsdk::Calibration::calibrate_accelerometer_async(const CalibrateAccelerometerCallback &callback)
```


Perform accelerometer calibration.


**Parameters**

* const [CalibrateAccelerometerCallback](classmavsdk_1_1_calibration.md#classmavsdk_1_1_calibration_1a46f81dd310e70251240c619b5299d026)& **callback** - 

### calibrate_magnetometer_async() {#classmavsdk_1_1_calibration_1aa8a04e0f687a9d2aa7fe9a2346a233fa}
```cpp
void mavsdk::Calibration::calibrate_magnetometer_async(const CalibrateMagnetometerCallback &callback)
```


Perform magnetometer calibration.


**Parameters**

* const [CalibrateMagnetometerCallback](classmavsdk_1_1_calibration.md#classmavsdk_1_1_calibration_1a0b61d29092a8fc8407b0c0c13c825438)& **callback** - 

### calibrate_level_horizon_async() {#classmavsdk_1_1_calibration_1a77dd4e73da90801d63da973e1736628d}
```cpp
void mavsdk::Calibration::calibrate_level_horizon_async(const CalibrateLevelHorizonCallback &callback)
```


Perform board level horizon calibration.


**Parameters**

* const [CalibrateLevelHorizonCallback](classmavsdk_1_1_calibration.md#classmavsdk_1_1_calibration_1a397475f5df5fd0e1a181437d4e146aa4)& **callback** - 

### calibrate_gimbal_accelerometer_async() {#classmavsdk_1_1_calibration_1a9b95e383527253c17cd0990653682cd6}
```cpp
void mavsdk::Calibration::calibrate_gimbal_accelerometer_async(const CalibrateGimbalAccelerometerCallback &callback)
```


Perform gimbal accelerometer calibration.


**Parameters**

* const [CalibrateGimbalAccelerometerCallback](classmavsdk_1_1_calibration.md#classmavsdk_1_1_calibration_1aa254e3a18042794e182fc7f3685aad01)& **callback** - 

### cancel() {#classmavsdk_1_1_calibration_1ae26f45164d36576d56a186ee69e32ffb}
```cpp
Result mavsdk::Calibration::cancel() const
```


Cancel ongoing calibration process.

This function is blocking.

**Returns**

&emsp;[Result](classmavsdk_1_1_calibration.md#classmavsdk_1_1_calibration_1a6e1ce7a3a07eb098edc06821d23a8ec1) - Result of request.

### operator=() {#classmavsdk_1_1_calibration_1ada12d974bb516745ea67f94c72abf59b}
```cpp
const Calibration& mavsdk::Calibration::operator=(const Calibration &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [Calibration](classmavsdk_1_1_calibration.md)&  - 

**Returns**

&emsp;const [Calibration](classmavsdk_1_1_calibration.md) & - 