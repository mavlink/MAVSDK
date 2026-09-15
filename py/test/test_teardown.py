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

        from aiomavsdk import ComponentType, Configuration, Mavsdk
        from aiomavsdk.plugins.mission_raw_server import MissionRawServerAsync

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
