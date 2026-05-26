# Ftp

Implements file transfer functionality using MAVLink FTP.

## `FtpAsync`

Implements file transfer functionality using MAVLink FTP.

Async wrapper around `Ftp` that mirrors the gRPC-based
asyncio API while using the ctypes-based C library directly.

### `download`

```python
async def download(remote_file_path, local_dir, use_burst) → AsyncGenerator
```

Downloads a file to local directory.

### `upload`

```python
async def upload(local_file_path, remote_dir) → AsyncGenerator
```

Uploads local file to remote directory.

### `list_directory`

```python
async def list_directory(remote_dir)
```

Lists items from a remote directory.

Parameters
----------

remote_dir : str
Returns
-------

data : ListDirectoryData
Raises
------

FtpError
If the request fails. The error contains the reason for the failure.

### `create_directory`

```python
async def create_directory(remote_dir)
```

Creates a remote directory.

Parameters
----------

remote_dir : str
Raises
------

FtpError
If the request fails. The error contains the reason for the failure.

### `remove_directory`

```python
async def remove_directory(remote_dir)
```

Removes a remote directory.

Parameters
----------

remote_dir : str
Raises
------

FtpError
If the request fails. The error contains the reason for the failure.

### `remove_file`

```python
async def remove_file(remote_file_path)
```

Removes a remote file.

Parameters
----------

remote_file_path : str
Raises
------

FtpError
If the request fails. The error contains the reason for the failure.

### `rename`

```python
async def rename(remote_from_path, remote_to_path)
```

Renames a remote file or remote directory.

Parameters
----------

remote_from_path : str
remote_to_path : str
Raises
------

FtpError
If the request fails. The error contains the reason for the failure.

### `are_files_identical`

```python
async def are_files_identical(local_file_path, remote_file_path)
```

Compares a local file to a remote file using a CRC32 checksum.

Parameters
----------

local_file_path : str
remote_file_path : str
Returns
-------

are_identical : bool
Raises
------

FtpError
If the request fails. The error contains the reason for the failure.

### `set_target_compid`

```python
async def set_target_compid(compid)
```

Set target component ID. By default it is the autopilot.

Parameters
----------

compid : int
Raises
------

FtpError
If the request fails. The error contains the reason for the failure.
