# mavsdk::ArmAuthorizerServer Class Reference
`#include: arm_authorizer_server.h`

----


Use arm authorization. 


## Public Types


Type | Description
--- | ---
enum [RejectionReason](#classmavsdk_1_1_arm_authorizer_server_1a8e2f877e83640591859db072cb26a6ee) | The rejection reason.
enum [Result](#classmavsdk_1_1_arm_authorizer_server_1aa8139980ee55b72cf04b69c453e04011) | The result.
std::function< void([Result](classmavsdk_1_1_arm_authorizer_server.md#classmavsdk_1_1_arm_authorizer_server_1aa8139980ee55b72cf04b69c453e04011))> [ResultCallback](#classmavsdk_1_1_arm_authorizer_server_1ab4c5e8114b66740bd4382d0daa3052d9) | Callback type for asynchronous [ArmAuthorizerServer](classmavsdk_1_1_arm_authorizer_server.md) calls.
std::function< void(uint32_t)> [ArmAuthorizationCallback](#classmavsdk_1_1_arm_authorizer_server_1ac608fecb1db7daea51078b1194c0ff86) | Callback type for subscribe_arm_authorization.
[Handle](classmavsdk_1_1_handle.md)< uint32_t > [ArmAuthorizationHandle](#classmavsdk_1_1_arm_authorizer_server_1af0c4f4d88ebf36ae6adeb7d2af163fa5) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_arm_authorization.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [ArmAuthorizerServer](#classmavsdk_1_1_arm_authorizer_server_1a7c43f4e97453fa762a233c766da48402) (std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > server_component) | Constructor. Creates the plugin for a [ServerComponent](classmavsdk_1_1_server_component.md) instance.
&nbsp; | [~ArmAuthorizerServer](#classmavsdk_1_1_arm_authorizer_server_1a5412419e4c6e16160928583551841e25) () override | Destructor (internal use only).
&nbsp; | [ArmAuthorizerServer](#classmavsdk_1_1_arm_authorizer_server_1a209d19bdc496402350967da7fd6a827e) (const [ArmAuthorizerServer](classmavsdk_1_1_arm_authorizer_server.md) & other) | Copy constructor.
[ArmAuthorizationHandle](classmavsdk_1_1_arm_authorizer_server.md#classmavsdk_1_1_arm_authorizer_server_1af0c4f4d88ebf36ae6adeb7d2af163fa5) | [subscribe_arm_authorization](#classmavsdk_1_1_arm_authorizer_server_1a334ede8582f9dfe196df01cc73293ab2) (const [ArmAuthorizationCallback](classmavsdk_1_1_arm_authorizer_server.md#classmavsdk_1_1_arm_authorizer_server_1ac608fecb1db7daea51078b1194c0ff86) & callback) | Subscribe to arm authorization request messages. Each request received should respond to using RespondArmAuthorization.
void | [unsubscribe_arm_authorization](#classmavsdk_1_1_arm_authorizer_server_1a07869a29c05428f346138016dd63dd22) ([ArmAuthorizationHandle](classmavsdk_1_1_arm_authorizer_server.md#classmavsdk_1_1_arm_authorizer_server_1af0c4f4d88ebf36ae6adeb7d2af163fa5) handle) | Unsubscribe from subscribe_arm_authorization.
[Result](classmavsdk_1_1_arm_authorizer_server.md#classmavsdk_1_1_arm_authorizer_server_1aa8139980ee55b72cf04b69c453e04011) | [accept_arm_authorization](#classmavsdk_1_1_arm_authorizer_server_1afd9dd0c6c1058c06773f63b220c24d39) (int32_t valid_time_s)const | Authorize arm for the specific time.
[Result](classmavsdk_1_1_arm_authorizer_server.md#classmavsdk_1_1_arm_authorizer_server_1aa8139980ee55b72cf04b69c453e04011) | [reject_arm_authorization](#classmavsdk_1_1_arm_authorizer_server_1a99f8b59b8d6db4b074f4df5018c9d786) (bool temporarily, [RejectionReason](classmavsdk_1_1_arm_authorizer_server.md#classmavsdk_1_1_arm_authorizer_server_1a8e2f877e83640591859db072cb26a6ee) reason, int32_t extra_info)const | Reject arm authorization request.
const [ArmAuthorizerServer](classmavsdk_1_1_arm_authorizer_server.md) & | [operator=](#classmavsdk_1_1_arm_authorizer_server_1ae44588d79926001c8e33024ce2221b06) (const [ArmAuthorizerServer](classmavsdk_1_1_arm_authorizer_server.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### ArmAuthorizerServer() {#classmavsdk_1_1_arm_authorizer_server_1a7c43f4e97453fa762a233c766da48402}
```cpp
mavsdk::ArmAuthorizerServer::ArmAuthorizerServer(std::shared_ptr< ServerComponent > server_component)
```


Constructor. Creates the plugin for a [ServerComponent](classmavsdk_1_1_server_component.md) instance.

The plugin is typically created as shown below: 

```cpp
auto arm_authorizer_server = ArmAuthorizerServer(server_component);
```

**Parameters**

* std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > **server_component** - The [ServerComponent](classmavsdk_1_1_server_component.md) instance associated with this server plugin.

### ~ArmAuthorizerServer() {#classmavsdk_1_1_arm_authorizer_server_1a5412419e4c6e16160928583551841e25}
```cpp
mavsdk::ArmAuthorizerServer::~ArmAuthorizerServer() override
```


Destructor (internal use only).


### ArmAuthorizerServer() {#classmavsdk_1_1_arm_authorizer_server_1a209d19bdc496402350967da7fd6a827e}
```cpp
mavsdk::ArmAuthorizerServer::ArmAuthorizerServer(const ArmAuthorizerServer &other)
```


Copy constructor.


**Parameters**

* const [ArmAuthorizerServer](classmavsdk_1_1_arm_authorizer_server.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_arm_authorizer_server_1ab4c5e8114b66740bd4382d0daa3052d9}

```cpp
using mavsdk::ArmAuthorizerServer::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [ArmAuthorizerServer](classmavsdk_1_1_arm_authorizer_server.md) calls.


### typedef ArmAuthorizationCallback {#classmavsdk_1_1_arm_authorizer_server_1ac608fecb1db7daea51078b1194c0ff86}

```cpp
using mavsdk::ArmAuthorizerServer::ArmAuthorizationCallback =  std::function<void(uint32_t)>
```


Callback type for subscribe_arm_authorization.


### typedef ArmAuthorizationHandle {#classmavsdk_1_1_arm_authorizer_server_1af0c4f4d88ebf36ae6adeb7d2af163fa5}

```cpp
using mavsdk::ArmAuthorizerServer::ArmAuthorizationHandle =  Handle<uint32_t>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_arm_authorization.


## Member Enumeration Documentation


### enum RejectionReason {#classmavsdk_1_1_arm_authorizer_server_1a8e2f877e83640591859db072cb26a6ee}


The rejection reason.


Value | Description
--- | ---
<span id="classmavsdk_1_1_arm_authorizer_server_1a8e2f877e83640591859db072cb26a6eea8045a0a6c688b0635e3caccc408a1446"></span> `Generic` | Not a specific reason. 
<span id="classmavsdk_1_1_arm_authorizer_server_1a8e2f877e83640591859db072cb26a6eea6adf97f83acf6453d4a6a4b1070f3754"></span> `None` | Authorizer will send the error as string to GCS. 
<span id="classmavsdk_1_1_arm_authorizer_server_1a8e2f877e83640591859db072cb26a6eea84a0b14553789948d0301a2aa796b388"></span> `InvalidWaypoint` | At least one waypoint have a invalid value. 
<span id="classmavsdk_1_1_arm_authorizer_server_1a8e2f877e83640591859db072cb26a6eeac85a251cc457840f1e032f1b733e9398"></span> `Timeout` | Timeout in the authorizer process(in case it depends on network). 
<span id="classmavsdk_1_1_arm_authorizer_server_1a8e2f877e83640591859db072cb26a6eea7f14f36daf6cd8c8266a28ae69a39e90"></span> `AirspaceInUse` | Airspace of the mission in use by another vehicle, second result parameter can have the waypoint id that caused it to be denied.. 
<span id="classmavsdk_1_1_arm_authorizer_server_1a8e2f877e83640591859db072cb26a6eea4579786d67f36ba9295f37fd40151cf2"></span> `BadWeather` | Weather is not good to fly. 

### enum Result {#classmavsdk_1_1_arm_authorizer_server_1aa8139980ee55b72cf04b69c453e04011}


The result.


Value | Description
--- | ---
<span id="classmavsdk_1_1_arm_authorizer_server_1aa8139980ee55b72cf04b69c453e04011a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_arm_authorizer_server_1aa8139980ee55b72cf04b69c453e04011a505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Command accepted. 
<span id="classmavsdk_1_1_arm_authorizer_server_1aa8139980ee55b72cf04b69c453e04011ad7c8c85bf79bbe1b7188497c32c3b0ca"></span> `Failed` | Command failed. 

## Member Function Documentation


### subscribe_arm_authorization() {#classmavsdk_1_1_arm_authorizer_server_1a334ede8582f9dfe196df01cc73293ab2}
```cpp
ArmAuthorizationHandle mavsdk::ArmAuthorizerServer::subscribe_arm_authorization(const ArmAuthorizationCallback &callback)
```


Subscribe to arm authorization request messages. Each request received should respond to using RespondArmAuthorization.


**Parameters**

* const [ArmAuthorizationCallback](classmavsdk_1_1_arm_authorizer_server.md#classmavsdk_1_1_arm_authorizer_server_1ac608fecb1db7daea51078b1194c0ff86)& **callback** - 

**Returns**

&emsp;[ArmAuthorizationHandle](classmavsdk_1_1_arm_authorizer_server.md#classmavsdk_1_1_arm_authorizer_server_1af0c4f4d88ebf36ae6adeb7d2af163fa5) - 

### unsubscribe_arm_authorization() {#classmavsdk_1_1_arm_authorizer_server_1a07869a29c05428f346138016dd63dd22}
```cpp
void mavsdk::ArmAuthorizerServer::unsubscribe_arm_authorization(ArmAuthorizationHandle handle)
```


Unsubscribe from subscribe_arm_authorization.


**Parameters**

* [ArmAuthorizationHandle](classmavsdk_1_1_arm_authorizer_server.md#classmavsdk_1_1_arm_authorizer_server_1af0c4f4d88ebf36ae6adeb7d2af163fa5) **handle** - 

### accept_arm_authorization() {#classmavsdk_1_1_arm_authorizer_server_1afd9dd0c6c1058c06773f63b220c24d39}
```cpp
Result mavsdk::ArmAuthorizerServer::accept_arm_authorization(int32_t valid_time_s) const
```


Authorize arm for the specific time.

This function is blocking.

**Parameters**

* int32_t **valid_time_s** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_arm_authorizer_server.md#classmavsdk_1_1_arm_authorizer_server_1aa8139980ee55b72cf04b69c453e04011) - Result of request.

### reject_arm_authorization() {#classmavsdk_1_1_arm_authorizer_server_1a99f8b59b8d6db4b074f4df5018c9d786}
```cpp
Result mavsdk::ArmAuthorizerServer::reject_arm_authorization(bool temporarily, RejectionReason reason, int32_t extra_info) const
```


Reject arm authorization request.

This function is blocking.

**Parameters**

* bool **temporarily** - 
* [RejectionReason](classmavsdk_1_1_arm_authorizer_server.md#classmavsdk_1_1_arm_authorizer_server_1a8e2f877e83640591859db072cb26a6ee) **reason** - 
* int32_t **extra_info** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_arm_authorizer_server.md#classmavsdk_1_1_arm_authorizer_server_1aa8139980ee55b72cf04b69c453e04011) - Result of request.

### operator=() {#classmavsdk_1_1_arm_authorizer_server_1ae44588d79926001c8e33024ce2221b06}
```cpp
const ArmAuthorizerServer & mavsdk::ArmAuthorizerServer::operator=(const ArmAuthorizerServer &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [ArmAuthorizerServer](classmavsdk_1_1_arm_authorizer_server.md)&  - 

**Returns**

&emsp;const [ArmAuthorizerServer](classmavsdk_1_1_arm_authorizer_server.md) & - 