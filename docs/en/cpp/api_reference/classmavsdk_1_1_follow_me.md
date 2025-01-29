# mavsdk::FollowMe Class Reference
`#include: follow_me.h`

----


Allow users to command the vehicle to follow a specific target. The target is provided as a GPS coordinate and altitude. 


## Data Structures


struct [Config](structmavsdk_1_1_follow_me_1_1_config.md)

struct [TargetLocation](structmavsdk_1_1_follow_me_1_1_target_location.md)

## Public Types


Type | Description
--- | ---
enum [Result](#classmavsdk_1_1_follow_me_1a2b3f334ea72fd84d9e925fb3756451d8) | Possible results returned for followme operations.
std::function< void([Result](classmavsdk_1_1_follow_me.md#classmavsdk_1_1_follow_me_1a2b3f334ea72fd84d9e925fb3756451d8))> [ResultCallback](#classmavsdk_1_1_follow_me_1a6e02ca57a63131594f0bf5fb5689309e) | Callback type for asynchronous [FollowMe](classmavsdk_1_1_follow_me.md) calls.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [FollowMe](#classmavsdk_1_1_follow_me_1ab85b27fcd899f4861cd2e99c08083b9e) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [FollowMe](#classmavsdk_1_1_follow_me_1a422ef7ca182c92052b0da32d789b0c14) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~FollowMe](#classmavsdk_1_1_follow_me_1a51cc272f759657eb583830cbde2785cf) () override | Destructor (internal use only).
&nbsp; | [FollowMe](#classmavsdk_1_1_follow_me_1a204548d586c8355ab60991be4b63c7aa) (const [FollowMe](classmavsdk_1_1_follow_me.md) & other) | Copy constructor.
[FollowMe::Config](structmavsdk_1_1_follow_me_1_1_config.md) | [get_config](#classmavsdk_1_1_follow_me_1aca2e599cd6fb889b9f80dc7a9da57ee9) () const | Get current configuration.
[Result](classmavsdk_1_1_follow_me.md#classmavsdk_1_1_follow_me_1a2b3f334ea72fd84d9e925fb3756451d8) | [set_config](#classmavsdk_1_1_follow_me_1aa76aab9a21bc3ae475bee6a55c0e4d30) ([Config](structmavsdk_1_1_follow_me_1_1_config.md) config)const | Apply configuration by sending it to the system.
bool | [is_active](#classmavsdk_1_1_follow_me_1a48ab77939257e52159bd9ed19335a7de) () const | Check if [FollowMe](classmavsdk_1_1_follow_me.md) is active.
[Result](classmavsdk_1_1_follow_me.md#classmavsdk_1_1_follow_me_1a2b3f334ea72fd84d9e925fb3756451d8) | [set_target_location](#classmavsdk_1_1_follow_me_1a1a99e282472235f726bfde430873ffd5) ([TargetLocation](structmavsdk_1_1_follow_me_1_1_target_location.md) location)const | Set location of the moving target.
[FollowMe::TargetLocation](structmavsdk_1_1_follow_me_1_1_target_location.md) | [get_last_location](#classmavsdk_1_1_follow_me_1af2a1af346ee2fa7761b58b406e9e6e0c) () const | Get the last location of the target.
[Result](classmavsdk_1_1_follow_me.md#classmavsdk_1_1_follow_me_1a2b3f334ea72fd84d9e925fb3756451d8) | [start](#classmavsdk_1_1_follow_me_1a4b6ae3ec1ff07d8b3a79038e04992003) () const | Start [FollowMe](classmavsdk_1_1_follow_me.md) mode.
[Result](classmavsdk_1_1_follow_me.md#classmavsdk_1_1_follow_me_1a2b3f334ea72fd84d9e925fb3756451d8) | [stop](#classmavsdk_1_1_follow_me_1a202a7b9edf56d9b883c974a09c14ba7d) () const | Stop [FollowMe](classmavsdk_1_1_follow_me.md) mode.
const [FollowMe](classmavsdk_1_1_follow_me.md) & | [operator=](#classmavsdk_1_1_follow_me_1a6292f6dd2c91cedde0e3b82952d83510) (const [FollowMe](classmavsdk_1_1_follow_me.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### FollowMe() {#classmavsdk_1_1_follow_me_1ab85b27fcd899f4861cd2e99c08083b9e}
```cpp
mavsdk::FollowMe::FollowMe(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto follow_me = FollowMe(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### FollowMe() {#classmavsdk_1_1_follow_me_1a422ef7ca182c92052b0da32d789b0c14}
```cpp
mavsdk::FollowMe::FollowMe(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto follow_me = FollowMe(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~FollowMe() {#classmavsdk_1_1_follow_me_1a51cc272f759657eb583830cbde2785cf}
```cpp
mavsdk::FollowMe::~FollowMe() override
```


Destructor (internal use only).


### FollowMe() {#classmavsdk_1_1_follow_me_1a204548d586c8355ab60991be4b63c7aa}
```cpp
mavsdk::FollowMe::FollowMe(const FollowMe &other)
```


Copy constructor.


**Parameters**

* const [FollowMe](classmavsdk_1_1_follow_me.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_follow_me_1a6e02ca57a63131594f0bf5fb5689309e}

```cpp
using mavsdk::FollowMe::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [FollowMe](classmavsdk_1_1_follow_me.md) calls.


## Member Enumeration Documentation


### enum Result {#classmavsdk_1_1_follow_me_1a2b3f334ea72fd84d9e925fb3756451d8}


Possible results returned for followme operations.


Value | Description
--- | ---
<span id="classmavsdk_1_1_follow_me_1a2b3f334ea72fd84d9e925fb3756451d8a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_follow_me_1a2b3f334ea72fd84d9e925fb3756451d8a505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Request succeeded. 
<span id="classmavsdk_1_1_follow_me_1a2b3f334ea72fd84d9e925fb3756451d8a1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system connected. 
<span id="classmavsdk_1_1_follow_me_1a2b3f334ea72fd84d9e925fb3756451d8a094a6f6b0868122a9dd008cb91c083e4"></span> `ConnectionError` | Connection error. 
<span id="classmavsdk_1_1_follow_me_1a2b3f334ea72fd84d9e925fb3756451d8ad8a942ef2b04672adfafef0ad817a407"></span> `Busy` | Vehicle is busy. 
<span id="classmavsdk_1_1_follow_me_1a2b3f334ea72fd84d9e925fb3756451d8a3398e12855176d55f43d53e04f472c8a"></span> `CommandDenied` | Command denied. 
<span id="classmavsdk_1_1_follow_me_1a2b3f334ea72fd84d9e925fb3756451d8ac85a251cc457840f1e032f1b733e9398"></span> `Timeout` | Request timed out. 
<span id="classmavsdk_1_1_follow_me_1a2b3f334ea72fd84d9e925fb3756451d8a2b2c350e9a20a57773a9efa14c8c36e5"></span> `NotActive` | [FollowMe](classmavsdk_1_1_follow_me.md) is not active. 
<span id="classmavsdk_1_1_follow_me_1a2b3f334ea72fd84d9e925fb3756451d8a0287d8ed2f10c2d6492478b92b5e7e21"></span> `SetConfigFailed` | Failed to set [FollowMe](classmavsdk_1_1_follow_me.md) configuration. 

## Member Function Documentation


### get_config() {#classmavsdk_1_1_follow_me_1aca2e599cd6fb889b9f80dc7a9da57ee9}
```cpp
FollowMe::Config mavsdk::FollowMe::get_config() const
```


Get current configuration.

This function is blocking.

**Returns**

&emsp;[FollowMe::Config](structmavsdk_1_1_follow_me_1_1_config.md) - Result of request.

### set_config() {#classmavsdk_1_1_follow_me_1aa76aab9a21bc3ae475bee6a55c0e4d30}
```cpp
Result mavsdk::FollowMe::set_config(Config config) const
```


Apply configuration by sending it to the system.

This function is blocking.

**Parameters**

* [Config](structmavsdk_1_1_follow_me_1_1_config.md) **config** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_follow_me.md#classmavsdk_1_1_follow_me_1a2b3f334ea72fd84d9e925fb3756451d8) - Result of request.

### is_active() {#classmavsdk_1_1_follow_me_1a48ab77939257e52159bd9ed19335a7de}
```cpp
bool mavsdk::FollowMe::is_active() const
```


Check if [FollowMe](classmavsdk_1_1_follow_me.md) is active.

This function is blocking.

**Returns**

&emsp;bool - Result of request.

### set_target_location() {#classmavsdk_1_1_follow_me_1a1a99e282472235f726bfde430873ffd5}
```cpp
Result mavsdk::FollowMe::set_target_location(TargetLocation location) const
```


Set location of the moving target.

This function is blocking.

**Parameters**

* [TargetLocation](structmavsdk_1_1_follow_me_1_1_target_location.md) **location** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_follow_me.md#classmavsdk_1_1_follow_me_1a2b3f334ea72fd84d9e925fb3756451d8) - Result of request.

### get_last_location() {#classmavsdk_1_1_follow_me_1af2a1af346ee2fa7761b58b406e9e6e0c}
```cpp
FollowMe::TargetLocation mavsdk::FollowMe::get_last_location() const
```


Get the last location of the target.

This function is blocking.

**Returns**

&emsp;[FollowMe::TargetLocation](structmavsdk_1_1_follow_me_1_1_target_location.md) - Result of request.

### start() {#classmavsdk_1_1_follow_me_1a4b6ae3ec1ff07d8b3a79038e04992003}
```cpp
Result mavsdk::FollowMe::start() const
```


Start [FollowMe](classmavsdk_1_1_follow_me.md) mode.

This function is blocking.

**Returns**

&emsp;[Result](classmavsdk_1_1_follow_me.md#classmavsdk_1_1_follow_me_1a2b3f334ea72fd84d9e925fb3756451d8) - Result of request.

### stop() {#classmavsdk_1_1_follow_me_1a202a7b9edf56d9b883c974a09c14ba7d}
```cpp
Result mavsdk::FollowMe::stop() const
```


Stop [FollowMe](classmavsdk_1_1_follow_me.md) mode.

This function is blocking.

**Returns**

&emsp;[Result](classmavsdk_1_1_follow_me.md#classmavsdk_1_1_follow_me_1a2b3f334ea72fd84d9e925fb3756451d8) - Result of request.

### operator=() {#classmavsdk_1_1_follow_me_1a6292f6dd2c91cedde0e3b82952d83510}
```cpp
const FollowMe& mavsdk::FollowMe::operator=(const FollowMe &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [FollowMe](classmavsdk_1_1_follow_me.md)&  - 

**Returns**

&emsp;const [FollowMe](classmavsdk_1_1_follow_me.md) & - 