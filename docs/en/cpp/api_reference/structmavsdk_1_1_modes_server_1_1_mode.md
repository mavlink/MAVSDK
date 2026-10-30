# mavsdk::ModesServer::Mode Struct Reference
`#include: modes_server.hpp`

----


[Mode](structmavsdk_1_1_modes_server_1_1_mode.md) type. 


## Data Fields


std::string [name](#structmavsdk_1_1_modes_server_1_1_mode_1ac5ca70359b50803d00182db82aaa0678) {} - Name of the mode, for a standard mode one of the standard names, e.g. "Position" or "Mission".

uint32_t [custom_mode](#structmavsdk_1_1_modes_server_1_1_mode_1ab2a2fdfa272306478aa6eabff8a04b35) {} - [Autopilot](namespacemavsdk.md#namespacemavsdk_1aba05635d1785223a4d7b457ae0407297) specific custom mode, needs to be unique.

bool [is_standard](#structmavsdk_1_1_modes_server_1_1_mode_1a8e17d024e9aceba54ed455b6ea2a0800) {} - True if this is a MAVLink standard mode.

bool [is_user_selectable](#structmavsdk_1_1_modes_server_1_1_mode_1ab320fb716a9c538f032f1948f72b37e9) {} - True if the mode can be set by the user.

bool [is_advanced](#structmavsdk_1_1_modes_server_1_1_mode_1a878496bba33d49fbe88bb0a9427bf46f) {} - True if the mode is for advanced users only.

bool [is_auto_mode](#structmavsdk_1_1_modes_server_1_1_mode_1a037782296d8022bf7ba040d6128ae85d) {} - True if the mode runs autonomously, e.g. [Mission](classmavsdk_1_1_mission.md) or Land.


## Field Documentation


### name {#structmavsdk_1_1_modes_server_1_1_mode_1ac5ca70359b50803d00182db82aaa0678}

```cpp
std::string mavsdk::ModesServer::Mode::name {}
```


Name of the mode, for a standard mode one of the standard names, e.g. "Position" or "Mission".


### custom_mode {#structmavsdk_1_1_modes_server_1_1_mode_1ab2a2fdfa272306478aa6eabff8a04b35}

```cpp
uint32_t mavsdk::ModesServer::Mode::custom_mode {}
```


[Autopilot](namespacemavsdk.md#namespacemavsdk_1aba05635d1785223a4d7b457ae0407297) specific custom mode, needs to be unique.


### is_standard {#structmavsdk_1_1_modes_server_1_1_mode_1a8e17d024e9aceba54ed455b6ea2a0800}

```cpp
bool mavsdk::ModesServer::Mode::is_standard {}
```


True if this is a MAVLink standard mode.


### is_user_selectable {#structmavsdk_1_1_modes_server_1_1_mode_1ab320fb716a9c538f032f1948f72b37e9}

```cpp
bool mavsdk::ModesServer::Mode::is_user_selectable {}
```


True if the mode can be set by the user.


### is_advanced {#structmavsdk_1_1_modes_server_1_1_mode_1a878496bba33d49fbe88bb0a9427bf46f}

```cpp
bool mavsdk::ModesServer::Mode::is_advanced {}
```


True if the mode is for advanced users only.


### is_auto_mode {#structmavsdk_1_1_modes_server_1_1_mode_1a037782296d8022bf7ba040d6128ae85d}

```cpp
bool mavsdk::ModesServer::Mode::is_auto_mode {}
```


True if the mode runs autonomously, e.g. [Mission](classmavsdk_1_1_mission.md) or Land.

