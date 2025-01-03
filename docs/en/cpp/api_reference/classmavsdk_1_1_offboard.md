# mavsdk::Offboard Class Reference
`#include: offboard.h`

----


<ul>
<li><p>Control a drone with position, velocity, attitude or motor commands. </p>
</li>
</ul>


The module is called offboard because the commands can be sent from external sources as opposed to onboard control right inside the autopilot "board".


Client code must specify a setpoint before starting offboard mode. [Mavsdk](classmavsdk_1_1_mavsdk.md) automatically sends setpoints at 20Hz (PX4 [Offboard](classmavsdk_1_1_offboard.md) mode requires that setpoints are minimally sent at 2Hz). 


## Data Structures


struct [AccelerationNed](structmavsdk_1_1_offboard_1_1_acceleration_ned.md)

struct [ActuatorControl](structmavsdk_1_1_offboard_1_1_actuator_control.md)

struct [ActuatorControlGroup](structmavsdk_1_1_offboard_1_1_actuator_control_group.md)

struct [Attitude](structmavsdk_1_1_offboard_1_1_attitude.md)

struct [AttitudeRate](structmavsdk_1_1_offboard_1_1_attitude_rate.md)

struct [PositionGlobalYaw](structmavsdk_1_1_offboard_1_1_position_global_yaw.md)

struct [PositionNedYaw](structmavsdk_1_1_offboard_1_1_position_ned_yaw.md)

struct [VelocityBodyYawspeed](structmavsdk_1_1_offboard_1_1_velocity_body_yawspeed.md)

struct [VelocityNedYaw](structmavsdk_1_1_offboard_1_1_velocity_ned_yaw.md)

## Public Types


Type | Description
--- | ---
enum [Result](#classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9) | Possible results returned for offboard requests.
std::function< void([Result](classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9))> [ResultCallback](#classmavsdk_1_1_offboard_1a16d95f55251e9f992261d46da89ef8b9) | Callback type for asynchronous [Offboard](classmavsdk_1_1_offboard.md) calls.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [Offboard](#classmavsdk_1_1_offboard_1aedb8ed185acabd2caa3b536f51b68dcb) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [Offboard](#classmavsdk_1_1_offboard_1a9e11ce3a850ceb9a6c047288b5bc3b84) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~Offboard](#classmavsdk_1_1_offboard_1a1cdbf816ec02e63681eeee3ef6f5c41a) () override | Destructor (internal use only).
&nbsp; | [Offboard](#classmavsdk_1_1_offboard_1ab3674a3889b978e7a52626d5de6a6fa0) (const [Offboard](classmavsdk_1_1_offboard.md) & other) | Copy constructor.
void | [start_async](#classmavsdk_1_1_offboard_1a0c880ad3f663142e194dd6f187cfc934) (const [ResultCallback](classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a16d95f55251e9f992261d46da89ef8b9) callback) | Start offboard control.
[Result](classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9) | [start](#classmavsdk_1_1_offboard_1ab71d0dd2a81f76e3a0330b0304daa30b) () const | Start offboard control.
void | [stop_async](#classmavsdk_1_1_offboard_1a86c163d7fa1217b4e82a03daf52065c3) (const [ResultCallback](classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a16d95f55251e9f992261d46da89ef8b9) callback) | Stop offboard control.
[Result](classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9) | [stop](#classmavsdk_1_1_offboard_1a626810cbfa02b36019dde2d2fd4c3da9) () const | Stop offboard control.
bool | [is_active](#classmavsdk_1_1_offboard_1aa5e0f3c02a03f2667f82d5e162221ff5) () const | Check if offboard control is active.
[Result](classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9) | [set_attitude](#classmavsdk_1_1_offboard_1af6f0d3991bb6b62f39d862c46fcffb34) ([Attitude](structmavsdk_1_1_offboard_1_1_attitude.md) attitude)const | Set the attitude in terms of roll, pitch and yaw in degrees with thrust.
[Result](classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9) | [set_actuator_control](#classmavsdk_1_1_offboard_1a619ee02a1f73041a5cae6a5c72169b96) ([ActuatorControl](structmavsdk_1_1_offboard_1_1_actuator_control.md) actuator_control)const | Set direct actuator control values to groups #0 and #1.
[Result](classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9) | [set_attitude_rate](#classmavsdk_1_1_offboard_1a034a98a0e7a5a7a0fac507adabf6ecf8) ([AttitudeRate](structmavsdk_1_1_offboard_1_1_attitude_rate.md) attitude_rate)const | Set the attitude rate in terms of pitch, roll and yaw angular rate along with thrust.
[Result](classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9) | [set_position_ned](#classmavsdk_1_1_offboard_1ade7dcec93ebee17de60687a75184b8b6) ([PositionNedYaw](structmavsdk_1_1_offboard_1_1_position_ned_yaw.md) position_ned_yaw)const | Set the position in NED coordinates and yaw.
[Result](classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9) | [set_position_global](#classmavsdk_1_1_offboard_1a97b2bd8f516e267e5b28373f369ff8d3) ([PositionGlobalYaw](structmavsdk_1_1_offboard_1_1_position_global_yaw.md) position_global_yaw)const | Set the position in Global coordinates (latitude, longitude, altitude) and yaw.
[Result](classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9) | [set_velocity_body](#classmavsdk_1_1_offboard_1abe7364f0a48dda4df34c5c67d177cfb4) ([VelocityBodyYawspeed](structmavsdk_1_1_offboard_1_1_velocity_body_yawspeed.md) velocity_body_yawspeed)const | Set the velocity in body coordinates and yaw angular rate. Not available for fixed-wing aircraft.
[Result](classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9) | [set_velocity_ned](#classmavsdk_1_1_offboard_1a4edbc6e4528ff955d4e46e7c4e711732) ([VelocityNedYaw](structmavsdk_1_1_offboard_1_1_velocity_ned_yaw.md) velocity_ned_yaw)const | Set the velocity in NED coordinates and yaw. Not available for fixed-wing aircraft.
[Result](classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9) | [set_position_velocity_ned](#classmavsdk_1_1_offboard_1ae422165680b434eed74e84cc901e3a33) ([PositionNedYaw](structmavsdk_1_1_offboard_1_1_position_ned_yaw.md) position_ned_yaw, [VelocityNedYaw](structmavsdk_1_1_offboard_1_1_velocity_ned_yaw.md) velocity_ned_yaw)const | Set the position in NED coordinates, with the velocity to be used as feed-forward.
[Result](classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9) | [set_position_velocity_acceleration_ned](#classmavsdk_1_1_offboard_1a845aab746fc078d1ee2848df33c04eb9) ([PositionNedYaw](structmavsdk_1_1_offboard_1_1_position_ned_yaw.md) position_ned_yaw, [VelocityNedYaw](structmavsdk_1_1_offboard_1_1_velocity_ned_yaw.md) velocity_ned_yaw, [AccelerationNed](structmavsdk_1_1_offboard_1_1_acceleration_ned.md) acceleration_ned)const | Set the position, velocity and acceleration in NED coordinates, with velocity and acceleration used as feed-forward.
[Result](classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9) | [set_acceleration_ned](#classmavsdk_1_1_offboard_1ac0d471609df13c79a37e0e352be71d03) ([AccelerationNed](structmavsdk_1_1_offboard_1_1_acceleration_ned.md) acceleration_ned)const | Set the acceleration in NED coordinates.
const [Offboard](classmavsdk_1_1_offboard.md) & | [operator=](#classmavsdk_1_1_offboard_1acb01657624668251c0a022bc3f8135cd) (const [Offboard](classmavsdk_1_1_offboard.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### Offboard() {#classmavsdk_1_1_offboard_1aedb8ed185acabd2caa3b536f51b68dcb}
```cpp
mavsdk::Offboard::Offboard(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto offboard = Offboard(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### Offboard() {#classmavsdk_1_1_offboard_1a9e11ce3a850ceb9a6c047288b5bc3b84}
```cpp
mavsdk::Offboard::Offboard(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto offboard = Offboard(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~Offboard() {#classmavsdk_1_1_offboard_1a1cdbf816ec02e63681eeee3ef6f5c41a}
```cpp
mavsdk::Offboard::~Offboard() override
```


Destructor (internal use only).


### Offboard() {#classmavsdk_1_1_offboard_1ab3674a3889b978e7a52626d5de6a6fa0}
```cpp
mavsdk::Offboard::Offboard(const Offboard &other)
```


Copy constructor.


**Parameters**

* const [Offboard](classmavsdk_1_1_offboard.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_offboard_1a16d95f55251e9f992261d46da89ef8b9}

```cpp
using mavsdk::Offboard::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [Offboard](classmavsdk_1_1_offboard.md) calls.


## Member Enumeration Documentation


### enum Result {#classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9}


Possible results returned for offboard requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9a505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Request succeeded. 
<span id="classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9a1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system is connected. 
<span id="classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9a094a6f6b0868122a9dd008cb91c083e4"></span> `ConnectionError` | Connection error. 
<span id="classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9ad8a942ef2b04672adfafef0ad817a407"></span> `Busy` | Vehicle is busy. 
<span id="classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9a3398e12855176d55f43d53e04f472c8a"></span> `CommandDenied` | Command denied. 
<span id="classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9ac85a251cc457840f1e032f1b733e9398"></span> `Timeout` | Request timed out. 
<span id="classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9abe52cc8668acd0977040ce92d399c0bb"></span> `NoSetpointSet` | Cannot start without setpoint set. 
<span id="classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9ad7c8c85bf79bbe1b7188497c32c3b0ca"></span> `Failed` | Request failed. 

## Member Function Documentation


### start_async() {#classmavsdk_1_1_offboard_1a0c880ad3f663142e194dd6f187cfc934}
```cpp
void mavsdk::Offboard::start_async(const ResultCallback callback)
```


Start offboard control.

This function is non-blocking. See 'start' for the blocking counterpart.

**Parameters**

* const [ResultCallback](classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a16d95f55251e9f992261d46da89ef8b9) **callback** - 

### start() {#classmavsdk_1_1_offboard_1ab71d0dd2a81f76e3a0330b0304daa30b}
```cpp
Result mavsdk::Offboard::start() const
```


Start offboard control.

This function is blocking. See 'start_async' for the non-blocking counterpart.

**Returns**

&emsp;[Result](classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9) - Result of request.

### stop_async() {#classmavsdk_1_1_offboard_1a86c163d7fa1217b4e82a03daf52065c3}
```cpp
void mavsdk::Offboard::stop_async(const ResultCallback callback)
```


Stop offboard control.

The vehicle will be put into Hold mode: [https://docs.px4.io/en/flight_modes/hold.html](https://docs.px4.io/en/flight_modes/hold.html)


This function is non-blocking. See 'stop' for the blocking counterpart.

**Parameters**

* const [ResultCallback](classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a16d95f55251e9f992261d46da89ef8b9) **callback** - 

### stop() {#classmavsdk_1_1_offboard_1a626810cbfa02b36019dde2d2fd4c3da9}
```cpp
Result mavsdk::Offboard::stop() const
```


Stop offboard control.

The vehicle will be put into Hold mode: [https://docs.px4.io/en/flight_modes/hold.html](https://docs.px4.io/en/flight_modes/hold.html)


This function is blocking. See 'stop_async' for the non-blocking counterpart.

**Returns**

&emsp;[Result](classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9) - Result of request.

### is_active() {#classmavsdk_1_1_offboard_1aa5e0f3c02a03f2667f82d5e162221ff5}
```cpp
bool mavsdk::Offboard::is_active() const
```


Check if offboard control is active.

True means that the vehicle is in offboard mode and we are actively sending setpoints.


This function is blocking.

**Returns**

&emsp;bool - Result of request.

### set_attitude() {#classmavsdk_1_1_offboard_1af6f0d3991bb6b62f39d862c46fcffb34}
```cpp
Result mavsdk::Offboard::set_attitude(Attitude attitude) const
```


Set the attitude in terms of roll, pitch and yaw in degrees with thrust.

This function is blocking.

**Parameters**

* [Attitude](structmavsdk_1_1_offboard_1_1_attitude.md) **attitude** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9) - Result of request.

### set_actuator_control() {#classmavsdk_1_1_offboard_1a619ee02a1f73041a5cae6a5c72169b96}
```cpp
Result mavsdk::Offboard::set_actuator_control(ActuatorControl actuator_control) const
```


Set direct actuator control values to groups #0 and #1.

First 8 controls will go to control group 0, the following 8 controls to control group 1 (if actuator_control.num_controls more than 8).


This function is blocking.

**Parameters**

* [ActuatorControl](structmavsdk_1_1_offboard_1_1_actuator_control.md) **actuator_control** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9) - Result of request.

### set_attitude_rate() {#classmavsdk_1_1_offboard_1a034a98a0e7a5a7a0fac507adabf6ecf8}
```cpp
Result mavsdk::Offboard::set_attitude_rate(AttitudeRate attitude_rate) const
```


Set the attitude rate in terms of pitch, roll and yaw angular rate along with thrust.

This function is blocking.

**Parameters**

* [AttitudeRate](structmavsdk_1_1_offboard_1_1_attitude_rate.md) **attitude_rate** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9) - Result of request.

### set_position_ned() {#classmavsdk_1_1_offboard_1ade7dcec93ebee17de60687a75184b8b6}
```cpp
Result mavsdk::Offboard::set_position_ned(PositionNedYaw position_ned_yaw) const
```


Set the position in NED coordinates and yaw.

This function is blocking.

**Parameters**

* [PositionNedYaw](structmavsdk_1_1_offboard_1_1_position_ned_yaw.md) **position_ned_yaw** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9) - Result of request.

### set_position_global() {#classmavsdk_1_1_offboard_1a97b2bd8f516e267e5b28373f369ff8d3}
```cpp
Result mavsdk::Offboard::set_position_global(PositionGlobalYaw position_global_yaw) const
```


Set the position in Global coordinates (latitude, longitude, altitude) and yaw.

This function is blocking.

**Parameters**

* [PositionGlobalYaw](structmavsdk_1_1_offboard_1_1_position_global_yaw.md) **position_global_yaw** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9) - Result of request.

### set_velocity_body() {#classmavsdk_1_1_offboard_1abe7364f0a48dda4df34c5c67d177cfb4}
```cpp
Result mavsdk::Offboard::set_velocity_body(VelocityBodyYawspeed velocity_body_yawspeed) const
```


Set the velocity in body coordinates and yaw angular rate. Not available for fixed-wing aircraft.

This function is blocking.

**Parameters**

* [VelocityBodyYawspeed](structmavsdk_1_1_offboard_1_1_velocity_body_yawspeed.md) **velocity_body_yawspeed** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9) - Result of request.

### set_velocity_ned() {#classmavsdk_1_1_offboard_1a4edbc6e4528ff955d4e46e7c4e711732}
```cpp
Result mavsdk::Offboard::set_velocity_ned(VelocityNedYaw velocity_ned_yaw) const
```


Set the velocity in NED coordinates and yaw. Not available for fixed-wing aircraft.

This function is blocking.

**Parameters**

* [VelocityNedYaw](structmavsdk_1_1_offboard_1_1_velocity_ned_yaw.md) **velocity_ned_yaw** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9) - Result of request.

### set_position_velocity_ned() {#classmavsdk_1_1_offboard_1ae422165680b434eed74e84cc901e3a33}
```cpp
Result mavsdk::Offboard::set_position_velocity_ned(PositionNedYaw position_ned_yaw, VelocityNedYaw velocity_ned_yaw) const
```


Set the position in NED coordinates, with the velocity to be used as feed-forward.

This function is blocking.

**Parameters**

* [PositionNedYaw](structmavsdk_1_1_offboard_1_1_position_ned_yaw.md) **position_ned_yaw** - 
* [VelocityNedYaw](structmavsdk_1_1_offboard_1_1_velocity_ned_yaw.md) **velocity_ned_yaw** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9) - Result of request.

### set_position_velocity_acceleration_ned() {#classmavsdk_1_1_offboard_1a845aab746fc078d1ee2848df33c04eb9}
```cpp
Result mavsdk::Offboard::set_position_velocity_acceleration_ned(PositionNedYaw position_ned_yaw, VelocityNedYaw velocity_ned_yaw, AccelerationNed acceleration_ned) const
```


Set the position, velocity and acceleration in NED coordinates, with velocity and acceleration used as feed-forward.

This function is blocking.

**Parameters**

* [PositionNedYaw](structmavsdk_1_1_offboard_1_1_position_ned_yaw.md) **position_ned_yaw** - 
* [VelocityNedYaw](structmavsdk_1_1_offboard_1_1_velocity_ned_yaw.md) **velocity_ned_yaw** - 
* [AccelerationNed](structmavsdk_1_1_offboard_1_1_acceleration_ned.md) **acceleration_ned** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9) - Result of request.

### set_acceleration_ned() {#classmavsdk_1_1_offboard_1ac0d471609df13c79a37e0e352be71d03}
```cpp
Result mavsdk::Offboard::set_acceleration_ned(AccelerationNed acceleration_ned) const
```


Set the acceleration in NED coordinates.

This function is blocking.

**Parameters**

* [AccelerationNed](structmavsdk_1_1_offboard_1_1_acceleration_ned.md) **acceleration_ned** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_offboard.md#classmavsdk_1_1_offboard_1a2d4d594301d8c756429457b0982130e9) - Result of request.

### operator=() {#classmavsdk_1_1_offboard_1acb01657624668251c0a022bc3f8135cd}
```cpp
const Offboard& mavsdk::Offboard::operator=(const Offboard &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [Offboard](classmavsdk_1_1_offboard.md)&  - 

**Returns**

&emsp;const [Offboard](classmavsdk_1_1_offboard.md) & - 