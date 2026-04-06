# Component Metadata

Access component metadata json definitions, such as parameters.

## Enums

### `MetadataType`

The metadata type

| Value | Description |
|-------|-------------|
| `ALL_COMPLETED` (0) | |
| `PARAMETER` (1) | |
| `EVENTS` (2) | |
| `ACTUATORS` (3) | |

### `ComponentMetadataResult`

Possible results returned

| Value | Description |
|-------|-------------|
| `SUCCESS` (0) | |
| `NOT_AVAILABLE` (1) | |
| `CONNECTION_ERROR` (2) | |
| `UNSUPPORTED` (3) | |
| `DENIED` (4) | |
| `FAILED` (5) | |
| `TIMEOUT` (6) | |
| `NO_SYSTEM` (7) | |
| `NOT_REQUESTED` (8) | |

## Structs

### `MetadataDataCStruct`

Internal C structure for MetadataData.
Used only for C library communication.

### `MetadataUpdateCStruct`

Internal C structure for MetadataUpdate.
Used only for C library communication.

### `MetadataData`

Metadata response

**Fields:**

- `json_metadata`

### `MetadataUpdate`

Metadata for a given component and type

**Fields:**

- `compid`
- `type`
- `json_metadata`

### `ComponentMetadata`

Access component metadata json definitions, such as parameters.

**Fields:**

- `system`
