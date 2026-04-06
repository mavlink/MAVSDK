# Follow Me

Allow users to command the vehicle to follow a specific target.
The target is provided as a GPS coordinate and altitude.

## Structs

### `FollowMeAsync`

Allow users to command the vehicle to follow a specific target.
The target is provided as a GPS coordinate and altitude.

Async wrapper around `FollowMe` that mirrors the gRPC-based
asyncio API while using the ctypes-based C library directly.

**Fields:**

- `system`
