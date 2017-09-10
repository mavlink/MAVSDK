#!/usr/bin/env python


from __future__ import print_function
import grpc
from threading import Thread
import time
import dronecore_pb2 as dc
import dronecore_pb2_grpc


thread_status = True


def wait_func(future_status):
    global thread_status
    ret = future_status.result()
    print(ret.ret)
    thread_status = False


def run():
    global thread_status
    channel = grpc.insecure_channel('0.0.0.0:50051')
    stub = dronecore_pb2_grpc.DroneCoreRPCStub(channel)

    mission_items = []
    mission_items.append(dc.MissionItem(latitude=47.398170327054473,
                                        longitude=8.5456490218639658,
                                        altitude=10,
                                        speed=2,
                                        is_fly_through=True,
                                        pitch=0,
                                        yaw=-60,
                                        camera_action=5))
    mission_items.append(dc.MissionItem(latitude=47.398241338125118,
                                        longitude=8.5455360114574432,
                                        altitude=10,
                                        speed=2,
                                        is_fly_through=True,
                                        pitch=0,
                                        yaw=-60,
                                        camera_action=0))

    stub.AddMission(dc.Mission(mission_items=mission_items))


    time.sleep(2)
    future_status = stub.FlyMission.future(dc.Empty())
    t = Thread(target=wait_func, args=(future_status,))
    t.start()
    while(thread_status):
        print("Waiting for thread to exit")
        time.sleep(5)


if __name__ == '__main__':
    run()
