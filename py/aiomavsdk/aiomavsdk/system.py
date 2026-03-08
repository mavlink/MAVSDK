"""Async wrapper for System."""

import asyncio

from typing import AsyncGenerator, List

from mavsdk.system import System as _System


class System:
    """
    Async wrapper around mavsdk's :class:`~mavsdk.system.System`.

    Simple accessors are exposed as ``async def`` for a uniform ``await``
    style. The connection-state subscription is exposed as an async generator.
    """

    def __init__(self, system: _System):
        self._system = system

    # ------------------------------------------------------------------
    # Non-blocking accessors
    # ------------------------------------------------------------------

    async def has_autopilot(self) -> bool:
        """Check if system has autopilot."""
        return self._system.has_autopilot()

    async def is_standalone(self) -> bool:
        """Check if system is standalone."""
        return self._system.is_standalone()

    async def has_camera(self, camera_id: int = -1) -> bool:
        """Check if system has camera."""
        return self._system.has_camera(camera_id)

    async def has_gimbal(self) -> bool:
        """Check if system has gimbal."""
        return self._system.has_gimbal()

    async def is_connected(self) -> bool:
        """Check if system is currently connected."""
        return self._system.is_connected()

    async def get_system_id(self) -> int:
        """Get system ID."""
        return self._system.get_system_id()

    # ------------------------------------------------------------------
    # Potentially blocking accessors — offloaded to executor
    # ------------------------------------------------------------------

    async def component_ids(self) -> List[int]:
        """Get list of component IDs."""
        loop = asyncio.get_running_loop()
        return await loop.run_in_executor(None, self._system.component_ids)

    # ------------------------------------------------------------------
    # Streaming subscription
    # ------------------------------------------------------------------

    async def is_connected_state(self) -> AsyncGenerator[bool, None]:
        """
        Subscribe to connection state changes.

        Yields
        ------
        is_connected : bool
            ``True`` when the system connects, ``False`` when it disconnects.

        Example
        -------
        ::

            async for connected in system.is_connected_state():
                print("Connected!" if connected else "Disconnected.")
        """
        loop = asyncio.get_running_loop()
        queue: asyncio.Queue = asyncio.Queue()

        def callback(connected: bool, _user_data):
            loop.call_soon_threadsafe(queue.put_nowait, connected)

        handle = self._system.subscribe_is_connected(callback)
        try:
            while True:
                yield await queue.get()
        finally:
            self._system.unsubscribe_is_connected(handle)
