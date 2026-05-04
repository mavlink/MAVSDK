# Geofence

Enable setting a geofence.

## `GeofenceAsync`

Enable setting a geofence.

Async wrapper around `Geofence` that mirrors the gRPC-based
asyncio API while using the ctypes-based C library directly.

### `upload_geofence`

```python
async def upload_geofence(geofence_data)
```

Upload geofences.

Polygon and Circular geofences are uploaded to a drone. Once uploaded, the geofence will remain
on the drone even if a connection is lost.

Parameters
----------

geofence_data : GeofenceData
Raises
------

GeofenceError
    If the request fails. The error contains the reason for the failure.

### `download_geofence`

```python
async def download_geofence()
```

Download geofences from the vehicle.

Downloads polygon and circular geofences from the vehicle.

Returns
-------

geofence_data : GeofenceData
Raises
------

GeofenceError
    If the request fails. The error contains the reason for the failure.

### `clear_geofence`

```python
async def clear_geofence()
```

Clear all geofences saved on the vehicle.

Raises
------

GeofenceError
If the request fails. The error contains the reason for the failure.
