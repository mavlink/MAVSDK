# Mavlink Direct

Enable direct MAVLink communication using libmav.

## Enums

### `MavlinkDirectResult`

Possible results returned for action requests.

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `SUCCESS` (1) | |
| `ERROR` (2) | |
| `INVALID_MESSAGE` (3) | |
| `INVALID_FIELD` (4) | |
| `CONNECTION_ERROR` (5) | |
| `NO_SYSTEM` (6) | |
| `TIMEOUT` (7) | |

## Structs

### `MavlinkMessageCStruct`

Internal C structure for MavlinkMessage.
Used only for C library communication.

### `MavlinkMessage`

A complete MAVLink message with all header information and fields

**Fields:**

- `message_name`
- `system_id`
- `component_id`
- `target_system_id`
- `target_component_id`
- `fields_json`

### `MavlinkDirect`

Enable direct MAVLink communication using libmav.

**Fields:**

- `system`
