# Ftp

Implements file transfer functionality using MAVLink FTP.

## Enums

### `FtpResult`

Possible results returned for FTP commands

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `SUCCESS` (1) | |
| `NEXT` (2) | |
| `TIMEOUT` (3) | |
| `BUSY` (4) | |
| `FILE_IO_ERROR` (5) | |
| `FILE_EXISTS` (6) | |
| `FILE_DOES_NOT_EXIST` (7) | |
| `FILE_PROTECTED` (8) | |
| `INVALID_PARAMETER` (9) | |
| `UNSUPPORTED` (10) | |
| `PROTOCOL_ERROR` (11) | |
| `NO_SYSTEM` (12) | |

## Structs

### `ListDirectoryData`

The output of a directory list

**Fields:**

- `dirs`
- `files`

### `ProgressData`

Progress data type for file transfer.

**Fields:**

- `bytes_transferred`
- `total_bytes`

## `Ftp`

Implements file transfer functionality using MAVLink FTP.

### `download_async`

```python
def download_async(remote_file_path, local_dir, use_burst, callback: Callable, user_data: Any = None)
```

Downloads a file to local directory.

### `upload_async`

```python
def upload_async(local_file_path, remote_dir, callback: Callable, user_data: Any = None)
```

Uploads local file to remote directory.

### `list_directory_async`

```python
def list_directory_async(remote_dir, callback: Callable, user_data: Any = None)
```

Lists items from a remote directory.

### `list_directory`

```python
def list_directory(remote_dir)
```

Get list_directory (blocking)

### `create_directory_async`

```python
def create_directory_async(remote_dir, callback: Callable, user_data: Any = None)
```

Creates a remote directory.

### `create_directory`

```python
def create_directory(remote_dir)
```

Get create_directory (blocking)

### `remove_directory_async`

```python
def remove_directory_async(remote_dir, callback: Callable, user_data: Any = None)
```

Removes a remote directory.

### `remove_directory`

```python
def remove_directory(remote_dir)
```

Get remove_directory (blocking)

### `remove_file_async`

```python
def remove_file_async(remote_file_path, callback: Callable, user_data: Any = None)
```

Removes a remote file.

### `remove_file`

```python
def remove_file(remote_file_path)
```

Get remove_file (blocking)

### `rename_async`

```python
def rename_async(remote_from_path, remote_to_path, callback: Callable, user_data: Any = None)
```

Renames a remote file or remote directory.

### `rename`

```python
def rename(remote_from_path, remote_to_path)
```

Get rename (blocking)

### `are_files_identical_async`

```python
def are_files_identical_async(local_file_path, remote_file_path, callback: Callable, user_data: Any = None)
```

Compares a local file to a remote file using a CRC32 checksum.

### `are_files_identical`

```python
def are_files_identical(local_file_path, remote_file_path)
```

Get are_files_identical (blocking)

### `set_target_compid`

```python
def set_target_compid(compid)
```

Get set_target_compid (blocking)
