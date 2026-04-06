# Shell

Allow to communicate with the vehicle's system shell.

## `ShellAsync`

Allow to communicate with the vehicle's system shell.

Async wrapper around `Shell` that mirrors the gRPC-based
asyncio API while using the ctypes-based C library directly.

### `send`

```python
async def send(command)
```

Send a command line.

Parameters
----------

command : str
Raises
------

ShellError
If the request fails. The error contains the reason for the failure.

### `subscribe_receive`

```python
async def subscribe_receive() → AsyncGenerator[str, None]
```

Receive feedback from a sent command line.

This subscription needs to be made before a command line is sent, otherwise, no response will be sent.

Yields
------

 : str
     The next update

### `destroy`

```python
def destroy()
```
