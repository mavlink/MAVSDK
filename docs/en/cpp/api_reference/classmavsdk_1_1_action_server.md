# mavsdk::ActionServer Class Reference
`#include: action_server.h`

----


Provide vehicle actions (as a server) such as arming, taking off, and landing. 


## Data Structures


struct [AllowableFlightModes](structmavsdk_1_1_action_server_1_1_allowable_flight_modes.md)

struct [ArmDisarm](structmavsdk_1_1_action_server_1_1_arm_disarm.md)

## Public Types


Type | Description
--- | ---
enum [FlightMode](#classmavsdk_1_1_action_server_1aee12027f5d9380f2c13fa7813c6ae1d8) | Flight modes.
enum [Result](#classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8) | Possible results returned for action requests.
std::function< void([Result](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8))> [ResultCallback](#classmavsdk_1_1_action_server_1affc7fcdc5f60e367bb9032b938223b95) | Callback type for asynchronous [ActionServer](classmavsdk_1_1_action_server.md) calls.
std::function< void([Result](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8), [ArmDisarm](structmavsdk_1_1_action_server_1_1_arm_disarm.md))> [ArmDisarmCallback](#classmavsdk_1_1_action_server_1a8ef1c6ec94f6bf5173d208be15b14910) | Callback type for subscribe_arm_disarm.
[Handle](classmavsdk_1_1_handle.md)< [Result](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8), [ArmDisarm](structmavsdk_1_1_action_server_1_1_arm_disarm.md) > [ArmDisarmHandle](#classmavsdk_1_1_action_server_1a5974c4b3e20da3418f9ca2b9de92157b) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_arm_disarm.
std::function< void([Result](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8), [FlightMode](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1aee12027f5d9380f2c13fa7813c6ae1d8))> [FlightModeChangeCallback](#classmavsdk_1_1_action_server_1a58a1284a240908bf4d9dd998d5939f6f) | Callback type for subscribe_flight_mode_change.
[Handle](classmavsdk_1_1_handle.md)< [Result](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8), [FlightMode](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1aee12027f5d9380f2c13fa7813c6ae1d8) > [FlightModeChangeHandle](#classmavsdk_1_1_action_server_1aa9746c14f9f1d65ee548ab97b433da2f) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_flight_mode_change.
std::function< void([Result](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8), bool)> [TakeoffCallback](#classmavsdk_1_1_action_server_1a23074d52d687eff2c4fc0184ac1b61fd) | Callback type for subscribe_takeoff.
[Handle](classmavsdk_1_1_handle.md)< [Result](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8), bool > [TakeoffHandle](#classmavsdk_1_1_action_server_1aea7eff252335602cd3785a6551c6ba64) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_takeoff.
std::function< void([Result](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8), bool)> [LandCallback](#classmavsdk_1_1_action_server_1af2eb6d32c9f903a6e9f773ddf2acf4a0) | Callback type for subscribe_land.
[Handle](classmavsdk_1_1_handle.md)< [Result](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8), bool > [LandHandle](#classmavsdk_1_1_action_server_1ae7c1a7d2a06cc045c9e0f29422f16278) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_land.
std::function< void([Result](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8), bool)> [RebootCallback](#classmavsdk_1_1_action_server_1a8b2ad3a5fc6a5ac256e21b60043093bd) | Callback type for subscribe_reboot.
[Handle](classmavsdk_1_1_handle.md)< [Result](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8), bool > [RebootHandle](#classmavsdk_1_1_action_server_1ab1544cd3b8ae97dcb37af0ff7320f29a) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_reboot.
std::function< void([Result](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8), bool)> [ShutdownCallback](#classmavsdk_1_1_action_server_1aaa5057d10ec2b242c4a35fec46e5602a) | Callback type for subscribe_shutdown.
[Handle](classmavsdk_1_1_handle.md)< [Result](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8), bool > [ShutdownHandle](#classmavsdk_1_1_action_server_1a7604a04e4ccf230a321593aaec209375) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_shutdown.
std::function< void([Result](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8), bool)> [TerminateCallback](#classmavsdk_1_1_action_server_1a5eb879b36b5efc4f14bed82ed7655384) | Callback type for subscribe_terminate.
[Handle](classmavsdk_1_1_handle.md)< [Result](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8), bool > [TerminateHandle](#classmavsdk_1_1_action_server_1a5ee91dbf03cfc2f19b0cdd3aee6e4214) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_terminate.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [ActionServer](#classmavsdk_1_1_action_server_1aa7a3da260716d5dc7857b26b40d45299) (std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > server_component) | Constructor. Creates the plugin for a [ServerComponent](classmavsdk_1_1_server_component.md) instance.
&nbsp; | [~ActionServer](#classmavsdk_1_1_action_server_1a1ddccf377314e9fffee6ab56edaf2b17) () override | Destructor (internal use only).
&nbsp; | [ActionServer](#classmavsdk_1_1_action_server_1a42228e948d6a50b42ac8e506dc9d094c) (const [ActionServer](classmavsdk_1_1_action_server.md) & other) | Copy constructor.
[ArmDisarmHandle](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a5974c4b3e20da3418f9ca2b9de92157b) | [subscribe_arm_disarm](#classmavsdk_1_1_action_server_1a898cccbbf0fbb8e50c2d7ec75dd12164) (const [ArmDisarmCallback](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a8ef1c6ec94f6bf5173d208be15b14910) & callback) | Subscribe to ARM/DISARM commands.
void | [unsubscribe_arm_disarm](#classmavsdk_1_1_action_server_1adef1a0b5964278a9188ffaf17d7cc1ed) ([ArmDisarmHandle](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a5974c4b3e20da3418f9ca2b9de92157b) handle) | Unsubscribe from subscribe_arm_disarm.
[FlightModeChangeHandle](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1aa9746c14f9f1d65ee548ab97b433da2f) | [subscribe_flight_mode_change](#classmavsdk_1_1_action_server_1a4f66d83144feb3657bc50316339f1a63) (const [FlightModeChangeCallback](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a58a1284a240908bf4d9dd998d5939f6f) & callback) | Subscribe to DO_SET_MODE.
void | [unsubscribe_flight_mode_change](#classmavsdk_1_1_action_server_1acafa3948ad27afc697926f267a5a92d4) ([FlightModeChangeHandle](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1aa9746c14f9f1d65ee548ab97b433da2f) handle) | Unsubscribe from subscribe_flight_mode_change.
[TakeoffHandle](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1aea7eff252335602cd3785a6551c6ba64) | [subscribe_takeoff](#classmavsdk_1_1_action_server_1ab289bc925bf87ecc67a16ae0f4356015) (const [TakeoffCallback](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a23074d52d687eff2c4fc0184ac1b61fd) & callback) | Subscribe to takeoff command.
void | [unsubscribe_takeoff](#classmavsdk_1_1_action_server_1ace083efcfa2323f3fa19bc65e373ff6f) ([TakeoffHandle](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1aea7eff252335602cd3785a6551c6ba64) handle) | Unsubscribe from subscribe_takeoff.
[LandHandle](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1ae7c1a7d2a06cc045c9e0f29422f16278) | [subscribe_land](#classmavsdk_1_1_action_server_1ac3d8a43058e281f49e2352dad9faf44a) (const [LandCallback](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1af2eb6d32c9f903a6e9f773ddf2acf4a0) & callback) | Subscribe to land command.
void | [unsubscribe_land](#classmavsdk_1_1_action_server_1aecda6dc238780a1ae269b4f60815acf0) ([LandHandle](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1ae7c1a7d2a06cc045c9e0f29422f16278) handle) | Unsubscribe from subscribe_land.
[RebootHandle](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1ab1544cd3b8ae97dcb37af0ff7320f29a) | [subscribe_reboot](#classmavsdk_1_1_action_server_1ae4ea60fe031763a284e596bb3b805d97) (const [RebootCallback](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a8b2ad3a5fc6a5ac256e21b60043093bd) & callback) | Subscribe to reboot command.
void | [unsubscribe_reboot](#classmavsdk_1_1_action_server_1a57fe5c92cb5448e6982ae6f62c13d2ba) ([RebootHandle](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1ab1544cd3b8ae97dcb37af0ff7320f29a) handle) | Unsubscribe from subscribe_reboot.
[ShutdownHandle](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a7604a04e4ccf230a321593aaec209375) | [subscribe_shutdown](#classmavsdk_1_1_action_server_1ad30e10cb343d65e52efa2bfb71a52abf) (const [ShutdownCallback](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1aaa5057d10ec2b242c4a35fec46e5602a) & callback) | Subscribe to shutdown command.
void | [unsubscribe_shutdown](#classmavsdk_1_1_action_server_1a87dfe8c81f23f5e183f15bdbaf2645d2) ([ShutdownHandle](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a7604a04e4ccf230a321593aaec209375) handle) | Unsubscribe from subscribe_shutdown.
[TerminateHandle](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a5ee91dbf03cfc2f19b0cdd3aee6e4214) | [subscribe_terminate](#classmavsdk_1_1_action_server_1ac332d0380be93c58208c8be626d6d797) (const [TerminateCallback](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a5eb879b36b5efc4f14bed82ed7655384) & callback) | Subscribe to terminate command.
void | [unsubscribe_terminate](#classmavsdk_1_1_action_server_1a3e236694f1f0beae030021b1b1d42633) ([TerminateHandle](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a5ee91dbf03cfc2f19b0cdd3aee6e4214) handle) | Unsubscribe from subscribe_terminate.
[Result](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8) | [set_allow_takeoff](#classmavsdk_1_1_action_server_1a250a8a8d1aed87d7c4125c6cfe797250) (bool allow_takeoff)const | Can the vehicle takeoff.
[Result](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8) | [set_armable](#classmavsdk_1_1_action_server_1ab5b21457f69f9f315d54dbc06e745a7a) (bool armable, bool force_armable)const | Can the vehicle arm when requested.
[Result](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8) | [set_disarmable](#classmavsdk_1_1_action_server_1afae1336100d7a91a4f4521cee56a1ecb) (bool disarmable, bool force_disarmable)const | Can the vehicle disarm when requested.
[Result](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8) | [set_allowable_flight_modes](#classmavsdk_1_1_action_server_1a3041d1b923a3b01021433ad43ab93b3a) ([AllowableFlightModes](structmavsdk_1_1_action_server_1_1_allowable_flight_modes.md) flight_modes)const | Set which modes the vehicle can transition to (Manual always allowed)
[ActionServer::AllowableFlightModes](structmavsdk_1_1_action_server_1_1_allowable_flight_modes.md) | [get_allowable_flight_modes](#classmavsdk_1_1_action_server_1a0960a6ec243823729a418a3c68feaf2a) () const | Get which modes the vehicle can transition to (Manual always allowed)
const [ActionServer](classmavsdk_1_1_action_server.md) & | [operator=](#classmavsdk_1_1_action_server_1a86d8bb24723ad8222669aec1f5c523ca) (const [ActionServer](classmavsdk_1_1_action_server.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### ActionServer() {#classmavsdk_1_1_action_server_1aa7a3da260716d5dc7857b26b40d45299}
```cpp
mavsdk::ActionServer::ActionServer(std::shared_ptr< ServerComponent > server_component)
```


Constructor. Creates the plugin for a [ServerComponent](classmavsdk_1_1_server_component.md) instance.

The plugin is typically created as shown below: 

```cpp
auto action_server = ActionServer(server_component);
```

**Parameters**

* std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > **server_component** - The [ServerComponent](classmavsdk_1_1_server_component.md) instance associated with this server plugin.

### ~ActionServer() {#classmavsdk_1_1_action_server_1a1ddccf377314e9fffee6ab56edaf2b17}
```cpp
mavsdk::ActionServer::~ActionServer() override
```


Destructor (internal use only).


### ActionServer() {#classmavsdk_1_1_action_server_1a42228e948d6a50b42ac8e506dc9d094c}
```cpp
mavsdk::ActionServer::ActionServer(const ActionServer &other)
```


Copy constructor.


**Parameters**

* const [ActionServer](classmavsdk_1_1_action_server.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_action_server_1affc7fcdc5f60e367bb9032b938223b95}

```cpp
using mavsdk::ActionServer::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [ActionServer](classmavsdk_1_1_action_server.md) calls.


### typedef ArmDisarmCallback {#classmavsdk_1_1_action_server_1a8ef1c6ec94f6bf5173d208be15b14910}

```cpp
using mavsdk::ActionServer::ArmDisarmCallback =  std::function<void(Result, ArmDisarm)>
```


Callback type for subscribe_arm_disarm.


### typedef ArmDisarmHandle {#classmavsdk_1_1_action_server_1a5974c4b3e20da3418f9ca2b9de92157b}

```cpp
using mavsdk::ActionServer::ArmDisarmHandle =  Handle<Result, ArmDisarm>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_arm_disarm.


### typedef FlightModeChangeCallback {#classmavsdk_1_1_action_server_1a58a1284a240908bf4d9dd998d5939f6f}

```cpp
using mavsdk::ActionServer::FlightModeChangeCallback =  std::function<void(Result, FlightMode)>
```


Callback type for subscribe_flight_mode_change.


### typedef FlightModeChangeHandle {#classmavsdk_1_1_action_server_1aa9746c14f9f1d65ee548ab97b433da2f}

```cpp
using mavsdk::ActionServer::FlightModeChangeHandle =  Handle<Result, FlightMode>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_flight_mode_change.


### typedef TakeoffCallback {#classmavsdk_1_1_action_server_1a23074d52d687eff2c4fc0184ac1b61fd}

```cpp
using mavsdk::ActionServer::TakeoffCallback =  std::function<void(Result, bool)>
```


Callback type for subscribe_takeoff.


### typedef TakeoffHandle {#classmavsdk_1_1_action_server_1aea7eff252335602cd3785a6551c6ba64}

```cpp
using mavsdk::ActionServer::TakeoffHandle =  Handle<Result, bool>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_takeoff.


### typedef LandCallback {#classmavsdk_1_1_action_server_1af2eb6d32c9f903a6e9f773ddf2acf4a0}

```cpp
using mavsdk::ActionServer::LandCallback =  std::function<void(Result, bool)>
```


Callback type for subscribe_land.


### typedef LandHandle {#classmavsdk_1_1_action_server_1ae7c1a7d2a06cc045c9e0f29422f16278}

```cpp
using mavsdk::ActionServer::LandHandle =  Handle<Result, bool>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_land.


### typedef RebootCallback {#classmavsdk_1_1_action_server_1a8b2ad3a5fc6a5ac256e21b60043093bd}

```cpp
using mavsdk::ActionServer::RebootCallback =  std::function<void(Result, bool)>
```


Callback type for subscribe_reboot.


### typedef RebootHandle {#classmavsdk_1_1_action_server_1ab1544cd3b8ae97dcb37af0ff7320f29a}

```cpp
using mavsdk::ActionServer::RebootHandle =  Handle<Result, bool>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_reboot.


### typedef ShutdownCallback {#classmavsdk_1_1_action_server_1aaa5057d10ec2b242c4a35fec46e5602a}

```cpp
using mavsdk::ActionServer::ShutdownCallback =  std::function<void(Result, bool)>
```


Callback type for subscribe_shutdown.


### typedef ShutdownHandle {#classmavsdk_1_1_action_server_1a7604a04e4ccf230a321593aaec209375}

```cpp
using mavsdk::ActionServer::ShutdownHandle =  Handle<Result, bool>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_shutdown.


### typedef TerminateCallback {#classmavsdk_1_1_action_server_1a5eb879b36b5efc4f14bed82ed7655384}

```cpp
using mavsdk::ActionServer::TerminateCallback =  std::function<void(Result, bool)>
```


Callback type for subscribe_terminate.


### typedef TerminateHandle {#classmavsdk_1_1_action_server_1a5ee91dbf03cfc2f19b0cdd3aee6e4214}

```cpp
using mavsdk::ActionServer::TerminateHandle =  Handle<Result, bool>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_terminate.


## Member Enumeration Documentation


### enum FlightMode {#classmavsdk_1_1_action_server_1aee12027f5d9380f2c13fa7813c6ae1d8}


Flight modes.

For more information about flight modes, check out [https://docs.px4.io/master/en/config/flight_mode.html](https://docs.px4.io/master/en/config/flight_mode.html).

Value | Description
--- | ---
<span id="classmavsdk_1_1_action_server_1aee12027f5d9380f2c13fa7813c6ae1d8a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Mode not known. 
<span id="classmavsdk_1_1_action_server_1aee12027f5d9380f2c13fa7813c6ae1d8ae7d31fc0602fb2ede144d18cdffd816b"></span> `Ready` | Armed and ready to take off. 
<span id="classmavsdk_1_1_action_server_1aee12027f5d9380f2c13fa7813c6ae1d8a56373a80447c41b9a29e500e62d6884e"></span> `Takeoff` | Taking off. 
<span id="classmavsdk_1_1_action_server_1aee12027f5d9380f2c13fa7813c6ae1d8abcd8db575b47c838e5d551e3973db4ac"></span> `Hold` | Holding (hovering in place (or circling for fixed-wing vehicles). 
<span id="classmavsdk_1_1_action_server_1aee12027f5d9380f2c13fa7813c6ae1d8a70d529695c253d17e992cb9265abc57f"></span> `Mission` | In mission. 
<span id="classmavsdk_1_1_action_server_1aee12027f5d9380f2c13fa7813c6ae1d8a146115c3278581584dcbaac1a77a2588"></span> `ReturnToLaunch` | Returning to launch position (then landing). 
<span id="classmavsdk_1_1_action_server_1aee12027f5d9380f2c13fa7813c6ae1d8a512ef7c688a2c8572d5e16f44e17e869"></span> `Land` | Landing. 
<span id="classmavsdk_1_1_action_server_1aee12027f5d9380f2c13fa7813c6ae1d8abb085f1e0d3843dda2a4c70437ad1410"></span> `Offboard` | In 'offboard' mode. 
<span id="classmavsdk_1_1_action_server_1aee12027f5d9380f2c13fa7813c6ae1d8a08bb3de7dafcf47f05b8c5a9dc0983c0"></span> `FollowMe` | In 'follow-me' mode. 
<span id="classmavsdk_1_1_action_server_1aee12027f5d9380f2c13fa7813c6ae1d8ae1ba155a9f2e8c3be94020eef32a0301"></span> `Manual` | In 'Manual' mode. 
<span id="classmavsdk_1_1_action_server_1aee12027f5d9380f2c13fa7813c6ae1d8aa7a697581399b9be37a545416d4cd2d9"></span> `Altctl` | In 'Altitude Control' mode. 
<span id="classmavsdk_1_1_action_server_1aee12027f5d9380f2c13fa7813c6ae1d8a46780e4f8c113481c868da4dd16fcd41"></span> `Posctl` | In 'Position Control' mode. 
<span id="classmavsdk_1_1_action_server_1aee12027f5d9380f2c13fa7813c6ae1d8ae10e0f017fee32a9bb2fa9fae53afd70"></span> `Acro` | In 'Acro' mode. 
<span id="classmavsdk_1_1_action_server_1aee12027f5d9380f2c13fa7813c6ae1d8afda22026db89cdc5e88b262ad9424b41"></span> `Stabilized` | In 'Stabilize' mode. 

### enum Result {#classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8}


Possible results returned for action requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8a505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Request was successful. 
<span id="classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8a1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system is connected. 
<span id="classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8a094a6f6b0868122a9dd008cb91c083e4"></span> `ConnectionError` | Connection error. 
<span id="classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8ad8a942ef2b04672adfafef0ad817a407"></span> `Busy` | Vehicle is busy. 
<span id="classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8a3398e12855176d55f43d53e04f472c8a"></span> `CommandDenied` | Command refused by vehicle. 
<span id="classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8a2dfd6d9491cefa8820ce47f77471e5e3"></span> `CommandDeniedLandedStateUnknown` | Command refused because landed state is unknown. 
<span id="classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8a73d9a6e3b34d98fa3c4fac08f3434a9a"></span> `CommandDeniedNotLanded` | Command refused because vehicle not landed. 
<span id="classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8ac85a251cc457840f1e032f1b733e9398"></span> `Timeout` | Request timed out. 
<span id="classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8a7006c2f81a7a1a65a23cfc16b0326336"></span> `VtolTransitionSupportUnknown` | Hybrid/VTOL transition support is unknown. 
<span id="classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8a5039f02d708e3874858b22610ed63d1e"></span> `NoVtolTransitionSupport` | Vehicle does not support hybrid/VTOL transitions. 
<span id="classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8a189976b733e988a6903e4d19d8cd2fea"></span> `ParameterError` | Error getting or setting parameter. 
<span id="classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8a10ac3d04253ef7e1ddc73e6091c0cd55"></span> `Next` | Intermediate message showing progress or instructions on the next steps. 

## Member Function Documentation


### subscribe_arm_disarm() {#classmavsdk_1_1_action_server_1a898cccbbf0fbb8e50c2d7ec75dd12164}
```cpp
ArmDisarmHandle mavsdk::ActionServer::subscribe_arm_disarm(const ArmDisarmCallback &callback)
```


Subscribe to ARM/DISARM commands.


**Parameters**

* const [ArmDisarmCallback](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a8ef1c6ec94f6bf5173d208be15b14910)& **callback** - 

**Returns**

&emsp;[ArmDisarmHandle](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a5974c4b3e20da3418f9ca2b9de92157b) - 

### unsubscribe_arm_disarm() {#classmavsdk_1_1_action_server_1adef1a0b5964278a9188ffaf17d7cc1ed}
```cpp
void mavsdk::ActionServer::unsubscribe_arm_disarm(ArmDisarmHandle handle)
```


Unsubscribe from subscribe_arm_disarm.


**Parameters**

* [ArmDisarmHandle](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a5974c4b3e20da3418f9ca2b9de92157b) **handle** - 

### subscribe_flight_mode_change() {#classmavsdk_1_1_action_server_1a4f66d83144feb3657bc50316339f1a63}
```cpp
FlightModeChangeHandle mavsdk::ActionServer::subscribe_flight_mode_change(const FlightModeChangeCallback &callback)
```


Subscribe to DO_SET_MODE.


**Parameters**

* const [FlightModeChangeCallback](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a58a1284a240908bf4d9dd998d5939f6f)& **callback** - 

**Returns**

&emsp;[FlightModeChangeHandle](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1aa9746c14f9f1d65ee548ab97b433da2f) - 

### unsubscribe_flight_mode_change() {#classmavsdk_1_1_action_server_1acafa3948ad27afc697926f267a5a92d4}
```cpp
void mavsdk::ActionServer::unsubscribe_flight_mode_change(FlightModeChangeHandle handle)
```


Unsubscribe from subscribe_flight_mode_change.


**Parameters**

* [FlightModeChangeHandle](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1aa9746c14f9f1d65ee548ab97b433da2f) **handle** - 

### subscribe_takeoff() {#classmavsdk_1_1_action_server_1ab289bc925bf87ecc67a16ae0f4356015}
```cpp
TakeoffHandle mavsdk::ActionServer::subscribe_takeoff(const TakeoffCallback &callback)
```


Subscribe to takeoff command.


**Parameters**

* const [TakeoffCallback](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a23074d52d687eff2c4fc0184ac1b61fd)& **callback** - 

**Returns**

&emsp;[TakeoffHandle](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1aea7eff252335602cd3785a6551c6ba64) - 

### unsubscribe_takeoff() {#classmavsdk_1_1_action_server_1ace083efcfa2323f3fa19bc65e373ff6f}
```cpp
void mavsdk::ActionServer::unsubscribe_takeoff(TakeoffHandle handle)
```


Unsubscribe from subscribe_takeoff.


**Parameters**

* [TakeoffHandle](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1aea7eff252335602cd3785a6551c6ba64) **handle** - 

### subscribe_land() {#classmavsdk_1_1_action_server_1ac3d8a43058e281f49e2352dad9faf44a}
```cpp
LandHandle mavsdk::ActionServer::subscribe_land(const LandCallback &callback)
```


Subscribe to land command.


**Parameters**

* const [LandCallback](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1af2eb6d32c9f903a6e9f773ddf2acf4a0)& **callback** - 

**Returns**

&emsp;[LandHandle](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1ae7c1a7d2a06cc045c9e0f29422f16278) - 

### unsubscribe_land() {#classmavsdk_1_1_action_server_1aecda6dc238780a1ae269b4f60815acf0}
```cpp
void mavsdk::ActionServer::unsubscribe_land(LandHandle handle)
```


Unsubscribe from subscribe_land.


**Parameters**

* [LandHandle](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1ae7c1a7d2a06cc045c9e0f29422f16278) **handle** - 

### subscribe_reboot() {#classmavsdk_1_1_action_server_1ae4ea60fe031763a284e596bb3b805d97}
```cpp
RebootHandle mavsdk::ActionServer::subscribe_reboot(const RebootCallback &callback)
```


Subscribe to reboot command.


**Parameters**

* const [RebootCallback](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a8b2ad3a5fc6a5ac256e21b60043093bd)& **callback** - 

**Returns**

&emsp;[RebootHandle](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1ab1544cd3b8ae97dcb37af0ff7320f29a) - 

### unsubscribe_reboot() {#classmavsdk_1_1_action_server_1a57fe5c92cb5448e6982ae6f62c13d2ba}
```cpp
void mavsdk::ActionServer::unsubscribe_reboot(RebootHandle handle)
```


Unsubscribe from subscribe_reboot.


**Parameters**

* [RebootHandle](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1ab1544cd3b8ae97dcb37af0ff7320f29a) **handle** - 

### subscribe_shutdown() {#classmavsdk_1_1_action_server_1ad30e10cb343d65e52efa2bfb71a52abf}
```cpp
ShutdownHandle mavsdk::ActionServer::subscribe_shutdown(const ShutdownCallback &callback)
```


Subscribe to shutdown command.


**Parameters**

* const [ShutdownCallback](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1aaa5057d10ec2b242c4a35fec46e5602a)& **callback** - 

**Returns**

&emsp;[ShutdownHandle](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a7604a04e4ccf230a321593aaec209375) - 

### unsubscribe_shutdown() {#classmavsdk_1_1_action_server_1a87dfe8c81f23f5e183f15bdbaf2645d2}
```cpp
void mavsdk::ActionServer::unsubscribe_shutdown(ShutdownHandle handle)
```


Unsubscribe from subscribe_shutdown.


**Parameters**

* [ShutdownHandle](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a7604a04e4ccf230a321593aaec209375) **handle** - 

### subscribe_terminate() {#classmavsdk_1_1_action_server_1ac332d0380be93c58208c8be626d6d797}
```cpp
TerminateHandle mavsdk::ActionServer::subscribe_terminate(const TerminateCallback &callback)
```


Subscribe to terminate command.


**Parameters**

* const [TerminateCallback](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a5eb879b36b5efc4f14bed82ed7655384)& **callback** - 

**Returns**

&emsp;[TerminateHandle](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a5ee91dbf03cfc2f19b0cdd3aee6e4214) - 

### unsubscribe_terminate() {#classmavsdk_1_1_action_server_1a3e236694f1f0beae030021b1b1d42633}
```cpp
void mavsdk::ActionServer::unsubscribe_terminate(TerminateHandle handle)
```


Unsubscribe from subscribe_terminate.


**Parameters**

* [TerminateHandle](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a5ee91dbf03cfc2f19b0cdd3aee6e4214) **handle** - 

### set_allow_takeoff() {#classmavsdk_1_1_action_server_1a250a8a8d1aed87d7c4125c6cfe797250}
```cpp
Result mavsdk::ActionServer::set_allow_takeoff(bool allow_takeoff) const
```


Can the vehicle takeoff.

This function is blocking.

**Parameters**

* bool **allow_takeoff** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8) - Result of request.

### set_armable() {#classmavsdk_1_1_action_server_1ab5b21457f69f9f315d54dbc06e745a7a}
```cpp
Result mavsdk::ActionServer::set_armable(bool armable, bool force_armable) const
```


Can the vehicle arm when requested.

This function is blocking.

**Parameters**

* bool **armable** - 
* bool **force_armable** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8) - Result of request.

### set_disarmable() {#classmavsdk_1_1_action_server_1afae1336100d7a91a4f4521cee56a1ecb}
```cpp
Result mavsdk::ActionServer::set_disarmable(bool disarmable, bool force_disarmable) const
```


Can the vehicle disarm when requested.

This function is blocking.

**Parameters**

* bool **disarmable** - 
* bool **force_disarmable** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8) - Result of request.

### set_allowable_flight_modes() {#classmavsdk_1_1_action_server_1a3041d1b923a3b01021433ad43ab93b3a}
```cpp
Result mavsdk::ActionServer::set_allowable_flight_modes(AllowableFlightModes flight_modes) const
```


Set which modes the vehicle can transition to (Manual always allowed)

This function is blocking.

**Parameters**

* [AllowableFlightModes](structmavsdk_1_1_action_server_1_1_allowable_flight_modes.md) **flight_modes** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_action_server.md#classmavsdk_1_1_action_server_1a4a8eb4fe9d098a5b7891232fc5bf32f8) - Result of request.

### get_allowable_flight_modes() {#classmavsdk_1_1_action_server_1a0960a6ec243823729a418a3c68feaf2a}
```cpp
ActionServer::AllowableFlightModes mavsdk::ActionServer::get_allowable_flight_modes() const
```


Get which modes the vehicle can transition to (Manual always allowed)

This function is blocking.

**Returns**

&emsp;[ActionServer::AllowableFlightModes](structmavsdk_1_1_action_server_1_1_allowable_flight_modes.md) - Result of request.

### operator=() {#classmavsdk_1_1_action_server_1a86d8bb24723ad8222669aec1f5c523ca}
```cpp
const ActionServer& mavsdk::ActionServer::operator=(const ActionServer &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [ActionServer](classmavsdk_1_1_action_server.md)&  - 

**Returns**

&emsp;const [ActionServer](classmavsdk_1_1_action_server.md) & - 