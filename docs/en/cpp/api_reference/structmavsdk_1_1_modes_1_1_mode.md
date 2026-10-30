# mavsdk::Modes::Mode Struct Reference
`#include: modes.hpp`

----


[Mode](structmavsdk_1_1_modes_1_1_mode.md) type. 


## Data Fields


std::string [name](#structmavsdk_1_1_modes_1_1_mode_1a9c3520ebdb5cbe478b92ef9c98137792) {} - Name of the mode, e.g. "Position" or "Stabilize".

[StandardMode](classmavsdk_1_1_modes.md#classmavsdk_1_1_modes_1a07200f4c73aa57ac268500ba4c61503f) [standard_mode](#structmavsdk_1_1_modes_1_1_mode_1a0186961f33c099adbe98ceeca5fbd49d) {} - Standard mode, or non-standard for custom modes.

uint32_t [custom_mode](#structmavsdk_1_1_modes_1_1_mode_1a80dcd8a21f248af4d24d59b0bdbbf699) {} - [Autopilot](namespacemavsdk.md#namespacemavsdk_1aba05635d1785223a4d7b457ae0407297) specific custom mode.

bool [is_user_selectable](#structmavsdk_1_1_modes_1_1_mode_1ac7f37f7e4a591d6af2941c2a8a566cf0) {} - True if the mode can be set by the user.

bool [is_advanced](#structmavsdk_1_1_modes_1_1_mode_1a8207dfa0646c5ce4881946cfa64c1a11) {} - True if the mode is for advanced users only.

bool [is_auto_mode](#structmavsdk_1_1_modes_1_1_mode_1ab9cd1c87be745ee532231809e6a95e1d) {} - True if the mode runs autonomously, e.g. [Mission](classmavsdk_1_1_mission.md) or Land.


## Field Documentation


### name {#structmavsdk_1_1_modes_1_1_mode_1a9c3520ebdb5cbe478b92ef9c98137792}

```cpp
std::string mavsdk::Modes::Mode::name {}
```


Name of the mode, e.g. "Position" or "Stabilize".


### standard_mode {#structmavsdk_1_1_modes_1_1_mode_1a0186961f33c099adbe98ceeca5fbd49d}

```cpp
StandardMode mavsdk::Modes::Mode::standard_mode {}
```


Standard mode, or non-standard for custom modes.


### custom_mode {#structmavsdk_1_1_modes_1_1_mode_1a80dcd8a21f248af4d24d59b0bdbbf699}

```cpp
uint32_t mavsdk::Modes::Mode::custom_mode {}
```


[Autopilot](namespacemavsdk.md#namespacemavsdk_1aba05635d1785223a4d7b457ae0407297) specific custom mode.


### is_user_selectable {#structmavsdk_1_1_modes_1_1_mode_1ac7f37f7e4a591d6af2941c2a8a566cf0}

```cpp
bool mavsdk::Modes::Mode::is_user_selectable {}
```


True if the mode can be set by the user.


### is_advanced {#structmavsdk_1_1_modes_1_1_mode_1a8207dfa0646c5ce4881946cfa64c1a11}

```cpp
bool mavsdk::Modes::Mode::is_advanced {}
```


True if the mode is for advanced users only.


### is_auto_mode {#structmavsdk_1_1_modes_1_1_mode_1ab9cd1c87be745ee532231809e6a95e1d}

```cpp
bool mavsdk::Modes::Mode::is_auto_mode {}
```


True if the mode runs autonomously, e.g. [Mission](classmavsdk_1_1_mission.md) or Land.

