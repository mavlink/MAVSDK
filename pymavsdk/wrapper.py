"""Main ctypes wrapper for cmavsdk library"""

import ctypes
import sys
from pathlib import Path
from typing import Optional, List, Callable, Any
from .exceptions import LibraryNotFoundError, ConnectionError as ConnError
from .enums import ComponentType, ForwardingOption, ConnectionResult
from .types import (
    ConnectionError as CConnectionError,
    MavsdkMessage,
    ConnectionResultWithHandle,
    ConnectionErrorCallback,
    NewSystemCallback,
    InterceptJsonCallback,
    IsConnectedCallback,
    ComponentDiscoveredCallback,
    ComponentDiscoveredIdCallback,
    LogCallback,
)

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

class MavsdkConfiguration:
    """Wrapper for mavsdk_configuration_t"""
    
    def __init__(self, lib: ctypes.CDLL, handle: ctypes.c_void_p):
        self._lib = lib
        self._handle = handle
    
    @classmethod
    def create_with_component_type(cls, lib: ctypes.CDLL, component_type: ComponentType):
        """Create configuration with component type"""
        handle = lib.mavsdk_configuration_create_with_component_type(int(component_type))
        return cls(lib, handle)
    
    @classmethod
    def create_manual(cls, lib: ctypes.CDLL, system_id: int, component_id: int, 
                     always_send_heartbeats: bool):
        """Create manual configuration"""
        handle = lib.mavsdk_configuration_create_manual(
            system_id, component_id, int(always_send_heartbeats)
        )
        return cls(lib, handle)
    
    def destroy(self):
        """Destroy configuration"""
        if self._handle:
            self._lib.mavsdk_configuration_destroy(self._handle)
            self._handle = None
    
    @property
    def system_id(self) -> int:
        return self._lib.mavsdk_configuration_get_system_id(self._handle)
    
    @system_id.setter
    def system_id(self, value: int):
        self._lib.mavsdk_configuration_set_system_id(self._handle, value)
    
    @property
    def component_id(self) -> int:
        return self._lib.mavsdk_configuration_get_component_id(self._handle)
    
    @component_id.setter
    def component_id(self, value: int):
        self._lib.mavsdk_configuration_set_component_id(self._handle, value)
    
    def __del__(self):
        self.destroy()

class MavsdkSystem:
    """Wrapper for mavsdk_system_t"""
    
    def __init__(self, lib: ctypes.CDLL, handle: ctypes.c_void_p):
        self._lib = lib
        self._handle = handle
        self._callbacks = []  # Keep references to prevent GC
    
    def has_autopilot(self) -> bool:
        """Check if system has autopilot"""
        return self._lib.mavsdk_system_has_autopilot(self._handle)
    
    def is_standalone(self) -> bool:
        """Check if system is standalone"""
        return self._lib.mavsdk_system_is_standalone(self._handle)
    
    def has_camera(self, camera_id: int = -1) -> bool:
        """Check if system has camera"""
        return self._lib.mavsdk_system_has_camera(self._handle, camera_id)
    
    def has_gimbal(self) -> bool:
        """Check if system has gimbal"""
        return self._lib.mavsdk_system_has_gimbal(self._handle)
    
    def is_connected(self) -> bool:
        """Check if system is connected"""
        return self._lib.mavsdk_system_is_connected(self._handle)
    
    def get_system_id(self) -> int:
        """Get system ID"""
        return self._lib.mavsdk_system_get_system_id(self._handle)
    
    def component_ids(self) -> List[int]:
        """Get list of component IDs"""
        count = ctypes.c_size_t()
        ids_ptr = self._lib.mavsdk_system_component_ids(self._handle, ctypes.byref(count))
        
        if not ids_ptr:
            return []
        
        ids = [ids_ptr[i] for i in range(count.value)]
        self._lib.mavsdk_system_free_component_ids(ids_ptr)
        return ids
    
    def subscribe_is_connected(self, callback: Callable[[bool, Any], None], 
                               user_data: Any = None):
        """Subscribe to connection state changes"""
        c_callback = IsConnectedCallback(lambda connected, ud: callback(connected, user_data))
        self._callbacks.append(c_callback)
        
        handle = self._lib.mavsdk_system_subscribe_is_connected(
            self._handle, c_callback, None
        )
        return handle
    
    def unsubscribe_is_connected(self, handle: ctypes.c_void_p):
        """Unsubscribe from connection state changes"""
        self._lib.mavsdk_system_unsubscribe_is_connected(self._handle, handle)
    
    def enable_timesync(self):
        """Enable time synchronization"""
        self._lib.mavsdk_system_enable_timesync(self._handle)
    
    def autopilot_type(self) -> int:
        """Get autopilot type"""
        return self._lib.mavsdk_system_autopilot_type(self._handle)
    
    def vehicle_type(self) -> int:
        """Get vehicle type"""
        return self._lib.mavsdk_system_vehicle_type(self._handle)


class Mavsdk:
    """Python wrapper for cmavsdk library"""
    
    def __init__(self, configuration: Optional[MavsdkConfiguration] = None, 
                 lib_path: Optional[str] = None):
        """
        Initialize the cmavsdk wrapper
        
        Args:
            configuration: Optional MavsdkConfiguration object
            lib_path: Optional explicit path to the library
        """
        if lib_path:
            self._lib = ctypes.CDLL(lib_path)
        else:
            self._lib = _find_library()
        
        self._callbacks = []  # Keep references to prevent GC
        self._setup_functions()
        
        # If no configuration provided, create a default one
        if configuration is None:
            print("Creating default configuration...")
            default_config = MavsdkConfiguration.create_with_component_type(
                self._lib, ComponentType.GROUND_STATION
            )
            config_handle = default_config._handle
            print(f"✓ Created default config handle: {config_handle}")
        else:
            config_handle = configuration._handle
        
        print(f"Creating Mavsdk with config: {config_handle}")
        self._handle = self._lib.mavsdk_create(config_handle)
        self._destroyed = False
        print(f"✓ Created Mavsdk handle: {self._handle}")

    def _setup_functions(self):
        """Define C function signatures"""
        
        # Configuration functions
        self._lib.mavsdk_configuration_create_with_component_type.argtypes = [ctypes.c_int]
        self._lib.mavsdk_configuration_create_with_component_type.restype = ctypes.c_void_p
        
        self._lib.mavsdk_configuration_create_manual.argtypes = [
            ctypes.c_uint8, ctypes.c_uint8, ctypes.c_int
        ]
        self._lib.mavsdk_configuration_create_manual.restype = ctypes.c_void_p
        
        self._lib.mavsdk_configuration_destroy.argtypes = [ctypes.c_void_p]
        self._lib.mavsdk_configuration_destroy.restype = None
        
        self._lib.mavsdk_configuration_get_system_id.argtypes = [ctypes.c_void_p]
        self._lib.mavsdk_configuration_get_system_id.restype = ctypes.c_uint8
        
        self._lib.mavsdk_configuration_set_system_id.argtypes = [ctypes.c_void_p, ctypes.c_uint8]
        self._lib.mavsdk_configuration_set_system_id.restype = None
        
        self._lib.mavsdk_configuration_get_component_id.argtypes = [ctypes.c_void_p]
        self._lib.mavsdk_configuration_get_component_id.restype = ctypes.c_uint8
        
        self._lib.mavsdk_configuration_set_component_id.argtypes = [ctypes.c_void_p, ctypes.c_uint8]
        self._lib.mavsdk_configuration_set_component_id.restype = None
        
        # Core functions
        self._lib.mavsdk_create.argtypes = [ctypes.c_void_p]
        self._lib.mavsdk_create.restype = ctypes.c_void_p
        
        self._lib.mavsdk_destroy.argtypes = [ctypes.c_void_p]
        self._lib.mavsdk_destroy.restype = None
        
        self._lib.mavsdk_version.argtypes = [ctypes.c_void_p]
        self._lib.mavsdk_version.restype = ctypes.c_char_p
        
        # Connection functions
        self._lib.mavsdk_add_any_connection.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
        self._lib.mavsdk_add_any_connection.restype = ctypes.c_int
        
        self._lib.mavsdk_add_any_connection_with_handle.argtypes = [
            ctypes.c_void_p, ctypes.c_char_p
        ]
        self._lib.mavsdk_add_any_connection_with_handle.restype = ConnectionResultWithHandle
        
        self._lib.mavsdk_remove_connection.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
        self._lib.mavsdk_remove_connection.restype = None
        
        # System functions
        self._lib.mavsdk_system_count.argtypes = [ctypes.c_void_p]
        self._lib.mavsdk_system_count.restype = ctypes.c_size_t
        
        self._lib.mavsdk_get_systems.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_size_t)]
        self._lib.mavsdk_get_systems.restype = ctypes.POINTER(ctypes.c_void_p)
        
        self._lib.mavsdk_free_systems_array.argtypes = [ctypes.POINTER(ctypes.c_void_p)]
        self._lib.mavsdk_free_systems_array.restype = None
        
        self._lib.mavsdk_first_autopilot.argtypes = [ctypes.c_void_p, ctypes.c_double]
        self._lib.mavsdk_first_autopilot.restype = ctypes.c_void_p
        
        # Subscription functions
        self._lib.mavsdk_subscribe_on_new_system.argtypes = [
            ctypes.c_void_p, NewSystemCallback, ctypes.c_void_p
        ]
        self._lib.mavsdk_subscribe_on_new_system.restype = ctypes.c_void_p
        
        self._lib.mavsdk_unsubscribe_on_new_system.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
        self._lib.mavsdk_unsubscribe_on_new_system.restype = None
        
        # System functions
        self._lib.mavsdk_system_has_autopilot.argtypes = [ctypes.c_void_p]
        self._lib.mavsdk_system_has_autopilot.restype = ctypes.c_bool
        
        self._lib.mavsdk_system_is_standalone.argtypes = [ctypes.c_void_p]
        self._lib.mavsdk_system_is_standalone.restype = ctypes.c_bool
        
        self._lib.mavsdk_system_has_camera.argtypes = [ctypes.c_void_p, ctypes.c_int]
        self._lib.mavsdk_system_has_camera.restype = ctypes.c_bool
        
        self._lib.mavsdk_system_has_gimbal.argtypes = [ctypes.c_void_p]
        self._lib.mavsdk_system_has_gimbal.restype = ctypes.c_bool
        
        self._lib.mavsdk_system_is_connected.argtypes = [ctypes.c_void_p]
        self._lib.mavsdk_system_is_connected.restype = ctypes.c_bool
        
        self._lib.mavsdk_system_get_system_id.argtypes = [ctypes.c_void_p]
        self._lib.mavsdk_system_get_system_id.restype = ctypes.c_uint8
        
        self._lib.mavsdk_system_component_ids.argtypes = [
            ctypes.c_void_p, ctypes.POINTER(ctypes.c_size_t)
        ]
        self._lib.mavsdk_system_component_ids.restype = ctypes.POINTER(ctypes.c_uint8)
        
        self._lib.mavsdk_system_free_component_ids.argtypes = [ctypes.POINTER(ctypes.c_uint8)]
        self._lib.mavsdk_system_free_component_ids.restype = None
        
        self._lib.mavsdk_system_subscribe_is_connected.argtypes = [
            ctypes.c_void_p, IsConnectedCallback, ctypes.c_void_p
        ]
        self._lib.mavsdk_system_subscribe_is_connected.restype = ctypes.c_void_p
        
        self._lib.mavsdk_system_unsubscribe_is_connected.argtypes = [
            ctypes.c_void_p, ctypes.c_void_p
        ]
        self._lib.mavsdk_system_unsubscribe_is_connected.restype = None
        
        self._lib.mavsdk_system_enable_timesync.argtypes = [ctypes.c_void_p]
        self._lib.mavsdk_system_enable_timesync.restype = None
        
        self._lib.mavsdk_system_autopilot_type.argtypes = [ctypes.c_void_p]
        self._lib.mavsdk_system_autopilot_type.restype = ctypes.c_int
        
        self._lib.mavsdk_system_vehicle_type.argtypes = [ctypes.c_void_p]
        self._lib.mavsdk_system_vehicle_type.restype = ctypes.c_int
    
    def version(self) -> str:
        """Get MAVSDK version string"""
        version_bytes = self._lib.mavsdk_version(self._handle)
        return version_bytes.decode('utf-8') if version_bytes else ""
    
    def add_any_connection(self, connection_url: str) -> ConnectionResult:
        """Add a connection"""
        result = self._lib.mavsdk_add_any_connection(
            self._handle, connection_url.encode('utf-8')
        )
        return ConnectionResult(result)
    
    def add_any_connection_with_handle(self, connection_url: str):
        """Add a connection and get handle"""
        result = self._lib.mavsdk_add_any_connection_with_handle(
            self._handle, connection_url.encode('utf-8')
        )
        if result.result != ConnectionResult.SUCCESS:
            raise ConnError(f"Connection failed: {ConnectionResult(result.result).name}")
        return result.handle
    
    def remove_connection(self, handle: ctypes.c_void_p):
        """Remove a connection"""
        self._lib.mavsdk_remove_connection(self._handle, handle)
    
    def system_count(self) -> int:
        """Get number of discovered systems"""
        return self._lib.mavsdk_system_count(self._handle)
    
    def get_systems(self) -> List[MavsdkSystem]:
        """Get all discovered systems"""
        count = ctypes.c_size_t()
        systems_ptr = self._lib.mavsdk_get_systems(self._handle, ctypes.byref(count))
        
        if not systems_ptr:
            return []
        
        systems = [
            MavsdkSystem(self._lib, systems_ptr[i]) 
            for i in range(count.value)
        ]
        
        self._lib.mavsdk_free_systems_array(systems_ptr)
        return systems
    
    def first_autopilot(self, timeout_s: float = 3.0) -> Optional[MavsdkSystem]:
        """Wait for and return first autopilot system"""
        handle = self._lib.mavsdk_first_autopilot(self._handle, timeout_s)
        if handle:
            return MavsdkSystem(self._lib, handle)
        return None
    
    def subscribe_on_new_system(self, callback: Callable[[Any], None], 
                                user_data: Any = None):
        """Subscribe to new system discoveries"""
        c_callback = NewSystemCallback(lambda ud: callback(user_data))
        self._callbacks.append(c_callback)
        
        handle = self._lib.mavsdk_subscribe_on_new_system(
            self._handle, c_callback, None
        )
        return handle
    
    def unsubscribe_on_new_system(self, handle: ctypes.c_void_p):
        """Unsubscribe from new system discoveries"""
        self._lib.mavsdk_unsubscribe_on_new_system(self._handle, handle)
    
    def destroy(self):
        """Destroy the Mavsdk instance"""
        if not self._destroyed and self._handle:
            self._lib.mavsdk_destroy(self._handle)
            self._handle = None
            self._destroyed = True
    
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.destroy()
    
def __del__(self):
    try:
        import sys
        if sys is not None and hasattr(self, '_handle') and self._handle:
            import warnings
            warnings.warn(
                "Mavsdk destroyed by garbage collector. Use context manager or call destroy() explicitly.",
                ResourceWarning,
                stacklevel=2
            )
            self.destroy()
    except (ImportError, AttributeError):
        # Python is shutting down, skip cleanup silently
        pass
