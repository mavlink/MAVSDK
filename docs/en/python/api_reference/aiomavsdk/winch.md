# Winch

Allows users to send winch actions, as well as receive status information from winch systems.

## `WinchAsync`

Allows users to send winch actions, as well as receive status information from winch systems.

Async wrapper around `Winch` that mirrors the gRPC-based
asyncio API while using the ctypes-based C library directly.

### `subscribe_status`

```python
async def subscribe_status() → AsyncGenerator[Status, None]
```

Subscribe to 'winch status' updates.

Yields
------

status : Status
The next update

### `status`

```python
async def status()
```

Subscribe to 'winch status' updates.

Returns
-------

status : Status
Raises
------

WinchError
If the request fails. The error contains the reason for the failure.

### `relax`

```python
async def relax(instance)
```

Allow motor to freewheel.

Parameters
----------

instance : int
Raises
------

WinchError
If the request fails. The error contains the reason for the failure.

### `relative_length_control`

```python
async def relative_length_control(instance, length_m, rate_m_s)
```

Wind or unwind specified length of line, optionally using specified rate.

Parameters
----------

instance : int
length_m : float
rate_m_s : float
Raises
------

WinchError
If the request fails. The error contains the reason for the failure.

### `rate_control`

```python
async def rate_control(instance, rate_m_s)
```

Wind or unwind line at specified rate.

Parameters
----------

instance : int
rate_m_s : float
Raises
------

WinchError
If the request fails. The error contains the reason for the failure.

### `lock`

```python
async def lock(instance)
```

Perform the locking sequence to relieve motor while in the fully retracted position.

Parameters
----------

instance : int
Raises
------

WinchError
If the request fails. The error contains the reason for the failure.

### `deliver`

```python
async def deliver(instance)
```

Sequence of drop, slow down, touch down, reel up, lock.

Parameters
----------

instance : int
Raises
------

WinchError
If the request fails. The error contains the reason for the failure.

### `hold`

```python
async def hold(instance)
```

Engage motor and hold current position.

Parameters
----------

instance : int
Raises
------

WinchError
If the request fails. The error contains the reason for the failure.

### `retract`

```python
async def retract(instance)
```

Return the reel to the fully retracted position.

Parameters
----------

instance : int
Raises
------

WinchError
If the request fails. The error contains the reason for the failure.

### `load_line`

```python
async def load_line(instance)
```

Load the reel with line.

The winch will calculate the total loaded length and stop when the tension exceeds a threshold.

Parameters
----------

instance : int
Raises
------

WinchError
    If the request fails. The error contains the reason for the failure.

### `abandon_line`

```python
async def abandon_line(instance)
```

Spool out the entire length of the line.

Parameters
----------

instance : int
Raises
------

WinchError
If the request fails. The error contains the reason for the failure.

### `load_payload`

```python
async def load_payload(instance)
```

Spools out just enough to present the hook to the user to load the payload.

Parameters
----------

instance : int
Raises
------

WinchError
If the request fails. The error contains the reason for the failure.
