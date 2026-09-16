"""System test: uploading a raw mission with the ``aiomavsdk`` bindings.

The ``mavsdk`` counterpart of this test is in ``test_mission_raw_upload.py``;
this one drives the same exchange through the asyncio wrapper, which is where
#3087 dropped parameters on the way through.
"""

import asyncio

import pytest
from aiomavsdk import ComponentType, Configuration, Mavsdk
from aiomavsdk.plugins.mission_raw import (
    MissionItem,
    MissionPlan,
    MissionRawAsync,
    MissionRawResult,
)
from aiomavsdk.plugins.mission_raw_server import MissionRawServerAsync

DISCOVERY_TIMEOUT_S = 10.0
UPLOAD_TIMEOUT_S = 10.0

MAV_CMD_NAV_WAYPOINT = 16
MAV_CMD_NAV_TAKEOFF = 22
MAV_FRAME_GLOBAL_RELATIVE_ALT_INT = 3
MAV_MISSION_TYPE_MISSION = 0


def make_mission_item(seq, command, latitude_deg, longitude_deg, altitude_m):
    """Build a MISSION_ITEM_INT the way an autopilot expects to receive it."""
    return MissionItem(
        seq=seq,
        frame=MAV_FRAME_GLOBAL_RELATIVE_ALT_INT,
        command=command,
        current=1 if seq == 0 else 0,
        autocontinue=1,
        param1=0.0,
        param2=0.0,
        param3=0.0,
        param4=0.0,
        x=int(latitude_deg * 1e7),
        y=int(longitude_deg * 1e7),
        z=altitude_m,
        mission_type=MAV_MISSION_TYPE_MISSION,
    )


def example_mission():
    """A short but non-trivial mission.

    More than one item on purpose: with a single item, a dropped size argument
    can still look like it worked.
    """
    return [
        make_mission_item(0, MAV_CMD_NAV_TAKEOFF, 47.398, 8.546, 10.0),
        make_mission_item(1, MAV_CMD_NAV_WAYPOINT, 47.399, 8.547, 10.0),
        make_mission_item(2, MAV_CMD_NAV_WAYPOINT, 47.400, 8.548, 20.0),
    ]


def assert_missions_match(uploaded, received):
    """Compare the items the server got against the ones the client sent."""
    assert received is not None, "server never reported an incoming mission"
    assert len(received) == len(uploaded), (
        f"server received {len(received)} items, expected {len(uploaded)}"
    )
    for expected, actual in zip(uploaded, received):
        assert actual.seq == expected.seq
        assert actual.command == expected.command
        assert actual.frame == expected.frame
        assert actual.x == expected.x
        assert actual.y == expected.y
        assert actual.z == pytest.approx(expected.z)


def test_upload_mission():
    """Awaited upload: the item array and its size both reach the autopilot."""

    async def scenario():
        groundstation = Mavsdk(
            Configuration.create_with_component_type(ComponentType.GROUND_STATION)
        )
        autopilot = Mavsdk(
            Configuration.create_with_component_type(ComponentType.AUTOPILOT)
        )

        # Register the server plugin before the connections, so the MISSION_INT
        # capability is advertised in the first AUTOPILOT_VERSION exchange.
        server = MissionRawServerAsync(await autopilot.server_component())
        incoming: asyncio.Queue = asyncio.Queue()

        async def pump():
            async for _result, plan in server.subscribe_incoming_mission():
                await incoming.put(plan)

        pump_task = asyncio.create_task(pump())

        # The asyncio wrapper raises on failure rather than returning a result.
        await groundstation.add_any_connection("udpin://0.0.0.0:17012")
        await autopilot.add_any_connection("udpout://127.0.0.1:17012")

        system = await groundstation.first_autopilot(DISCOVERY_TIMEOUT_S)
        assert system is not None, "ground station did not discover the autopilot"

        uploaded = example_mission()
        assert await MissionRawAsync(system).upload_mission(uploaded) == (
            MissionRawResult.SUCCESS
        )

        plan = await asyncio.wait_for(incoming.get(), UPLOAD_TIMEOUT_S)
        assert_missions_match(uploaded, plan.mission_items)

        # Wait for the cancellation to land, so the subscription unsubscribes
        # while the plugin is still there.
        pump_task.cancel()
        await asyncio.gather(pump_task, return_exceptions=True)

        groundstation.destroy()
        autopilot.destroy()

    asyncio.run(scenario())


def test_upload_mission_with_progress():
    """The asyncio wrapper forwards the mission plan on to the sync binding
    instead of dropping it, and yields progress until the upload completes."""

    async def scenario():
        groundstation = Mavsdk(
            Configuration.create_with_component_type(ComponentType.GROUND_STATION)
        )
        autopilot = Mavsdk(
            Configuration.create_with_component_type(ComponentType.AUTOPILOT)
        )

        server = MissionRawServerAsync(await autopilot.server_component())
        incoming: asyncio.Queue = asyncio.Queue()

        async def pump():
            async for _result, plan in server.subscribe_incoming_mission():
                await incoming.put(plan)

        pump_task = asyncio.create_task(pump())

        await groundstation.add_any_connection("udpin://0.0.0.0:17013")
        await autopilot.add_any_connection("udpout://127.0.0.1:17013")

        system = await groundstation.first_autopilot(DISCOVERY_TIMEOUT_S)
        assert system is not None, "ground station did not discover the autopilot"

        uploaded = example_mission()
        results = []
        async for result, _data in MissionRawAsync(system).upload_mission_with_progress(
            MissionPlan(mission_items=uploaded)
        ):
            results.append(result)

        assert results[-1] == MissionRawResult.SUCCESS

        plan = await asyncio.wait_for(incoming.get(), UPLOAD_TIMEOUT_S)
        assert_missions_match(uploaded, plan.mission_items)

        pump_task.cancel()
        await asyncio.gather(pump_task, return_exceptions=True)

        groundstation.destroy()
        autopilot.destroy()

    asyncio.run(scenario())
