# mavsdk::Events Class Reference
`#include: events.h`

----


Get event notifications, such as takeoff, or arming checks. 


## Data Structures


struct [Event](structmavsdk_1_1_events_1_1_event.md)

struct [HealthAndArmingCheckMode](structmavsdk_1_1_events_1_1_health_and_arming_check_mode.md)

struct [HealthAndArmingCheckProblem](structmavsdk_1_1_events_1_1_health_and_arming_check_problem.md)

struct [HealthAndArmingCheckReport](structmavsdk_1_1_events_1_1_health_and_arming_check_report.md)

struct [HealthComponentReport](structmavsdk_1_1_events_1_1_health_component_report.md)

## Public Types


Type | Description
--- | ---
enum [LogLevel](#classmavsdk_1_1_events_1a237c8de77f3995138125db44d148cecc) | Log level type.
enum [Result](#classmavsdk_1_1_events_1abf1e4db55bd810ee6f5fce5d5c9439b1) | Possible results returned.
std::function< void([Result](classmavsdk_1_1_events.md#classmavsdk_1_1_events_1abf1e4db55bd810ee6f5fce5d5c9439b1))> [ResultCallback](#classmavsdk_1_1_events_1a36c781e03d503c528d7da31506cb0af3) | Callback type for asynchronous [Events](classmavsdk_1_1_events.md) calls.
std::function< void([Event](structmavsdk_1_1_events_1_1_event.md))> [EventsCallback](#classmavsdk_1_1_events_1aaa9c2f1d7915aedfdb0da5de7e31bbfb) | Callback type for subscribe_events.
[Handle](classmavsdk_1_1_handle.md)< [Event](structmavsdk_1_1_events_1_1_event.md) > [EventsHandle](#classmavsdk_1_1_events_1a64c99a834c6babf6cdb3b466a923e3ee) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_events.
std::function< void([HealthAndArmingCheckReport](structmavsdk_1_1_events_1_1_health_and_arming_check_report.md))> [HealthAndArmingChecksCallback](#classmavsdk_1_1_events_1ada96803a50810356893417591d91b1a3) | Callback type for subscribe_health_and_arming_checks.
[Handle](classmavsdk_1_1_handle.md)< [HealthAndArmingCheckReport](structmavsdk_1_1_events_1_1_health_and_arming_check_report.md) > [HealthAndArmingChecksHandle](#classmavsdk_1_1_events_1a09ac962d8d560de37c93b16484474fa4) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_health_and_arming_checks.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [Events](#classmavsdk_1_1_events_1a2f3778cf7118bd5a11b68e28655ef485) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [Events](#classmavsdk_1_1_events_1ae8e05a45bfac1043356936f1a15f8f2f) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~Events](#classmavsdk_1_1_events_1ac04cade0cb57f7858655c75a321c9370) () override | Destructor (internal use only).
&nbsp; | [Events](#classmavsdk_1_1_events_1a888740505c24a9bdad1f6e9ccd60a130) (const [Events](classmavsdk_1_1_events.md) & other) | Copy constructor.
[EventsHandle](classmavsdk_1_1_events.md#classmavsdk_1_1_events_1a64c99a834c6babf6cdb3b466a923e3ee) | [subscribe_events](#classmavsdk_1_1_events_1a4f167095c9a2c6362a9e5c336a7646a3) (const [EventsCallback](classmavsdk_1_1_events.md#classmavsdk_1_1_events_1aaa9c2f1d7915aedfdb0da5de7e31bbfb) & callback) | Subscribe to event updates.
void | [unsubscribe_events](#classmavsdk_1_1_events_1a4e7146bf1708b3d8a00bc29dd1205c97) ([EventsHandle](classmavsdk_1_1_events.md#classmavsdk_1_1_events_1a64c99a834c6babf6cdb3b466a923e3ee) handle) | Unsubscribe from subscribe_events.
[HealthAndArmingChecksHandle](classmavsdk_1_1_events.md#classmavsdk_1_1_events_1a09ac962d8d560de37c93b16484474fa4) | [subscribe_health_and_arming_checks](#classmavsdk_1_1_events_1a1d95f16aec0b9d9af3fc379029883b3e) (const [HealthAndArmingChecksCallback](classmavsdk_1_1_events.md#classmavsdk_1_1_events_1ada96803a50810356893417591d91b1a3) & callback) | Subscribe to arming check updates.
void | [unsubscribe_health_and_arming_checks](#classmavsdk_1_1_events_1a6e37b2dc8ae70404e459bf8f6d37bcb7) ([HealthAndArmingChecksHandle](classmavsdk_1_1_events.md#classmavsdk_1_1_events_1a09ac962d8d560de37c93b16484474fa4) handle) | Unsubscribe from subscribe_health_and_arming_checks.
std::pair< [Result](classmavsdk_1_1_events.md#classmavsdk_1_1_events_1abf1e4db55bd810ee6f5fce5d5c9439b1), [Events::HealthAndArmingCheckReport](structmavsdk_1_1_events_1_1_health_and_arming_check_report.md) > | [get_health_and_arming_checks_report](#classmavsdk_1_1_events_1ae8af916ca33c66d0d6ffb6fe8e8ae60d) () const | Get the latest report.
const [Events](classmavsdk_1_1_events.md) & | [operator=](#classmavsdk_1_1_events_1a7fa7923b2757e03a566ec46646440f75) (const [Events](classmavsdk_1_1_events.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### Events() {#classmavsdk_1_1_events_1a2f3778cf7118bd5a11b68e28655ef485}
```cpp
mavsdk::Events::Events(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto events = Events(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### Events() {#classmavsdk_1_1_events_1ae8e05a45bfac1043356936f1a15f8f2f}
```cpp
mavsdk::Events::Events(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto events = Events(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~Events() {#classmavsdk_1_1_events_1ac04cade0cb57f7858655c75a321c9370}
```cpp
mavsdk::Events::~Events() override
```


Destructor (internal use only).


### Events() {#classmavsdk_1_1_events_1a888740505c24a9bdad1f6e9ccd60a130}
```cpp
mavsdk::Events::Events(const Events &other)
```


Copy constructor.


**Parameters**

* const [Events](classmavsdk_1_1_events.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_events_1a36c781e03d503c528d7da31506cb0af3}

```cpp
using mavsdk::Events::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [Events](classmavsdk_1_1_events.md) calls.


### typedef EventsCallback {#classmavsdk_1_1_events_1aaa9c2f1d7915aedfdb0da5de7e31bbfb}

```cpp
using mavsdk::Events::EventsCallback =  std::function<void(Event)>
```


Callback type for subscribe_events.


### typedef EventsHandle {#classmavsdk_1_1_events_1a64c99a834c6babf6cdb3b466a923e3ee}

```cpp
using mavsdk::Events::EventsHandle =  Handle<Event>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_events.


### typedef HealthAndArmingChecksCallback {#classmavsdk_1_1_events_1ada96803a50810356893417591d91b1a3}

```cpp
using mavsdk::Events::HealthAndArmingChecksCallback =  std::function<void(HealthAndArmingCheckReport)>
```


Callback type for subscribe_health_and_arming_checks.


### typedef HealthAndArmingChecksHandle {#classmavsdk_1_1_events_1a09ac962d8d560de37c93b16484474fa4}

```cpp
using mavsdk::Events::HealthAndArmingChecksHandle =  Handle<HealthAndArmingCheckReport>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_health_and_arming_checks.


## Member Enumeration Documentation


### enum LogLevel {#classmavsdk_1_1_events_1a237c8de77f3995138125db44d148cecc}


Log level type.


Value | Description
--- | ---
<span id="classmavsdk_1_1_events_1a237c8de77f3995138125db44d148ceccaa3fa706f20bc0b7858b7ae6932261940"></span> `Emergency` | Emergency. 
<span id="classmavsdk_1_1_events_1a237c8de77f3995138125db44d148ceccab92071d61c88498171928745ca53078b"></span> `Alert` | Alert. 
<span id="classmavsdk_1_1_events_1a237c8de77f3995138125db44d148cecca278d01e5af56273bae1bb99a98b370cd"></span> `Critical` | Critical. 
<span id="classmavsdk_1_1_events_1a237c8de77f3995138125db44d148cecca902b0d55fddef6f8d651fe1035b7d4bd"></span> `Error` | Error. 
<span id="classmavsdk_1_1_events_1a237c8de77f3995138125db44d148cecca0eaadb4fcb48a0a0ed7bc9868be9fbaa"></span> `Warning` | Warning. 
<span id="classmavsdk_1_1_events_1a237c8de77f3995138125db44d148cecca24efa7ee4511563b16144f39706d594f"></span> `Notice` | Notice. 
<span id="classmavsdk_1_1_events_1a237c8de77f3995138125db44d148cecca4059b0251f66a18cb56f544728796875"></span> `Info` | [Info](classmavsdk_1_1_info.md). 
<span id="classmavsdk_1_1_events_1a237c8de77f3995138125db44d148ceccaa603905470e2a5b8c13e96b579ef0dba"></span> `Debug` | Debug. 

### enum Result {#classmavsdk_1_1_events_1abf1e4db55bd810ee6f5fce5d5c9439b1}


Possible results returned.


Value | Description
--- | ---
<span id="classmavsdk_1_1_events_1abf1e4db55bd810ee6f5fce5d5c9439b1a505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Successful result. 
<span id="classmavsdk_1_1_events_1abf1e4db55bd810ee6f5fce5d5c9439b1a534ceac854da4ba59c4dc41b7ab732dc"></span> `NotAvailable` | Not available. 
<span id="classmavsdk_1_1_events_1abf1e4db55bd810ee6f5fce5d5c9439b1a094a6f6b0868122a9dd008cb91c083e4"></span> `ConnectionError` | Connection error. 
<span id="classmavsdk_1_1_events_1abf1e4db55bd810ee6f5fce5d5c9439b1ab4080bdf74febf04d578ff105cce9d3f"></span> `Unsupported` | Unsupported. 
<span id="classmavsdk_1_1_events_1abf1e4db55bd810ee6f5fce5d5c9439b1a58d036b9b7f0e7eb38cfb90f1cc70a73"></span> `Denied` | Denied. 
<span id="classmavsdk_1_1_events_1abf1e4db55bd810ee6f5fce5d5c9439b1ad7c8c85bf79bbe1b7188497c32c3b0ca"></span> `Failed` | Failed. 
<span id="classmavsdk_1_1_events_1abf1e4db55bd810ee6f5fce5d5c9439b1ac85a251cc457840f1e032f1b733e9398"></span> `Timeout` | Timeout. 
<span id="classmavsdk_1_1_events_1abf1e4db55bd810ee6f5fce5d5c9439b1a1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system available. 
<span id="classmavsdk_1_1_events_1abf1e4db55bd810ee6f5fce5d5c9439b1a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 

## Member Function Documentation


### subscribe_events() {#classmavsdk_1_1_events_1a4f167095c9a2c6362a9e5c336a7646a3}
```cpp
EventsHandle mavsdk::Events::subscribe_events(const EventsCallback &callback)
```


Subscribe to event updates.


**Parameters**

* const [EventsCallback](classmavsdk_1_1_events.md#classmavsdk_1_1_events_1aaa9c2f1d7915aedfdb0da5de7e31bbfb)& **callback** - 

**Returns**

&emsp;[EventsHandle](classmavsdk_1_1_events.md#classmavsdk_1_1_events_1a64c99a834c6babf6cdb3b466a923e3ee) - 

### unsubscribe_events() {#classmavsdk_1_1_events_1a4e7146bf1708b3d8a00bc29dd1205c97}
```cpp
void mavsdk::Events::unsubscribe_events(EventsHandle handle)
```


Unsubscribe from subscribe_events.


**Parameters**

* [EventsHandle](classmavsdk_1_1_events.md#classmavsdk_1_1_events_1a64c99a834c6babf6cdb3b466a923e3ee) **handle** - 

### subscribe_health_and_arming_checks() {#classmavsdk_1_1_events_1a1d95f16aec0b9d9af3fc379029883b3e}
```cpp
HealthAndArmingChecksHandle mavsdk::Events::subscribe_health_and_arming_checks(const HealthAndArmingChecksCallback &callback)
```


Subscribe to arming check updates.


**Parameters**

* const [HealthAndArmingChecksCallback](classmavsdk_1_1_events.md#classmavsdk_1_1_events_1ada96803a50810356893417591d91b1a3)& **callback** - 

**Returns**

&emsp;[HealthAndArmingChecksHandle](classmavsdk_1_1_events.md#classmavsdk_1_1_events_1a09ac962d8d560de37c93b16484474fa4) - 

### unsubscribe_health_and_arming_checks() {#classmavsdk_1_1_events_1a6e37b2dc8ae70404e459bf8f6d37bcb7}
```cpp
void mavsdk::Events::unsubscribe_health_and_arming_checks(HealthAndArmingChecksHandle handle)
```


Unsubscribe from subscribe_health_and_arming_checks.


**Parameters**

* [HealthAndArmingChecksHandle](classmavsdk_1_1_events.md#classmavsdk_1_1_events_1a09ac962d8d560de37c93b16484474fa4) **handle** - 

### get_health_and_arming_checks_report() {#classmavsdk_1_1_events_1ae8af916ca33c66d0d6ffb6fe8e8ae60d}
```cpp
std::pair< Result, Events::HealthAndArmingCheckReport > mavsdk::Events::get_health_and_arming_checks_report() const
```


Get the latest report.

This function is blocking.

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_events.md#classmavsdk_1_1_events_1abf1e4db55bd810ee6f5fce5d5c9439b1), [Events::HealthAndArmingCheckReport](structmavsdk_1_1_events_1_1_health_and_arming_check_report.md) > - Result of request.

### operator=() {#classmavsdk_1_1_events_1a7fa7923b2757e03a566ec46646440f75}
```cpp
const Events & mavsdk::Events::operator=(const Events &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [Events](classmavsdk_1_1_events.md)&  - 

**Returns**

&emsp;const [Events](classmavsdk_1_1_events.md) & - 