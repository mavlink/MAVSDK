# mavsdk::LogFiles::Entry Struct Reference
`#include: log_files.h`

----


Log file entry type. 


## Data Fields


uint32_t [id](#structmavsdk_1_1_log_files_1_1_entry_1af70d624d97e7a94f3c0018c660c0d467) {} - ID of the log file, to specify a file to be downloaded.

std::string [date](#structmavsdk_1_1_log_files_1_1_entry_1a5727aa32b382e3ce4be745a36efdd4bb) {} - Date of the log file in UTC in ISO 8601 format "yyyy-mm-ddThh:mm:ssZ".

uint32_t [size_bytes](#structmavsdk_1_1_log_files_1_1_entry_1a3dab9428227b8674c0b55f7f7f9b98b4) {} - Size of file in bytes.


## Field Documentation


### id {#structmavsdk_1_1_log_files_1_1_entry_1af70d624d97e7a94f3c0018c660c0d467}

```cpp
uint32_t mavsdk::LogFiles::Entry::id {}
```


ID of the log file, to specify a file to be downloaded.


### date {#structmavsdk_1_1_log_files_1_1_entry_1a5727aa32b382e3ce4be745a36efdd4bb}

```cpp
std::string mavsdk::LogFiles::Entry::date {}
```


Date of the log file in UTC in ISO 8601 format "yyyy-mm-ddThh:mm:ssZ".


### size_bytes {#structmavsdk_1_1_log_files_1_1_entry_1a3dab9428227b8674c0b55f7f7f9b98b4}

```cpp
uint32_t mavsdk::LogFiles::Entry::size_bytes {}
```


Size of file in bytes.

