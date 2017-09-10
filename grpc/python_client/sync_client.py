#!/usr/bin/env python

from __future__ import print_function
import grpc
import time
import dronecore_pb2 as dc
import dronecore_pb2_grpc


def run():
    channel = grpc.insecure_channel('0.0.0.0:50051')
    stub = dronecore_pb2_grpc.DroneCoreRPCStub(channel)

    arm_result = stub.Arm(dc.Empty())
    if arm_result.result == dc.ActionResult.SUCCESS:
        print("arming ok")
    else:
        print("arming failed: " + arm_result.result_str)

    time.sleep(2)

    takeoff_result = stub.TakeOff(dc.Empty())
    if takeoff_result.result == dc.ActionResult.SUCCESS:
        print("takeoff ok")
    else:
        print("takeoff failed: " + takeoff_result.result_str)

    time.sleep(5)

    land_result = stub.Land(dc.Empty())
    if land_result.result == dc.ActionResult.SUCCESS:
        print("landing ok")
    else:
        print("landing failed: " + land_result.result_str)


if __name__ == '__main__':
    run()
