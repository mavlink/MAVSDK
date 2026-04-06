# Rtk

Service to send RTK corrections to the vehicle.

## Enums

### `RtkResult`

Possible results returned for rtk requests.

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `SUCCESS` (1) | |
| `TOO_LONG` (2) | |
| `NO_SYSTEM` (3) | |
| `CONNECTION_ERROR` (4) | |

## Structs

### `RtcmDataCStruct`

Internal C structure for RtcmData.
Used only for C library communication.

### `RtcmData`

RTCM data type

**Fields:**

- `data_base64`

## `Rtk`

Service to send RTK corrections to the vehicle.

### `send_rtcm_data`

```python
def send_rtcm_data(rtcm_data)
```

Get send_rtcm_data (blocking)

### `destroy`

```python
def destroy()
```

Destroy the plugin instance
