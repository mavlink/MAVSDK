# mavsdk::RemoteId::BasicId Struct Reference
`#include: UNKNOWN`

----


## Public Types


Type | Description
--- | ---
enum [IdType](#structmavsdk_1_1_remote_id_1_1_basic_id_1a5e2f7f685e0b408d29262a0816811f7b) | 
enum [UasType](#structmavsdk_1_1_remote_id_1_1_basic_id_1a04c1d046beca708faf48a9e53f726edf) | 

## Data Fields


[IdType](structmavsdk_1_1_remote_id_1_1_basic_id.md#structmavsdk_1_1_remote_id_1_1_basic_id_1a5e2f7f685e0b408d29262a0816811f7b) [id_type](#structmavsdk_1_1_remote_id_1_1_basic_id_1ac316d55a8263375e8752a8b0bd5717f8) {} - Indicates the format for the uas_id field of this message.

[UasType](structmavsdk_1_1_remote_id_1_1_basic_id.md#structmavsdk_1_1_remote_id_1_1_basic_id_1a04c1d046beca708faf48a9e53f726edf) [ua_type](#structmavsdk_1_1_remote_id_1_1_basic_id_1ac02581d00adb3a85f417abedceacfc87) {} - Indicates the type of UA (Unmanned Aircraft).

std::string [uas_id](#structmavsdk_1_1_remote_id_1_1_basic_id_1ae5799be973e7913f9761b65a4eee4e58) {} - UAS ID following the format specified by id_type.


## Member Enumeration Documentation


### enum IdType {#structmavsdk_1_1_remote_id_1_1_basic_id_1a5e2f7f685e0b408d29262a0816811f7b}


Value | Description
--- | ---
<span id="structmavsdk_1_1_remote_id_1_1_basic_id_1a5e2f7f685e0b408d29262a0816811f7ba6adf97f83acf6453d4a6a4b1070f3754"></span> `None` | No type defined.. 
<span id="structmavsdk_1_1_remote_id_1_1_basic_id_1a5e2f7f685e0b408d29262a0816811f7baa54372ef430f726c8343fc5550fade42"></span> `SerialNumber` | Manufacturer Serial Number (ANSI/CTA-2063 format).. 
<span id="structmavsdk_1_1_remote_id_1_1_basic_id_1a5e2f7f685e0b408d29262a0816811f7bae512bb0c6f6d459b55c6929a563ea746"></span> `CaaRegistrationId` | CAA (Civil Aviation Authority) registered ID. Format: [ICAO Country Code].[CAA Assigned ID].. 
<span id="structmavsdk_1_1_remote_id_1_1_basic_id_1a5e2f7f685e0b408d29262a0816811f7ba8d92461a9df5bbcb757e6949ce15c754"></span> `UtmAssignedUuid` | UTM (Unmanned Traffic Management) assigned UUID (RFC4122).. 
<span id="structmavsdk_1_1_remote_id_1_1_basic_id_1a5e2f7f685e0b408d29262a0816811f7ba1fbaffa3139f0e39cb706765c14ebb98"></span> `SpecificSessionId` | A 20 byte ID for a specific flight/session.. 

### enum UasType {#structmavsdk_1_1_remote_id_1_1_basic_id_1a04c1d046beca708faf48a9e53f726edf}


Value | Description
--- | ---
<span id="structmavsdk_1_1_remote_id_1_1_basic_id_1a04c1d046beca708faf48a9e53f726edfa6adf97f83acf6453d4a6a4b1070f3754"></span> `None` | No UA (Unmanned Aircraft) type defined.. 
<span id="structmavsdk_1_1_remote_id_1_1_basic_id_1a04c1d046beca708faf48a9e53f726edfade52829dd983634cfe2b05011a5a6089"></span> `Aeroplane` | Aeroplane/Airplane. Fixed wing.. 
<span id="structmavsdk_1_1_remote_id_1_1_basic_id_1a04c1d046beca708faf48a9e53f726edfa7779c057882da6f023ec32bd00e6b656"></span> `HelicopterOrMultirotor` | Helicopter or multirotor.. 
<span id="structmavsdk_1_1_remote_id_1_1_basic_id_1a04c1d046beca708faf48a9e53f726edfa38e0e67fe8415b21281292ac29411b54"></span> `Gyroplane` | Gyroplane.. 
<span id="structmavsdk_1_1_remote_id_1_1_basic_id_1a04c1d046beca708faf48a9e53f726edfada7bb3331bff2cdb9762717835943ecc"></span> `HybridLift` | VTOL (Vertical Take-Off and Landing). Fixed wing aircraft that can take off vertically.. 
<span id="structmavsdk_1_1_remote_id_1_1_basic_id_1a04c1d046beca708faf48a9e53f726edfa24f14913c97f3201b7438dabfb3682e0"></span> `Ornithopter` | Ornithopter.. 
<span id="structmavsdk_1_1_remote_id_1_1_basic_id_1a04c1d046beca708faf48a9e53f726edfa4478825cc7d5fe8b90892985f941acdf"></span> `Glider` | Glider.. 
<span id="structmavsdk_1_1_remote_id_1_1_basic_id_1a04c1d046beca708faf48a9e53f726edfa65c449ad44e2c0b6c1fffc5c5cea7413"></span> `Kite` | Kite.. 
<span id="structmavsdk_1_1_remote_id_1_1_basic_id_1a04c1d046beca708faf48a9e53f726edfa3dd340796c8755fa272c9b776f7b1058"></span> `FreeBalloon` | Free Balloon.. 
<span id="structmavsdk_1_1_remote_id_1_1_basic_id_1a04c1d046beca708faf48a9e53f726edfa45437b7f0c093be0a3b50f06b6c71908"></span> `CaptiveBalloon` | Captive Balloon.. 
<span id="structmavsdk_1_1_remote_id_1_1_basic_id_1a04c1d046beca708faf48a9e53f726edfade750545706d10adc6a3b6ca1b0f7c3f"></span> `Airship` | Airship. E.g. a blimp.. 
<span id="structmavsdk_1_1_remote_id_1_1_basic_id_1a04c1d046beca708faf48a9e53f726edfaae50eb8262c2e64baad14d9ffbfd2b43"></span> `FreeFallParachute` | Free Fall/Parachute (unpowered).. 
<span id="structmavsdk_1_1_remote_id_1_1_basic_id_1a04c1d046beca708faf48a9e53f726edfa15e8bfff2c10ca22da244d85b2bf7d94"></span> `Rocket` | Rocket.. 
<span id="structmavsdk_1_1_remote_id_1_1_basic_id_1a04c1d046beca708faf48a9e53f726edfa6bdebe35fe6e80b4db020b438528a20c"></span> `TetheredPoweredAircraft` | Tethered powered aircraft.. 
<span id="structmavsdk_1_1_remote_id_1_1_basic_id_1a04c1d046beca708faf48a9e53f726edfa9359d5f3ea6721150c54b95c46f49cba"></span> `GroundObstacle` | Ground Obstacle.. 
<span id="structmavsdk_1_1_remote_id_1_1_basic_id_1a04c1d046beca708faf48a9e53f726edfa6311ae17c1ee52b36e68aaf4ad066387"></span> `Other` | Other type of aircraft not listed earlier.. 

## Field Documentation


### id_type {#structmavsdk_1_1_remote_id_1_1_basic_id_1ac316d55a8263375e8752a8b0bd5717f8}

```cpp
IdType mavsdk::RemoteId::BasicId::id_type {}
```


Indicates the format for the uas_id field of this message.


### ua_type {#structmavsdk_1_1_remote_id_1_1_basic_id_1ac02581d00adb3a85f417abedceacfc87}

```cpp
UasType mavsdk::RemoteId::BasicId::ua_type {}
```


Indicates the type of UA (Unmanned Aircraft).


### uas_id {#structmavsdk_1_1_remote_id_1_1_basic_id_1ae5799be973e7913f9761b65a4eee4e58}

```cpp
std::string mavsdk::RemoteId::BasicId::uas_id {}
```


UAS ID following the format specified by id_type.

