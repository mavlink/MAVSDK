# Camera Server

Provides handling of camera interface

## Structs

### `CameraServerAsync`

Provides handling of camera interface

Async wrapper around `CameraServer` that mirrors the gRPC-based
asyncio API while using the ctypes-based C library directly.

**Fields:**

- `server_component`
