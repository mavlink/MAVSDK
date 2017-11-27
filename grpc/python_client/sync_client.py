#!/usr/bin/env python

from __future__ import print_function
import grpc
import time
# import dronecore_pb2 as dc
# import dronecore_pb2_grpc
import action_pb2 as dc_action
import action_pb2_grpc
from google.protobuf import empty_pb2


def run():
    channel = grpc.insecure_channel('0.0.0.0:50051')
    action_stub = action_pb2_grpc.ActionRPCStub(channel)

    arm_result = action_stub.Arm(empty_pb2.Empty())
    if arm_result.result == dc_action.ActionResult.SUCCESS:
        print("arming ok")
    else:
        print("arming failed: " + arm_result.result_str)

    time.sleep(2)

    takeoff_result = action_stub.TakeOff(empty_pb2.Empty())
    if takeoff_result.result == dc_action.ActionResult.SUCCESS:
        print("takeoff ok")
    else:
        print("takeoff failed: " + takeoff_result.result_str)

    time.sleep(5)

    land_result = action_stub.Land(empty_pb2.Empty())
    if land_result.result == dc_action.ActionResult.SUCCESS:
        print("landing ok")
    else:
        print("landing failed: " + land_result.result_str)


if __name__ == '__main__':
    run()
