# Mission Raw Server

Acts as a vehicle and receives incoming missions from GCS (in raw MAVLINK format).
Provides current mission item state, so the server can progress through missions.

## Structs

### `MissionRawServerAsync`

Acts as a vehicle and receives incoming missions from GCS (in raw MAVLINK format).
Provides current mission item state, so the server can progress through missions.

Async wrapper around `MissionRawServer` that mirrors the gRPC-based
asyncio API while using the ctypes-based C library directly.

**Fields:**

- `server_component`
