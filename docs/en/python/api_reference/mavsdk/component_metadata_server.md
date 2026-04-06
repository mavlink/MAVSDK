# Component Metadata Server

Provide component metadata json definitions, such as parameters.

## Enums

### `MetadataType`

The metadata type

| Value | Description |
|-------|-------------|
| `PARAMETER` (0) | |
| `EVENTS` (1) | |
| `ACTUATORS` (2) | |

## Structs

### `MetadataCStruct`

Internal C structure for Metadata.
Used only for C library communication.

### `Metadata`

The metadata type and content

**Fields:**

- `type`
- `json_metadata`

### `ComponentMetadataServer`

Provide component metadata json definitions, such as parameters.

**Fields:**

- `server_component`
