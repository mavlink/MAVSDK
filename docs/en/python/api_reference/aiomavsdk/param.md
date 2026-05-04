# Param

Provide raw access to get and set parameters.

## `ParamAsync`

Provide raw access to get and set parameters.

Async wrapper around `Param` that mirrors the gRPC-based
asyncio API while using the ctypes-based C library directly.

### `get_param_int`

```python
async def get_param_int(name)
```

Get an int parameter.

If the type is wrong, the result will be `WRONG_TYPE`.

Parameters
----------

name : str
Returns
-------

value : int
Raises
------

ParamError
    If the request fails. The error contains the reason for the failure.

### `set_param_int`

```python
async def set_param_int(name, value)
```

Set an int parameter.

If the type is wrong, the result will be `WRONG_TYPE`.

Parameters
----------

name : str
value : int
Raises
------

ParamError
    If the request fails. The error contains the reason for the failure.

### `get_param_float`

```python
async def get_param_float(name)
```

Get a float parameter.

If the type is wrong, the result will be `WRONG_TYPE`.

Parameters
----------

name : str
Returns
-------

value : float
Raises
------

ParamError
    If the request fails. The error contains the reason for the failure.

### `set_param_float`

```python
async def set_param_float(name, value)
```

Set a float parameter.

If the type is wrong, the result will be `WRONG_TYPE`.

Parameters
----------

name : str
value : float
Raises
------

ParamError
    If the request fails. The error contains the reason for the failure.

### `get_param_custom`

```python
async def get_param_custom(name)
```

Get a custom parameter.

If the type is wrong, the result will be `WRONG_TYPE`.

Parameters
----------

name : str
Returns
-------

value : str
Raises
------

ParamError
    If the request fails. The error contains the reason for the failure.

### `set_param_custom`

```python
async def set_param_custom(name, value)
```

Set a custom parameter.

If the type is wrong, the result will be `WRONG_TYPE`.

Parameters
----------

name : str
value : str
Raises
------

ParamError
    If the request fails. The error contains the reason for the failure.

### `get_all_params`

```python
async def get_all_params()
```

Get all parameters.

Returns
-------

params : AllParams
Raises
------

ParamError
If the request fails. The error contains the reason for the failure.

### `select_component`

```python
async def select_component(component_id, protocol_version)
```

Select component ID of parameter component to talk to and param protocol version.

Default is the autopilot component (1), and Version (0).

Parameters
----------

component_id : int
protocol_version : ProtocolVersion
Raises
------

ParamError
    If the request fails. The error contains the reason for the failure.
