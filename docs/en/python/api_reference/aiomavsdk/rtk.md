# Rtk

Service to send RTK corrections to the vehicle.

## `RtkAsync`

Service to send RTK corrections to the vehicle.

Async wrapper around :class:`Rtk` that mirrors the gRPC-based
asyncio API while using the ctypes-based C library directly.

### `send_rtcm_data`

```python
async def send_rtcm_data(rtcm_data)
```

Send RTCM data.

Parameters
----------
rtcm_data : RtcmData
Raises
------
RtkError
If the request fails. The error contains the reason for the failure.

### `destroy`

```python
def destroy()
```
