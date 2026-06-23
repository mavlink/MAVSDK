# mavsdk::Ftp::FilesystemEntry Struct Reference
`#include: ftp.hpp`

----


A file system entry (file or directory) with metadata. 


## Public Types


Type | Description
--- | ---
enum [EntryType](#structmavsdk_1_1_ftp_1_1_filesystem_entry_1ad290d84f6a318b24a2cf2a3919c4ad3e) | The type of a file system entry.

## Data Fields


std::string [name](#structmavsdk_1_1_ftp_1_1_filesystem_entry_1aa4c0fe986ebbb751382701d436078437) {} - The name of the file or directory.

[EntryType](structmavsdk_1_1_ftp_1_1_filesystem_entry.md#structmavsdk_1_1_ftp_1_1_filesystem_entry_1ad290d84f6a318b24a2cf2a3919c4ad3e) [entry_type](#structmavsdk_1_1_ftp_1_1_filesystem_entry_1aff7dd122e2896fb6b1b4ba050849f772) {} - Whether the entry is a file or a directory.

uint64_t [size_bytes](#structmavsdk_1_1_ftp_1_1_filesystem_entry_1ab98f49cc81a65ed8da76882099553042) {} - The size of the file in bytes (0 for directories).

uint64_t [modification_time_s](#structmavsdk_1_1_ftp_1_1_filesystem_entry_1a3245477015cdc377b28a9b3ba89b55c7) {} - Last modification time in seconds since UNIX epoch (UTC), 0 if unknown.


## Member Enumeration Documentation


### enum EntryType {#structmavsdk_1_1_ftp_1_1_filesystem_entry_1ad290d84f6a318b24a2cf2a3919c4ad3e}


The type of a file system entry.


Value | Description
--- | ---
<span id="structmavsdk_1_1_ftp_1_1_filesystem_entry_1ad290d84f6a318b24a2cf2a3919c4ad3ea88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown entry type. 
<span id="structmavsdk_1_1_ftp_1_1_filesystem_entry_1ad290d84f6a318b24a2cf2a3919c4ad3ea0b27918290ff5323bea1e3b78a9cf04e"></span> `File` | A regular file. 
<span id="structmavsdk_1_1_ftp_1_1_filesystem_entry_1ad290d84f6a318b24a2cf2a3919c4ad3eae73cda510e8bb947f7e61089e5581494"></span> `Directory` | A directory. 

## Field Documentation


### name {#structmavsdk_1_1_ftp_1_1_filesystem_entry_1aa4c0fe986ebbb751382701d436078437}

```cpp
std::string mavsdk::Ftp::FilesystemEntry::name {}
```


The name of the file or directory.


### entry_type {#structmavsdk_1_1_ftp_1_1_filesystem_entry_1aff7dd122e2896fb6b1b4ba050849f772}

```cpp
EntryType mavsdk::Ftp::FilesystemEntry::entry_type {}
```


Whether the entry is a file or a directory.


### size_bytes {#structmavsdk_1_1_ftp_1_1_filesystem_entry_1ab98f49cc81a65ed8da76882099553042}

```cpp
uint64_t mavsdk::Ftp::FilesystemEntry::size_bytes {}
```


The size of the file in bytes (0 for directories).


### modification_time_s {#structmavsdk_1_1_ftp_1_1_filesystem_entry_1a3245477015cdc377b28a9b3ba89b55c7}

```cpp
uint64_t mavsdk::Ftp::FilesystemEntry::modification_time_s {}
```


Last modification time in seconds since UNIX epoch (UTC), 0 if unknown.

