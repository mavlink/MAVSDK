# Mission Raw Server

Acts as a vehicle and receives incoming missions from GCS (in raw MAVLINK format).
Provides current mission item state, so the server can progress through missions.

## Enums

### `MissionRawServerResult`

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
| `UNSUPPORTED_MISSION_CMD` (9) | |
| `TRANSFER_CANCELLED` (10) | |
| `NO_SYSTEM` (11) | |
| `NEXT` (12) | |

## Structs

### `MissionItemCStruct`

Internal C structure for MissionItem.
Used only for C library communication.

### `MissionPlanCStruct`

Internal C structure for MissionPlan.
Used only for C library communication.

### `MissionProgressCStruct`

Internal C structure for MissionProgress.
Used only for C library communication.

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

### `MissionPlan`

Mission plan type

**Fields:**

- `mission_items`

### `MissionProgress`

Mission progress type.

**Fields:**

- `current`
- `total`

### `MissionRawServer`

Acts as a vehicle and receives incoming missions from GCS (in raw MAVLINK format).
Provides current mission item state, so the server can progress through missions.

**Fields:**

- `server_component`
