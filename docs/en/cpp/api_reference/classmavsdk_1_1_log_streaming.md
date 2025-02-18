# mavsdk::LogStreaming Class Reference
`#include: log_streaming.h`

----


Provide log streaming data. 


## Data Structures


struct [LogStreamingRaw](structmavsdk_1_1_log_streaming_1_1_log_streaming_raw.md)

## Public Types


Type | Description
--- | ---
enum [Result](#classmavsdk_1_1_log_streaming_1a29f3219817d4aefebf1a0d59cda39c17) | Possible results returned for logging requests.
std::function< void([Result](classmavsdk_1_1_log_streaming.md#classmavsdk_1_1_log_streaming_1a29f3219817d4aefebf1a0d59cda39c17))> [ResultCallback](#classmavsdk_1_1_log_streaming_1af5383eb2fda579379ab3c722d074f818) | Callback type for asynchronous [LogStreaming](classmavsdk_1_1_log_streaming.md) calls.
std::function< void([LogStreamingRaw](structmavsdk_1_1_log_streaming_1_1_log_streaming_raw.md))> [LogStreamingRawCallback](#classmavsdk_1_1_log_streaming_1af3197cf6a041f85c6ea879dcccbc354a) | Callback type for subscribe_log_streaming_raw.
[Handle](classmavsdk_1_1_handle.md)< [LogStreamingRaw](structmavsdk_1_1_log_streaming_1_1_log_streaming_raw.md) > [LogStreamingRawHandle](#classmavsdk_1_1_log_streaming_1af245af3fad21ca1198987babbd728925) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_log_streaming_raw.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [LogStreaming](#classmavsdk_1_1_log_streaming_1a16a64f4ea20ba98b5151b588fb84f5ee) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [LogStreaming](#classmavsdk_1_1_log_streaming_1a8be3bbd9068f03c390fc78dd426058b4) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~LogStreaming](#classmavsdk_1_1_log_streaming_1a2b99fc2e1f535afb8f107430d1fb1926) () override | Destructor (internal use only).
&nbsp; | [LogStreaming](#classmavsdk_1_1_log_streaming_1a2e2dddc59ab55af1cf57479dc73c9842) (const [LogStreaming](classmavsdk_1_1_log_streaming.md) & other) | Copy constructor.
void | [start_log_streaming_async](#classmavsdk_1_1_log_streaming_1a00797f79a2d17db031be24184ca326ea) (const [ResultCallback](classmavsdk_1_1_log_streaming.md#classmavsdk_1_1_log_streaming_1af5383eb2fda579379ab3c722d074f818) callback) | Start streaming logging data.
[Result](classmavsdk_1_1_log_streaming.md#classmavsdk_1_1_log_streaming_1a29f3219817d4aefebf1a0d59cda39c17) | [start_log_streaming](#classmavsdk_1_1_log_streaming_1aad5edb63b5b66bc9b4189c298ab5f957) () const | Start streaming logging data.
void | [stop_log_streaming_async](#classmavsdk_1_1_log_streaming_1a6d4db2bb2a2c869fcbc11d6c2124f02c) (const [ResultCallback](classmavsdk_1_1_log_streaming.md#classmavsdk_1_1_log_streaming_1af5383eb2fda579379ab3c722d074f818) callback) | Stop streaming logging data.
[Result](classmavsdk_1_1_log_streaming.md#classmavsdk_1_1_log_streaming_1a29f3219817d4aefebf1a0d59cda39c17) | [stop_log_streaming](#classmavsdk_1_1_log_streaming_1a656b17357693c83e579104c5fd3281d8) () const | Stop streaming logging data.
[LogStreamingRawHandle](classmavsdk_1_1_log_streaming.md#classmavsdk_1_1_log_streaming_1af245af3fad21ca1198987babbd728925) | [subscribe_log_streaming_raw](#classmavsdk_1_1_log_streaming_1afeef206b7843bdd3d1a85806764b2110) (const [LogStreamingRawCallback](classmavsdk_1_1_log_streaming.md#classmavsdk_1_1_log_streaming_1af3197cf6a041f85c6ea879dcccbc354a) & callback) | Subscribe to logging messages.
void | [unsubscribe_log_streaming_raw](#classmavsdk_1_1_log_streaming_1a31b33a0318907668f6939cea52f02d5b) ([LogStreamingRawHandle](classmavsdk_1_1_log_streaming.md#classmavsdk_1_1_log_streaming_1af245af3fad21ca1198987babbd728925) handle) | Unsubscribe from subscribe_log_streaming_raw.
const [LogStreaming](classmavsdk_1_1_log_streaming.md) & | [operator=](#classmavsdk_1_1_log_streaming_1a51a20fe975a0cde08e077fe89d9b3a99) (const [LogStreaming](classmavsdk_1_1_log_streaming.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### LogStreaming() {#classmavsdk_1_1_log_streaming_1a16a64f4ea20ba98b5151b588fb84f5ee}
```cpp
mavsdk::LogStreaming::LogStreaming(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto log_streaming = LogStreaming(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### LogStreaming() {#classmavsdk_1_1_log_streaming_1a8be3bbd9068f03c390fc78dd426058b4}
```cpp
mavsdk::LogStreaming::LogStreaming(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto log_streaming = LogStreaming(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~LogStreaming() {#classmavsdk_1_1_log_streaming_1a2b99fc2e1f535afb8f107430d1fb1926}
```cpp
mavsdk::LogStreaming::~LogStreaming() override
```


Destructor (internal use only).


### LogStreaming() {#classmavsdk_1_1_log_streaming_1a2e2dddc59ab55af1cf57479dc73c9842}
```cpp
mavsdk::LogStreaming::LogStreaming(const LogStreaming &other)
```


Copy constructor.


**Parameters**

* const [LogStreaming](classmavsdk_1_1_log_streaming.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_log_streaming_1af5383eb2fda579379ab3c722d074f818}

```cpp
using mavsdk::LogStreaming::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [LogStreaming](classmavsdk_1_1_log_streaming.md) calls.


### typedef LogStreamingRawCallback {#classmavsdk_1_1_log_streaming_1af3197cf6a041f85c6ea879dcccbc354a}

```cpp
using mavsdk::LogStreaming::LogStreamingRawCallback =  std::function<void(LogStreamingRaw)>
```


Callback type for subscribe_log_streaming_raw.


### typedef LogStreamingRawHandle {#classmavsdk_1_1_log_streaming_1af245af3fad21ca1198987babbd728925}

```cpp
using mavsdk::LogStreaming::LogStreamingRawHandle =  Handle<LogStreamingRaw>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_log_streaming_raw.


## Member Enumeration Documentation


### enum Result {#classmavsdk_1_1_log_streaming_1a29f3219817d4aefebf1a0d59cda39c17}


Possible results returned for logging requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_log_streaming_1a29f3219817d4aefebf1a0d59cda39c17a505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Request succeeded. 
<span id="classmavsdk_1_1_log_streaming_1a29f3219817d4aefebf1a0d59cda39c17a1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system connected. 
<span id="classmavsdk_1_1_log_streaming_1a29f3219817d4aefebf1a0d59cda39c17a094a6f6b0868122a9dd008cb91c083e4"></span> `ConnectionError` | Connection error. 
<span id="classmavsdk_1_1_log_streaming_1a29f3219817d4aefebf1a0d59cda39c17ad8a942ef2b04672adfafef0ad817a407"></span> `Busy` | [System](classmavsdk_1_1_system.md) busy. 
<span id="classmavsdk_1_1_log_streaming_1a29f3219817d4aefebf1a0d59cda39c17a3398e12855176d55f43d53e04f472c8a"></span> `CommandDenied` | Command denied. 
<span id="classmavsdk_1_1_log_streaming_1a29f3219817d4aefebf1a0d59cda39c17ac85a251cc457840f1e032f1b733e9398"></span> `Timeout` | Timeout. 
<span id="classmavsdk_1_1_log_streaming_1a29f3219817d4aefebf1a0d59cda39c17ab4080bdf74febf04d578ff105cce9d3f"></span> `Unsupported` | Unsupported. 
<span id="classmavsdk_1_1_log_streaming_1a29f3219817d4aefebf1a0d59cda39c17a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown error. 

## Member Function Documentation


### start_log_streaming_async() {#classmavsdk_1_1_log_streaming_1a00797f79a2d17db031be24184ca326ea}
```cpp
void mavsdk::LogStreaming::start_log_streaming_async(const ResultCallback callback)
```


Start streaming logging data.

This function is non-blocking. See 'start_log_streaming' for the blocking counterpart.

**Parameters**

* const [ResultCallback](classmavsdk_1_1_log_streaming.md#classmavsdk_1_1_log_streaming_1af5383eb2fda579379ab3c722d074f818) **callback** - 

### start_log_streaming() {#classmavsdk_1_1_log_streaming_1aad5edb63b5b66bc9b4189c298ab5f957}
```cpp
Result mavsdk::LogStreaming::start_log_streaming() const
```


Start streaming logging data.

This function is blocking. See 'start_log_streaming_async' for the non-blocking counterpart.

**Returns**

&emsp;[Result](classmavsdk_1_1_log_streaming.md#classmavsdk_1_1_log_streaming_1a29f3219817d4aefebf1a0d59cda39c17) - Result of request.

### stop_log_streaming_async() {#classmavsdk_1_1_log_streaming_1a6d4db2bb2a2c869fcbc11d6c2124f02c}
```cpp
void mavsdk::LogStreaming::stop_log_streaming_async(const ResultCallback callback)
```


Stop streaming logging data.

This function is non-blocking. See 'stop_log_streaming' for the blocking counterpart.

**Parameters**

* const [ResultCallback](classmavsdk_1_1_log_streaming.md#classmavsdk_1_1_log_streaming_1af5383eb2fda579379ab3c722d074f818) **callback** - 

### stop_log_streaming() {#classmavsdk_1_1_log_streaming_1a656b17357693c83e579104c5fd3281d8}
```cpp
Result mavsdk::LogStreaming::stop_log_streaming() const
```


Stop streaming logging data.

This function is blocking. See 'stop_log_streaming_async' for the non-blocking counterpart.

**Returns**

&emsp;[Result](classmavsdk_1_1_log_streaming.md#classmavsdk_1_1_log_streaming_1a29f3219817d4aefebf1a0d59cda39c17) - Result of request.

### subscribe_log_streaming_raw() {#classmavsdk_1_1_log_streaming_1afeef206b7843bdd3d1a85806764b2110}
```cpp
LogStreamingRawHandle mavsdk::LogStreaming::subscribe_log_streaming_raw(const LogStreamingRawCallback &callback)
```


Subscribe to logging messages.


**Parameters**

* const [LogStreamingRawCallback](classmavsdk_1_1_log_streaming.md#classmavsdk_1_1_log_streaming_1af3197cf6a041f85c6ea879dcccbc354a)& **callback** - 

**Returns**

&emsp;[LogStreamingRawHandle](classmavsdk_1_1_log_streaming.md#classmavsdk_1_1_log_streaming_1af245af3fad21ca1198987babbd728925) - 

### unsubscribe_log_streaming_raw() {#classmavsdk_1_1_log_streaming_1a31b33a0318907668f6939cea52f02d5b}
```cpp
void mavsdk::LogStreaming::unsubscribe_log_streaming_raw(LogStreamingRawHandle handle)
```


Unsubscribe from subscribe_log_streaming_raw.


**Parameters**

* [LogStreamingRawHandle](classmavsdk_1_1_log_streaming.md#classmavsdk_1_1_log_streaming_1af245af3fad21ca1198987babbd728925) **handle** - 

### operator=() {#classmavsdk_1_1_log_streaming_1a51a20fe975a0cde08e077fe89d9b3a99}
```cpp
const LogStreaming & mavsdk::LogStreaming::operator=(const LogStreaming &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [LogStreaming](classmavsdk_1_1_log_streaming.md)&  - 

**Returns**

&emsp;const [LogStreaming](classmavsdk_1_1_log_streaming.md) & - 