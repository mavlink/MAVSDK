# Tune

Enable creating and sending a tune to be played on the system.

## `TuneAsync`

Enable creating and sending a tune to be played on the system.

Async wrapper around `Tune` that mirrors the gRPC-based
asyncio API while using the ctypes-based C library directly.

### `play_tune`

```python
async def play_tune(tune_description)
```

Send a tune to be played by the system.

Parameters
----------

tune_description : TuneDescription
Raises
------

TuneError
If the request fails. The error contains the reason for the failure.
