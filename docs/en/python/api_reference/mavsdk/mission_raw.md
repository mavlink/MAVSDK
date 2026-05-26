# Mission Raw

Enable raw missions as exposed by MAVLink.

## Enums

### `MissionRawResult`

Possible results returned for action requests.

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `SUCCESS` (1) | |
| `ERROR` (2) | |
| `TOO_MANY_MISSION_ITEMS` (3) | |
| `BUSY` (4) | |
| `TIMEOUT` (5) | |
| `INVALID_ARGUMENT` (6) | |
| `UNSUPPORTED` (7) | |
| `NO_MISSION_AVAILABLE` (8) | |
| `TRANSFER_CANCELLED` (9) | |
| `FAILED_TO_OPEN_QGC_PLAN` (10) | |
| `FAILED_TO_PARSE_QGC_PLAN` (11) | |
| `NO_SYSTEM` (12) | |
| `DENIED` (13) | |
| `MISSION_TYPE_NOT_CONSISTENT` (14) | |
| `INVALID_SEQUENCE` (15) | |
| `CURRENT_INVALID` (16) | |
| `PROTOCOL_ERROR` (17) | |
| `INT_MESSAGES_NOT_SUPPORTED` (18) | |
| `FAILED_TO_OPEN_MISSION_PLANNER_PLAN` (19) | |
| `FAILED_TO_PARSE_MISSION_PLANNER_PLAN` (20) | |

## Structs

### `MissionProgress`

Mission progress type.

**Fields:**

- `current`
- `total`

### `MissionItem`

Mission item exactly identical to MAVLink MISSION_ITEM_INT.

**Fields:**

- `seq`
- `frame`
- `command`
- `current`
- `autocontinue`
- `param1`
- `param2`
- `param3`
- `param4`
- `x`
- `y`
- `z`
- `mission_type`

### `MissionImportData`

Mission import data

**Fields:**

- `mission_items`
- `geofence_items`
- `rally_items`

### `MissionRaw`

Enable raw missions as exposed by MAVLink.

**Fields:**

- `system`
