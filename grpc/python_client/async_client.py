#!/usr/bin/env python

import grpc
import time
import dronecore_pb2 as dc
import dronecore_pb2_grpc

def wait_until(status):
    ret = status.result()
    return ret

def run():
    channel = grpc.insecure_channel('0.0.0.0:50051')
    stub = dronecore_pb2_grpc.DroneCoreRPCStub(channel)

    arm_result = stub.Arm.future(dc.Empty())
    arm_result = wait_until(arm_result)
    if arm_result.result == dc.ActionResult.SUCCESS:
        print("arming ok")
    else:
        print("arming failed: " + arm_result.result_str)

    time.sleep(2)

    takeoff_result = stub.TakeOff.future(dc.Empty())
    takeoff_result = wait_until(takeoff_result)
    if takeoff_result.result == dc.ActionResult.SUCCESS:
        print("takeoff ok")
    else:
        print("takeoff failed: " + takeoff_result.result_str)

    time.sleep(5)

    land_result = stub.Land.future(dc.Empty())
    land_result = wait_until(land_result)
    if land_result.result == dc.ActionResult.SUCCESS:
        print("landing ok")
    else:
        print("landing failed: " + land_result.result_str)


if __name__ == '__main__':
    run()
