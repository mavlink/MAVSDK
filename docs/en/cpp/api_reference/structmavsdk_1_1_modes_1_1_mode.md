# mavsdk::Modes::Mode Struct Reference
`#include: modes.hpp`

----


[Mode](structmavsdk_1_1_modes_1_1_mode.md) type. 


## Data Fields


std::string [name](#structmavsdk_1_1_modes_1_1_mode_1a9c3520ebdb5cbe478b92ef9c98137792) {} - Name of the mode, e.g. "Position" or "Stabilize".

uint32_t [custom_mode](#structmavsdk_1_1_modes_1_1_mode_1a80dcd8a21f248af4d24d59b0bdbbf699) {} - [Autopilot](namespacemavsdk.md#namespacemavsdk_1aba05635d1785223a4d7b457ae0407297) specific custom mode.

bool [is_standard](#structmavsdk_1_1_modes_1_1_mode_1ab07349894a75582bc5f24f419074df2c) {} - True if this is a MAVLink standard mode, e.g. "Position" or "Mission".

bool [is_user_selectable](#structmavsdk_1_1_modes_1_1_mode_1ac7f37f7e4a591d6af2941c2a8a566cf0) {} - True if the mode can be set by the user.

bool [is_advanced](#structmavsdk_1_1_modes_1_1_mode_1a8207dfa0646c5ce4881946cfa64c1a11) {} - True if the mode is for advanced users only.

bool [is_auto_mode](#structmavsdk_1_1_modes_1_1_mode_1ab9cd1c87be745ee532231809e6a95e1d) {} - True if the mode runs autonomously, e.g. [Mission](classmavsdk_1_1_mission.md) or Land.


## Field Documentation


### name {#structmavsdk_1_1_modes_1_1_mode_1a9c3520ebdb5cbe478b92ef9c98137792}

```cpp
std::string mavsdk::Modes::Mode::name {}
```


Name of the mode, e.g. "Position" or "Stabilize".


### custom_mode {#structmavsdk_1_1_modes_1_1_mode_1a80dcd8a21f248af4d24d59b0bdbbf699}

```cpp
uint32_t mavsdk::Modes::Mode::custom_mode {}
```


[Autopilot](namespacemavsdk.md#namespacemavsdk_1aba05635d1785223a4d7b457ae0407297) specific custom mode.


### is_standard {#structmavsdk_1_1_modes_1_1_mode_1ab07349894a75582bc5f24f419074df2c}

```cpp
bool mavsdk::Modes::Mode::is_standard {}
```


True if this is a MAVLink standard mode, e.g. "Position" or "Mission".


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

