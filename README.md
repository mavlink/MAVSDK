# pymavsdk

Python wrapper for the cmavsdk C library using ctypes.

## Installation

```bash
pip install pymavsdk
```

Or install from source:

```bash
git clone https://github.com/yourusername/pymavsdk.git
cd pymavsdk
pip install -e .
```

## Prerequisites

The cmavsdk shared library must be installed on your system. The wrapper will search for:
- Linux: `libcmavsdk.so`
- macOS: `libcmavsdk.dylib`
- Windows: `cmavsdk.dll`

You can place the library in:
- The current working directory
- A `lib/` subdirectory
- System library paths

## Usage

### Basic Example

```python
from pymavsdk import Mavsdk, ComponentType
import time

# Create Mavsdk instance
mavsdk = Mavsdk()

# Add connection
mavsdk.add_any_connection("udp://:14540")

# Wait for system
print("Waiting for system...")
system = mavsdk.first_autopilot(timeout_s=5.0)

if system:
    print(f"Found system with ID: {system.get_system_id()}")
    print(f"Has autopilot: {system.has_autopilot()}")
    print(f"Is connected: {system.is_connected()}")
    print(f"Autopilot type: {system.autopilot_type()}")
    print(f"Vehicle type: {system.vehicle_type()}")
else:
    print("No system found")

mavsdk.destroy()
```

### Using Configuration

```python
from pymavsdk import Mavsdk, MavsdkConfiguration, ComponentType

# Create configuration
config = MavsdkConfiguration.create_with_component_type(
    mavsdk._lib,  # Need library reference
    ComponentType.COMPANION_COMPUTER
)
config.system_id = 1
config.component_id = 191

# Create Mavsdk with config
mavsdk = Mavsdk(configuration=config)
mavsdk.add_any_connection("udp://:14540")
```

### Context Manager

```python
from pymavsdk import Mavsdk

with Mavsdk() as mavsdk:
    mavsdk.add_any_connection("udp://:14540")
    system = mavsdk.first_autopilot()
    if system:
        print(f"Connected to system {system.get_system_id()}")
    # Automatic cleanup on exit
```

### Subscribing to System Discovery

```python
from pymavsdk import Mavsdk

def on_new_system(user_data):
    print("New system discovered!")

mavsdk = Mavsdk()
mavsdk.add_any_connection("udp://:14540")

# Subscribe to new systems
handle = mavsdk.subscribe_on_new_system(on_new_system)

# Wait for systems...
import time
time.sleep(10)

# Cleanup
mavsdk.unsubscribe_on_new_system(handle)
mavsdk.destroy()
```

### Connection State Monitoring

```python
from pymavsdk import Mavsdk

def on_connection_change(is_connected, user_data):
    status = "connected" if is_connected else "disconnected"
    print(f"System {status}")

mavsdk = Mavsdk()
mavsdk.add_any_connection("udp://:14540")

system = mavsdk.first_autopilot()
if system:
    handle = system.subscribe_is_connected(on_connection_change)
    
    # Monitor connection...
    import time
    time.sleep(30)
    
    system.unsubscribe_is_connected(handle)

mavsdk.destroy()
```

### Logging

```python
from pymavsdk import log_subscribe, log_unsubscribe, LogLevel

def my_log_handler(level, message, file, line):
    if level >= LogLevel.WARN:
        print(f"[{level.name}] {message}")
        if file:
            print(f"  at {file}:{line}")
    return True  # Suppress default logging

# Subscribe to logs
log_subscribe(my_log_handler)

# Your code here...

# Unsubscribe when done
log_unsubscribe()
```

### Getting System Information

```python
from pymavsdk import Mavsdk, Autopilot, Vehicle

mavsdk = Mavsdk()
mavsdk.add_any_connection("udp://:14540")

system = mavsdk.first_autopilot()
if system:
    # Get component IDs
    components = system.component_ids()
    print(f"Components: {components}")
    
    # Check capabilities
    print(f"Has camera: {system.has_camera()}")
    print(f"Has gimbal: {system.has_gimbal()}")
    
    # Get types
    autopilot = Autopilot(system.autopilot_type())
    vehicle = Vehicle(system.vehicle_type())
    print(f"Autopilot: {autopilot.name}")
    print(f"Vehicle: {vehicle.name}")

mavsdk.destroy()
```

## Enumerations

### ComponentType
- `AUTOPILOT`
- `GROUND_STATION`
- `COMPANION_COMPUTER`
- `CAMERA`
- `GIMBAL`
- `REMOTE_ID`
- `CUSTOM`

### Autopilot
- `UNKNOWN`
- `PX4`
- `ARDUPILOT`
- `GENERIC`

### Vehicle
- `UNKNOWN`
- `GENERIC`
- `FIXED_WING`
- `MULTICOPTER`
- `ROVER`
- `SUBMARINE`

### ConnectionResult
- `SUCCESS`
- `TIMEOUT`
- `SOCKET_ERROR`
- `BIND_ERROR`
- `SOCKET_CONNECTION_ERROR`
- `CONNECTION_ERROR`
- `NOT_IMPLEMENTED`
- `SYSTEM_NOT_CONNECTED`
- `SYSTEM_BUSY`
- `COMMAND_DENIED`
- `DESTINATION_IP_UNKNOWN`
- `CONNECTIONS_EXHAUSTED`
- `CONNECTION_URL_INVALID`
- `BAUDRATE_UNKNOWN`

### LogLevel
- `DEBUG`
- `INFO`
- `WARN`
- `ERROR`

## Project Structure

```
pymavsdk/
├── pymavsdk/
│   ├── __init__.py       # Package initialization
│   ├── wrapper.py        # Main ctypes wrapper
│   ├── exceptions.py     # Custom exceptions
│   ├── types.py          # C structure definitions
│   ├── enums.py          # Enumeration types
│   └── logging.py        # Logging support
├── tests/
│   └── test_wrapper.py   # Unit tests
├── examples/
│   └── basic_usage.py    # Example scripts
├── setup.py              # Package setup
├── README.md             # Documentation
└── requirements.txt      # Dependencies
```

## API Documentation

### Mavsdk Class

#### `__init__(configuration=None, lib_path=None)`
Create a new Mavsdk instance.

#### `version() -> str`
Get the MAVSDK version string.

#### `add_any_connection(connection_url: str) -> ConnectionResult`
Add a connection. Returns ConnectionResult enum.

#### `add_any_connection_with_handle(connection_url: str) -> handle`
Add a connection and return handle for later removal.

#### `remove_connection(handle)`
Remove a connection by handle.

#### `system_count() -> int`
Get number of discovered systems.

#### `get_systems() -> List[MavsdkSystem]`
Get all discovered systems.

#### `first_autopilot(timeout_s: float = 3.0) -> Optional[MavsdkSystem]`
Wait for and return first autopilot system.

#### `subscribe_on_new_system(callback, user_data=None) -> handle`
Subscribe to new system discoveries.

#### `unsubscribe_on_new_system(handle)`
Unsubscribe from new system discoveries.

### MavsdkSystem Class

#### `has_autopilot() -> bool`
Check if system has autopilot.

#### `is_standalone() -> bool`
Check if system is standalone.

#### `has_camera(camera_id: int = -1) -> bool`
Check if system has camera.

#### `has_gimbal() -> bool`
Check if system has gimbal.

#### `is_connected() -> bool`
Check if system is connected.

#### `get_system_id() -> int`
Get the system ID.

#### `component_ids() -> List[int]`
Get list of component IDs.

#### `autopilot_type() -> int`
Get autopilot type (use Autopilot enum).

#### `vehicle_type() -> int`
Get vehicle type (use Vehicle enum).

#### `subscribe_is_connected(callback, user_data=None) -> handle`
Subscribe to connection state changes.

#### `enable_timesync()`
Enable time synchronization.

### MavsdkConfiguration Class

#### `create_with_component_type(lib, component_type: ComponentType)`
Create configuration with component type.

#### `create_manual(lib, system_id: int, component_id: int, always_send_heartbeats: bool)`
Create manual configuration.

#### Properties
- `system_id`: Get/set system ID
- `component_id`: Get/set component ID

## Development

### Running Tests

```bash
pytest tests/
```

### Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

