# mavsdk::Tune Class Reference
`#include: tune.h`

----


Enable creating and sending a tune to be played on the system. 


## Data Structures


struct [TuneDescription](structmavsdk_1_1_tune_1_1_tune_description.md)

## Public Types


Type | Description
--- | ---
enum [SongElement](#classmavsdk_1_1_tune_1a97d85b5d578c74edc82a4337fc7bed57) | An element of the tune.
enum [Result](#classmavsdk_1_1_tune_1aed2b008974298098cedd69b7e95e909d) | Possible results returned for tune requests.
std::function< void([Result](classmavsdk_1_1_tune.md#classmavsdk_1_1_tune_1aed2b008974298098cedd69b7e95e909d))> [ResultCallback](#classmavsdk_1_1_tune_1aa283f6824bee43d341fec56d7ff70985) | Callback type for asynchronous [Tune](classmavsdk_1_1_tune.md) calls.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [Tune](#classmavsdk_1_1_tune_1a1f60ea360006940e2bbae055733cc40a) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [Tune](#classmavsdk_1_1_tune_1ae53cb416ec500a56d796e85376420c0a) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~Tune](#classmavsdk_1_1_tune_1a5f157ec4a056b0dad4fb7c15542a555f) () override | Destructor (internal use only).
&nbsp; | [Tune](#classmavsdk_1_1_tune_1ae3fb9d4ec2116f1c9543c0e09371b2e0) (const [Tune](classmavsdk_1_1_tune.md) & other) | Copy constructor.
void | [play_tune_async](#classmavsdk_1_1_tune_1a6a88b2cfe944a4c1e4ab6945b06620d5) ([TuneDescription](structmavsdk_1_1_tune_1_1_tune_description.md) tune_description, const [ResultCallback](classmavsdk_1_1_tune.md#classmavsdk_1_1_tune_1aa283f6824bee43d341fec56d7ff70985) callback) | Send a tune to be played by the system.
[Result](classmavsdk_1_1_tune.md#classmavsdk_1_1_tune_1aed2b008974298098cedd69b7e95e909d) | [play_tune](#classmavsdk_1_1_tune_1a94df5e7c5d8f5ca49fd9844557060695) ([TuneDescription](structmavsdk_1_1_tune_1_1_tune_description.md) tune_description)const | Send a tune to be played by the system.
const [Tune](classmavsdk_1_1_tune.md) & | [operator=](#classmavsdk_1_1_tune_1ae4a076da0417c6858df6fb59fa5110b5) (const [Tune](classmavsdk_1_1_tune.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### Tune() {#classmavsdk_1_1_tune_1a1f60ea360006940e2bbae055733cc40a}
```cpp
mavsdk::Tune::Tune(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto tune = Tune(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### Tune() {#classmavsdk_1_1_tune_1ae53cb416ec500a56d796e85376420c0a}
```cpp
mavsdk::Tune::Tune(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto tune = Tune(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~Tune() {#classmavsdk_1_1_tune_1a5f157ec4a056b0dad4fb7c15542a555f}
```cpp
mavsdk::Tune::~Tune() override
```


Destructor (internal use only).


### Tune() {#classmavsdk_1_1_tune_1ae3fb9d4ec2116f1c9543c0e09371b2e0}
```cpp
mavsdk::Tune::Tune(const Tune &other)
```


Copy constructor.


**Parameters**

* const [Tune](classmavsdk_1_1_tune.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_tune_1aa283f6824bee43d341fec56d7ff70985}

```cpp
using mavsdk::Tune::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [Tune](classmavsdk_1_1_tune.md) calls.


## Member Enumeration Documentation


### enum SongElement {#classmavsdk_1_1_tune_1a97d85b5d578c74edc82a4337fc7bed57}


An element of the tune.


Value | Description
--- | ---
<span id="classmavsdk_1_1_tune_1a97d85b5d578c74edc82a4337fc7bed57ac830472147f0e7d7a397f4ebe1efa6f3"></span> `StyleLegato` | After this element, start playing legato. 
<span id="classmavsdk_1_1_tune_1a97d85b5d578c74edc82a4337fc7bed57a8cb71997964d545dec42802f02c1e5af"></span> `StyleNormal` | After this element, start playing normal. 
<span id="classmavsdk_1_1_tune_1a97d85b5d578c74edc82a4337fc7bed57acca75620eab0dffb73ec1a4b2b7a334c"></span> `StyleStaccato` | After this element, start playing staccato. 
<span id="classmavsdk_1_1_tune_1a97d85b5d578c74edc82a4337fc7bed57a63787476eb9cebc1d976e06c4a89085d"></span> `Duration1` | After this element, set the note duration to 1. 
<span id="classmavsdk_1_1_tune_1a97d85b5d578c74edc82a4337fc7bed57ab60dc533a85bdbf419ac39982fae8ce8"></span> `Duration2` | After this element, set the note duration to 2. 
<span id="classmavsdk_1_1_tune_1a97d85b5d578c74edc82a4337fc7bed57a48914c473999e8240f09b96c63c37a84"></span> `Duration4` | After this element, set the note duration to 4. 
<span id="classmavsdk_1_1_tune_1a97d85b5d578c74edc82a4337fc7bed57a5c027e5349ad17104491057d3b153192"></span> `Duration8` | After this element, set the note duration to 8. 
<span id="classmavsdk_1_1_tune_1a97d85b5d578c74edc82a4337fc7bed57add80adb2a6a9aae642795959bb077b19"></span> `Duration16` | After this element, set the note duration to 16. 
<span id="classmavsdk_1_1_tune_1a97d85b5d578c74edc82a4337fc7bed57a5238379d32263441c68c00fefb9fee74"></span> `Duration32` | After this element, set the note duration to 32. 
<span id="classmavsdk_1_1_tune_1a97d85b5d578c74edc82a4337fc7bed57adae7803873ef1a4be05a28fd47f928cc"></span> `NoteA` | Play note A. 
<span id="classmavsdk_1_1_tune_1a97d85b5d578c74edc82a4337fc7bed57adf0f2ea8b78208955005301fabc40578"></span> `NoteB` | Play note B. 
<span id="classmavsdk_1_1_tune_1a97d85b5d578c74edc82a4337fc7bed57a96efe8adf521933e0ee897b2526f1040"></span> `NoteC` | Play note C. 
<span id="classmavsdk_1_1_tune_1a97d85b5d578c74edc82a4337fc7bed57a8dc7aeb7d481ca13b24b89ea5781d5f0"></span> `NoteD` | Play note D. 
<span id="classmavsdk_1_1_tune_1a97d85b5d578c74edc82a4337fc7bed57a8499410b04b4830cc664546160bbee41"></span> `NoteE` | Play note E. 
<span id="classmavsdk_1_1_tune_1a97d85b5d578c74edc82a4337fc7bed57adff4ed70eb41ce84bd176316cb3c82f8"></span> `NoteF` | Play note F. 
<span id="classmavsdk_1_1_tune_1a97d85b5d578c74edc82a4337fc7bed57a57afabb08eebc4b8052efdb10878fa88"></span> `NoteG` | Play note G. 
<span id="classmavsdk_1_1_tune_1a97d85b5d578c74edc82a4337fc7bed57a24dfb97f7be37bc76be4233a0d18440b"></span> `NotePause` | Play a rest. 
<span id="classmavsdk_1_1_tune_1a97d85b5d578c74edc82a4337fc7bed57a60c17eb98355db0fe5ce016db6c6f444"></span> `Sharp` | After this element, sharp the note (half a step up). 
<span id="classmavsdk_1_1_tune_1a97d85b5d578c74edc82a4337fc7bed57a745e3db6a7ffd50e1a72b39482f0882d"></span> `Flat` | After this element, flat the note (half a step down). 
<span id="classmavsdk_1_1_tune_1a97d85b5d578c74edc82a4337fc7bed57a1713d26780742b9524fdc6e40c6ef6cd"></span> `OctaveUp` | After this element, shift the note 1 octave up. 
<span id="classmavsdk_1_1_tune_1a97d85b5d578c74edc82a4337fc7bed57ab41646df7c887cc0e3647ba5f8b26afe"></span> `OctaveDown` | After this element, shift the note 1 octave down. 

### enum Result {#classmavsdk_1_1_tune_1aed2b008974298098cedd69b7e95e909d}


Possible results returned for tune requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_tune_1aed2b008974298098cedd69b7e95e909da88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_tune_1aed2b008974298098cedd69b7e95e909da505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Request succeeded. 
<span id="classmavsdk_1_1_tune_1aed2b008974298098cedd69b7e95e909dab94ce655e90ee954ba1c4c5d0e13659e"></span> `InvalidTempo` | Invalid tempo (range: 32 - 255). 
<span id="classmavsdk_1_1_tune_1aed2b008974298098cedd69b7e95e909da088c77b60655734bff629e03e1968495"></span> `TuneTooLong` | Invalid tune: encoded string must be at most 247 chars. 
<span id="classmavsdk_1_1_tune_1aed2b008974298098cedd69b7e95e909da902b0d55fddef6f8d651fe1035b7d4bd"></span> `Error` | Failed to send the request. 
<span id="classmavsdk_1_1_tune_1aed2b008974298098cedd69b7e95e909da1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system connected. 

## Member Function Documentation


### play_tune_async() {#classmavsdk_1_1_tune_1a6a88b2cfe944a4c1e4ab6945b06620d5}
```cpp
void mavsdk::Tune::play_tune_async(TuneDescription tune_description, const ResultCallback callback)
```


Send a tune to be played by the system.

This function is non-blocking. See 'play_tune' for the blocking counterpart.

**Parameters**

* [TuneDescription](structmavsdk_1_1_tune_1_1_tune_description.md) **tune_description** - 
* const [ResultCallback](classmavsdk_1_1_tune.md#classmavsdk_1_1_tune_1aa283f6824bee43d341fec56d7ff70985) **callback** - 

### play_tune() {#classmavsdk_1_1_tune_1a94df5e7c5d8f5ca49fd9844557060695}
```cpp
Result mavsdk::Tune::play_tune(TuneDescription tune_description) const
```


Send a tune to be played by the system.

This function is blocking. See 'play_tune_async' for the non-blocking counterpart.

**Parameters**

* [TuneDescription](structmavsdk_1_1_tune_1_1_tune_description.md) **tune_description** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_tune.md#classmavsdk_1_1_tune_1aed2b008974298098cedd69b7e95e909d) - Result of request.

### operator=() {#classmavsdk_1_1_tune_1ae4a076da0417c6858df6fb59fa5110b5}
```cpp
const Tune& mavsdk::Tune::operator=(const Tune &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [Tune](classmavsdk_1_1_tune.md)&  - 

**Returns**

&emsp;const [Tune](classmavsdk_1_1_tune.md) & - 