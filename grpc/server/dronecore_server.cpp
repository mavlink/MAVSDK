#include <dronecore/dronecore.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdint>

#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>
#include "dronecore.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using dronecorerpc::Bool;
using dronecorerpc::Empty;
using dronecorerpc::DroneCoreRPC;

using namespace dronecore;

class DroneCoreRPCImpl final : public DroneCoreRPC::Service {

  Status Arm(ServerContext* context, const Empty* request, Bool* response) override  {
  Device &device = dc.device();
  const Action::Result arm_result = device.action().arm();
  if (arm_result != Action::Result::SUCCESS) {
    std::cout << "Arming failed:" << Action::result_str(arm_result) << std::endl;
    response->set_return_(false);
  }
  response->set_return_(true);
  return Status::OK;
  }

  Status TakeOff(ServerContext* context, const Empty* request, Bool* response)override {
    Device &device = dc.device();
    const Action::Result takeoff_result = device.action().takeoff();
    if (takeoff_result != Action::Result::SUCCESS) {
        std::cout << "Takeoff failed:" << Action::result_str(takeoff_result) << std::endl;
        response->set_return_(false);
    }
  response->set_return_(true);
  return Status::OK;
  }

  Status Land(ServerContext* context, const Empty* request, Bool* response)override {
    Device &device = dc.device();
    const Action::Result land_result = device.action().land();
    if (land_result != Action::Result::SUCCESS) {
      std::cout << "Land failed:" << Action::result_str(land_result) << std::endl;
      response->set_return_(false);
    }
    response->set_return_(true);
    return Status::OK;
  }

public:
	DroneCore dc;
};

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  DroneCoreRPCImpl service;

  bool discovered_device = false;
  DroneCore::ConnectionResult connection_result = service.dc.add_udp_connection();
  if (connection_result != DroneCore::ConnectionResult::SUCCESS) {
      std::cout << "Connection failed: " << DroneCore::connection_result_str(
                    connection_result) << std::endl;
  return;
  }
  std::cout << "Waiting to discover device..." << std::endl;
  service.dc.register_on_discover([&discovered_device](uint64_t uuid) {
      std::cout << "Discovered device with UUID: " << uuid << std::endl;
      discovered_device = true;
  });
  // We usually receive heartbeats at 1Hz, therefore we should find a device after around 2 seconds.
  std::this_thread::sleep_for(std::chrono::seconds(2));
  if (!discovered_device) {
      std::cout << "No device found, exiting." << std::endl;
  return;
  }

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
}

int main(int argc, char** argv) {
  RunServer();
  return 0;
}
