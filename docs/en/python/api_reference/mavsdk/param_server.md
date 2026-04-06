# Param Server

Provide raw access to retrieve and provide server parameters.

## Enums

### `ParamServerResult`

Possible results returned for param requests.

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `SUCCESS` (1) | |
| `NOT_FOUND` (2) | |
| `WRONG_TYPE` (3) | |
| `PARAM_NAME_TOO_LONG` (4) | |
| `NO_SYSTEM` (5) | |
| `PARAM_VALUE_TOO_LONG` (6) | |
| `PARAM_PROVIDED_TOO_LATE` (7) | |

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

Type for float parameters.

**Fields:**

- `name`
- `value`

### `AllParams`

Type collecting all integer, float, and custom parameters.

**Fields:**

- `int_params`
- `float_params`
- `custom_params`

### `ParamServer`

Provide raw access to retrieve and provide server parameters.

**Fields:**

- `server_component`
