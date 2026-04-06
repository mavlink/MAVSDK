# Info

Provide information about the hardware and/or software of a system.

## `InfoAsync`

Provide information about the hardware and/or software of a system.

Async wrapper around `Info` that mirrors the gRPC-based
asyncio API while using the ctypes-based C library directly.

### `get_flight_information`

```python
async def get_flight_information()
```

Get flight information of the system.

Returns
-------

flight_info : FlightInfo
Raises
------

InfoError
If the request fails. The error contains the reason for the failure.

### `get_identification`

```python
async def get_identification()
```

Get the identification of the system.

Returns
-------

identification : Identification
Raises
------

InfoError
If the request fails. The error contains the reason for the failure.

### `get_product`

```python
async def get_product()
```

Get product information of the system.

Returns
-------

product : Product
Raises
------

InfoError
If the request fails. The error contains the reason for the failure.

### `get_version`

```python
async def get_version()
```

Get the version information of the system.

Returns
-------

version : Version
Raises
------

InfoError
If the request fails. The error contains the reason for the failure.

### `get_speed_factor`

```python
async def get_speed_factor()
```

Get the speed factor of a simulation (with lockstep a simulation can run faster or slower than realtime).

Returns
-------

speed_factor : float
Raises
------

InfoError
If the request fails. The error contains the reason for the failure.

### `subscribe_flight_information`

```python
async def subscribe_flight_information() → AsyncGenerator[FlightInfo, None]
```

Subscribe to 'flight information' updates.

Yields
------

flight_info : FlightInfo
The next update

### `destroy`

```python
def destroy()
```
