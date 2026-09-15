"""System test: uploading a raw mission with the ``mavsdk`` bindings.

Two MAVSDK instances talk to each other over a local UDP link, one as a ground
station and one as an autopilot, and the mission the autopilot receives is
checked item by item. Nothing is mocked.

This is the regression test for #3087, where the generated bindings passed the
wrong arguments down to the C layer: a dropped array-size argument and a
mission plan that was never converted to its C struct.
"""

import queue

import pytest
from mavsdk import ComponentType, Configuration, ConnectionResult, Mavsdk
from mavsdk.plugins.mission_raw import (
    MissionItem,
    MissionPlan,
    MissionRaw,
    MissionRawResult,
)
from mavsdk.plugins.mission_raw_server import MissionRawServer

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
    """Blocking upload: the item array and its size both reach the autopilot."""
    groundstation = Mavsdk(
        Configuration.create_with_component_type(ComponentType.GROUND_STATION)
    )
    autopilot = Mavsdk(
        Configuration.create_with_component_type(ComponentType.AUTOPILOT)
    )

    # Register the server plugin before the connections, so the MISSION_INT
    # capability is advertised in the first AUTOPILOT_VERSION exchange.
    mission_raw_server = MissionRawServer(autopilot.server_component())
    incoming: queue.Queue = queue.Queue()
    mission_raw_server.subscribe_incoming_mission(
        lambda _result, plan, _user_data: incoming.put(plan)
    )

    assert (
        groundstation.add_any_connection("udpin://0.0.0.0:17010")
        == ConnectionResult.SUCCESS
    )
    assert (
        autopilot.add_any_connection("udpout://127.0.0.1:17010")
        == ConnectionResult.SUCCESS
    )

    system = groundstation.first_autopilot(DISCOVERY_TIMEOUT_S)
    assert system is not None, "ground station did not discover the autopilot"

    uploaded = example_mission()
    assert MissionRaw(system).upload_mission(uploaded) == MissionRawResult.SUCCESS

    plan = incoming.get(timeout=UPLOAD_TIMEOUT_S)
    assert_missions_match(uploaded, plan.mission_items)

    groundstation.destroy()
    autopilot.destroy()


def test_upload_mission_with_progress():
    """Progress-reporting upload: the mission plan reaches the autopilot, and
    progress is reported until the upload completes."""
    groundstation = Mavsdk(
        Configuration.create_with_component_type(ComponentType.GROUND_STATION)
    )
    autopilot = Mavsdk(
        Configuration.create_with_component_type(ComponentType.AUTOPILOT)
    )

    mission_raw_server = MissionRawServer(autopilot.server_component())
    incoming: queue.Queue = queue.Queue()
    mission_raw_server.subscribe_incoming_mission(
        lambda _result, plan, _user_data: incoming.put(plan)
    )

    assert (
        groundstation.add_any_connection("udpin://0.0.0.0:17011")
        == ConnectionResult.SUCCESS
    )
    assert (
        autopilot.add_any_connection("udpout://127.0.0.1:17011")
        == ConnectionResult.SUCCESS
    )

    system = groundstation.first_autopilot(DISCOVERY_TIMEOUT_S)
    assert system is not None, "ground station did not discover the autopilot"

    uploaded = example_mission()
    updates: queue.Queue = queue.Queue()
    MissionRaw(system).upload_mission_with_progress_async(
        MissionPlan(mission_items=uploaded),
        lambda result, data, _user_data: updates.put((result, data)),
    )

    results = []
    while True:
        result, _data = updates.get(timeout=UPLOAD_TIMEOUT_S)
        results.append(result)
        if result != MissionRawResult.NEXT:
            break

    assert results[-1] == MissionRawResult.SUCCESS

    plan = incoming.get(timeout=UPLOAD_TIMEOUT_S)
    assert_missions_match(uploaded, plan.mission_items)

    groundstation.destroy()
    autopilot.destroy()
