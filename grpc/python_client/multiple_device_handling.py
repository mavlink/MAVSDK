#!/usr/bin/env python

from __future__ import print_function
import grpc
import time
import action_pb2 as dc_action
import action_pb2_grpc
import dronecore_pb2 as dronecore
import dronecore_pb2_grpc
from google.protobuf import empty_pb2

def run():
    channel = grpc.insecure_channel('0.0.0.0:50051')
    action_stub = action_pb2_grpc.ActionRPCStub(channel)
    dronecore_stub = dronecore_pb2_grpc.DroneCoreRPCStub(channel)
    response=dronecore_stub.Get_UUID_List(empty_pb2.Empty())
    size=len(response.uuid_list)
    print("Devices registered : {}".format(size))
    print(response.uuid_list[0].uuid)

    for i in range(size):
        uuid_item=dronecore.UUID()
        uuid_item.uuid=response.uuid_list[i].uuid
        print(uuid_item.uuid)
        arm_result = action_stub.Arm(uuid_item)
        if arm_result.result == dc_action.ActionResult.SUCCESS:
            print("arming ok")
        else:
            print("arming failed: " + arm_result.result_str)

        time.sleep(2)

        takeoff_result = action_stub.TakeOff(uuid_item)
        if takeoff_result.result == dc_action.ActionResult.SUCCESS:
            print("takeoff ok")
        else:
            print("takeoff failed: " + takeoff_result.result_str)

        time.sleep(5)

        land_result = action_stub.Land(uuid_item)
        if land_result.result == dc_action.ActionResult.SUCCESS:
            print("landing ok")
        else:
            print("landing failed: " + land_result.result_str)


if __name__ == '__main__':
    run()
