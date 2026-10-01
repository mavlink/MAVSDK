"""Teardown tests for the Python bindings.

Objects on the Python side hold handles into C++ objects owned by the Mavsdk
instance, and Python decides on its own when to let go of them: a subscription
generator unwinds from a ``finally`` block, ``__del__`` runs at interpreter
shutdown. Both can happen after the Mavsdk instance is already destroyed, which
used to take the process down with a segfault or a double free.

Each scenario runs in its own interpreter and is checked by its exit code, so a
regression here reports as one failed test rather than killing the whole run.
"""

import subprocess
import sys
import textwrap

SCENARIO_TIMEOUT_S = 60.0


def run_scenario(script: str) -> None:
    """Run a teardown scenario in its own interpreter and expect a clean exit."""
    result = subprocess.run(
        [sys.executable, "-c", textwrap.dedent(script)],
        capture_output=True,
        text=True,
        timeout=SCENARIO_TIMEOUT_S,
        check=False,
    )
    assert result.returncode == 0, (
        f"scenario exited with {result.returncode}\n"
        f"--- stdout ---\n{result.stdout}\n--- stderr ---\n{result.stderr}"
    )


def test_unsubscribe_after_destroy():
    run_scenario("""
        from mavsdk import ComponentType, Configuration, Mavsdk
        from mavsdk.plugins.mission_raw_server import MissionRawServer

        mavsdk = Mavsdk(
            Configuration.create_with_component_type(ComponentType.AUTOPILOT)
        )
        server = MissionRawServer(mavsdk.server_component())
        handle = server.subscribe_incoming_mission(lambda *_: None)

        mavsdk.destroy()
        server.unsubscribe_incoming_mission(handle)
    """)


def test_unsubscribe_twice():
    run_scenario("""
        from mavsdk import ComponentType, Configuration, Mavsdk
        from mavsdk.plugins.mission_raw_server import MissionRawServer

        mavsdk = Mavsdk(
            Configuration.create_with_component_type(ComponentType.AUTOPILOT)
        )
        server = MissionRawServer(mavsdk.server_component())
        handle = server.subscribe_incoming_mission(lambda *_: None)

        server.unsubscribe_incoming_mission(handle)
        server.unsubscribe_incoming_mission(handle)
        mavsdk.destroy()
    """)


def test_async_subscription_outlives_mavsdk():
    run_scenario("""
        import asyncio

        from mavsdk.asyncio import ComponentType, Configuration, Mavsdk
        from mavsdk.asyncio.plugins.mission_raw_server import MissionRawServerAsync

        async def main():
            mavsdk = Mavsdk(
                Configuration.create_with_component_type(ComponentType.AUTOPILOT)
            )
            server = MissionRawServerAsync(await mavsdk.server_component())

            async def pump():
                async for _ in server.subscribe_incoming_mission():
                    pass

            asyncio.create_task(pump())
            await asyncio.sleep(0.1)  # let the subscription get established

            # Destroy first and leave the generator open: asyncio.run closes it
            # during shutdown, so unsubscribe lands after the plugin is gone.
            mavsdk.destroy()

        asyncio.run(main())
    """)


def test_plugin_collected_at_interpreter_exit():
    run_scenario("""
        from mavsdk import ComponentType, Configuration, Mavsdk
        from mavsdk.plugins.mission_raw_server import MissionRawServer

        mavsdk = Mavsdk(
            Configuration.create_with_component_type(ComponentType.AUTOPILOT)
        )
        server = MissionRawServer(mavsdk.server_component())
        server.subscribe_incoming_mission(lambda *_: None)

        mavsdk.destroy()
        # Fall off the end with the plugin still referenced: __del__ runs during
        # interpreter shutdown, against an instance that is already destroyed.
    """)


def test_unsubscribe_then_destroy():
    run_scenario("""
        from mavsdk import ComponentType, Configuration, Mavsdk

        mavsdk = Mavsdk(
            Configuration.create_with_component_type(ComponentType.GROUND_STATION)
        )
        mavsdk.add_any_connection("raw://")
        raw_handle = mavsdk.subscribe_raw_bytes_to_be_sent(lambda _: None)
        system_handle = mavsdk.subscribe_on_new_system(lambda _: None)

        mavsdk.unsubscribe_raw_bytes_to_be_sent(raw_handle)
        mavsdk.unsubscribe_on_new_system(system_handle)
        # destroy() releases what is still subscribed, which must not include
        # the handles unsubscribed above.
        mavsdk.destroy()
    """)


def test_core_unsubscribe_twice():
    run_scenario("""
        from mavsdk import ComponentType, Configuration, Mavsdk

        mavsdk = Mavsdk(
            Configuration.create_with_component_type(ComponentType.GROUND_STATION)
        )
        mavsdk.add_any_connection("raw://")
        raw_handle = mavsdk.subscribe_raw_bytes_to_be_sent(lambda _: None)
        system_handle = mavsdk.subscribe_on_new_system(lambda _: None)

        mavsdk.unsubscribe_raw_bytes_to_be_sent(raw_handle)
        mavsdk.unsubscribe_raw_bytes_to_be_sent(raw_handle)
        mavsdk.unsubscribe_on_new_system(system_handle)
        mavsdk.unsubscribe_on_new_system(system_handle)
        mavsdk.destroy()
    """)


def test_system_unsubscribe_twice():
    run_scenario("""
        from mavsdk import ComponentType, Configuration, Mavsdk

        autopilot = Mavsdk(
            Configuration.create_with_component_type(ComponentType.AUTOPILOT)
        )
        groundstation = Mavsdk(
            Configuration.create_with_component_type(ComponentType.GROUND_STATION)
        )
        groundstation.add_any_connection("udpin://0.0.0.0:17020")
        autopilot.add_any_connection("udpout://127.0.0.1:17020")

        # A System only exists once one is discovered, hence the second instance.
        system = groundstation.first_autopilot(10.0)
        assert system is not None
        handle = system.subscribe_is_connected(lambda *_: None)

        system.unsubscribe_is_connected(handle)
        system.unsubscribe_is_connected(handle)
        groundstation.destroy()
        autopilot.destroy()
    """)


def test_plugin_call_after_destroy_raises():
    run_scenario("""
        from mavsdk import ComponentType, Configuration, Mavsdk
        from mavsdk.plugins.mission_raw_server import MissionRawServer

        mavsdk = Mavsdk(
            Configuration.create_with_component_type(ComponentType.AUTOPILOT)
        )
        server = MissionRawServer(mavsdk.server_component())

        mavsdk.destroy()
        # The plugin went down with it. Using it anyway has to say so, rather
        # than hand a null handle to the C wrapper and segfault there.
        for call in (
            server.set_current_item_complete,
            lambda: server.subscribe_incoming_mission(lambda *_: None),
        ):
            try:
                call()
            except RuntimeError:
                continue
            raise AssertionError("expected RuntimeError from a destroyed plugin")
    """)


def test_dropped_instance_is_released():
    run_scenario("""
        import gc
        import warnings
        import weakref

        from mavsdk import ComponentType, Configuration, Mavsdk

        mavsdk = Mavsdk(
            Configuration.create_with_component_type(ComponentType.GROUND_STATION)
        )
        mavsdk.add_any_connection("udpin://0.0.0.0:17021")
        ref = weakref.ref(mavsdk)

        with warnings.catch_warnings(record=True) as caught:
            warnings.simplefilter("always")
            del mavsdk
            gc.collect()

        # Nothing may outlive the last reference: an instance kept alive would
        # hold its connections and io thread open for the rest of the process.
        assert ref() is None, "dropped instance was not collected"
        assert any(w.category is ResourceWarning for w in caught), (
            "dropping an undestroyed instance should warn"
        )
    """)
