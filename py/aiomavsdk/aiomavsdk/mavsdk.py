"""Async wrapper for Mavsdk and Configuration."""

import asyncio

from typing import Optional, List, AsyncGenerator

from mavsdk.mavsdk import Mavsdk as _Mavsdk, Configuration
from mavsdk.component_type import ComponentType
from mavsdk.server_component import ServerComponent
from .system import System
from .exceptions import MavsdkConnectionError


# Configuration has no blocking calls and no subscriptions — re-export as-is
# so users construct it exactly the same way as with mavsdk.
__all__ = ["Configuration", "Mavsdk"]


class Mavsdk:
    """
    Async wrapper around mavsdk's :class:`~mavsdk.mavsdk.Mavsdk`.

    Usage mirrors the gRPC-based asyncio API:

    - Methods that may block are wrapped with ``run_in_executor`` so the
      event loop stays responsive.
    - ``on_new_system`` is an async generator instead of a callback-based
      subscription.
    - Returned :class:`System` objects are the aiomavsdk async wrapper, not
      the raw mavsdk ones.
    - Use as an async context manager (``async with``) or call
      :meth:`destroy` explicitly.

    Example
    -------
    ::

        config = Configuration.create_with_component_type(ComponentType.GROUND_STATION)
        async with Mavsdk(config) as mavsdk:
            await mavsdk.add_any_connection("udp://:14540")
            system = await mavsdk.first_autopilot(timeout_s=10.0)
    """

    def __init__(self, configuration: Configuration):
        self._mavsdk = _Mavsdk(configuration)
        self._subscription_handles: dict = {}

    # ------------------------------------------------------------------
    # Non-blocking accessors
    # ------------------------------------------------------------------

    async def version(self) -> str:
        """Get MAVSDK version string."""
        return self._mavsdk.version()

    async def system_count(self) -> int:
        """Get number of discovered systems."""
        return self._mavsdk.system_count()

    # ------------------------------------------------------------------
    # Potentially blocking calls — offloaded to executor
    # ------------------------------------------------------------------

    async def add_any_connection(self, connection_url: str) -> None:
        """
        Add a connection.

        Parameters
        ----------
        connection_url : str
            Connection URL, e.g. ``"udp://:14540"`` or ``"serial:///dev/ttyUSB0:57600"``.

        Raises
        ------
        MavsdkConnectionError
            If the connection fails.
        """
        loop = asyncio.get_running_loop()
        try:
            await loop.run_in_executor(
                None, self._mavsdk.add_any_connection_with_handle, connection_url
            )
        except Exception as e:
            raise MavsdkConnectionError(str(e)) from e

    async def remove_connection(self, handle) -> None:
        """Remove a previously added connection."""
        loop = asyncio.get_running_loop()
        await loop.run_in_executor(None, self._mavsdk.remove_connection, handle)

    async def get_systems(self) -> List[System]:
        """Get all currently discovered systems."""
        loop = asyncio.get_running_loop()
        raw_systems = await loop.run_in_executor(None, self._mavsdk.get_systems)
        return [System(s) for s in raw_systems]

    async def first_autopilot(self, timeout_s: float = 3.0) -> Optional[System]:
        """
        Wait for and return the first autopilot system.

        Blocks the thread (in an executor) for up to *timeout_s* seconds so
        the event loop stays responsive.

        Parameters
        ----------
        timeout_s : float
            How long to wait for an autopilot to appear, in seconds.

        Returns
        -------
        System or None
            The first autopilot system found, or ``None`` if the timeout
            expires before one appears.
        """
        loop = asyncio.get_running_loop()
        raw = await loop.run_in_executor(None, self._mavsdk.first_autopilot, timeout_s)
        return System(raw) if raw is not None else None

    async def server_component(self, instance: int = 0) -> Optional[ServerComponent]:
        """Get server component by instance."""
        return self._mavsdk.server_component(instance)

    async def server_component_by_type(
        self, component_type: ComponentType, instance: int = 0
    ) -> Optional[ServerComponent]:
        """Get server component by type and instance."""
        return self._mavsdk.server_component_by_type(component_type, instance)

    async def server_component_by_id(
        self, component_id: int
    ) -> Optional[ServerComponent]:
        """Get server component by component ID."""
        return self._mavsdk.server_component_by_id(component_id)

    # ------------------------------------------------------------------
    # Streaming subscription
    # ------------------------------------------------------------------

    async def on_new_system(self) -> AsyncGenerator[None, None]:
        """
        Subscribe to new system discoveries.

        Yields once each time a new system is discovered. Call
        :meth:`get_systems` inside the loop body to retrieve the updated list.

        Example
        -------
        ::

            async for _ in mavsdk.on_new_system():
                systems = await mavsdk.get_systems()
                print(f"New system discovered, total: {len(systems)}")
                break  # stop after first discovery
        """

        loop = asyncio.get_running_loop()
        queue: asyncio.Queue = asyncio.Queue()

        def callback(_user_data):
            loop.call_soon_threadsafe(queue.put_nowait, None)

        handle = self._mavsdk.subscribe_on_new_system(callback)
        self._subscription_handles[id(queue)] = handle
        try:
            while True:
                yield await queue.get()
        finally:
            if id(queue) in self._subscription_handles:
                self._subscription_handles.pop(id(queue))
                self._mavsdk.unsubscribe_on_new_system(handle)

    # ------------------------------------------------------------------
    # Lifecycle
    # ------------------------------------------------------------------

    def destroy(self) -> None:
        """Destroy the underlying Mavsdk instance and release resources."""
        self._mavsdk.destroy()
        self._subscription_handles.clear()

    async def __aenter__(self):
        return self

    async def __aexit__(self, exc_type, exc_val, exc_tb):
        self.destroy()
