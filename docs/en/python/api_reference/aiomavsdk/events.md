# Events

Get event notifications, such as takeoff, or arming checks

## `EventsAsync`

Get event notifications, such as takeoff, or arming checks

Async wrapper around `Events` that mirrors the gRPC-based
asyncio API while using the ctypes-based C library directly.

### `subscribe_events`

```python
async def subscribe_events() → AsyncGenerator[Event, None]
```

Subscribe to event updates.

Yields
------

event : Event
The next update

### `subscribe_health_and_arming_checks`

```python
async def subscribe_health_and_arming_checks() → AsyncGenerator[HealthAndArmingCheckReport, None]
```

Subscribe to arming check updates.

Yields
------

health_and_arming_check_report : HealthAndArmingCheckReport
The next update

### `get_health_and_arming_checks_report`

```python
async def get_health_and_arming_checks_report()
```

Get the latest report.

Returns
-------

report : HealthAndArmingCheckReport
Raises
------

EventsError
If the request fails. The error contains the reason for the failure.

### `destroy`

```python
def destroy()
```
