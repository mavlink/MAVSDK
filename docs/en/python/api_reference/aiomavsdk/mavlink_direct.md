# Mavlink Direct

Enable direct MAVLink communication using libmav.

## Structs

### `MavlinkDirectAsync`

Enable direct MAVLink communication using libmav.

Async wrapper around :class:`MavlinkDirect` that mirrors the gRPC-based
asyncio API while using the ctypes-based C library directly.

**Fields:**

- `system`
