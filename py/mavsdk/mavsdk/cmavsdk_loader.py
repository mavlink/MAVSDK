import ctypes
import sys

from pathlib import Path

from .exceptions import LibraryNotFoundError


def _find_library() -> ctypes.CDLL:
    """Locate and load the cmavsdk shared library"""

    lib_names = {
        "linux": ["libcmavsdk.so"],
        "darwin": ["libcmavsdk.dylib"],
        "win32": ["cmavsdk.dll", "libcmavsdk.dll"],
    }

    platform = sys.platform
    if platform.startswith("linux"):
        platform = "linux"

    names = lib_names.get(platform, lib_names["linux"])

    search_paths = [
        Path(__file__).parent
        / "lib",  # mavsdk/lib/ — works both installed and editable
        Path.cwd() / "lib",  # fallback
    ]

    attempts = []
    for path in search_paths:
        for name in names:
            lib_path = path / name
            if not lib_path.exists():
                attempts.append(f"{lib_path}: not found")
                continue
            try:
                return ctypes.CDLL(str(lib_path))
            except OSError as e:
                attempts.append(f"{lib_path}: {e}")

    raise LibraryNotFoundError(
        f"Could not load cmavsdk library on {platform}. Tried:\n  "
        + "\n  ".join(attempts)
    )


_cmavsdk_lib = _find_library()
