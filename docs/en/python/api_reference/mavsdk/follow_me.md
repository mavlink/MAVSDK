# Follow Me

Allow users to command the vehicle to follow a specific target.
The target is provided as a GPS coordinate and altitude.

## Enums

### `FollowMeResult`

Possible results returned for followme operations

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `SUCCESS` (1) | |
| `NO_SYSTEM` (2) | |
| `CONNECTION_ERROR` (3) | |
| `BUSY` (4) | |
| `COMMAND_DENIED` (5) | |
| `TIMEOUT` (6) | |
| `NOT_ACTIVE` (7) | |
| `SET_CONFIG_FAILED` (8) | |

## Structs

### `Config`

Configuration type.

**Fields:**

- `follow_height_m`
- `follow_distance_m`
- `responsiveness`
- `altitude_mode`
- `max_tangential_vel_m_s`
- `follow_angle_deg`

### `TargetLocation`

Target location for the vehicle to follow

**Fields:**

- `latitude_deg`
- `longitude_deg`
- `absolute_altitude_m`
- `velocity_x_m_s`
- `velocity_y_m_s`
- `velocity_z_m_s`

### `FollowMe`

Allow users to command the vehicle to follow a specific target.
The target is provided as a GPS coordinate and altitude.

**Fields:**

- `system`
