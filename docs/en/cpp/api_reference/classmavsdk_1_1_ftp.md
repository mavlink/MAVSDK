# mavsdk::Ftp Class Reference
`#include: ftp.h`

----


Implements file transfer functionality using MAVLink FTP. 


## Data Structures


struct [ProgressData](structmavsdk_1_1_ftp_1_1_progress_data.md)

## Public Types


Type | Description
--- | ---
enum [Result](#classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4) | Possible results returned for FTP commands.
std::function< void([Result](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4))> [ResultCallback](#classmavsdk_1_1_ftp_1a04a12a1ab954b24a54570300f89486b0) | Callback type for asynchronous [Ftp](classmavsdk_1_1_ftp.md) calls.
std::function< void([Result](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4), [ProgressData](structmavsdk_1_1_ftp_1_1_progress_data.md))> [DownloadCallback](#classmavsdk_1_1_ftp_1a7c8e0377726e349a8a4e12495db42c75) | Callback type for download_async.
std::function< void([Result](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4), [ProgressData](structmavsdk_1_1_ftp_1_1_progress_data.md))> [UploadCallback](#classmavsdk_1_1_ftp_1a50ffb77f0730267f499656d40291f5a3) | Callback type for upload_async.
std::function< void([Result](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4), std::vector< std::string >)> [ListDirectoryCallback](#classmavsdk_1_1_ftp_1a87a77c4e013a8665017504a550d876b7) | Callback type for list_directory_async.
std::function< void([Result](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4), bool)> [AreFilesIdenticalCallback](#classmavsdk_1_1_ftp_1abe24e99f7141a234206f8952d2f61318) | Callback type for are_files_identical_async.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [Ftp](#classmavsdk_1_1_ftp_1ad0b5909e631746c1dd5e2151c727e67f) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [Ftp](#classmavsdk_1_1_ftp_1a88cabb7b13f63e8285e7701292dac302) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~Ftp](#classmavsdk_1_1_ftp_1a9a2492d3b79cde5cd3f5a09eadbc40a3) () override | Destructor (internal use only).
&nbsp; | [Ftp](#classmavsdk_1_1_ftp_1a7b7486356a7b04fb231c430e090c673e) (const [Ftp](classmavsdk_1_1_ftp.md) & other) | Copy constructor.
void | [download_async](#classmavsdk_1_1_ftp_1a0845245b8e1d0e74ed8961c90c96d1d3) (std::string remote_file_path, std::string local_dir, bool use_burst, const [DownloadCallback](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a7c8e0377726e349a8a4e12495db42c75) & callback) | Downloads a file to local directory.
void | [upload_async](#classmavsdk_1_1_ftp_1affe86a8b0a035109e7df9bd85c99f442) (std::string local_file_path, std::string remote_dir, const [UploadCallback](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a50ffb77f0730267f499656d40291f5a3) & callback) | Uploads local file to remote directory.
void | [list_directory_async](#classmavsdk_1_1_ftp_1abf5d83104a7293413b62e7a8ba1a0f2c) (std::string remote_dir, const [ListDirectoryCallback](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a87a77c4e013a8665017504a550d876b7) callback) | Lists items from a remote directory.
std::pair< [Result](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4), std::vector< std::string > > | [list_directory](#classmavsdk_1_1_ftp_1a2301aba586be9dd4569d984a58967ccc) (std::string remote_dir)const | Lists items from a remote directory.
void | [create_directory_async](#classmavsdk_1_1_ftp_1aa53fdd5c005bd4da3e0cb29d448689d3) (std::string remote_dir, const [ResultCallback](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a04a12a1ab954b24a54570300f89486b0) callback) | Creates a remote directory.
[Result](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4) | [create_directory](#classmavsdk_1_1_ftp_1ac6edf64fe63d5934b892c95778e1c548) (std::string remote_dir)const | Creates a remote directory.
void | [remove_directory_async](#classmavsdk_1_1_ftp_1a25823c7298dc2d081532dd094d013b8a) (std::string remote_dir, const [ResultCallback](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a04a12a1ab954b24a54570300f89486b0) callback) | Removes a remote directory.
[Result](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4) | [remove_directory](#classmavsdk_1_1_ftp_1a585e0877f7c129b15aed66771a31a446) (std::string remote_dir)const | Removes a remote directory.
void | [remove_file_async](#classmavsdk_1_1_ftp_1a3ecda69288fb860a8da1f8fad25af31c) (std::string remote_file_path, const [ResultCallback](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a04a12a1ab954b24a54570300f89486b0) callback) | Removes a remote file.
[Result](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4) | [remove_file](#classmavsdk_1_1_ftp_1a2700a813ec54b115cb9a349ba486d6bd) (std::string remote_file_path)const | Removes a remote file.
void | [rename_async](#classmavsdk_1_1_ftp_1afea8b15ad7b5748b0b5f68fd7103514a) (std::string remote_from_path, std::string remote_to_path, const [ResultCallback](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a04a12a1ab954b24a54570300f89486b0) callback) | Renames a remote file or remote directory.
[Result](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4) | [rename](#classmavsdk_1_1_ftp_1ac7411b38ea31f84f4f679b6b85313032) (std::string remote_from_path, std::string remote_to_path)const | Renames a remote file or remote directory.
void | [are_files_identical_async](#classmavsdk_1_1_ftp_1abddebf1a103b2853116e68f5f870e4a7) (std::string local_file_path, std::string remote_file_path, const [AreFilesIdenticalCallback](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1abe24e99f7141a234206f8952d2f61318) callback) | Compares a local file to a remote file using a CRC32 checksum.
std::pair< [Result](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4), bool > | [are_files_identical](#classmavsdk_1_1_ftp_1a1ace427243aedef4b0988a055fc414bf) (std::string local_file_path, std::string remote_file_path)const | Compares a local file to a remote file using a CRC32 checksum.
[Result](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4) | [set_target_compid](#classmavsdk_1_1_ftp_1ad93744cd25a89beffe0d72a090748229) (uint32_t compid)const | Set target component ID. By default it is the autopilot.
const [Ftp](classmavsdk_1_1_ftp.md) & | [operator=](#classmavsdk_1_1_ftp_1a01dc5f41d1e684a667d31c213728b376) (const [Ftp](classmavsdk_1_1_ftp.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### Ftp() {#classmavsdk_1_1_ftp_1ad0b5909e631746c1dd5e2151c727e67f}
```cpp
mavsdk::Ftp::Ftp(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto ftp = Ftp(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### Ftp() {#classmavsdk_1_1_ftp_1a88cabb7b13f63e8285e7701292dac302}
```cpp
mavsdk::Ftp::Ftp(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto ftp = Ftp(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~Ftp() {#classmavsdk_1_1_ftp_1a9a2492d3b79cde5cd3f5a09eadbc40a3}
```cpp
mavsdk::Ftp::~Ftp() override
```


Destructor (internal use only).


### Ftp() {#classmavsdk_1_1_ftp_1a7b7486356a7b04fb231c430e090c673e}
```cpp
mavsdk::Ftp::Ftp(const Ftp &other)
```


Copy constructor.


**Parameters**

* const [Ftp](classmavsdk_1_1_ftp.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_ftp_1a04a12a1ab954b24a54570300f89486b0}

```cpp
using mavsdk::Ftp::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [Ftp](classmavsdk_1_1_ftp.md) calls.


### typedef DownloadCallback {#classmavsdk_1_1_ftp_1a7c8e0377726e349a8a4e12495db42c75}

```cpp
using mavsdk::Ftp::DownloadCallback =  std::function<void(Result, ProgressData)>
```


Callback type for download_async.


### typedef UploadCallback {#classmavsdk_1_1_ftp_1a50ffb77f0730267f499656d40291f5a3}

```cpp
using mavsdk::Ftp::UploadCallback =  std::function<void(Result, ProgressData)>
```


Callback type for upload_async.


### typedef ListDirectoryCallback {#classmavsdk_1_1_ftp_1a87a77c4e013a8665017504a550d876b7}

```cpp
using mavsdk::Ftp::ListDirectoryCallback =  std::function<void(Result, std::vector<std::string>)>
```


Callback type for list_directory_async.


### typedef AreFilesIdenticalCallback {#classmavsdk_1_1_ftp_1abe24e99f7141a234206f8952d2f61318}

```cpp
using mavsdk::Ftp::AreFilesIdenticalCallback =  std::function<void(Result, bool)>
```


Callback type for are_files_identical_async.


## Member Enumeration Documentation


### enum Result {#classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4}


Possible results returned for FTP commands.


Value | Description
--- | ---
<span id="classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4a505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Success. 
<span id="classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4a10ac3d04253ef7e1ddc73e6091c0cd55"></span> `Next` | Intermediate message showing progress. 
<span id="classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4ac85a251cc457840f1e032f1b733e9398"></span> `Timeout` | Timeout. 
<span id="classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4ad8a942ef2b04672adfafef0ad817a407"></span> `Busy` | Operation is already in progress. 
<span id="classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4a1d9fef8a4733b0ea18ff683be51fc196"></span> `FileIoError` | File IO operation error. 
<span id="classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4aea1776502570f06ddc0569c079cb9ee2"></span> `FileExists` | File exists already. 
<span id="classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4aab7e9f2fa8b2404bc25061dd572e9797"></span> `FileDoesNotExist` | File does not exist. 
<span id="classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4a07b15a02a7ed4e09d46ea06d10d0e0cd"></span> `FileProtected` | File is write protected. 
<span id="classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4a627251310d3384b591e4138be21145d5"></span> `InvalidParameter` | Invalid parameter. 
<span id="classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4ab4080bdf74febf04d578ff105cce9d3f"></span> `Unsupported` | Unsupported command. 
<span id="classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4aca3da8f495e4e628912a7798655da6c2"></span> `ProtocolError` | General protocol error. 
<span id="classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4a1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system connected. 

## Member Function Documentation


### download_async() {#classmavsdk_1_1_ftp_1a0845245b8e1d0e74ed8961c90c96d1d3}
```cpp
void mavsdk::Ftp::download_async(std::string remote_file_path, std::string local_dir, bool use_burst, const DownloadCallback &callback)
```


Downloads a file to local directory.


**Parameters**

* std::string **remote_file_path** - 
* std::string **local_dir** - 
* bool **use_burst** - 
* const [DownloadCallback](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a7c8e0377726e349a8a4e12495db42c75)& **callback** - 

### upload_async() {#classmavsdk_1_1_ftp_1affe86a8b0a035109e7df9bd85c99f442}
```cpp
void mavsdk::Ftp::upload_async(std::string local_file_path, std::string remote_dir, const UploadCallback &callback)
```


Uploads local file to remote directory.


**Parameters**

* std::string **local_file_path** - 
* std::string **remote_dir** - 
* const [UploadCallback](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a50ffb77f0730267f499656d40291f5a3)& **callback** - 

### list_directory_async() {#classmavsdk_1_1_ftp_1abf5d83104a7293413b62e7a8ba1a0f2c}
```cpp
void mavsdk::Ftp::list_directory_async(std::string remote_dir, const ListDirectoryCallback callback)
```


Lists items from a remote directory.

This function is non-blocking. See 'list_directory' for the blocking counterpart.

**Parameters**

* std::string **remote_dir** - 
* const [ListDirectoryCallback](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a87a77c4e013a8665017504a550d876b7) **callback** - 

### list_directory() {#classmavsdk_1_1_ftp_1a2301aba586be9dd4569d984a58967ccc}
```cpp
std::pair<Result, std::vector<std::string> > mavsdk::Ftp::list_directory(std::string remote_dir) const
```


Lists items from a remote directory.

This function is blocking. See 'list_directory_async' for the non-blocking counterpart.

**Parameters**

* std::string **remote_dir** - 

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4), std::vector< std::string > > - Result of request.

### create_directory_async() {#classmavsdk_1_1_ftp_1aa53fdd5c005bd4da3e0cb29d448689d3}
```cpp
void mavsdk::Ftp::create_directory_async(std::string remote_dir, const ResultCallback callback)
```


Creates a remote directory.

This function is non-blocking. See 'create_directory' for the blocking counterpart.

**Parameters**

* std::string **remote_dir** - 
* const [ResultCallback](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a04a12a1ab954b24a54570300f89486b0) **callback** - 

### create_directory() {#classmavsdk_1_1_ftp_1ac6edf64fe63d5934b892c95778e1c548}
```cpp
Result mavsdk::Ftp::create_directory(std::string remote_dir) const
```


Creates a remote directory.

This function is blocking. See 'create_directory_async' for the non-blocking counterpart.

**Parameters**

* std::string **remote_dir** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4) - Result of request.

### remove_directory_async() {#classmavsdk_1_1_ftp_1a25823c7298dc2d081532dd094d013b8a}
```cpp
void mavsdk::Ftp::remove_directory_async(std::string remote_dir, const ResultCallback callback)
```


Removes a remote directory.

This function is non-blocking. See 'remove_directory' for the blocking counterpart.

**Parameters**

* std::string **remote_dir** - 
* const [ResultCallback](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a04a12a1ab954b24a54570300f89486b0) **callback** - 

### remove_directory() {#classmavsdk_1_1_ftp_1a585e0877f7c129b15aed66771a31a446}
```cpp
Result mavsdk::Ftp::remove_directory(std::string remote_dir) const
```


Removes a remote directory.

This function is blocking. See 'remove_directory_async' for the non-blocking counterpart.

**Parameters**

* std::string **remote_dir** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4) - Result of request.

### remove_file_async() {#classmavsdk_1_1_ftp_1a3ecda69288fb860a8da1f8fad25af31c}
```cpp
void mavsdk::Ftp::remove_file_async(std::string remote_file_path, const ResultCallback callback)
```


Removes a remote file.

This function is non-blocking. See 'remove_file' for the blocking counterpart.

**Parameters**

* std::string **remote_file_path** - 
* const [ResultCallback](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a04a12a1ab954b24a54570300f89486b0) **callback** - 

### remove_file() {#classmavsdk_1_1_ftp_1a2700a813ec54b115cb9a349ba486d6bd}
```cpp
Result mavsdk::Ftp::remove_file(std::string remote_file_path) const
```


Removes a remote file.

This function is blocking. See 'remove_file_async' for the non-blocking counterpart.

**Parameters**

* std::string **remote_file_path** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4) - Result of request.

### rename_async() {#classmavsdk_1_1_ftp_1afea8b15ad7b5748b0b5f68fd7103514a}
```cpp
void mavsdk::Ftp::rename_async(std::string remote_from_path, std::string remote_to_path, const ResultCallback callback)
```


Renames a remote file or remote directory.

This function is non-blocking. See 'rename' for the blocking counterpart.

**Parameters**

* std::string **remote_from_path** - 
* std::string **remote_to_path** - 
* const [ResultCallback](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a04a12a1ab954b24a54570300f89486b0) **callback** - 

### rename() {#classmavsdk_1_1_ftp_1ac7411b38ea31f84f4f679b6b85313032}
```cpp
Result mavsdk::Ftp::rename(std::string remote_from_path, std::string remote_to_path) const
```


Renames a remote file or remote directory.

This function is blocking. See 'rename_async' for the non-blocking counterpart.

**Parameters**

* std::string **remote_from_path** - 
* std::string **remote_to_path** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4) - Result of request.

### are_files_identical_async() {#classmavsdk_1_1_ftp_1abddebf1a103b2853116e68f5f870e4a7}
```cpp
void mavsdk::Ftp::are_files_identical_async(std::string local_file_path, std::string remote_file_path, const AreFilesIdenticalCallback callback)
```


Compares a local file to a remote file using a CRC32 checksum.

This function is non-blocking. See 'are_files_identical' for the blocking counterpart.

**Parameters**

* std::string **local_file_path** - 
* std::string **remote_file_path** - 
* const [AreFilesIdenticalCallback](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1abe24e99f7141a234206f8952d2f61318) **callback** - 

### are_files_identical() {#classmavsdk_1_1_ftp_1a1ace427243aedef4b0988a055fc414bf}
```cpp
std::pair<Result, bool> mavsdk::Ftp::are_files_identical(std::string local_file_path, std::string remote_file_path) const
```


Compares a local file to a remote file using a CRC32 checksum.

This function is blocking. See 'are_files_identical_async' for the non-blocking counterpart.

**Parameters**

* std::string **local_file_path** - 
* std::string **remote_file_path** - 

**Returns**

&emsp;std::pair< [Result](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4), bool > - Result of request.

### set_target_compid() {#classmavsdk_1_1_ftp_1ad93744cd25a89beffe0d72a090748229}
```cpp
Result mavsdk::Ftp::set_target_compid(uint32_t compid) const
```


Set target component ID. By default it is the autopilot.

This function is blocking.

**Parameters**

* uint32_t **compid** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_ftp.md#classmavsdk_1_1_ftp_1a4cc4f42a3ef6d63403d811e594b946e4) - Result of request.

### operator=() {#classmavsdk_1_1_ftp_1a01dc5f41d1e684a667d31c213728b376}
```cpp
const Ftp& mavsdk::Ftp::operator=(const Ftp &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [Ftp](classmavsdk_1_1_ftp.md)&  - 

**Returns**

&emsp;const [Ftp](classmavsdk_1_1_ftp.md) & - 