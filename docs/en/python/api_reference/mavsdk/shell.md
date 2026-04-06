# Shell

Allow to communicate with the vehicle's system shell.

## Enums

### `ShellResult`

Possible results returned for shell requests

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `SUCCESS` (1) | |
| `NO_SYSTEM` (2) | |
| `CONNECTION_ERROR` (3) | |
| `NO_RESPONSE` (4) | |
| `BUSY` (5) | |

## `Shell`

Allow to communicate with the vehicle's system shell.

### `send`

```python
def send(command)
```

Get send (blocking)

### `subscribe_receive`

```python
def subscribe_receive(callback: Callable, user_data: Any = None)
```

Receive feedback from a sent command line.

This subscription needs to be made before a command line is sent, otherwise, no response will be sent.

### `unsubscribe_receive`

```python
def unsubscribe_receive(handle: ctypes.c_void_p)
```

Unsubscribe from receive

### `destroy`

```python
def destroy()
```

Destroy the plugin instance
