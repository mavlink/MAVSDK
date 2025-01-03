# mavsdk::Transponder Class Reference
`#include: transponder.h`

----


Allow users to get ADS-B information and set ADS-B update rates. 


## Data Structures


struct [AdsbVehicle](structmavsdk_1_1_transponder_1_1_adsb_vehicle.md)

## Public Types


Type | Description
--- | ---
enum [AdsbEmitterType](#classmavsdk_1_1_transponder_1a0b16f74131c28940c6604bc55776c039) | ADSB classification for the type of vehicle emitting the transponder signal.
enum [AdsbAltitudeType](#classmavsdk_1_1_transponder_1a64af76be070a2496e0db1c201ece9313) | Altitude type used in [AdsbVehicle](structmavsdk_1_1_transponder_1_1_adsb_vehicle.md) message.
enum [Result](#classmavsdk_1_1_transponder_1a683477ab9a02d00b2524bc2dcea3ecc8) | Possible results returned for transponder requests.
std::function< void([Result](classmavsdk_1_1_transponder.md#classmavsdk_1_1_transponder_1a683477ab9a02d00b2524bc2dcea3ecc8))> [ResultCallback](#classmavsdk_1_1_transponder_1a57166e61c37ad92ecd5420bc1b7972a5) | Callback type for asynchronous [Transponder](classmavsdk_1_1_transponder.md) calls.
std::function< void([AdsbVehicle](structmavsdk_1_1_transponder_1_1_adsb_vehicle.md))> [TransponderCallback](#classmavsdk_1_1_transponder_1a239211c3cf8ce383d0f5c82b671aed34) | Callback type for subscribe_transponder.
[Handle](classmavsdk_1_1_handle.md)< [AdsbVehicle](structmavsdk_1_1_transponder_1_1_adsb_vehicle.md) > [TransponderHandle](#classmavsdk_1_1_transponder_1abd96184c88f8bcb8bbb0aa138c3355b7) | [Handle](classmavsdk_1_1_handle.md) type for subscribe_transponder.

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [Transponder](#classmavsdk_1_1_transponder_1a14df6f56a65f692f0a77e12255aa5813) ([System](classmavsdk_1_1_system.md) & system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [Transponder](#classmavsdk_1_1_transponder_1aa24bac9cdb6a648e83630a58feee120c) (std::shared_ptr< [System](classmavsdk_1_1_system.md) > system) | Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).
&nbsp; | [~Transponder](#classmavsdk_1_1_transponder_1a2bd5706f4b1bd5cf773fdd8870d5c457) () override | Destructor (internal use only).
&nbsp; | [Transponder](#classmavsdk_1_1_transponder_1a610383ecbf848dd7ef214b13c445e170) (const [Transponder](classmavsdk_1_1_transponder.md) & other) | Copy constructor.
[TransponderHandle](classmavsdk_1_1_transponder.md#classmavsdk_1_1_transponder_1abd96184c88f8bcb8bbb0aa138c3355b7) | [subscribe_transponder](#classmavsdk_1_1_transponder_1a1854383cab590dbc28cb18bb8bf3415f) (const [TransponderCallback](classmavsdk_1_1_transponder.md#classmavsdk_1_1_transponder_1a239211c3cf8ce383d0f5c82b671aed34) & callback) | Subscribe to 'transponder' updates.
void | [unsubscribe_transponder](#classmavsdk_1_1_transponder_1a28735123acd679533a315a39dd096db0) ([TransponderHandle](classmavsdk_1_1_transponder.md#classmavsdk_1_1_transponder_1abd96184c88f8bcb8bbb0aa138c3355b7) handle) | Unsubscribe from subscribe_transponder.
[AdsbVehicle](structmavsdk_1_1_transponder_1_1_adsb_vehicle.md) | [transponder](#classmavsdk_1_1_transponder_1a1674c21928af3368e2be84bd182251d9) () const | Poll for '[AdsbVehicle](structmavsdk_1_1_transponder_1_1_adsb_vehicle.md)' (blocking).
void | [set_rate_transponder_async](#classmavsdk_1_1_transponder_1a098f5692a3f2b76a201ce0b22a56f0ec) (double rate_hz, const [ResultCallback](classmavsdk_1_1_transponder.md#classmavsdk_1_1_transponder_1a57166e61c37ad92ecd5420bc1b7972a5) callback) | Set rate to 'transponder' updates.
[Result](classmavsdk_1_1_transponder.md#classmavsdk_1_1_transponder_1a683477ab9a02d00b2524bc2dcea3ecc8) | [set_rate_transponder](#classmavsdk_1_1_transponder_1a9d9f4b4899ef61b6d13d9e25f44dbecd) (double rate_hz)const | Set rate to 'transponder' updates.
const [Transponder](classmavsdk_1_1_transponder.md) & | [operator=](#classmavsdk_1_1_transponder_1aaee71763de213002fadeb9dec3ec9fb6) (const [Transponder](classmavsdk_1_1_transponder.md) &)=delete | Equality operator (object is not copyable).


## Constructor & Destructor Documentation


### Transponder() {#classmavsdk_1_1_transponder_1a14df6f56a65f692f0a77e12255aa5813}
```cpp
mavsdk::Transponder::Transponder(System &system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto transponder = Transponder(system);
```

**Parameters**

* [System](classmavsdk_1_1_system.md)& **system** - The specific system associated with this plugin.

### Transponder() {#classmavsdk_1_1_transponder_1aa24bac9cdb6a648e83630a58feee120c}
```cpp
mavsdk::Transponder::Transponder(std::shared_ptr< System > system)
```


Constructor. Creates the plugin for a specific [System](classmavsdk_1_1_system.md).

The plugin is typically created as shown below: 

```cpp
auto transponder = Transponder(system);
```

**Parameters**

* std::shared_ptr< [System](classmavsdk_1_1_system.md) > **system** - The specific system associated with this plugin.

### ~Transponder() {#classmavsdk_1_1_transponder_1a2bd5706f4b1bd5cf773fdd8870d5c457}
```cpp
mavsdk::Transponder::~Transponder() override
```


Destructor (internal use only).


### Transponder() {#classmavsdk_1_1_transponder_1a610383ecbf848dd7ef214b13c445e170}
```cpp
mavsdk::Transponder::Transponder(const Transponder &other)
```


Copy constructor.


**Parameters**

* const [Transponder](classmavsdk_1_1_transponder.md)& **other** - 

## Member Typdef Documentation


### typedef ResultCallback {#classmavsdk_1_1_transponder_1a57166e61c37ad92ecd5420bc1b7972a5}

```cpp
using mavsdk::Transponder::ResultCallback =  std::function<void(Result)>
```


Callback type for asynchronous [Transponder](classmavsdk_1_1_transponder.md) calls.


### typedef TransponderCallback {#classmavsdk_1_1_transponder_1a239211c3cf8ce383d0f5c82b671aed34}

```cpp
using mavsdk::Transponder::TransponderCallback =  std::function<void(AdsbVehicle)>
```


Callback type for subscribe_transponder.


### typedef TransponderHandle {#classmavsdk_1_1_transponder_1abd96184c88f8bcb8bbb0aa138c3355b7}

```cpp
using mavsdk::Transponder::TransponderHandle =  Handle<AdsbVehicle>
```


[Handle](classmavsdk_1_1_handle.md) type for subscribe_transponder.


## Member Enumeration Documentation


### enum AdsbEmitterType {#classmavsdk_1_1_transponder_1a0b16f74131c28940c6604bc55776c039}


ADSB classification for the type of vehicle emitting the transponder signal.


Value | Description
--- | ---
<span id="classmavsdk_1_1_transponder_1a0b16f74131c28940c6604bc55776c039a625e050b8bdbcdaf58be79b116182983"></span> `NoInfo` | No emitter info.. 
<span id="classmavsdk_1_1_transponder_1a0b16f74131c28940c6604bc55776c039a9914a0ce04a7b7b6a8e39bec55064b82"></span> `Light` | Light emitter.. 
<span id="classmavsdk_1_1_transponder_1a0b16f74131c28940c6604bc55776c039a2660064e68655415da2628c2ae2f7592"></span> `Small` | Small emitter.. 
<span id="classmavsdk_1_1_transponder_1a0b16f74131c28940c6604bc55776c039a3a69b34ce86dacb205936a8094f6c743"></span> `Large` | Large emitter.. 
<span id="classmavsdk_1_1_transponder_1a0b16f74131c28940c6604bc55776c039addfd92ad877929bcac04af0b597509bc"></span> `HighVortexLarge` | High vortex emitter.. 
<span id="classmavsdk_1_1_transponder_1a0b16f74131c28940c6604bc55776c039af9ffe67a20da9cbff56b420fca4bd491"></span> `Heavy` | Heavy emitter.. 
<span id="classmavsdk_1_1_transponder_1a0b16f74131c28940c6604bc55776c039ab6743b28ad73d2ca0cad014b8d85f253"></span> `HighlyManuv` | Highly maneuverable emitter.. 
<span id="classmavsdk_1_1_transponder_1a0b16f74131c28940c6604bc55776c039a39cd0f9efe12f4f3193f0291b7c1689f"></span> `Rotocraft` | Rotorcraft emitter.. 
<span id="classmavsdk_1_1_transponder_1a0b16f74131c28940c6604bc55776c039a3476bf9c3af766198bfbd4f065a51e69"></span> `Unassigned` | Unassigned emitter.. 
<span id="classmavsdk_1_1_transponder_1a0b16f74131c28940c6604bc55776c039a4478825cc7d5fe8b90892985f941acdf"></span> `Glider` | Glider emitter.. 
<span id="classmavsdk_1_1_transponder_1a0b16f74131c28940c6604bc55776c039a8981b42b85c4619a269d92771467c5d1"></span> `LighterAir` | Lighter air emitter.. 
<span id="classmavsdk_1_1_transponder_1a0b16f74131c28940c6604bc55776c039a889d6c668249b544803bafc6c18dbf34"></span> `Parachute` | Parachute emitter.. 
<span id="classmavsdk_1_1_transponder_1a0b16f74131c28940c6604bc55776c039a076f18af3899aa85725250ed87f5edd5"></span> `UltraLight` | Ultra light emitter.. 
<span id="classmavsdk_1_1_transponder_1a0b16f74131c28940c6604bc55776c039aba7f6d4d8fa7dc539c768ddb96a735a7"></span> `Unassigned2` | Unassigned2 emitter.. 
<span id="classmavsdk_1_1_transponder_1a0b16f74131c28940c6604bc55776c039a1b9adcd5e46391adeb16755deed0c1e9"></span> `Uav` | UAV emitter.. 
<span id="classmavsdk_1_1_transponder_1a0b16f74131c28940c6604bc55776c039ad511f8439ecde36647437fbba67a4394"></span> `Space` | Space emitter.. 
<span id="classmavsdk_1_1_transponder_1a0b16f74131c28940c6604bc55776c039a2962ccaa2e25a976b4604df8a62ca4b3"></span> `Unassgined3` | Unassigned3 emitter.. 
<span id="classmavsdk_1_1_transponder_1a0b16f74131c28940c6604bc55776c039aacb0de550adaedb2c0e48bbf7d7fdb86"></span> `EmergencySurface` | Emergency emitter.. 
<span id="classmavsdk_1_1_transponder_1a0b16f74131c28940c6604bc55776c039a4897a01e8c93f52319e630dd79259517"></span> `ServiceSurface` | Service surface emitter.. 
<span id="classmavsdk_1_1_transponder_1a0b16f74131c28940c6604bc55776c039a30f9fbea6febbed04f700fdba031b165"></span> `PointObstacle` | Point obstacle emitter.. 

### enum AdsbAltitudeType {#classmavsdk_1_1_transponder_1a64af76be070a2496e0db1c201ece9313}


Altitude type used in [AdsbVehicle](structmavsdk_1_1_transponder_1_1_adsb_vehicle.md) message.


Value | Description
--- | ---
<span id="classmavsdk_1_1_transponder_1a64af76be070a2496e0db1c201ece9313ada779575df55e1b05ec9e4468ed0d232"></span> `PressureQnh` | Altitude reported from a Baro source using QNH reference. 
<span id="classmavsdk_1_1_transponder_1a64af76be070a2496e0db1c201ece9313aadf9f772a73f364e4ce6c52b9fae740d"></span> `Geometric` | Altitude reported from a GNSS source. 

### enum Result {#classmavsdk_1_1_transponder_1a683477ab9a02d00b2524bc2dcea3ecc8}


Possible results returned for transponder requests.


Value | Description
--- | ---
<span id="classmavsdk_1_1_transponder_1a683477ab9a02d00b2524bc2dcea3ecc8a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | Unknown result. 
<span id="classmavsdk_1_1_transponder_1a683477ab9a02d00b2524bc2dcea3ecc8a505a83f220c02df2f85c3810cd9ceb38"></span> `Success` | Success: the transponder command was accepted by the vehicle. 
<span id="classmavsdk_1_1_transponder_1a683477ab9a02d00b2524bc2dcea3ecc8a1119faf72ba0dfb23aeea644fed960ad"></span> `NoSystem` | No system connected. 
<span id="classmavsdk_1_1_transponder_1a683477ab9a02d00b2524bc2dcea3ecc8a094a6f6b0868122a9dd008cb91c083e4"></span> `ConnectionError` | Connection error. 
<span id="classmavsdk_1_1_transponder_1a683477ab9a02d00b2524bc2dcea3ecc8ad8a942ef2b04672adfafef0ad817a407"></span> `Busy` | Vehicle is busy. 
<span id="classmavsdk_1_1_transponder_1a683477ab9a02d00b2524bc2dcea3ecc8a3398e12855176d55f43d53e04f472c8a"></span> `CommandDenied` | Command refused by vehicle. 
<span id="classmavsdk_1_1_transponder_1a683477ab9a02d00b2524bc2dcea3ecc8ac85a251cc457840f1e032f1b733e9398"></span> `Timeout` | Request timed out. 

## Member Function Documentation


### subscribe_transponder() {#classmavsdk_1_1_transponder_1a1854383cab590dbc28cb18bb8bf3415f}
```cpp
TransponderHandle mavsdk::Transponder::subscribe_transponder(const TransponderCallback &callback)
```


Subscribe to 'transponder' updates.


**Parameters**

* const [TransponderCallback](classmavsdk_1_1_transponder.md#classmavsdk_1_1_transponder_1a239211c3cf8ce383d0f5c82b671aed34)& **callback** - 

**Returns**

&emsp;[TransponderHandle](classmavsdk_1_1_transponder.md#classmavsdk_1_1_transponder_1abd96184c88f8bcb8bbb0aa138c3355b7) - 

### unsubscribe_transponder() {#classmavsdk_1_1_transponder_1a28735123acd679533a315a39dd096db0}
```cpp
void mavsdk::Transponder::unsubscribe_transponder(TransponderHandle handle)
```


Unsubscribe from subscribe_transponder.


**Parameters**

* [TransponderHandle](classmavsdk_1_1_transponder.md#classmavsdk_1_1_transponder_1abd96184c88f8bcb8bbb0aa138c3355b7) **handle** - 

### transponder() {#classmavsdk_1_1_transponder_1a1674c21928af3368e2be84bd182251d9}
```cpp
AdsbVehicle mavsdk::Transponder::transponder() const
```


Poll for '[AdsbVehicle](structmavsdk_1_1_transponder_1_1_adsb_vehicle.md)' (blocking).


**Returns**

&emsp;[AdsbVehicle](structmavsdk_1_1_transponder_1_1_adsb_vehicle.md) - One [AdsbVehicle](structmavsdk_1_1_transponder_1_1_adsb_vehicle.md) update.

### set_rate_transponder_async() {#classmavsdk_1_1_transponder_1a098f5692a3f2b76a201ce0b22a56f0ec}
```cpp
void mavsdk::Transponder::set_rate_transponder_async(double rate_hz, const ResultCallback callback)
```


Set rate to 'transponder' updates.

This function is non-blocking. See 'set_rate_transponder' for the blocking counterpart.

**Parameters**

* double **rate_hz** - 
* const [ResultCallback](classmavsdk_1_1_transponder.md#classmavsdk_1_1_transponder_1a57166e61c37ad92ecd5420bc1b7972a5) **callback** - 

### set_rate_transponder() {#classmavsdk_1_1_transponder_1a9d9f4b4899ef61b6d13d9e25f44dbecd}
```cpp
Result mavsdk::Transponder::set_rate_transponder(double rate_hz) const
```


Set rate to 'transponder' updates.

This function is blocking. See 'set_rate_transponder_async' for the non-blocking counterpart.

**Parameters**

* double **rate_hz** - 

**Returns**

&emsp;[Result](classmavsdk_1_1_transponder.md#classmavsdk_1_1_transponder_1a683477ab9a02d00b2524bc2dcea3ecc8) - Result of request.

### operator=() {#classmavsdk_1_1_transponder_1aaee71763de213002fadeb9dec3ec9fb6}
```cpp
const Transponder& mavsdk::Transponder::operator=(const Transponder &)=delete
```


Equality operator (object is not copyable).


**Parameters**

* const [Transponder](classmavsdk_1_1_transponder.md)&  - 

**Returns**

&emsp;const [Transponder](classmavsdk_1_1_transponder.md) & - 