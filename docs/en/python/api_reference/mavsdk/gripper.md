# Gripper

Allows users to send gripper actions.

## Enums

### `GripperAction`

Gripper Actions.

Available gripper actions are defined in mavlink under
https://mavlink.io/en/messages/common.html#GRIPPER_ACTIONS

| Value | Description |
|-------|-------------|
| `RELEASE` (0) | |
| `GRAB` (1) | |

### `GripperResult`

Possible results returned for gripper action requests.

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `SUCCESS` (1) | |
| `NO_SYSTEM` (2) | |
| `BUSY` (3) | |
| `TIMEOUT` (4) | |
| `UNSUPPORTED` (5) | |
| `FAILED` (6) | |

## `Gripper`

Allows users to send gripper actions.

### `grab_async`

```python
def grab_async(instance, callback: Callable, user_data: Any = None)
```

Gripper grab cargo.

### `grab`

```python
def grab(instance)
```

Get grab (blocking)

### `release_async`

```python
def release_async(instance, callback: Callable, user_data: Any = None)
```

Gripper release cargo.

### `release`

```python
def release(instance)
```

Get release (blocking)

### `destroy`

```python
def destroy()
```

Destroy the plugin instance
