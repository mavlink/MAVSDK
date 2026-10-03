"""Tests for ``mavsdk.asyncio.Mavsdk.first_autopilot``.

It waits on the event loop rather than blocking an executor thread, so it has
to wake up when an autopilot appears, time out on its own, and leave no
subscription behind when it is cancelled.
"""

import asyncio
import time

from mavsdk.asyncio import ComponentType, Configuration, Mavsdk

DISCOVERY_TIMEOUT_S = 10.0


def make(component_type):
    return Mavsdk(Configuration.create_with_component_type(component_type))


def test_finds_autopilot_already_connected():
    async def scenario():
        groundstation = make(ComponentType.GROUND_STATION)
        autopilot = make(ComponentType.AUTOPILOT)
        await groundstation.add_any_connection("udpin://0.0.0.0:17030")
        await autopilot.add_any_connection("udpout://127.0.0.1:17030")

        system = await groundstation.first_autopilot(DISCOVERY_TIMEOUT_S)
        assert system is not None
        assert await system.has_autopilot()

        groundstation.destroy()
        autopilot.destroy()

    asyncio.run(scenario())


def test_finds_autopilot_that_appears_while_waiting():
    async def scenario():
        groundstation = make(ComponentType.GROUND_STATION)
        autopilot = make(ComponentType.AUTOPILOT)
        await groundstation.add_any_connection("udpin://0.0.0.0:17031")

        waiting = asyncio.create_task(
            groundstation.first_autopilot(DISCOVERY_TIMEOUT_S)
        )
        await asyncio.sleep(0.5)
        assert not waiting.done()

        # Only now let the autopilot talk, so it is found by the wake-up and
        # not by the first check.
        await autopilot.add_any_connection("udpout://127.0.0.1:17031")
        system = await waiting
        assert system is not None
        assert await system.has_autopilot()

        groundstation.destroy()
        autopilot.destroy()

    asyncio.run(scenario())


def test_times_out():
    async def scenario():
        groundstation = make(ComponentType.GROUND_STATION)

        assert await groundstation.first_autopilot(0) is None

        start = time.monotonic()
        assert await groundstation.first_autopilot(0.5) is None
        assert time.monotonic() - start < 2.0

        groundstation.destroy()

    asyncio.run(scenario())


def test_cancel_leaves_no_subscription():
    async def scenario():
        groundstation = make(ComponentType.GROUND_STATION)

        waiting = asyncio.create_task(groundstation.first_autopilot(30.0))
        await asyncio.sleep(0.2)
        waiting.cancel()
        await asyncio.gather(waiting, return_exceptions=True)

        assert not groundstation._mavsdk._subscriptions
        groundstation.destroy()

    asyncio.run(scenario())


def test_finds_autopilot_joining_connected_system():
    # A companion computer on the autopilot's system ID is heard first, so the
    # system is already connected when its autopilot joins.
    async def scenario():
        groundstation = make(ComponentType.GROUND_STATION)
        await groundstation.add_any_connection("udpin://0.0.0.0:17032")

        companion_config = Configuration.create_with_component_type(
            ComponentType.COMPANION_COMPUTER
        )
        companion_config.system_id = 1
        companion = Mavsdk(companion_config)
        await companion.add_any_connection("udpout://127.0.0.1:17032")
        await asyncio.sleep(1.5)
        assert await groundstation.first_autopilot(0) is None

        autopilot = make(ComponentType.AUTOPILOT)
        waiting = asyncio.create_task(
            groundstation.first_autopilot(DISCOVERY_TIMEOUT_S)
        )
        await asyncio.sleep(0.5)
        start = time.monotonic()
        await autopilot.add_any_connection("udpout://127.0.0.1:17032")

        system = await waiting
        assert system is not None
        assert await system.has_autopilot()
        assert time.monotonic() - start < 5.0

        for instance in (groundstation, companion, autopilot):
            instance.destroy()

    asyncio.run(scenario())


def test_sync_first_autopilot_async_and_cancel():
    import threading

    from mavsdk import Mavsdk as SyncMavsdk, Configuration as SyncConfiguration

    groundstation = SyncMavsdk(
        SyncConfiguration.create_with_component_type(ComponentType.GROUND_STATION)
    )

    # Times out, and the callback says so from another thread.
    results = []
    done = threading.Event()
    caller = threading.get_ident()

    def on_result(system):
        results.append((system, threading.get_ident()))
        done.set()

    handle = groundstation.first_autopilot_async(0.3, on_result)
    assert done.wait(3.0)
    groundstation.cancel_first_autopilot(handle)
    assert results[0][0] is None
    assert results[0][1] != caller

    # Cancelled before anything happens: never called, and released.
    called = threading.Event()
    handle = groundstation.first_autopilot_async(0.3, lambda _: called.set())
    groundstation.cancel_first_autopilot(handle)
    groundstation.cancel_first_autopilot(handle)
    assert not called.wait(1.0)
    assert not groundstation._subscriptions

    groundstation.destroy()
