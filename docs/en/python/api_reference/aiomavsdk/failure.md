# Failure

Inject failures into system to test failsafes.

## `FailureAsync`

Inject failures into system to test failsafes.

Async wrapper around `Failure` that mirrors the gRPC-based
asyncio API while using the ctypes-based C library directly.

### `inject`

```python
async def inject(failure_unit, failure_type, instance)
```

Injects a failure.

Parameters
----------

failure_unit : FailureUnit
failure_type : FailureType
instance : int
Raises
------

FailureError
If the request fails. The error contains the reason for the failure.

### `destroy`

```python
def destroy()
```
