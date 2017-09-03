#!/usr/bin/env python

import grpc
import time
import dronecore_pb2
import dronecore_pb2_grpc

def run():
  channel = grpc.insecure_channel('0.0.0.0:50051')
  stub = dronecore_pb2_grpc.DroneCoreRPCStub(channel)
  stub.Arm(dronecore_pb2.Empty())
  time.sleep(2)
  stub.TakeOff(dronecore_pb2.Empty())
  time.sleep(5)
  stub.Land(dronecore_pb2.Empty())

if __name__=='__main__':
  run()
