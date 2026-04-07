# Transponder

Allow users to get ADS-B information
and set ADS-B update rates.

## `TransponderAsync`

Allow users to get ADS-B information
and set ADS-B update rates.

Async wrapper around `Transponder` that mirrors the gRPC-based
asyncio API while using the ctypes-based C library directly.

### `subscribe_transponder`

```python
async def subscribe_transponder() → AsyncGenerator[AdsbVehicle, None]
```

Subscribe to 'transponder' updates.

Yields
------

adsb_vehicle : AdsbVehicle
The next update

### `transponder`

```python
async def transponder()
```

Subscribe to 'transponder' updates.

Returns
-------

transponder : AdsbVehicle
Raises
------

TransponderError
If the request fails. The error contains the reason for the failure.

### `set_rate_transponder`

```python
async def set_rate_transponder(rate_hz)
```

Set rate to 'transponder' updates.

Parameters
----------

rate_hz : float
Raises
------

TransponderError
If the request fails. The error contains the reason for the failure.
