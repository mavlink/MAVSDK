# Log Streaming

Provide log streaming data.

## Enums

### `LogStreamingResult`

Possible results returned for logging requests

| Value | Description |
|-------|-------------|
| `SUCCESS` (0) | |
| `NO_SYSTEM` (1) | |
| `CONNECTION_ERROR` (2) | |
| `BUSY` (3) | |
| `COMMAND_DENIED` (4) | |
| `TIMEOUT` (5) | |
| `UNSUPPORTED` (6) | |
| `UNKNOWN` (7) | |

## Structs

### `LogStreamingRaw`

Raw logging data type

**Fields:**

- `data_base64`

### `LogStreaming`

Provide log streaming data.

**Fields:**

- `system`
