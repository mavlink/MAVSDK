# Log Files

Allow to download log files from the vehicle after a flight is complete.
For log streaming during flight check the logging plugin.

## Structs

### `LogFilesAsync`

Allow to download log files from the vehicle after a flight is complete.
For log streaming during flight check the logging plugin.

Async wrapper around `LogFiles` that mirrors the gRPC-based
asyncio API while using the ctypes-based C library directly.

**Fields:**

- `system`
