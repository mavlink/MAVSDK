# Gripper

Allows users to send gripper actions.

## `GripperAsync`

Allows users to send gripper actions.

Async wrapper around :class:`Gripper` that mirrors the gRPC-based
asyncio API while using the ctypes-based C library directly.

### `grab`

```python
async def grab(instance)
```

Gripper grab cargo.

Parameters
----------
instance : int
Raises
------
GripperError
If the request fails. The error contains the reason for the failure.

### `release`

```python
async def release(instance)
```

Gripper release cargo.

Parameters
----------
instance : int
Raises
------
GripperError
If the request fails. The error contains the reason for the failure.

### `destroy`

```python
def destroy()
```
