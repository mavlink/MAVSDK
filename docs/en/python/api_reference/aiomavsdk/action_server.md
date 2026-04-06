# Action Server

Provide vehicle actions (as a server) such as arming, taking off, and landing.

## Structs

### `ActionServerAsync`

Provide vehicle actions (as a server) such as arming, taking off, and landing.

Async wrapper around :class:`ActionServer` that mirrors the gRPC-based
asyncio API while using the ctypes-based C library directly.

**Fields:**

- `server_component`
