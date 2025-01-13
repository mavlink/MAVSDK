# mavsdk::Failure Class Reference
`#include: failure.h`

----


Inject failures into system to test failsafes. 


## Public Types


Type | Description
--- | ---
enum [FailureUnit](#classmavsdk_1_1_failure_1a471f93c1fbff2124ebdea7fb681e23f1) | A failure unit.
enum [FailureType](#classmavsdk_1_1_failure_1ade0813be29826ae35e6692f506e8ab72) | A failure type.
enum [Result](#classmavsdk_1_1_failure_1a6dcdd665b49a7ddf48c76b41475022f1) | Possible results returned for failure requests.
std::function< void([Result](classmavsdk_1_1_failure.md#classmavsdk_1_1_failure_1a6dcdd665b49a7ddf48c76b41475022f1))> [ResultCallback](#classmavsdk_1_1_failure_1afe3469d737d8fe07a4325b85c79b62e6) | Callback type for asynchronous [Failure](classmavsdk_1_1_failure.md) calls.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [Failure](#classmavsdk_1_1_failure_1a7fd948954e053597be4223a5507e7f77) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [Failure](#classmavsdk_1_1_failure_1abb4693ce8ebfac246d489bbab438be04) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~Failure](#classmavsdk_1_1_failure_1a1358a71e00d96af5415236183c6508cd) () override | Destructor (internal use only).
&nbsp; | [Failure](#classmavsdk_1_1_failure_1ae12162366b96624fa85d345a7166749c) (const [Failure](classmavsdk_1_1_failure.md) & other) | Copy constructor.
[Result](classmavsdk_1_1_failure.md#classmavsdk_1_1_failure_1a6dcdd665b49a7ddf48c76b41475022f1) | [inject](#classmavsdk_1_1_failure_1ae937d22216ecae38f4c763f503cbbd88) ([FailureUnit](classmavsdk_1_1_failure.md#classmavsdk_1_1_failure_1a471f93c1fbff2124ebdea7fb681e23f1) failure_unit, [FailureType](classmavsdk_1_1_failure.md#classmavsdk_1_1_failure_1ade0813be29826ae35e6692f506e8ab72) failure_type, int32_t instance)const | Injects a failure.
const [Failure](classmavsdk_1_1_failure.md) & | [operator=](#classmavsdk_1_1_failure_1a255209f9df74ffde1f63b707f17bdb72) (const [Failure](classmavsdk_1_1_failure.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### Failure() {#classmavsdk_1_1_failure_1a7fd948954e053597be4223a5507e7f77}
```cpp
mavsdk::Failure::Failure(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto failure = Failure(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### Failure() {#classmavsdk_1_1_failure_1abb4693ce8ebfac246d489bbab438be04}
```cpp
mavsdk::Failure::Failure(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto failure = Failure(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~Failure() {#classmavsdk_1_1_failure_1a1358a71e00d96af5415236183c6508cd}
```cpp
mavsdk::Failure::~Failure() override
```


Destructor (internal use only).


### Failure() {#classmavsdk_1_1_failure_1ae12162366b96624fa85d345a7166749c}
```cpp
mavsdk::Failure::Failure(const Failure &other)
```


Copy constructor.


**Parameters**

* const [Failure](classmavsdk_1_1_failure.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_failure_1afe3469d737d8fe07a4325b85c79b62e6}

```cpp
using mavsdk::Failure::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [Failure](classmavsdk_1_1_failure.md) calls.


## Member Enumeration Documentation


### enum FailureUnit {#classmavsdk_1_1_failure_1a471f93c1fbff2124ebdea7fb681e23f1}


A failure unit.


Value | Description
--- | ---
<span id="classmavsdk_1_1_failure_1a471f93c1fbff2124ebdea7fb681e23f1a9448dda46d6fc2c491ac2a8efa2c70e9"></span> `SensorGyro` | Gyro. 
<span id="classmavsdk_1_1_failure_1a471f93c1fbff2124ebdea7fb681e23f1a56e5aa2721f8c41d27bfee2dfa9f033f"></span> `SensorAccel` | Accelerometer. 
<span id="classmavsdk_1_1_failure_1a471f93c1fbff2124ebdea7fb681e23f1a0b70fcf41daf7596d201eae9fb891e92"></span> `SensorMag` | Magnetometer. 
<span id="classmavsdk_1_1_failure_1a471f93c1fbff2124ebdea7fb681e23f1ae40672291a00488502c2327b4318cc2f"></span> `SensorBaro` | Barometer. 
<span id="classmavsdk_1_1_failure_1a471f93c1fbff2124ebdea7fb681e23f1af3b16483c0aaf29d61a20abd794b4bb7"></span> `SensorGps` | GPS. 
<span id="classmavsdk_1_1_failure_1a471f93c1fbff2124ebdea7fb681e23f1a9d34dac45709b8742e3e2e9515625317"></span> `SensorOpticalFlow` | Optical flow. 
<span id="classmavsdk_1_1_failure_1a471f93c1fbff2124ebdea7fb681e23f1a773b98493c19106ab80e769e3b34462d"></span> `SensorVio` | Visual inertial odometry. 
<span id="classmavsdk_1_1_failure_1a471f93c1fbff2124ebdea7fb681e23f1ab3fec9054f891711d849ac6ca5df27b4"></span> `SensorDistanceSensor` | Distance sensor. 
<span id="classmavsdk_1_1_failure_1a471f93c1fbff2124ebdea7fb681e23f1aab1a695e15197546a34dbd8ed2357805"></span> `SensorAirspeed` | Airspeed. 
<span id="classmavsdk_1_1_failure_1a471f93c1fbff2124ebdea7fb681e23f1ada1722a4e123339db45edad1c49ee81d"></span> `SystemBattery` | Battery. 
<span id="classmavsdk_1_1_failure_1a471f93c1fbff2124ebdea7fb681e23f1af5e88f49da7f44d9046f6e9c07ac9d10"></span> `SystemMotor` | Motor. 
<span id="classmavsdk_1_1_failure_1a471f93c1fbff2124ebdea7fb681e23f1a0815bcc72728a40813469c9c07d1ad10"></span> `SystemServo` | Servo. 
<span id="classmavsdk_1_1_failure_1a471f93c1fbff2124ebdea7fb681e23f1aeea9f8a53e93226933ce5b4f25e975bd"></span> `SystemAvoidance` | Avoidance. 
<span id="classmavsdk_1_1_failure_1a471f93c1fbff2124ebdea7fb681e23f1a0137c898117b9f8ef51c13af7cadd710"></span> `SystemRcSignal` | RC signal. 
<span id="classmavsdk_1_1_failure_1a471f93c1fbff2124ebdea7fb681e23f1a5971385898f49830581d69b301b5254d"></span> `SystemMavlinkSignal` | MAVLink signal. 

### enum FailureType {#classmavsdk_1_1_failure_1ade0813be29826ae35e6692f506e8ab72}


A failure type.


Value | Description
--- | ---
<span id="classmavsdk_1_1_failure_1ade0813be29826ae35e6692f506e8ab72aa60852f204ed8028c1c58808b746d115"></span> `Ok` | No failure injected, used to reset a previous failure. 
<span id="classmavsdk_1_1_failure_1ade0813be29826ae35e6692f506e8ab72ad15305d7a4e34e02489c74a5ef542f36"></span> `Off` | Sets unit off, so completely non-responsive. 
<span id="classmavsdk_1_1_failure_1ade0813be29826ae35e6692f506e8ab72aa2c4543906f921f4589e33086abcc102"></span> `Stuck` | Unit is stuck e.g. keeps reporting the same value. 
<span id="classmavsdk_1_1_failure_1ade0813be29826ae35e6692f506e8ab72a8c97626c3bda0721059f5eddbd2d255d"></span> `Garbage` | Unit is reporting complete garbage. 
<span id="classmavsdk_1_1_failure_1ade0813be29826ae35e6692f506e8ab72ab35e5a1e003084f6f4268ed1c8abceb5"></span> `Wrong` | Unit is consistently wrong. 
<span id="classmavsdk_1_1_failure_1ade0813be29826ae35e6692f506e8ab72aefa5397985b8609a5dbeb430a4bcadd3"></span> `Slow` | Unit is slow, so e.g. reporting at slower than expected rate. 
<span id="classmavsdk_1_1_failure_1ade0813be29826ae35e6692f506e8ab72a6e04a0730cfc0bca398610196b5f8467"></span> `Delayed` | Data of unit is delayed in time. 
<span id="classmavsdk_1_1_failure_1ade0813be29826ae35e6692f506e8ab72a9379b1b166d62d5c452a1893c079616e"></span> `Intermittent` | Unit is sometimes working, sometimes not. 

### enum Result {#classmavsdk_1_1_failure_1a6dcdd665b49a7ddf48c76b41475022f1}


Possible results returned for failure requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_failure_1a6dcdd665b49a7ddf48c76b41475022f1a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_failure_1a6dcdd665b49a7ddf48c76b41475022f1a505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Request succeeded. 
<span id="classmavsdk_1_1_failure_1a6dcdd665b49a7ddf48c76b41475022f1a1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system is connected. 
<span id="classmavsdk_1_1_failure_1a6dcdd665b49a7ddf48c76b41475022f1a094a6f6b0868122a9dd008cb91c083e4"></span> `ConnectionError` | Connection error. 
<span id="classmavsdk_1_1_failure_1a6dcdd665b49a7ddf48c76b41475022f1ab4080bdf74febf04d578ff105cce9d3f"></span> `Unsupported` | [Failure](classmavsdk_1_1_failure.md) not supported. 
<span id="classmavsdk_1_1_failure_1a6dcdd665b49a7ddf48c76b41475022f1a58d036b9b7f0e7eb38cfb90f1cc70a73"></span> `Denied` | [Failure](classmavsdk_1_1_failure.md) injection denied. 
<span id="classmavsdk_1_1_failure_1a6dcdd665b49a7ddf48c76b41475022f1ab9f5c797ebbf55adccdd8539a65a0241"></span> `Disabled` | [Failure](classmavsdk_1_1_failure.md) injection is disabled. 
<span id="classmavsdk_1_1_failure_1a6dcdd665b49a7ddf48c76b41475022f1ac85a251cc457840f1e032f1b733e9398"></span> `Timeout` | Request timed out. 

## Member Function Documentation


### inject() {#classmavsdk_1_1_failure_1ae937d22216ecae38f4c763f503cbbd88}
```cpp
Result mavsdk::Failure::inject(FailureUnit failure_unit, FailureType failure_type, int32_t instance) const
```


Injects a failure.

This function is blocking.

**Parameters**

* [FailureUnit](classmavsdk_1_1_failure.md#classmavsdk_1_1_failure_1a471f93c1fbff2124ebdea7fb681e23f1) **failure_unit** - 
* [FailureType](classmavsdk_1_1_failure.md#classmavsdk_1_1_failure_1ade0813be29826ae35e6692f506e8ab72) **failure_type** - 
* int32_t **instance** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_failure.md#classmavsdk_1_1_failure_1a6dcdd665b49a7ddf48c76b41475022f1) - Result of request.

### operator=() {#classmavsdk_1_1_failure_1a255209f9df74ffde1f63b707f17bdb72}
```cpp
const Failure& mavsdk::Failure::operator=(const Failure &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [Failure](classmavsdk_1_1_failure.md)&  - 

**Returns**

&emsp;const [Failure](classmavsdk_1_1_failure.md) & - 