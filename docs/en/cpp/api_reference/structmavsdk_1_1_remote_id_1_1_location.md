# mavsdk::RemoteId::Location Struct Reference
`#include: UNKNOWN`

----


## Public Types


Type | Description
--- | ---
enum [Status](#structmavsdk_1_1_remote_id_1_1_location_1a2e4d3aa74008d08c0b100dca6f866de1) | 
enum [HeightRef](#structmavsdk_1_1_remote_id_1_1_location_1a979ef5efb86bae44515b19ea071b23bd) | 
enum [HorAcc](#structmavsdk_1_1_remote_id_1_1_location_1afe5670937f44acec31645da7419d75ed) | 
enum [VerAcc](#structmavsdk_1_1_remote_id_1_1_location_1aecc40ed3c0b6cf01c241272b659729a3) | 
enum [SpeedAcc](#structmavsdk_1_1_remote_id_1_1_location_1a6f0c54daa485f00855b1459a5e904037) | 
enum [TimeAcc](#structmavsdk_1_1_remote_id_1_1_location_1a5b18b7e68ac6389a116730517b69a021) | 

## Data Fields


[Status](structmavsdk_1_1_remote_id_1_1_location.md#structmavsdk_1_1_remote_id_1_1_location_1a2e4d3aa74008d08c0b100dca6f866de1) [status](#structmavsdk_1_1_remote_id_1_1_location_1a5f8660072b73a7e8ca727485822215c4) {} - Indicates whether the unmanned aircraft is on the ground or in the air.

uint32_t [direction_deg](#structmavsdk_1_1_remote_id_1_1_location_1a8779a50080ca5a08602c8e1809c51d9e) {} - Direction over ground measured clockwise from true North: 0 - 359 deg.

float [speed_horizontal_m_s](#structmavsdk_1_1_remote_id_1_1_location_1a3cb06cb9e7ada424bd34439604fad72d) {} - Ground speed in meters per second. Positive only.

float [speed_vertical_m_s](#structmavsdk_1_1_remote_id_1_1_location_1a6704a6aaab52a6ce01c80c5580d4ede4) {} - The vertical speed in meters per second. Up is positive.

double [latitude_deg](#structmavsdk_1_1_remote_id_1_1_location_1ac333dc6081758c5df17e323d9e880150) {} - Current latitude of the unmanned aircraft.

double [longitude_deg](#structmavsdk_1_1_remote_id_1_1_location_1a952ea3e11e9468896098ac515eb26d50) {} - Current longitude of the unmanned aircraft.

float [altitude_barometric_m](#structmavsdk_1_1_remote_id_1_1_location_1a25ce4a6dd019beb0353c94bc308a9384) {} - The altitude calculated from the barometric pressure.

float [altitude_geodetic_m](#structmavsdk_1_1_remote_id_1_1_location_1a151c226fd8ad484a644a265bc976a377) {} - The geodetic altitude as defined by WGS84.

[HeightRef](structmavsdk_1_1_remote_id_1_1_location.md#structmavsdk_1_1_remote_id_1_1_location_1a979ef5efb86bae44515b19ea071b23bd) [height_reference](#structmavsdk_1_1_remote_id_1_1_location_1a64e11e22d956bbf5fbbc85f1a2f2d7a1) {} - Indicates the reference point for the height field.

float [height_m](#structmavsdk_1_1_remote_id_1_1_location_1aded6c7889936b3b54e964009eedd6c4c) {} - The current height of the unmanned aircraft. As indicated by height_reference.

uint64_t [time_utc_us](#structmavsdk_1_1_remote_id_1_1_location_1a7bfe8808c7f858185024f35474fbaafc) {} - Timestamp in UTC (since UNIX epoch) in microseconds.

[HorAcc](structmavsdk_1_1_remote_id_1_1_location.md#structmavsdk_1_1_remote_id_1_1_location_1afe5670937f44acec31645da7419d75ed) [horizontal_accuracy](#structmavsdk_1_1_remote_id_1_1_location_1a4dd1df7b7b4c023521ce6051b5fc4778) {} - The accuracy of the horizontal position.

[VerAcc](structmavsdk_1_1_remote_id_1_1_location.md#structmavsdk_1_1_remote_id_1_1_location_1aecc40ed3c0b6cf01c241272b659729a3) [vertical_accuracy](#structmavsdk_1_1_remote_id_1_1_location_1a61e645f4ea34a67f8f24c1f6ab219d18) {} - The accuracy of the vertical position.

[VerAcc](structmavsdk_1_1_remote_id_1_1_location.md#structmavsdk_1_1_remote_id_1_1_location_1aecc40ed3c0b6cf01c241272b659729a3) [barometer_accuracy](#structmavsdk_1_1_remote_id_1_1_location_1ad9f27a2896b8ace4fa4789f76f2f1525) {} - The accuracy of the barometric altitude.

[SpeedAcc](structmavsdk_1_1_remote_id_1_1_location.md#structmavsdk_1_1_remote_id_1_1_location_1a6f0c54daa485f00855b1459a5e904037) [speed_accuracy](#structmavsdk_1_1_remote_id_1_1_location_1a3197602c47fce8ae9949f4c26e2a8a1d) {} - The accuracy of the horizontal and vertical speed.

[TimeAcc](structmavsdk_1_1_remote_id_1_1_location.md#structmavsdk_1_1_remote_id_1_1_location_1a5b18b7e68ac6389a116730517b69a021) [timestamp_accuracy](#structmavsdk_1_1_remote_id_1_1_location_1a341024b473bdac57d3476b9cb3d9e497) {} - The accuracy of the timestamps.


## Member Enumeration Documentation


### enum Status {#structmavsdk_1_1_remote_id_1_1_location_1a2e4d3aa74008d08c0b100dca6f866de1}


Value | Description
--- | ---
<span id="structmavsdk_1_1_remote_id_1_1_location_1a2e4d3aa74008d08c0b100dca6f866de1a2af457eff1694fff8c7ef6c3a8974ce6"></span> `Undeclared` | The status of the (UA) Unmanned Aircraft is undefined.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1a2e4d3aa74008d08c0b100dca6f866de1a3519d51443d41746a097cd54cd5c11cf"></span> `Ground` | The UA is on the ground.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1a2e4d3aa74008d08c0b100dca6f866de1a5c1be1cc479d7bd224548b4ce08c8ea6"></span> `Airborne` | The UA is in the air.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1a2e4d3aa74008d08c0b100dca6f866de1aa3fa706f20bc0b7858b7ae6932261940"></span> `Emergency` | The UA is having an emergency.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1a2e4d3aa74008d08c0b100dca6f866de1aedd4ec22e417b97cda3338d6d74ec44d"></span> `RemoteIdSystemFailure` | The remote ID system is failing or unreliable in some way.. 

### enum HeightRef {#structmavsdk_1_1_remote_id_1_1_location_1a979ef5efb86bae44515b19ea071b23bd}


Value | Description
--- | ---
<span id="structmavsdk_1_1_remote_id_1_1_location_1a979ef5efb86bae44515b19ea071b23bda4af2e18490f5f0dc9f42699152e1f4f4"></span> `OverTakeoff` | The height field is relative to the take-off location.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1a979ef5efb86bae44515b19ea071b23bda3aa6eb3e467f3e016036abb9ee0044a0"></span> `OverGround` | The height field is relative to ground.. 

### enum HorAcc {#structmavsdk_1_1_remote_id_1_1_location_1afe5670937f44acec31645da7419d75ed}


Value | Description
--- | ---
<span id="structmavsdk_1_1_remote_id_1_1_location_1afe5670937f44acec31645da7419d75eda88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | The horizontal accuracy is unknown.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1afe5670937f44acec31645da7419d75eda84fdd3398e9195567eeb631cf532df6a"></span> `Nm10` | The horizontal accuracy is smaller than 10 Nautical Miles. 18.52 km.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1afe5670937f44acec31645da7419d75eda017a0efd3f2fbf1e5f012eddf9d632cd"></span> `Nm4` | The horizontal accuracy is smaller than 4 Nautical Miles. 7.408 km.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1afe5670937f44acec31645da7419d75eda58a5ac857e3a6ca1741660a052a234b6"></span> `Nm2` | The horizontal accuracy is smaller than 2 Nautical Miles. 3.704 km.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1afe5670937f44acec31645da7419d75eda7e75f693d04b81d995afa30bca6e0764"></span> `Nm1` | The horizontal accuracy is smaller than 1 Nautical Miles. 1.852 km.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1afe5670937f44acec31645da7419d75eda34ab232c0617a9c2b11a175051c12df4"></span> `Nm05` | The horizontal accuracy is smaller than 0.5 Nautical Miles. 926 m.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1afe5670937f44acec31645da7419d75edab24e12ddb8805f815df9d14e00cb0795"></span> `Nm03` | The horizontal accuracy is smaller than 0.3 Nautical Miles. 555.6 m.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1afe5670937f44acec31645da7419d75eda257ad43f0b28c87a25a5aa77f6936ed1"></span> `Nm01` | The horizontal accuracy is smaller than 0.1 Nautical Miles. 185.2 m.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1afe5670937f44acec31645da7419d75eda987c1ace84f6b94fb56eea7f3aeb26e6"></span> `Nm005` | The horizontal accuracy is smaller than 0.05 Nautical Miles. 92.6 m.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1afe5670937f44acec31645da7419d75eda7e1ed3ab0cc4f976a4daa8d3e7d7f3c7"></span> `Meter30` | The horizontal accuracy is smaller than 30 meter.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1afe5670937f44acec31645da7419d75edaa61221583d351bf6c7fc8102c04c9ae6"></span> `Meter10` | The horizontal accuracy is smaller than 10 meter.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1afe5670937f44acec31645da7419d75edad5dd3770cfd3e9ecb5788541bb39324b"></span> `Meter3` | The horizontal accuracy is smaller than 3 meter.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1afe5670937f44acec31645da7419d75eda740d971eeee32a09e3c197118d39c61b"></span> `Meter1` | The horizontal accuracy is smaller than 1 meter.. 

### enum VerAcc {#structmavsdk_1_1_remote_id_1_1_location_1aecc40ed3c0b6cf01c241272b659729a3}


Value | Description
--- | ---
<span id="structmavsdk_1_1_remote_id_1_1_location_1aecc40ed3c0b6cf01c241272b659729a3a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | The vertical accuracy is unknown.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1aecc40ed3c0b6cf01c241272b659729a3ad3c45f00dc468326749bf94a8bd9b84f"></span> `Meter150` | The vertical accuracy is smaller than 150 meter.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1aecc40ed3c0b6cf01c241272b659729a3af5fed292120ee7551f9d3d482f1f2be0"></span> `Meter45` | The vertical accuracy is smaller than 45 meter.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1aecc40ed3c0b6cf01c241272b659729a3a925301e9d1bbd28db688883f6590382b"></span> `Meter25` | The vertical accuracy is smaller than 25 meter.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1aecc40ed3c0b6cf01c241272b659729a3aa61221583d351bf6c7fc8102c04c9ae6"></span> `Meter10` | The vertical accuracy is smaller than 10 meter.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1aecc40ed3c0b6cf01c241272b659729a3ad5dd3770cfd3e9ecb5788541bb39324b"></span> `Meter3` | The vertical accuracy is smaller than 3 meter.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1aecc40ed3c0b6cf01c241272b659729a3a740d971eeee32a09e3c197118d39c61b"></span> `Meter1` | The vertical accuracy is smaller than 1 meter.. 

### enum SpeedAcc {#structmavsdk_1_1_remote_id_1_1_location_1a6f0c54daa485f00855b1459a5e904037}


Value | Description
--- | ---
<span id="structmavsdk_1_1_remote_id_1_1_location_1a6f0c54daa485f00855b1459a5e904037a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | The speed accuracy is unknown.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1a6f0c54daa485f00855b1459a5e904037a669f05d7e464691d9432398ffe87ec4b"></span> `MetersPerSecond10` | The speed accuracy is smaller than 10 meters per second.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1a6f0c54daa485f00855b1459a5e904037a614bf4070724024e405515a25d38da12"></span> `MetersPerSecond3` | The speed accuracy is smaller than 3 meters per second.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1a6f0c54daa485f00855b1459a5e904037a028bfc4d06016e15ecf30f87f986e22d"></span> `MetersPerSecond1` | The speed accuracy is smaller than 1 meters per second.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1a6f0c54daa485f00855b1459a5e904037a5891307a369406559de89de716706cb2"></span> `MetersPerSecond03` | The speed accuracy is smaller than 0.3 meters per second.. 

### enum TimeAcc {#structmavsdk_1_1_remote_id_1_1_location_1a5b18b7e68ac6389a116730517b69a021}


Value | Description
--- | ---
<span id="structmavsdk_1_1_remote_id_1_1_location_1a5b18b7e68ac6389a116730517b69a021a88183b946cc5f0e8c96b2e66e1c74a7e"></span> `Unknown` | The timestamp accuracy is unknown.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1a5b18b7e68ac6389a116730517b69a021a9723c8741740a5baeed2eee2ddcf92ca"></span> `Second01` | The timestamp accuracy is smaller than or equal to 0.1 second.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1a5b18b7e68ac6389a116730517b69a021abc1c487f037a09def20d4a0858a08de8"></span> `Second02` | The timestamp accuracy is smaller than or equal to 0.2 second.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1a5b18b7e68ac6389a116730517b69a021ae3c052ba2bf561d9fe21ee71fe4f1d87"></span> `Second03` | The timestamp accuracy is smaller than or equal to 0.3 second.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1a5b18b7e68ac6389a116730517b69a021afb8fff3660e92e2f926d59dea16e6605"></span> `Second04` | The timestamp accuracy is smaller than or equal to 0.4 second.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1a5b18b7e68ac6389a116730517b69a021ae684d9e60ae3692ea968f68795cf13a7"></span> `Second05` | The timestamp accuracy is smaller than or equal to 0.5 second.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1a5b18b7e68ac6389a116730517b69a021ad57931d2d5d52171b650d7ae64404502"></span> `Second06` | The timestamp accuracy is smaller than or equal to 0.6 second.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1a5b18b7e68ac6389a116730517b69a021a0afd7b88b694d8992c92bea3631ee73e"></span> `Second07` | The timestamp accuracy is smaller than or equal to 0.7 second.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1a5b18b7e68ac6389a116730517b69a021afcc19dbb04e764347a692602106b19bf"></span> `Second08` | The timestamp accuracy is smaller than or equal to 0.8 second.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1a5b18b7e68ac6389a116730517b69a021a4c1da4813cb1417f730f5e1dc8a70780"></span> `Second09` | The timestamp accuracy is smaller than or equal to 0.9 second.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1a5b18b7e68ac6389a116730517b69a021aef19212e5736c382e1acd30e60970aa8"></span> `Second10` | The timestamp accuracy is smaller than or equal to 1.0 second.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1a5b18b7e68ac6389a116730517b69a021af5790c8e73696923e34651926d5aa714"></span> `Second11` | The timestamp accuracy is smaller than or equal to 1.1 second.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1a5b18b7e68ac6389a116730517b69a021a7e989f2a2202d8eaf057110a1515c3cd"></span> `Second12` | The timestamp accuracy is smaller than or equal to 1.2 second.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1a5b18b7e68ac6389a116730517b69a021abdd113343418c67732df5e348081bebd"></span> `Second13` | The timestamp accuracy is smaller than or equal to 1.3 second.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1a5b18b7e68ac6389a116730517b69a021a2fb350692def4a53493bde06fb69d376"></span> `Second14` | The timestamp accuracy is smaller than or equal to 1.4 second.. 
<span id="structmavsdk_1_1_remote_id_1_1_location_1a5b18b7e68ac6389a116730517b69a021a22bc12031858774301683e123d30f8f0"></span> `Second15` | The timestamp accuracy is smaller than or equal to 1.5 second.. 

## Field Documentation


### status {#structmavsdk_1_1_remote_id_1_1_location_1a5f8660072b73a7e8ca727485822215c4}

```cpp
Status mavsdk::RemoteId::Location::status {}
```


Indicates whether the unmanned aircraft is on the ground or in the air.


### direction_deg {#structmavsdk_1_1_remote_id_1_1_location_1a8779a50080ca5a08602c8e1809c51d9e}

```cpp
uint32_t mavsdk::RemoteId::Location::direction_deg {}
```


Direction over ground measured clockwise from true North: 0 - 359 deg.


### speed_horizontal_m_s {#structmavsdk_1_1_remote_id_1_1_location_1a3cb06cb9e7ada424bd34439604fad72d}

```cpp
float mavsdk::RemoteId::Location::speed_horizontal_m_s {}
```


Ground speed in meters per second. Positive only.


### speed_vertical_m_s {#structmavsdk_1_1_remote_id_1_1_location_1a6704a6aaab52a6ce01c80c5580d4ede4}

```cpp
float mavsdk::RemoteId::Location::speed_vertical_m_s {}
```


The vertical speed in meters per second. Up is positive.


### latitude_deg {#structmavsdk_1_1_remote_id_1_1_location_1ac333dc6081758c5df17e323d9e880150}

```cpp
double mavsdk::RemoteId::Location::latitude_deg {}
```


Current latitude of the unmanned aircraft.


### longitude_deg {#structmavsdk_1_1_remote_id_1_1_location_1a952ea3e11e9468896098ac515eb26d50}

```cpp
double mavsdk::RemoteId::Location::longitude_deg {}
```


Current longitude of the unmanned aircraft.


### altitude_barometric_m {#structmavsdk_1_1_remote_id_1_1_location_1a25ce4a6dd019beb0353c94bc308a9384}

```cpp
float mavsdk::RemoteId::Location::altitude_barometric_m {}
```


The altitude calculated from the barometric pressure.


### altitude_geodetic_m {#structmavsdk_1_1_remote_id_1_1_location_1a151c226fd8ad484a644a265bc976a377}

```cpp
float mavsdk::RemoteId::Location::altitude_geodetic_m {}
```


The geodetic altitude as defined by WGS84.


### height_reference {#structmavsdk_1_1_remote_id_1_1_location_1a64e11e22d956bbf5fbbc85f1a2f2d7a1}

```cpp
HeightRef mavsdk::RemoteId::Location::height_reference {}
```


Indicates the reference point for the height field.


### height_m {#structmavsdk_1_1_remote_id_1_1_location_1aded6c7889936b3b54e964009eedd6c4c}

```cpp
float mavsdk::RemoteId::Location::height_m {}
```


The current height of the unmanned aircraft. As indicated by height_reference.


### time_utc_us {#structmavsdk_1_1_remote_id_1_1_location_1a7bfe8808c7f858185024f35474fbaafc}

```cpp
uint64_t mavsdk::RemoteId::Location::time_utc_us {}
```


Timestamp in UTC (since UNIX epoch) in microseconds.


### horizontal_accuracy {#structmavsdk_1_1_remote_id_1_1_location_1a4dd1df7b7b4c023521ce6051b5fc4778}

```cpp
HorAcc mavsdk::RemoteId::Location::horizontal_accuracy {}
```


The accuracy of the horizontal position.


### vertical_accuracy {#structmavsdk_1_1_remote_id_1_1_location_1a61e645f4ea34a67f8f24c1f6ab219d18}

```cpp
VerAcc mavsdk::RemoteId::Location::vertical_accuracy {}
```


The accuracy of the vertical position.


### barometer_accuracy {#structmavsdk_1_1_remote_id_1_1_location_1ad9f27a2896b8ace4fa4789f76f2f1525}

```cpp
VerAcc mavsdk::RemoteId::Location::barometer_accuracy {}
```


The accuracy of the barometric altitude.


### speed_accuracy {#structmavsdk_1_1_remote_id_1_1_location_1a3197602c47fce8ae9949f4c26e2a8a1d}

```cpp
SpeedAcc mavsdk::RemoteId::Location::speed_accuracy {}
```


The accuracy of the horizontal and vertical speed.


### timestamp_accuracy {#structmavsdk_1_1_remote_id_1_1_location_1a341024b473bdac57d3476b9cb3d9e497}

```cpp
TimeAcc mavsdk::RemoteId::Location::timestamp_accuracy {}
```


The accuracy of the timestamps.

