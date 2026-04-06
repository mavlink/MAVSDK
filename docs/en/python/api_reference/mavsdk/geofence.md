# Geofence

Enable setting a geofence.

## Enums

### `FenceType`

Geofence types.

| Value | Description |
|-------|-------------|
| `INCLUSION` (0) | |
| `EXCLUSION` (1) | |

### `GeofenceResult`

Possible results returned for geofence requests.

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `SUCCESS` (1) | |
| `ERROR` (2) | |
| `TOO_MANY_GEOFENCE_ITEMS` (3) | |
| `BUSY` (4) | |
| `TIMEOUT` (5) | |
| `INVALID_ARGUMENT` (6) | |
| `NO_SYSTEM` (7) | |

## Structs

### `PointCStruct`

Internal C structure for Point.
Used only for C library communication.

### `PolygonCStruct`

Internal C structure for Polygon.
Used only for C library communication.

### `CircleCStruct`

Internal C structure for Circle.
Used only for C library communication.

### `Point`

Point type.

**Fields:**

- `latitude_deg`
- `longitude_deg`

### `Polygon`

Polygon type.

**Fields:**

- `points`
- `fence_type`

### `Circle`

Circular type.

**Fields:**

- `point`
- `radius`
- `fence_type`

## `GeofenceDataCStruct`

Internal C structure for GeofenceData.
Used only for C library communication.

## `GeofenceData`

Geofence data type.

### `from_c_struct`

```python
def from_c_struct(cls, c_struct)
```

Convert from C structure to Python object

### `to_c_struct`

```python
def to_c_struct()
```

Convert to C structure for C library calls

## `Geofence`

Enable setting a geofence.

### `upload_geofence_async`

```python
def upload_geofence_async(geofence_data, callback: Callable, user_data: Any = None)
```

Upload geofences.

Polygon and Circular geofences are uploaded to a drone. Once uploaded, the geofence will remain
on the drone even if a connection is lost.

### `upload_geofence`

```python
def upload_geofence(geofence_data)
```

Get upload_geofence (blocking)

### `download_geofence_async`

```python
def download_geofence_async(callback: Callable, user_data: Any = None)
```

Download geofences from the vehicle.

Downloads polygon and circular geofences from the vehicle.

### `download_geofence`

```python
def download_geofence()
```

Get download_geofence (blocking)

### `clear_geofence_async`

```python
def clear_geofence_async(callback: Callable, user_data: Any = None)
```

Clear all geofences saved on the vehicle.

### `clear_geofence`

```python
def clear_geofence()
```

Get clear_geofence (blocking)

### `destroy`

```python
def destroy()
```

Destroy the plugin instance
