"""Struct defaults match the C++ ones, so unset fields convert to the C layer."""

import math

from mavsdk.plugins.mission import MissionItem, MissionPlan


def test_mission_item_defaults():
    item = MissionItem(latitude_deg=47.398, longitude_deg=8.546, relative_altitude_m=10)

    assert math.isnan(item.speed_m_s)
    assert math.isnan(item.yaw_deg)
    assert item.is_fly_through is False
    assert item.camera_action == MissionItem.CameraAction.NONE
    assert item.vehicle_action == MissionItem.VehicleAction.NONE
    assert item.camera_photo_interval_s == 1.0

    back = MissionItem.from_c_struct(item.to_c_struct())
    assert back.latitude_deg == 47.398
    assert math.isnan(back.loiter_time_s)

    MissionPlan([item]).to_c_struct()
