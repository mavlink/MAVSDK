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

## `GeofenceData`

Geofence data type.

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
