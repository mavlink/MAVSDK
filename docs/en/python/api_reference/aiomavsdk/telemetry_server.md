# Telemetry Server

Allow users to provide vehicle telemetry and state information
(e.g. battery, GPS, RC connection, flight mode etc.) and set telemetry update rates.

## Structs

### `TelemetryServerAsync`

Allow users to provide vehicle telemetry and state information
(e.g. battery, GPS, RC connection, flight mode etc.) and set telemetry update rates.

Async wrapper around `TelemetryServer` that mirrors the gRPC-based
asyncio API while using the ctypes-based C library directly.

**Fields:**

- `server_component`
