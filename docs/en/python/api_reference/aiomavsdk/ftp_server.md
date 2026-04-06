# Ftp Server

Provide files or directories to transfer.

## Structs

### `FtpServerAsync`

Provide files or directories to transfer.

Async wrapper around :class:`FtpServer` that mirrors the gRPC-based
asyncio API while using the ctypes-based C library directly.

**Fields:**

- `server_component`
