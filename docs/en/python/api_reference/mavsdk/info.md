# Info

Provide information about the hardware and/or software of a system.

## Enums

### `InfoResult`

Possible results returned for info requests.

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `SUCCESS` (1) | |
| `INFORMATION_NOT_RECEIVED_YET` (2) | |
| `NO_SYSTEM` (3) | |

## Structs

### `FlightInfoCStruct`

Internal C structure for FlightInfo.
Used only for C library communication.

### `IdentificationCStruct`

Internal C structure for Identification.
Used only for C library communication.

### `ProductCStruct`

Internal C structure for Product.
Used only for C library communication.

### `VersionCStruct`

Internal C structure for Version.
Used only for C library communication.

### `FlightInfo`

System flight information.

**Fields:**

- `time_boot_ms`
- `flight_uid`
- `duration_since_arming_ms`
- `duration_since_takeoff_ms`

### `Identification`

System identification.

**Fields:**

- `hardware_uid`
- `legacy_uid`

### `Product`

System product information.

**Fields:**

- `vendor_id`
- `vendor_name`
- `product_id`
- `product_name`

### `Version`

System version information.

**Fields:**

- `flight_sw_major`
- `flight_sw_minor`
- `flight_sw_patch`
- `flight_sw_vendor_major`
- `flight_sw_vendor_minor`
- `flight_sw_vendor_patch`
- `os_sw_major`
- `os_sw_minor`
- `os_sw_patch`
- `flight_sw_git_hash`
- `os_sw_git_hash`
- `flight_sw_version_type`

## `Info`

Provide information about the hardware and/or software of a system.

### `get_flight_information`

```python
def get_flight_information()
```

Get get_flight_information (blocking)

### `get_identification`

```python
def get_identification()
```

Get get_identification (blocking)

### `get_product`

```python
def get_product()
```

Get get_product (blocking)

### `get_version`

```python
def get_version()
```

Get get_version (blocking)

### `get_speed_factor`

```python
def get_speed_factor()
```

Get get_speed_factor (blocking)

### `subscribe_flight_information`

```python
def subscribe_flight_information(callback: Callable, user_data: Any = None)
```

Subscribe to 'flight information' updates.

### `unsubscribe_flight_information`

```python
def unsubscribe_flight_information(handle: ctypes.c_void_p)
```

Unsubscribe from flight_information

### `destroy`

```python
def destroy()
```

Destroy the plugin instance
