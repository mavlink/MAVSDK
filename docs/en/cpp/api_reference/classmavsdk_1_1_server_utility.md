# mavsdk::ServerUtility Class Reference
`#include: server_utility.h`

----


Utility for onboard MAVSDK instances for common "server" tasks. 


## Public Types


Type | Description
--- | ---
enum [StatusTextType](#classmavsdk_1_1_server_utility_1a763ddc41251f992acec6af7f19287233) | Status types.
enum [Result](#classmavsdk_1_1_server_utility_1accdda519179f5fc6fe946a727f75f468) | Possible results returned for server utility requests.
std::function< void([Result](classmavsdk_1_1_server_utility.md#classmavsdk_1_1_server_utility_1accdda519179f5fc6fe946a727f75f468))> [ResultCallback](#classmavsdk_1_1_server_utility_1acbb4204dda65430ee20a802fe13cc199) | Callback type for asynchronous [ServerUtility](classmavsdk_1_1_server_utility.md) calls.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [ServerUtility](#classmavsdk_1_1_server_utility_1af6a56f1c96b29e033c4412c669e02060) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [ServerUtility](#classmavsdk_1_1_server_utility_1a54e7ef77c7ff8bee6c8f3e8d5cc0864c) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~ServerUtility](#classmavsdk_1_1_server_utility_1a3f5fe15b02bcf41520a94feb2fc51fce) () override | Destructor (internal use only).
&nbsp; | [ServerUtility](#classmavsdk_1_1_server_utility_1a9cbd0e7b1b19b21d98ce7e914ea03f95) (const [ServerUtility](classmavsdk_1_1_server_utility.md) & other) | Copy constructor.
[Result](classmavsdk_1_1_server_utility.md#classmavsdk_1_1_server_utility_1accdda519179f5fc6fe946a727f75f468) | [send_status_text](#classmavsdk_1_1_server_utility_1a7b7bb0b568e94575615086a86a625ae1) ([StatusTextType](classmavsdk_1_1_server_utility.md#classmavsdk_1_1_server_utility_1a763ddc41251f992acec6af7f19287233) type, std::string text)const | Sends a statustext.
const [ServerUtility](classmavsdk_1_1_server_utility.md) & | [operator=](#classmavsdk_1_1_server_utility_1a11fcb91df55711ecb78d8687b1d47472) (const [ServerUtility](classmavsdk_1_1_server_utility.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### ServerUtility() {#classmavsdk_1_1_server_utility_1af6a56f1c96b29e033c4412c669e02060}
```cpp
mavsdk::ServerUtility::ServerUtility(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto server_utility = ServerUtility(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### ServerUtility() {#classmavsdk_1_1_server_utility_1a54e7ef77c7ff8bee6c8f3e8d5cc0864c}
```cpp
mavsdk::ServerUtility::ServerUtility(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto server_utility = ServerUtility(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~ServerUtility() {#classmavsdk_1_1_server_utility_1a3f5fe15b02bcf41520a94feb2fc51fce}
```cpp
mavsdk::ServerUtility::~ServerUtility() override
```


Destructor (internal use only).


### ServerUtility() {#classmavsdk_1_1_server_utility_1a9cbd0e7b1b19b21d98ce7e914ea03f95}
```cpp
mavsdk::ServerUtility::ServerUtility(const ServerUtility &other)
```


Copy constructor.


**Parameters**

* const [ServerUtility](classmavsdk_1_1_server_utility.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_server_utility_1acbb4204dda65430ee20a802fe13cc199}

```cpp
using mavsdk::ServerUtility::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [ServerUtility](classmavsdk_1_1_server_utility.md) calls.


## Member Enumeration Documentation


### enum StatusTextType {#classmavsdk_1_1_server_utility_1a763ddc41251f992acec6af7f19287233}


Status types.


Value | Description
--- | ---
<span id="classmavsdk_1_1_server_utility_1a763ddc41251f992acec6af7f19287233aa603905470e2a5b8c13e96b579ef0dba"></span> `Debug` | Debug. 
<span id="classmavsdk_1_1_server_utility_1a763ddc41251f992acec6af7f19287233a4059b0251f66a18cb56f544728796875"></span> `Info` | Information. 
<span id="classmavsdk_1_1_server_utility_1a763ddc41251f992acec6af7f19287233a24efa7ee4511563b16144f39706d594f"></span> `Notice` | Notice. 
<span id="classmavsdk_1_1_server_utility_1a763ddc41251f992acec6af7f19287233a0eaadb4fcb48a0a0ed7bc9868be9fbaa"></span> `Warning` | Warning. 
<span id="classmavsdk_1_1_server_utility_1a763ddc41251f992acec6af7f19287233a902b0d55fddef6f8d651fe1035b7d4bd"></span> `Error` | Error. 
<span id="classmavsdk_1_1_server_utility_1a763ddc41251f992acec6af7f19287233a278d01e5af56273bae1bb99a98b370cd"></span> `Critical` | Critical. 
<span id="classmavsdk_1_1_server_utility_1a763ddc41251f992acec6af7f19287233ab92071d61c88498171928745ca53078b"></span> `Alert` | Alert. 
<span id="classmavsdk_1_1_server_utility_1a763ddc41251f992acec6af7f19287233aa3fa706f20bc0b7858b7ae6932261940"></span> `Emergency` | Emergency. 

### enum Result {#classmavsdk_1_1_server_utility_1accdda519179f5fc6fe946a727f75f468}


Possible results returned for server utility requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_server_utility_1accdda519179f5fc6fe946a727f75f468a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_server_utility_1accdda519179f5fc6fe946a727f75f468a505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Request succeeded. 
<span id="classmavsdk_1_1_server_utility_1accdda519179f5fc6fe946a727f75f468a1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system is connected. 
<span id="classmavsdk_1_1_server_utility_1accdda519179f5fc6fe946a727f75f468a094a6f6b0868122a9dd008cb91c083e4"></span> `ConnectionError` | Connection error. 
<span id="classmavsdk_1_1_server_utility_1accdda519179f5fc6fe946a727f75f468a253ca7dd096ee0956cccee4d376cab8b"></span> `InvalidArgument` | Invalid argument. 

## Member Function Documentation


### send_status_text() {#classmavsdk_1_1_server_utility_1a7b7bb0b568e94575615086a86a625ae1}
```cpp
Result mavsdk::ServerUtility::send_status_text(StatusTextType type, std::string text) const
```


Sends a statustext.

This function is blocking.

**Parameters**

* [StatusTextType](classmavsdk_1_1_server_utility.md#classmavsdk_1_1_server_utility_1a763ddc41251f992acec6af7f19287233) **type** - 
* std::string **text** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_server_utility.md#classmavsdk_1_1_server_utility_1accdda519179f5fc6fe946a727f75f468) - Result of request.

### operator=() {#classmavsdk_1_1_server_utility_1a11fcb91df55711ecb78d8687b1d47472}
```cpp
const ServerUtility& mavsdk::ServerUtility::operator=(const ServerUtility &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [ServerUtility](classmavsdk_1_1_server_utility.md)&  - 

**Returns**

&emsp;const [ServerUtility](classmavsdk_1_1_server_utility.md) & - 