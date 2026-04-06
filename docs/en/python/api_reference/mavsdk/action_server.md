# Action Server

Provide vehicle actions (as a server) such as arming, taking off, and landing.

## Enums

### `FlightMode`

Flight modes.

For more information about flight modes, check out
https://docs.px4.io/main/en/config/flight_mode.html.

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `READY` (1) | |
| `TAKEOFF` (2) | |
| `HOLD` (3) | |
| `MISSION` (4) | |
| `RETURN_TO_LAUNCH` (5) | |
| `LAND` (6) | |
| `OFFBOARD` (7) | |
| `FOLLOW_ME` (8) | |
| `MANUAL` (9) | |
| `ALTCTL` (10) | |
| `POSCTL` (11) | |
| `ACRO` (12) | |
| `STABILIZED` (13) | |

### `ActionServerResult`

Possible results returned for action requests.

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `SUCCESS` (1) | |
| `NO_SYSTEM` (2) | |
| `CONNECTION_ERROR` (3) | |
| `BUSY` (4) | |
| `COMMAND_DENIED` (5) | |
| `COMMAND_DENIED_LANDED_STATE_UNKNOWN` (6) | |
| `COMMAND_DENIED_NOT_LANDED` (7) | |
| `TIMEOUT` (8) | |
| `VTOL_TRANSITION_SUPPORT_UNKNOWN` (9) | |
| `NO_VTOL_TRANSITION_SUPPORT` (10) | |
| `PARAMETER_ERROR` (11) | |
| `NEXT` (12) | |

## Structs

### `AllowableFlightModesCStruct`

Internal C structure for AllowableFlightModes.
Used only for C library communication.

### `ArmDisarmCStruct`

Internal C structure for ArmDisarm.
Used only for C library communication.

### `AllowableFlightModes`

State to check if the vehicle can transition to
respective flightmodes

**Fields:**

- `can_auto_mode`
- `can_guided_mode`
- `can_stabilize_mode`
- `can_auto_rtl_mode`
- `can_auto_takeoff_mode`
- `can_auto_land_mode`
- `can_auto_loiter_mode`

### `ArmDisarm`

Arming message type

**Fields:**

- `arm`
- `force`

### `ActionServer`

Provide vehicle actions (as a server) such as arming, taking off, and landing.

**Fields:**

- `server_component`
