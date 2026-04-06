# Param Server

Provide raw access to retrieve and provide server parameters.

## Structs

### `ParamServerAsync`

Provide raw access to retrieve and provide server parameters.

Async wrapper around :class:`ParamServer` that mirrors the gRPC-based
asyncio API while using the ctypes-based C library directly.

**Fields:**

- `server_component`
