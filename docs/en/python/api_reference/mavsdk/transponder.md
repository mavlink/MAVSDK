# Transponder

Allow users to get ADS-B information
and set ADS-B update rates.

## Enums

### `AdsbEmitterType`

ADSB classification for the type of vehicle emitting the transponder signal.

| Value | Description |
|-------|-------------|
| `NO_INFO` (0) | |
| `LIGHT` (1) | |
| `SMALL` (2) | |
| `LARGE` (3) | |
| `HIGH_VORTEX_LARGE` (4) | |
| `HEAVY` (5) | |
| `HIGHLY_MANUV` (6) | |
| `ROTOCRAFT` (7) | |
| `UNASSIGNED` (8) | |
| `GLIDER` (9) | |
| `LIGHTER_AIR` (10) | |
| `PARACHUTE` (11) | |
| `ULTRA_LIGHT` (12) | |
| `UNASSIGNED2` (13) | |
| `UAV` (14) | |
| `SPACE` (15) | |
| `UNASSGINED3` (16) | |
| `EMERGENCY_SURFACE` (17) | |
| `SERVICE_SURFACE` (18) | |
| `POINT_OBSTACLE` (19) | |

### `AdsbAltitudeType`

Altitude type used in AdsbVehicle message

| Value | Description |
|-------|-------------|
| `PRESSURE_QNH` (0) | |
| `GEOMETRIC` (1) | |

### `TransponderResult`

Possible results returned for transponder requests.

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `SUCCESS` (1) | |
| `NO_SYSTEM` (2) | |
| `CONNECTION_ERROR` (3) | |
| `BUSY` (4) | |
| `COMMAND_DENIED` (5) | |
| `TIMEOUT` (6) | |

## Structs

### `AdsbVehicleCStruct`

Internal C structure for AdsbVehicle.
Used only for C library communication.

### `AdsbVehicle`

ADSB Vehicle type.

**Fields:**

- `icao_address`
- `latitude_deg`
- `longitude_deg`
- `altitude_type`
- `absolute_altitude_m`
- `heading_deg`
- `horizontal_velocity_m_s`
- `vertical_velocity_m_s`
- `callsign`
- `emitter_type`
- `squawk`
- `tslc_s`

## `Transponder`

Allow users to get ADS-B information
and set ADS-B update rates.

### `subscribe_transponder`

```python
def subscribe_transponder(callback: Callable, user_data: Any = None)
```

Subscribe to 'transponder' updates.

### `unsubscribe_transponder`

```python
def unsubscribe_transponder(handle: ctypes.c_void_p)
```

Unsubscribe from transponder

### `transponder`

```python
def transponder()
```

Get transponder (blocking)

### `set_rate_transponder_async`

```python
def set_rate_transponder_async(rate_hz, callback: Callable, user_data: Any = None)
```

Set rate to 'transponder' updates.

### `set_rate_transponder`

```python
def set_rate_transponder(rate_hz)
```

Get set_rate_transponder (blocking)

### `destroy`

```python
def destroy()
```

Destroy the plugin instance
