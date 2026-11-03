"""System test: MAVLink 2 message signing with the ``mavsdk`` bindings.

Two MAVSDK instances talk to each other over a local UDP link, one as a ground
station and one as an autopilot. Arming only works if both use the same key.
"""

import pytest
from mavsdk import ComponentType, Configuration, ConnectionResult, Mavsdk
from mavsdk.plugins.action import Action, ActionError, ActionResult
from mavsdk.plugins.action_server import ActionServer

DISCOVERY_TIMEOUT_S = 10.0

KEY = bytes(range(32))
OTHER_KEY = bytes(range(1, 33))


def connect(port, groundstation_key, autopilot_key):
    groundstation = Mavsdk(
        Configuration.create_with_component_type(ComponentType.GROUND_STATION)
    )
    autopilot = Mavsdk(
        Configuration.create_with_component_type(ComponentType.AUTOPILOT)
    )
    if groundstation_key is not None:
        groundstation.enable_signing(groundstation_key)
    if autopilot_key is not None:
        autopilot.enable_signing(autopilot_key)

    action_server = ActionServer(autopilot.server_component())
    action_server.set_armable(True, True)

    assert (
        groundstation.add_any_connection(f"udpin://0.0.0.0:{port}")
        == ConnectionResult.SUCCESS
    )
    assert (
        autopilot.add_any_connection(f"udpout://127.0.0.1:{port}")
        == ConnectionResult.SUCCESS
    )

    # Heartbeats are accepted unsigned, so discovery works with any key.
    system = groundstation.first_autopilot(DISCOVERY_TIMEOUT_S)
    assert system is not None, "ground station did not discover the autopilot"
    return groundstation, autopilot, action_server, system


def test_signing_enabled_flag():
    mavsdk = Mavsdk(Configuration.create_with_component_type(ComponentType.AUTOPILOT))
    assert not mavsdk.is_signing_enabled()
    mavsdk.enable_signing(KEY)
    assert mavsdk.is_signing_enabled()
    mavsdk.disable_signing()
    assert not mavsdk.is_signing_enabled()
    mavsdk.destroy()


def test_signing_rejects_wrong_key_length():
    mavsdk = Mavsdk(Configuration.create_with_component_type(ComponentType.AUTOPILOT))
    with pytest.raises(ValueError):
        mavsdk.enable_signing(b"too short")
    assert not mavsdk.is_signing_enabled()
    mavsdk.destroy()


def test_signed_command():
    groundstation, autopilot, _server, system = connect(17040, KEY, KEY)

    assert Action(system).arm() == ActionResult.SUCCESS

    groundstation.destroy()
    autopilot.destroy()


def test_wrong_key_rejected():
    groundstation, autopilot, _server, system = connect(17041, KEY, OTHER_KEY)

    with pytest.raises(ActionError) as error:
        Action(system).arm()
    assert error.value.result == ActionResult.TIMEOUT

    groundstation.destroy()
    autopilot.destroy()
