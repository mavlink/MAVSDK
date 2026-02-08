# mavsdk::Events::Event Struct Reference
`#include: events.h`

----


[Event](structmavsdk_1_1_events_1_1_event.md) type. 


## Data Fields


uint32_t [compid](#structmavsdk_1_1_events_1_1_event_1a7ec91e98105fae94d3ff041d42b5ac0a) {} - The source component ID of the event.

std::string [message](#structmavsdk_1_1_events_1_1_event_1ae18a466fde5623f099c9b20753302852) {} - Short, single-line message.

std::string [description](#structmavsdk_1_1_events_1_1_event_1aee27a34850edf145d56ebc6b37829fba) {} - Detailed description (optional, might be multiple lines)

[LogLevel](classmavsdk_1_1_events.md#classmavsdk_1_1_events_1a237c8de77f3995138125db44d148cecc) [log_level](#structmavsdk_1_1_events_1_1_event_1ae1a2b68931086b7ec4f31a6b2d4de3fd) {} - Log level of message.

std::string [event_namespace](#structmavsdk_1_1_events_1_1_event_1afa6ed28311d60fb9a44318d985931050) {} - Namespace, e.g. "px4".

std::string [event_name](#structmavsdk_1_1_events_1_1_event_1a9e8ad465085e81a6b534c89614767963) {} - [Event](structmavsdk_1_1_events_1_1_event.md) name (unique within the namespace)


## Field Documentation


### compid {#structmavsdk_1_1_events_1_1_event_1a7ec91e98105fae94d3ff041d42b5ac0a}

```cpp
uint32_t mavsdk::Events::Event::compid {}
```


The source component ID of the event.


### message {#structmavsdk_1_1_events_1_1_event_1ae18a466fde5623f099c9b20753302852}

```cpp
std::string mavsdk::Events::Event::message {}
```


Short, single-line message.


### description {#structmavsdk_1_1_events_1_1_event_1aee27a34850edf145d56ebc6b37829fba}

```cpp
std::string mavsdk::Events::Event::description {}
```


Detailed description (optional, might be multiple lines)


### log_level {#structmavsdk_1_1_events_1_1_event_1ae1a2b68931086b7ec4f31a6b2d4de3fd}

```cpp
LogLevel mavsdk::Events::Event::log_level {}
```


Log level of message.


### event_namespace {#structmavsdk_1_1_events_1_1_event_1afa6ed28311d60fb9a44318d985931050}

```cpp
std::string mavsdk::Events::Event::event_namespace {}
```


Namespace, e.g. "px4".


### event_name {#structmavsdk_1_1_events_1_1_event_1a9e8ad465085e81a6b534c89614767963}

```cpp
std::string mavsdk::Events::Event::event_name {}
```


[Event](structmavsdk_1_1_events_1_1_event.md) name (unique within the namespace)

