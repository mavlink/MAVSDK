# Migrating from MAVSDK-Python

The `mavsdk` package on PyPI is changing hands.

It used to be [MAVSDK-Python](https://github.com/mavlink/MAVSDK-Python): an asyncio wrapper that talks gRPC to a `mavsdk_server` process bundled inside the package. From MAVSDK v4, the `mavsdk` name refers instead to a native binding that calls the MAVSDK C library directly, with no gRPC and no server process.

The two have different APIs. This page explains what to do about it.

::: warning
The native binding has not been released to PyPI yet — it will ship as `mavsdk` 4.0.0. Until then, `pip install mavsdk` still gives you the gRPC wrapper. Nothing breaks today; this page is here so the change doesn't surprise you later.
:::

## The short version

| | Before | From v4 |
|---|---|---|
| gRPC wrapper | `mavsdk` | `mavsdk-grpc`, imported as `mavsdk_grpc` |
| Native binding | *did not exist* | `mavsdk`, imported as `mavsdk` |

The gRPC wrapper deliberately installs under a *different import name* from the native binding, so the two can be installed side by side without overwriting each other.

## Which one do I want?

**Keep the gRPC API, keep getting updates** — switch to `mavsdk-grpc`. It continues to be developed, including releases tracking MAVSDK v4. Change your requirements and one import:

```sh
pip install mavsdk-grpc
```

```python
import mavsdk_grpc as mavsdk           # or: from mavsdk_grpc import System
```

Nothing else in your code needs to change.

**Change nothing at all** — pin the last release under the old name:

```
mavsdk<4
```

This keeps working indefinitely, but receives no further updates.

**Move to the native binding** — read on. It is worth it if you want to drop the `grpcio` dependency, avoid shipping and launching a `mavsdk_server` binary, or use the synchronous API.

## What the native binding changes

Beyond the API itself:

- **No `grpcio` dependency**, and no `mavsdk_server` subprocess. The package wraps the MAVSDK C library directly.
- **Two interfaces.** `mavsdk` exposes a synchronous, callback-based API; `mavsdk.asyncio` exposes an asyncio API close in spirit to MAVSDK-Python. Both come from the same distribution — there is nothing extra to install.
- **Explicit system discovery.** MAVSDK-Python hid connection and discovery behind `drone.connect()`. The native binding separates connecting from discovering systems, which matters once more than one vehicle is involved.
- **Plugins are constructed, not attributes.** `drone.action.arm()` becomes `Action(drone).arm()`.

## API mapping

| MAVSDK-Python | Native binding (asyncio) |
|---|---|
| `System()` | `Mavsdk(Configuration.create_with_component_type(...))` |
| `await drone.connect(system_address=url)` | `await mavsdk.add_any_connection(url)` |
| implicit in `connect()` | `async for _ in mavsdk.on_new_system(): ...` |
| `drone.action` | `ActionAsync(system)` |
| `drone.telemetry` | `TelemetryAsync(system)` |
| `drone.telemetry.position()` | `telemetry.subscribe_position()` |

## Side by side

Arming and taking off, before:

```python
import asyncio
from mavsdk import System


async def run():
    drone = System()
    await drone.connect(system_address="udpin://0.0.0.0:14540")

    async for state in drone.core.connection_state():
        if state.is_connected:
            break

    await drone.action.arm()
    await drone.action.takeoff()
    await asyncio.sleep(5)
    await drone.action.land()


asyncio.run(run())
```

and after, using `mavsdk.asyncio`:

```python
import asyncio
from mavsdk.asyncio import Mavsdk, Configuration, ComponentType
from mavsdk.asyncio.plugins.action import ActionAsync


async def run():
    configuration = Configuration.create_with_component_type(
        ComponentType.GROUND_STATION
    )
    mavsdk = Mavsdk(configuration)
    await mavsdk.add_any_connection("udpin://0.0.0.0:14540")

    drone = None
    async for _ in mavsdk.on_new_system():
        for system in await mavsdk.get_systems():
            if await system.has_autopilot() and await system.is_connected():
                drone = system
                break
        if drone is not None:
            break

    action = ActionAsync(drone)
    await action.arm()
    await action.takeoff()
    await asyncio.sleep(5)
    await action.land()


asyncio.run(run())
```

The same thing synchronously, which has no MAVSDK-Python equivalent:

```python
import time
from mavsdk import Mavsdk, Configuration, ComponentType
from mavsdk.plugins.action import Action

configuration = Configuration.create_with_component_type(ComponentType.GROUND_STATION)
mavsdk = Mavsdk(configuration)
mavsdk.add_any_connection("udpin://0.0.0.0:14540")

drone = None


def on_new_system(user_data=None):
    global drone
    for system in mavsdk.get_systems():
        if system.has_autopilot() and system.is_connected():
            drone = system
            break


handle = mavsdk.subscribe_on_new_system(on_new_system)
while drone is None:
    time.sleep(1)
mavsdk.unsubscribe_on_new_system(handle)

action = Action(drone)
action.arm()
action.takeoff()
time.sleep(5)
action.land()
```

## Getting help

If something here is wrong, unclear, or your use case isn't covered, please [open an issue](https://github.com/mavlink/MAVSDK/issues).
