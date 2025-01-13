# mavsdk::FtpServer Class Reference
`#include: ftp_server.h`

----


Provide files or directories to transfer. 


## Public Types


Type | Description
--- | ---
enum [Result](#classmavsdk_1_1_ftp_server_1a24027b6ade1f681dd191a81c25653763) | Possible results returned for FTP server requests.
std::function< void([Result](classmavsdk_1_1_ftp_server.md#classmavsdk_1_1_ftp_server_1a24027b6ade1f681dd191a81c25653763))> [ResultCallback](#classmavsdk_1_1_ftp_server_1a8de8a273470df377e1a665c3b14c866a) | Callback type for asynchronous [FtpServer](classmavsdk_1_1_ftp_server.md) calls.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [FtpServer](#classmavsdk_1_1_ftp_server_1aa22c68e2fe9a3ac22c4ef55f2636ddd3) (std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > server_component) | Constructor. Creates the plugin for a [ServerComponent](classmavsdk_1_1_server_component.md) instance.
&nbsp; | [~FtpServer](#classmavsdk_1_1_ftp_server_1a685d6499b3fbc7d8b4fac6f8031e7e99) () override | Destructor (internal use only).
&nbsp; | [FtpServer](#classmavsdk_1_1_ftp_server_1afed6d9c192b299e27d5e0014e63c275f) (const [FtpServer](classmavsdk_1_1_ftp_server.md) & other) | Copy constructor.
[Result](classmavsdk_1_1_ftp_server.md#classmavsdk_1_1_ftp_server_1a24027b6ade1f681dd191a81c25653763) | [set_root_dir](#classmavsdk_1_1_ftp_server_1aa79196f0d3cd0bc178a57711252ed8f3) (std::string path)const | Set root directory.
const [FtpServer](classmavsdk_1_1_ftp_server.md) & | [operator=](#classmavsdk_1_1_ftp_server_1aa3822f6508adad0b930cbca3be4fd5c5) (const [FtpServer](classmavsdk_1_1_ftp_server.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### FtpServer() {#classmavsdk_1_1_ftp_server_1aa22c68e2fe9a3ac22c4ef55f2636ddd3}
```cpp
mavsdk::FtpServer::FtpServer(std::shared_ptr< ServerComponent > server_component)
```


Constructor. Creates the plugin for a [ServerComponent](classmavsdk_1_1_server_component.md) instance.

The plugin is typically created as shown below: 

```cpp
auto ftp_server = FtpServer(server_component);
```

**Parameters**

* std::shared_ptr< [ServerComponent](classmavsdk_1_1_server_component.md) > **server_component** - The [ServerComponent](classmavsdk_1_1_server_component.md) instance associated with this server plugin.

### ~FtpServer() {#classmavsdk_1_1_ftp_server_1a685d6499b3fbc7d8b4fac6f8031e7e99}
```cpp
mavsdk::FtpServer::~FtpServer() override
```


Destructor (internal use only).


### FtpServer() {#classmavsdk_1_1_ftp_server_1afed6d9c192b299e27d5e0014e63c275f}
```cpp
mavsdk::FtpServer::FtpServer(const FtpServer &other)
```


Copy constructor.


**Parameters**

* const [FtpServer](classmavsdk_1_1_ftp_server.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_ftp_server_1a8de8a273470df377e1a665c3b14c866a}

```cpp
using mavsdk::FtpServer::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [FtpServer](classmavsdk_1_1_ftp_server.md) calls.


## Member Enumeration Documentation


### enum Result {#classmavsdk_1_1_ftp_server_1a24027b6ade1f681dd191a81c25653763}


Possible results returned for FTP server requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_ftp_server_1a24027b6ade1f681dd191a81c25653763a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_ftp_server_1a24027b6ade1f681dd191a81c25653763a505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Request succeeded. 
<span id="classmavsdk_1_1_ftp_server_1a24027b6ade1f681dd191a81c25653763ac47c93cf49de211bba5d62d65225f128"></span> `DoesNotExist` | Directory does not exist. 
<span id="classmavsdk_1_1_ftp_server_1a24027b6ade1f681dd191a81c25653763ad8a942ef2b04672adfafef0ad817a407"></span> `Busy` | Operations in progress. 

## Member Function Documentation


### set_root_dir() {#classmavsdk_1_1_ftp_server_1aa79196f0d3cd0bc178a57711252ed8f3}
```cpp
Result mavsdk::FtpServer::set_root_dir(std::string path) const
```


Set root directory.

This is the directory that can then be accessed by a client. The directory needs to exist when this is called. The permissions are the same as the file permission for the user running the server. The root directory can't be changed while an FTP process is in progress.


This function is blocking.

**Parameters**

* std::string **path** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_ftp_server.md#classmavsdk_1_1_ftp_server_1a24027b6ade1f681dd191a81c25653763) - Result of request.

### operator=() {#classmavsdk_1_1_ftp_server_1aa3822f6508adad0b930cbca3be4fd5c5}
```cpp
const FtpServer& mavsdk::FtpServer::operator=(const FtpServer &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [FtpServer](classmavsdk_1_1_ftp_server.md)&  - 

**Returns**

&emsp;const [FtpServer](classmavsdk_1_1_ftp_server.md) & - 