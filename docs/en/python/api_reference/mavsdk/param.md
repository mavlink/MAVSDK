# Param

Provide raw access to get and set parameters.

## Enums

### `ProtocolVersion`

Parameter version

| Value | Description |
|-------|-------------|
| `V1` (0) | |
| `EXT` (1) | |

### `ParamResult`

Possible results returned for param requests.

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `SUCCESS` (1) | |
| `TIMEOUT` (2) | |
| `CONNECTION_ERROR` (3) | |
| `WRONG_TYPE` (4) | |
| `PARAM_NAME_TOO_LONG` (5) | |
| `NO_SYSTEM` (6) | |
| `PARAM_VALUE_TOO_LONG` (7) | |
| `FAILED` (8) | |
| `DOES_NOT_EXIST` (9) | |
| `VALUE_OUT_OF_RANGE` (10) | |
| `PERMISSION_DENIED` (11) | |
| `COMPONENT_NOT_FOUND` (12) | |
| `READ_ONLY` (13) | |
| `TYPE_UNSUPPORTED` (14) | |
| `TYPE_MISMATCH` (15) | |
| `READ_FAIL` (16) | |

## Structs

### `IntParamCStruct`

Internal C structure for IntParam.
Used only for C library communication.

### `FloatParamCStruct`

Internal C structure for FloatParam.
Used only for C library communication.

### `CustomParamCStruct`

Internal C structure for CustomParam.
Used only for C library communication.

### `AllParamsCStruct`

Internal C structure for AllParams.
Used only for C library communication.

### `IntParam`

Type for integer parameters.

**Fields:**

- `name`
- `value`

### `FloatParam`

Type for float parameters.

**Fields:**

- `name`
- `value`

### `CustomParam`

Type for custom parameters

**Fields:**

- `name`
- `value`

### `AllParams`

Type collecting all integer, float, and custom parameters.

**Fields:**

- `int_params`
- `float_params`
- `custom_params`

## `Param`

Provide raw access to get and set parameters.

### `get_param_int`

```python
def get_param_int(name)
```

Get get_param_int (blocking)

### `set_param_int`

```python
def set_param_int(name, value)
```

Get set_param_int (blocking)

### `get_param_float`

```python
def get_param_float(name)
```

Get get_param_float (blocking)

### `set_param_float`

```python
def set_param_float(name, value)
```

Get set_param_float (blocking)

### `get_param_custom`

```python
def get_param_custom(name)
```

Get get_param_custom (blocking)

### `set_param_custom`

```python
def set_param_custom(name, value)
```

Get set_param_custom (blocking)

### `get_all_params`

```python
def get_all_params()
```

Get get_all_params (blocking)

### `select_component`

```python
def select_component(component_id, protocol_version)
```

Get select_component (blocking)

### `destroy`

```python
def destroy()
```

Destroy the plugin instance
