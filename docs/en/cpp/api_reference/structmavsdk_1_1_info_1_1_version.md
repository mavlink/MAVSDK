# mavsdk::Info::Version Struct Reference
`#include: info.h`

----


[System](classmavsdk_1_1_system.md) version information. 


## Public Types


Type | Description
--- | ---
enum [FlightSoftwareVersionType](#structmavsdk_1_1_info_1_1_version_1ae35252bd527add36f2babf5e6d815da9) | These values define the type of firmware/flight software release.

## Data Fields


int32_t [flight_sw_major](#structmavsdk_1_1_info_1_1_version_1a7f1fd24359024bb191f919cf65e5de62) {} - Flight software major version.

int32_t [flight_sw_minor](#structmavsdk_1_1_info_1_1_version_1aabfb9cab57116cc258f4698cf8bb071a) {} - Flight software minor version.

int32_t [flight_sw_patch](#structmavsdk_1_1_info_1_1_version_1a287e6f282f4f5283f1976801902a645b) {} - Flight software patch version.

int32_t [flight_sw_vendor_major](#structmavsdk_1_1_info_1_1_version_1a7e7873f5c308cae0b1d96bb3184138ee) {} - Flight software vendor major version.

int32_t [flight_sw_vendor_minor](#structmavsdk_1_1_info_1_1_version_1abef2aa4c5e8e80b51ae07a312b39cf7f) {} - Flight software vendor minor version.

int32_t [flight_sw_vendor_patch](#structmavsdk_1_1_info_1_1_version_1a882878580fd5123653c20d92dfbd5282) {} - Flight software vendor patch version.

int32_t [os_sw_major](#structmavsdk_1_1_info_1_1_version_1a8cc6c89673798b2b1e1e34a10dbf2574) {} - Operating system software major version.

int32_t [os_sw_minor](#structmavsdk_1_1_info_1_1_version_1a8896fea440d503d930d254ebabd557d5) {} - Operating system software minor version.

int32_t [os_sw_patch](#structmavsdk_1_1_info_1_1_version_1a1d990ecc76889c703f20fb134df5e476) {} - Operating system software patch version.

std::string [flight_sw_git_hash](#structmavsdk_1_1_info_1_1_version_1a684d49782eeca4e9ae802a84a32b715f) {} - Flight software git hash.

std::string [os_sw_git_hash](#structmavsdk_1_1_info_1_1_version_1ad6a2e4f45c02373ebca08ef5bd54cf47) {} - Operating system software git hash.

[FlightSoftwareVersionType](structmavsdk_1_1_info_1_1_version.md#structmavsdk_1_1_info_1_1_version_1ae35252bd527add36f2babf5e6d815da9) [flight_sw_version_type](#structmavsdk_1_1_info_1_1_version_1ab563fb15dfd6b929504b642309a9e97b) {} - Flight software version type.


## Member Enumeration Documentation


### enum FlightSoftwareVersionType {#structmavsdk_1_1_info_1_1_version_1ae35252bd527add36f2babf5e6d815da9}


These values define the type of firmware/flight software release.


Value | Description
--- | ---
<span id="structmavsdk_1_1_info_1_1_version_1ae35252bd527add36f2babf5e6d815da9a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown type. 
<span id="structmavsdk_1_1_info_1_1_version_1ae35252bd527add36f2babf5e6d815da9a55f37d1fd483f6a85379640fe51aafa8"></span> `Dev` | Development release. 
<span id="structmavsdk_1_1_info_1_1_version_1ae35252bd527add36f2babf5e6d815da9a6132295fcf5570fb8b0a944ef322a598"></span> `Alpha` | Alpha release. 
<span id="structmavsdk_1_1_info_1_1_version_1ae35252bd527add36f2babf5e6d815da9a0b87d66b88c72957dfea8c9605016442"></span> `Beta` | Beta release. 
<span id="structmavsdk_1_1_info_1_1_version_1ae35252bd527add36f2babf5e6d815da9a7c98274c63a1588cfa35ffb5c0adc833"></span> `Rc` | Release candidate. 
<span id="structmavsdk_1_1_info_1_1_version_1ae35252bd527add36f2babf5e6d815da9ab8e7b465df7c5979dc731d06e84ce2cf"></span> `Release` | Official stable release. 

## Field Documentation


### flight_sw_major {#structmavsdk_1_1_info_1_1_version_1a7f1fd24359024bb191f919cf65e5de62}

```cpp
int32_t mavsdk::Info::Version::flight_sw_major {}
```


Flight software major version.


### flight_sw_minor {#structmavsdk_1_1_info_1_1_version_1aabfb9cab57116cc258f4698cf8bb071a}

```cpp
int32_t mavsdk::Info::Version::flight_sw_minor {}
```


Flight software minor version.


### flight_sw_patch {#structmavsdk_1_1_info_1_1_version_1a287e6f282f4f5283f1976801902a645b}

```cpp
int32_t mavsdk::Info::Version::flight_sw_patch {}
```


Flight software patch version.


### flight_sw_vendor_major {#structmavsdk_1_1_info_1_1_version_1a7e7873f5c308cae0b1d96bb3184138ee}

```cpp
int32_t mavsdk::Info::Version::flight_sw_vendor_major {}
```


Flight software vendor major version.


### flight_sw_vendor_minor {#structmavsdk_1_1_info_1_1_version_1abef2aa4c5e8e80b51ae07a312b39cf7f}

```cpp
int32_t mavsdk::Info::Version::flight_sw_vendor_minor {}
```


Flight software vendor minor version.


### flight_sw_vendor_patch {#structmavsdk_1_1_info_1_1_version_1a882878580fd5123653c20d92dfbd5282}

```cpp
int32_t mavsdk::Info::Version::flight_sw_vendor_patch {}
```


Flight software vendor patch version.


### os_sw_major {#structmavsdk_1_1_info_1_1_version_1a8cc6c89673798b2b1e1e34a10dbf2574}

```cpp
int32_t mavsdk::Info::Version::os_sw_major {}
```


Operating system software major version.


### os_sw_minor {#structmavsdk_1_1_info_1_1_version_1a8896fea440d503d930d254ebabd557d5}

```cpp
int32_t mavsdk::Info::Version::os_sw_minor {}
```


Operating system software minor version.


### os_sw_patch {#structmavsdk_1_1_info_1_1_version_1a1d990ecc76889c703f20fb134df5e476}

```cpp
int32_t mavsdk::Info::Version::os_sw_patch {}
```


Operating system software patch version.


### flight_sw_git_hash {#structmavsdk_1_1_info_1_1_version_1a684d49782eeca4e9ae802a84a32b715f}

```cpp
std::string mavsdk::Info::Version::flight_sw_git_hash {}
```


Flight software git hash.


### os_sw_git_hash {#structmavsdk_1_1_info_1_1_version_1ad6a2e4f45c02373ebca08ef5bd54cf47}

```cpp
std::string mavsdk::Info::Version::os_sw_git_hash {}
```


Operating system software git hash.


### flight_sw_version_type {#structmavsdk_1_1_info_1_1_version_1ab563fb15dfd6b929504b642309a9e97b}

```cpp
FlightSoftwareVersionType mavsdk::Info::Version::flight_sw_version_type {}
```


Flight software version type.

