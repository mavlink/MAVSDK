#!/usr/bin/env python


from __future__ import print_function
import grpc
import time
import threading
import sys
# import action_pb2 as dc_action
import action_pb2_grpc
import mission_pb2 as dc_mission
import mission_pb2_grpc
from google.protobuf import empty_pb2
import dronecore_pb2 as dronecore
import dronecore_pb2_grpc


thread_status = True


def wait_func(future_status):
    global thread_status
    ret = future_status.result()
    print(ret.result_str)
    thread_status = False


def run():
    global thread_status
    channel = grpc.insecure_channel('0.0.0.0:50051')
    # stub = dronecore_pb2_grpc.DroneCoreRPCStub(channel)
    action_stub = action_pb2_grpc.ActionRPCStub(channel)
    mission_stub = mission_pb2_grpc.MissionRPCStub(channel)
    dronecore_stub = dronecore_pb2_grpc.DroneCoreRPCStub(channel)

    response=dronecore_stub.Get_UUID_List(empty_pb2.Empty())
    size=len(response.uuid_list)
    print("Devices registered : {}".format(size))

    device_uuid=dronecore.UUID()
    if len(sys.argv) == 2:
        device_uuid.uuid=int(sys.argv[1])
    else :
        device_uuid.uuid=response.uuid_list[0].uuid

    mission_items = []

    mission_items.append(dc_mission.MissionItem(
        latitude_deg=47.398170327054473,
        longitude_deg=8.5456490218639658,
        relative_altitude_m=10,
        speed_m_s=5,
        is_fly_through=False,
        gimbal_pitch_deg=20,
        gimbal_yaw_deg=60,
        camera_action=dc_mission.MissionItem.NONE))

    mission_items.append(dc_mission.MissionItem(
        latitude_deg=47.398241338125118,
        longitude_deg=8.5455360114574432,
        relative_altitude_m=10,
        speed_m_s=2,
        is_fly_through=True,
        gimbal_pitch_deg=0,
        gimbal_yaw_deg=-60,
        camera_action=dc_mission.MissionItem.TAKE_PHOTO))

    mission_items.append(dc_mission.MissionItem(
        latitude_deg=47.398139363821485,
        longitude_deg=8.5453846156597137,
        relative_altitude_m=10,
        speed_m_s=5,
        is_fly_through=True,
        gimbal_pitch_deg=-45,
        gimbal_yaw_deg=0,
        camera_action=dc_mission.MissionItem.START_VIDEO))

    mission_items.append(dc_mission.MissionItem(
        latitude_deg=47.398058617228855,
        longitude_deg=8.5454618036746979,
        relative_altitude_m=10,
        speed_m_s=2,
        is_fly_through=False,
        gimbal_pitch_deg=-90,
        gimbal_yaw_deg=30,
        camera_action=dc_mission.MissionItem.STOP_VIDEO))

    mission_items.append(dc_mission.MissionItem(
        latitude_deg=47.398100366082858,
        longitude_deg=8.5456969141960144,
        relative_altitude_m=10,
        speed_m_s=5,
        is_fly_through=False,
        gimbal_pitch_deg=-45,
        gimbal_yaw_deg=-30,
        camera_action=dc_mission.MissionItem.START_PHOTO_INTERVAL))

    mission_items.append(dc_mission.MissionItem(
        latitude_deg=47.398001890458097,
        longitude_deg=8.5455576181411743,
        relative_altitude_m=10,
        speed_m_s=5,
        is_fly_through=False,
        gimbal_pitch_deg=0,
        gimbal_yaw_deg=0,
        camera_action=dc_mission.MissionItem.STOP_PHOTO_INTERVAL))

    device_mission=dc_mission.Mission()
    device_mission.uuid=device_uuid.uuid
    device_mission.mission_items.extend(mission_items)
    mission_stub.SendMission(device_mission)
    time.sleep(1)

    action_stub.Arm(device_uuid)
    time.sleep(1)

    future_status = mission_stub.StartMission.future(device_uuid)
    t = threading.Thread(target=wait_func, args=(future_status,))
    t.start()

    while(thread_status):
        print("Waiting for thread to exit")
        time.sleep(5)


if __name__ == '__main__':
    run()
