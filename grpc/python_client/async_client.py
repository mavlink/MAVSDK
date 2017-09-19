#!/usr/bin/env python

from __future__ import print_function
import grpc
import time
import threading
import dronecore_pb2 as dc
import dronecore_pb2_grpc


class Colors:
    BLUE = "\033[34m"
    RESET = "\033[0m"


def wait_until(status):
    ret = status.result()
    return ret


def print_altitude(stub, stop):
    for i, position in enumerate(stub.TelemetryPositionSubscription(dc.Empty())):
        # Position updates with SITL are too fast, we skip 9/10.
        if i % 10 == 0:
            print(Colors.BLUE, end="")
            print("Got relative altitude: {:.2f} m".
                format(position.relative_altitude_m))
            print(Colors.RESET, end="")
        if stop.is_set():
            break


def run():
    channel = grpc.insecure_channel('0.0.0.0:50051')
    stub = dronecore_pb2_grpc.DroneCoreRPCStub(channel)

    # We use this stop event later to stop the thread.
    t_stop = threading.Event()
    t = threading.Thread(target=print_altitude, args=(stub, t_stop))
    t.start()

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

    time.sleep(3)

    # Stop the thread and wait for it to finish.
    t_stop.set()
    t.join()


if __name__ == '__main__':
    run()
