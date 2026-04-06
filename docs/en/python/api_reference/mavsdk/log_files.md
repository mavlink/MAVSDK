# Log Files

Allow to download log files from the vehicle after a flight is complete.
For log streaming during flight check the logging plugin.

## Enums

### `LogFilesResult`

Possible results returned for calibration commands

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `SUCCESS` (1) | |
| `NEXT` (2) | |
| `NO_LOGFILES` (3) | |
| `TIMEOUT` (4) | |
| `INVALID_ARGUMENT` (5) | |
| `FILE_OPEN_FAILED` (6) | |
| `NO_SYSTEM` (7) | |

## Structs

### `ProgressDataCStruct`

Internal C structure for ProgressData.
Used only for C library communication.

### `EntryCStruct`

Internal C structure for Entry.
Used only for C library communication.

### `ProgressData`

Progress data coming when downloading a log file.

**Fields:**

- `progress`

### `Entry`

Log file entry type.

**Fields:**

- `id`
- `date`
- `size_bytes`

### `LogFiles`

Allow to download log files from the vehicle after a flight is complete.
For log streaming during flight check the logging plugin.

**Fields:**

- `system`
