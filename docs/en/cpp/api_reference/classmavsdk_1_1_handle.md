# mavsdk::Handle Class Reference
`#include: handle.h`

----


A handle returned from subscribe which allows to unsubscribe again. 


## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [Handle](#classmavsdk_1_1_handle_1a4dde9be588a605aa7a7fa77cc4ed98f2) ()=default |
&nbsp; | [~Handle](#classmavsdk_1_1_handle_1a986e2ba72802e6cc5bdc5f06f27f6acb) ()=default |
bool | [valid](#classmavsdk_1_1_handle_1a424583fcc5165eb16b167829557c782b) () const | Wheter handle is valid.
bool | [operator<](#classmavsdk_1_1_handle_1ad0a7552f878d8bf20e998821ae2ed82f) (const [Handle](classmavsdk_1_1_handle.md) & other)const |
bool | [operator==](#classmavsdk_1_1_handle_1ad7b7891c68f244a615219fea16808998) (const [Handle](classmavsdk_1_1_handle.md) & other)const |


## Constructor & Destructor Documentation


### Handle() {#classmavsdk_1_1_handle_1a4dde9be588a605aa7a7fa77cc4ed98f2}
```cpp
mavsdk::Handle< Args >::Handle()=default
```


### ~Handle() {#classmavsdk_1_1_handle_1a986e2ba72802e6cc5bdc5f06f27f6acb}
```cpp
mavsdk::Handle< Args >::~Handle()=default
```


## Member Function Documentation


### valid() {#classmavsdk_1_1_handle_1a424583fcc5165eb16b167829557c782b}
```cpp
bool mavsdk::Handle< Args >::valid() const
```


Wheter handle is valid.


**Returns**

&emsp;bool - true if handle is valid

### operator<() {#classmavsdk_1_1_handle_1ad0a7552f878d8bf20e998821ae2ed82f}
```cpp
bool mavsdk::Handle< Args >::operator<(const Handle &other) const
```


**Parameters**

* const [Handle](classmavsdk_1_1_handle.md)& **other** - 

**Returns**

&emsp;bool - 

### operator==() {#classmavsdk_1_1_handle_1ad7b7891c68f244a615219fea16808998}
```cpp
bool mavsdk::Handle< Args >::operator==(const Handle &other) const
```


**Parameters**

* const [Handle](classmavsdk_1_1_handle.md)& **other** - 

**Returns**

&emsp;bool - 