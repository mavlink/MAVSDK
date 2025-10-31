import ctypes
import sys
from pathlib import Path

def _find_library() -> ctypes.CDLL:
    """Locate and load the cmavsdk shared library"""
    
    lib_names = {
        'linux': ['libcmavsdk.so'],
        'darwin': ['libcmavsdk.3.dylib'],
        'win32': ['cmavsdk.dll', 'libcmavsdk.dll']
    }
    
    # Dependency libraries to load first
    dep_names = {
        'linux': ['libmavsdk.so'],
        'darwin': ['libmavsdk.3.dylib'],
        'win32': ['mavsdk.dll', 'libmavsdk.dll']
    }
    
    platform = sys.platform
    if platform.startswith('linux'):
        platform = 'linux'
    
    names = lib_names.get(platform, lib_names['linux'])
    deps = dep_names.get(platform, dep_names['linux'])
    
    search_paths = [
        Path.cwd(),
        Path.cwd() / 'lib',
        Path(__file__).parent / 'lib',
    ]
    
    print(f"Searching for library on {platform}...")
    
    # First, try to load dependencies
    dep_lib = None
    for path in search_paths:
        for dep_name in deps:
            dep_path = path / dep_name
            if dep_path.exists():
                try:
                    print(f"Loading dependency: {dep_path}")
                    dep_lib = ctypes.CDLL(str(dep_path), mode=ctypes.RTLD_GLOBAL)
                    print(f"✓ Loaded dependency: {dep_name}")
                    break
                except OSError as e:
                    print(f"✗ Failed to load dependency {dep_path}: {e}")
        if dep_lib:
            break
    
    # Now load the main library
    for path in search_paths:
        for name in names:
            lib_path = path / name
            print(f"Checking: {lib_path}")
            if lib_path.exists():
                print(f"✓ Found library at: {lib_path}")
                try:
                    lib = ctypes.CDLL(str(lib_path))
                    print("✓ Successfully loaded library!")
                    return lib
                except OSError as e:
                    print(f"✗ Failed to load {lib_path}: {e}")
                    continue
    
    raise LibraryNotFoundError(
        f"Could not find cmavsdk library. Tried: {names}"
    )

_cmavsdk_lib = _find_library()
