#!/usr/bin/env python3
"""Record hashes of a repaired Linux wheel and its packaged C library."""

import hashlib
import sys
import zipfile
from pathlib import Path


def main():
    wheel = Path(sys.argv[1])
    output = Path(sys.argv[2])
    member = "mavsdk/lib/libcmavsdk.so"
    with zipfile.ZipFile(wheel) as archive:
        if archive.namelist().count(member) != 1:
            raise ValueError("wheel must contain exactly one libcmavsdk.so")
        with archive.open(member) as stream:
            library_hash = hashlib.file_digest(stream, "sha256").hexdigest()
    with wheel.open("rb") as stream:
        wheel_hash = hashlib.file_digest(stream, "sha256").hexdigest()
    output.mkdir(parents=True, exist_ok=True)
    (output / "wheel.sha256").write_text(f"{wheel_hash}  {wheel.name}\n")
    (output / "libcmavsdk.sha256").write_text(f"{library_hash}  {member}\n")


if __name__ == "__main__":
    main()
