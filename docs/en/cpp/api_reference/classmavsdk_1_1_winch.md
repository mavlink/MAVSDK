# mavsdk::Winch Class Reference
`#include: winch.h`

----


Allows users to send winch actions, as well as receive status information from winch systems. 


## Data Structures


struct [Status](structmavsdk_1_1_winch_1_1_status.md)

struct [StatusFlags](structmavsdk_1_1_winch_1_1_status_flags.md)

## Public Types


Type | Description
--- | ---
enum [WinchAction](#classmavsdk_1_1_winch_1a2c63ccb04edf046026919d5b3db77a83) | [Winch](classmavsdk_1_1_winch.md) [Action](classmavsdk_1_1_action.md) type.
enum [Result](#classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091) | Possible results returned for winch action requests.
std::function< void([Result](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091))> [ResultCallback](#classmavsdk_1_1_winch_1a683fc7d385d461efb059df917622a6b7) | Callback type for asynchronous [Winch](classmavsdk_1_1_winch.md) calls.
std::function< void([Status](structmavsdk_1_1_winch_1_1_status.md))> [StatusCallback](#classmavsdk_1_1_winch_1a6c84089eac00fc9de890ab7e8c0eb32a) | Callback type for subscribe_status.
[Handle](classmavsdk_1_1_handle.md)< [Status](structmavsdk_1_1_winch_1_1_status.md) > [StatusHandle](#classmavsdk_1_1_winch_1a78284180b73b3092c37e45b3c2bdc2ab) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_status.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [Winch](#classmavsdk_1_1_winch_1a222e543b5ef54c89ee41411e800d3b88) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [Winch](#classmavsdk_1_1_winch_1a7546040a07899cc9fe3edb00cb1b2007) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~Winch](#classmavsdk_1_1_winch_1a06330946e884dae7415003aa89c3010e) () override | Destructor (internal use only).
&nbsp; | [Winch](#classmavsdk_1_1_winch_1a9125a173b929f9a1a2e5ca0199961fb4) (const [Winch](classmavsdk_1_1_winch.md) & other) | Copy constructor.
[StatusHandle](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a78284180b73b3092c37e45b3c2bdc2ab) | [subscribe_status](#classmavsdk_1_1_winch_1a92363df861842edf0b81aa18fba26bc8) (const [StatusCallback](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a6c84089eac00fc9de890ab7e8c0eb32a) & callback) | Subscribe to 'winch status' updates.
void | [unsubscribe_status](#classmavsdk_1_1_winch_1af1176a4992e0a6850dbd44ea2b1f0bad) ([StatusHandle](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a78284180b73b3092c37e45b3c2bdc2ab) handle) | Unsubscribe from subscribe_status.
[Status](structmavsdk_1_1_winch_1_1_status.md) | [status](#classmavsdk_1_1_winch_1aa24ccd5e6e44db1fbf96e4aca6947689) () const | Poll for '[Status](structmavsdk_1_1_winch_1_1_status.md)' (blocking).
void | [relax_async](#classmavsdk_1_1_winch_1a48eec9e1fb693620e452234e0a740989) (uint32_t instance, const [ResultCallback](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a683fc7d385d461efb059df917622a6b7) callback) | Allow motor to freewheel.
[Result](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091) | [relax](#classmavsdk_1_1_winch_1ae1fa0d57d8d72e3bbbb4833770047ab3) (uint32_t instance)const | Allow motor to freewheel.
void | [relative_length_control_async](#classmavsdk_1_1_winch_1aa2cb382d36cda8e0fe918ae07fb7a9ec) (uint32_t instance, float length_m, float rate_m_s, const [ResultCallback](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a683fc7d385d461efb059df917622a6b7) callback) | Wind or unwind specified length of line, optionally using specified rate.
[Result](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091) | [relative_length_control](#classmavsdk_1_1_winch_1ae39f5844dd5b6d93b6ccf3953781cee0) (uint32_t instance, float length_m, float rate_m_s)const | Wind or unwind specified length of line, optionally using specified rate.
void | [rate_control_async](#classmavsdk_1_1_winch_1a21ce05ef8cedd2601d22209d31a980f8) (uint32_t instance, float rate_m_s, const [ResultCallback](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a683fc7d385d461efb059df917622a6b7) callback) | Wind or unwind line at specified rate.
[Result](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091) | [rate_control](#classmavsdk_1_1_winch_1aaec8a2bac41941e1f696cd9541abd4ca) (uint32_t instance, float rate_m_s)const | Wind or unwind line at specified rate.
void | [lock_async](#classmavsdk_1_1_winch_1a0ff91279058752931bee2bc5dfd7c711) (uint32_t instance, const [ResultCallback](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a683fc7d385d461efb059df917622a6b7) callback) | Perform the locking sequence to relieve motor while in the fully retracted position.
[Result](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091) | [lock](#classmavsdk_1_1_winch_1ad0f907f0164a724f8e8a57fe9796ecb3) (uint32_t instance)const | Perform the locking sequence to relieve motor while in the fully retracted position.
void | [deliver_async](#classmavsdk_1_1_winch_1acbd43a3f00ab1e910b6b05654afc9458) (uint32_t instance, const [ResultCallback](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a683fc7d385d461efb059df917622a6b7) callback) | Sequence of drop, slow down, touch down, reel up, lock.
[Result](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091) | [deliver](#classmavsdk_1_1_winch_1a8910df835085243a366290dbc4492680) (uint32_t instance)const | Sequence of drop, slow down, touch down, reel up, lock.
void | [hold_async](#classmavsdk_1_1_winch_1a6c97626d33532469871a687e8043e27e) (uint32_t instance, const [ResultCallback](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a683fc7d385d461efb059df917622a6b7) callback) | Engage motor and hold current position.
[Result](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091) | [hold](#classmavsdk_1_1_winch_1a2e7ae05d09b82964024c70211b1920d9) (uint32_t instance)const | Engage motor and hold current position.
void | [retract_async](#classmavsdk_1_1_winch_1a6485b743115d441b43f3d0e39341412d) (uint32_t instance, const [ResultCallback](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a683fc7d385d461efb059df917622a6b7) callback) | Return the reel to the fully retracted position.
[Result](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091) | [retract](#classmavsdk_1_1_winch_1a353cad0259d2eb8201e2884e037422eb) (uint32_t instance)const | Return the reel to the fully retracted position.
void | [load_line_async](#classmavsdk_1_1_winch_1ad0588792ef3fbd469511f99988140637) (uint32_t instance, const [ResultCallback](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a683fc7d385d461efb059df917622a6b7) callback) | Load the reel with line.
[Result](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091) | [load_line](#classmavsdk_1_1_winch_1aabeb779b2e03632176799c92058adb92) (uint32_t instance)const | Load the reel with line.
void | [abandon_line_async](#classmavsdk_1_1_winch_1a3970c74e067d3734ba8dd3382c534b0a) (uint32_t instance, const [ResultCallback](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a683fc7d385d461efb059df917622a6b7) callback) | Spool out the entire length of the line.
[Result](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091) | [abandon_line](#classmavsdk_1_1_winch_1a625f6d58d5447afbe0c40e3d896071b0) (uint32_t instance)const | Spool out the entire length of the line.
void | [load_payload_async](#classmavsdk_1_1_winch_1af715b6c01dc75c44868c2936af04d40a) (uint32_t instance, const [ResultCallback](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a683fc7d385d461efb059df917622a6b7) callback) | Spools out just enough to present the hook to the user to load the payload.
[Result](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091) | [load_payload](#classmavsdk_1_1_winch_1a01fec2758f7f09d8aef5de23f4566d47) (uint32_t instance)const | Spools out just enough to present the hook to the user to load the payload.
const [Winch](classmavsdk_1_1_winch.md) & | [operator=](#classmavsdk_1_1_winch_1a5216eb2d1533b1e737ad30f31b7eff7a) (const [Winch](classmavsdk_1_1_winch.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### Winch() {#classmavsdk_1_1_winch_1a222e543b5ef54c89ee41411e800d3b88}
```cpp
mavsdk::Winch::Winch(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto winch = Winch(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### Winch() {#classmavsdk_1_1_winch_1a7546040a07899cc9fe3edb00cb1b2007}
```cpp
mavsdk::Winch::Winch(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto winch = Winch(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~Winch() {#classmavsdk_1_1_winch_1a06330946e884dae7415003aa89c3010e}
```cpp
mavsdk::Winch::~Winch() override
```


Destructor (internal use only).


### Winch() {#classmavsdk_1_1_winch_1a9125a173b929f9a1a2e5ca0199961fb4}
```cpp
mavsdk::Winch::Winch(const Winch &other)
```


Copy constructor.


**Parameters**

* const [Winch](classmavsdk_1_1_winch.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_winch_1a683fc7d385d461efb059df917622a6b7}

```cpp
using mavsdk::Winch::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [Winch](classmavsdk_1_1_winch.md) calls.


### typedef StatusCallback {#classmavsdk_1_1_winch_1a6c84089eac00fc9de890ab7e8c0eb32a}

```cpp
using mavsdk::Winch::StatusCallback =  std::function<void(Status)>
```


Callback type for subscribe_status.


### typedef StatusHandle {#classmavsdk_1_1_winch_1a78284180b73b3092c37e45b3c2bdc2ab}

```cpp
using mavsdk::Winch::StatusHandle =  Handle<Status>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_status.


## Member Enumeration Documentation


### enum WinchAction {#classmavsdk_1_1_winch_1a2c63ccb04edf046026919d5b3db77a83}


[Winch](classmavsdk_1_1_winch.md) [Action](classmavsdk_1_1_action.md) type.


Value | Description
--- | ---
<span id="classmavsdk_1_1_winch_1a2c63ccb04edf046026919d5b3db77a83a4160be5df3fc34c3b165f28616f1bd16"></span> `Relaxed` | Allow motor to freewheel. 
<span id="classmavsdk_1_1_winch_1a2c63ccb04edf046026919d5b3db77a83afd7ed80d3677cf91582f0a9e6ce3477b"></span> `RelativeLengthControl` | Wind or unwind specified length of line, optionally using specified rate. 
<span id="classmavsdk_1_1_winch_1a2c63ccb04edf046026919d5b3db77a83aae2b389b99e74cc32f0fabded761cda6"></span> `RateControl` | Wind or unwind line at specified rate. 
<span id="classmavsdk_1_1_winch_1a2c63ccb04edf046026919d5b3db77a83ab485167c5b0e59d47009a16f90fe2659"></span> `Lock` | Perform the locking sequence to relieve motor while in the fully retracted position. 
<span id="classmavsdk_1_1_winch_1a2c63ccb04edf046026919d5b3db77a83a55f090686d433b9f61cdbacc4a741375"></span> `Deliver` | Sequence of drop, slow down, touch down, reel up, lock. 
<span id="classmavsdk_1_1_winch_1a2c63ccb04edf046026919d5b3db77a83abcd8db575b47c838e5d551e3973db4ac"></span> `Hold` | Engage motor and hold current position. 
<span id="classmavsdk_1_1_winch_1a2c63ccb04edf046026919d5b3db77a83a0dfc45bc99de47ebc26a3a7752eb1e19"></span> `Retract` | Return the reel to the fully retracted position. 
<span id="classmavsdk_1_1_winch_1a2c63ccb04edf046026919d5b3db77a83a460a2503753c200a2324d95a61c14e38"></span> `LoadLine` | Load the reel with line. The winch will calculate the total loaded length and stop when the tension exceeds a threshold. 
<span id="classmavsdk_1_1_winch_1a2c63ccb04edf046026919d5b3db77a83af0ac1672e2a313e081a0dbdfcce6c23e"></span> `AbandonLine` | Spool out the entire length of the line. 
<span id="classmavsdk_1_1_winch_1a2c63ccb04edf046026919d5b3db77a83aa296461c9776b8003324a6fb508d30bd"></span> `LoadPayload` | Spools out just enough to present the hook to the user to load the payload. 

### enum Result {#classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091}


Possible results returned for winch action requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091a505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Request was successful. 
<span id="classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091a1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system is connected. 
<span id="classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091ad8a942ef2b04672adfafef0ad817a407"></span> `Busy` | Temporarily rejected. 
<span id="classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091ac85a251cc457840f1e032f1b733e9398"></span> `Timeout` | Request timed out. 
<span id="classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091ab4080bdf74febf04d578ff105cce9d3f"></span> `Unsupported` | [Action](classmavsdk_1_1_action.md) not supported. 
<span id="classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091ad7c8c85bf79bbe1b7188497c32c3b0ca"></span> `Failed` | [Action](classmavsdk_1_1_action.md) failed. 

## Member Function Documentation


### subscribe_status() {#classmavsdk_1_1_winch_1a92363df861842edf0b81aa18fba26bc8}
```cpp
StatusHandle mavsdk::Winch::subscribe_status(const StatusCallback &callback)
```


Subscribe to 'winch status' updates.


**Parameters**

* const [StatusCallback](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a6c84089eac00fc9de890ab7e8c0eb32a)& **callback** - 

**Returns**

&emsp;[StatusHandle](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a78284180b73b3092c37e45b3c2bdc2ab) - 

### unsubscribe_status() {#classmavsdk_1_1_winch_1af1176a4992e0a6850dbd44ea2b1f0bad}
```cpp
void mavsdk::Winch::unsubscribe_status(StatusHandle handle)
```


Unsubscribe from subscribe_status.


**Parameters**

* [StatusHandle](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a78284180b73b3092c37e45b3c2bdc2ab) **handle** - 

### status() {#classmavsdk_1_1_winch_1aa24ccd5e6e44db1fbf96e4aca6947689}
```cpp
Status mavsdk::Winch::status() const
```


Poll for '[Status](structmavsdk_1_1_winch_1_1_status.md)' (blocking).


**Returns**

&emsp;[Status](structmavsdk_1_1_winch_1_1_status.md) - One [Status](structmavsdk_1_1_winch_1_1_status.md) update.

### relax_async() {#classmavsdk_1_1_winch_1a48eec9e1fb693620e452234e0a740989}
```cpp
void mavsdk::Winch::relax_async(uint32_t instance, const ResultCallback callback)
```


Allow motor to freewheel.

This function is non-blocking. See 'relax' for the blocking counterpart.

**Parameters**

* uint32_t **instance** - 
* const [ResultCallback](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a683fc7d385d461efb059df917622a6b7) **callback** - 

### relax() {#classmavsdk_1_1_winch_1ae1fa0d57d8d72e3bbbb4833770047ab3}
```cpp
Result mavsdk::Winch::relax(uint32_t instance) const
```


Allow motor to freewheel.

This function is blocking. See 'relax_async' for the non-blocking counterpart.

**Parameters**

* uint32_t **instance** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091) - Result of request.

### relative_length_control_async() {#classmavsdk_1_1_winch_1aa2cb382d36cda8e0fe918ae07fb7a9ec}
```cpp
void mavsdk::Winch::relative_length_control_async(uint32_t instance, float length_m, float rate_m_s, const ResultCallback callback)
```


Wind or unwind specified length of line, optionally using specified rate.

This function is non-blocking. See 'relative_length_control' for the blocking counterpart.

**Parameters**

* uint32_t **instance** - 
* float **length_m** - 
* float **rate_m_s** - 
* const [ResultCallback](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a683fc7d385d461efb059df917622a6b7) **callback** - 

### relative_length_control() {#classmavsdk_1_1_winch_1ae39f5844dd5b6d93b6ccf3953781cee0}
```cpp
Result mavsdk::Winch::relative_length_control(uint32_t instance, float length_m, float rate_m_s) const
```


Wind or unwind specified length of line, optionally using specified rate.

This function is blocking. See 'relative_length_control_async' for the non-blocking counterpart.

**Parameters**

* uint32_t **instance** - 
* float **length_m** - 
* float **rate_m_s** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091) - Result of request.

### rate_control_async() {#classmavsdk_1_1_winch_1a21ce05ef8cedd2601d22209d31a980f8}
```cpp
void mavsdk::Winch::rate_control_async(uint32_t instance, float rate_m_s, const ResultCallback callback)
```


Wind or unwind line at specified rate.

This function is non-blocking. See 'rate_control' for the blocking counterpart.

**Parameters**

* uint32_t **instance** - 
* float **rate_m_s** - 
* const [ResultCallback](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a683fc7d385d461efb059df917622a6b7) **callback** - 

### rate_control() {#classmavsdk_1_1_winch_1aaec8a2bac41941e1f696cd9541abd4ca}
```cpp
Result mavsdk::Winch::rate_control(uint32_t instance, float rate_m_s) const
```


Wind or unwind line at specified rate.

This function is blocking. See 'rate_control_async' for the non-blocking counterpart.

**Parameters**

* uint32_t **instance** - 
* float **rate_m_s** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091) - Result of request.

### lock_async() {#classmavsdk_1_1_winch_1a0ff91279058752931bee2bc5dfd7c711}
```cpp
void mavsdk::Winch::lock_async(uint32_t instance, const ResultCallback callback)
```


Perform the locking sequence to relieve motor while in the fully retracted position.

This function is non-blocking. See 'lock' for the blocking counterpart.

**Parameters**

* uint32_t **instance** - 
* const [ResultCallback](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a683fc7d385d461efb059df917622a6b7) **callback** - 

### lock() {#classmavsdk_1_1_winch_1ad0f907f0164a724f8e8a57fe9796ecb3}
```cpp
Result mavsdk::Winch::lock(uint32_t instance) const
```


Perform the locking sequence to relieve motor while in the fully retracted position.

This function is blocking. See 'lock_async' for the non-blocking counterpart.

**Parameters**

* uint32_t **instance** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091) - Result of request.

### deliver_async() {#classmavsdk_1_1_winch_1acbd43a3f00ab1e910b6b05654afc9458}
```cpp
void mavsdk::Winch::deliver_async(uint32_t instance, const ResultCallback callback)
```


Sequence of drop, slow down, touch down, reel up, lock.

This function is non-blocking. See 'deliver' for the blocking counterpart.

**Parameters**

* uint32_t **instance** - 
* const [ResultCallback](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a683fc7d385d461efb059df917622a6b7) **callback** - 

### deliver() {#classmavsdk_1_1_winch_1a8910df835085243a366290dbc4492680}
```cpp
Result mavsdk::Winch::deliver(uint32_t instance) const
```


Sequence of drop, slow down, touch down, reel up, lock.

This function is blocking. See 'deliver_async' for the non-blocking counterpart.

**Parameters**

* uint32_t **instance** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091) - Result of request.

### hold_async() {#classmavsdk_1_1_winch_1a6c97626d33532469871a687e8043e27e}
```cpp
void mavsdk::Winch::hold_async(uint32_t instance, const ResultCallback callback)
```


Engage motor and hold current position.

This function is non-blocking. See 'hold' for the blocking counterpart.

**Parameters**

* uint32_t **instance** - 
* const [ResultCallback](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a683fc7d385d461efb059df917622a6b7) **callback** - 

### hold() {#classmavsdk_1_1_winch_1a2e7ae05d09b82964024c70211b1920d9}
```cpp
Result mavsdk::Winch::hold(uint32_t instance) const
```


Engage motor and hold current position.

This function is blocking. See 'hold_async' for the non-blocking counterpart.

**Parameters**

* uint32_t **instance** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091) - Result of request.

### retract_async() {#classmavsdk_1_1_winch_1a6485b743115d441b43f3d0e39341412d}
```cpp
void mavsdk::Winch::retract_async(uint32_t instance, const ResultCallback callback)
```


Return the reel to the fully retracted position.

This function is non-blocking. See 'retract' for the blocking counterpart.

**Parameters**

* uint32_t **instance** - 
* const [ResultCallback](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a683fc7d385d461efb059df917622a6b7) **callback** - 

### retract() {#classmavsdk_1_1_winch_1a353cad0259d2eb8201e2884e037422eb}
```cpp
Result mavsdk::Winch::retract(uint32_t instance) const
```


Return the reel to the fully retracted position.

This function is blocking. See 'retract_async' for the non-blocking counterpart.

**Parameters**

* uint32_t **instance** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091) - Result of request.

### load_line_async() {#classmavsdk_1_1_winch_1ad0588792ef3fbd469511f99988140637}
```cpp
void mavsdk::Winch::load_line_async(uint32_t instance, const ResultCallback callback)
```


Load the reel with line.

The winch will calculate the total loaded length and stop when the tension exceeds a threshold.


This function is non-blocking. See 'load_line' for the blocking counterpart.

**Parameters**

* uint32_t **instance** - 
* const [ResultCallback](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a683fc7d385d461efb059df917622a6b7) **callback** - 

### load_line() {#classmavsdk_1_1_winch_1aabeb779b2e03632176799c92058adb92}
```cpp
Result mavsdk::Winch::load_line(uint32_t instance) const
```


Load the reel with line.

The winch will calculate the total loaded length and stop when the tension exceeds a threshold.


This function is blocking. See 'load_line_async' for the non-blocking counterpart.

**Parameters**

* uint32_t **instance** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091) - Result of request.

### abandon_line_async() {#classmavsdk_1_1_winch_1a3970c74e067d3734ba8dd3382c534b0a}
```cpp
void mavsdk::Winch::abandon_line_async(uint32_t instance, const ResultCallback callback)
```


Spool out the entire length of the line.

This function is non-blocking. See 'abandon_line' for the blocking counterpart.

**Parameters**

* uint32_t **instance** - 
* const [ResultCallback](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a683fc7d385d461efb059df917622a6b7) **callback** - 

### abandon_line() {#classmavsdk_1_1_winch_1a625f6d58d5447afbe0c40e3d896071b0}
```cpp
Result mavsdk::Winch::abandon_line(uint32_t instance) const
```


Spool out the entire length of the line.

This function is blocking. See 'abandon_line_async' for the non-blocking counterpart.

**Parameters**

* uint32_t **instance** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091) - Result of request.

### load_payload_async() {#classmavsdk_1_1_winch_1af715b6c01dc75c44868c2936af04d40a}
```cpp
void mavsdk::Winch::load_payload_async(uint32_t instance, const ResultCallback callback)
```


Spools out just enough to present the hook to the user to load the payload.

This function is non-blocking. See 'load_payload' for the blocking counterpart.

**Parameters**

* uint32_t **instance** - 
* const [ResultCallback](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a683fc7d385d461efb059df917622a6b7) **callback** - 

### load_payload() {#classmavsdk_1_1_winch_1a01fec2758f7f09d8aef5de23f4566d47}
```cpp
Result mavsdk::Winch::load_payload(uint32_t instance) const
```


Spools out just enough to present the hook to the user to load the payload.

This function is blocking. See 'load_payload_async' for the non-blocking counterpart.

**Parameters**

* uint32_t **instance** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_winch.md#classmavsdk_1_1_winch_1a48edb6e5176dc8d5e95bd30eacd7a091) - Result of request.

### operator=() {#classmavsdk_1_1_winch_1a5216eb2d1533b1e737ad30f31b7eff7a}
```cpp
const Winch& mavsdk::Winch::operator=(const Winch &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [Winch](classmavsdk_1_1_winch.md)&  - 

**Returns**

&emsp;const [Winch](classmavsdk_1_1_winch.md) & - 