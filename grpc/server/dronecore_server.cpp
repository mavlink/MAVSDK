#include <iostream>
#include <thread>
#include <chrono>
#include <cstdint>
#include <future>

#include <grpc++/grpc++.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>
#include <grpc/support/log.h>

#include "dronecore.h"
#include "dronecore.grpc.pb.h"

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerCompletionQueue;
using grpc::Status;
using dronecorerpc::Empty;
using dronecorerpc::DroneCoreRPC;

using namespace dronecore;
using namespace std::placeholders;

// Async server implementation mostly taken from:
// https://github.com/grpc/grpc/blob/master/examples/cpp/helloworld/greeter_async_server.cc

class DroneCoreRPCImpl final
{

public:
    ~DroneCoreRPCImpl()
    {
        _server->Shutdown();
        // Always shutdown the completion queue after the server.
        _cq->Shutdown();
    }

    void Run()
    {
        std::string server_address("0.0.0.0:50051");

        ServerBuilder builder;
        // Listen on the given address without any authentication mechanism.
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        // Register "_service" as the instance through which we'll communicate with
        // clients. In this case it corresponds to an *asynchronous* service.
        builder.RegisterService(&_service);
        // Get hold of the completion queue used for the asynchronous communication
        // with the gRPC runtime.
        _cq = builder.AddCompletionQueue();
        // Finally assemble the server.
        _server = builder.BuildAndStart();
        std::cout << "Server listening on " << server_address << std::endl;

        // Proceed to the server's main loop.
        HandleRpcs();
    }

private:
    // Class encompasing the state and logic needed to serve a request.
    class CallData
    {
public:
    // Take in the "service" instance (in this case representing an asynchronous
    // server) and the completion queue "cq" used for asynchronous communication
    // with the gRPC runtime.
    CallData(DroneCoreRPC::AsyncService *service, ServerCompletionQueue *cq)
        : _service(service), _cq(cq), _responder(&_ctx), _status(CREATE)
    {
        // Invoke the serving logic right away.
        Proceed();
    }

    void Proceed()
    {
        if (_status == CREATE) {
            // Make this instance progress to the PROCESS state.
            _status = PROCESS;

            // As part of the initial CREATE state, we *request* that the system
            // start processing SayHello requests. In this request, "this" acts are
            // the tag uniquely identifying the request (so that different CallData
            // instances can serve different requests concurrently), in this case
            // the memory address of this CallData instance.
            _service->RequestArm(&_ctx, &_request, &_responder, _cq, _cq, this);
        } else if (_status == PROCESS) {
            // Spawn a new CallData instance to serve new clients while we process
            // the one for this CallData. The instance will deallocate itself as
            // part of its FINISH state.
            new CallData(_service, _cq);

            // The actual processing.
            //std::string prefix("Hello ");
            _reply.set_ok(7);

            // And we are done! Let the gRPC runtime know we've finished, using the
            // memory address of this instance as the uniquely identifying tag for
            // the event.
            _status = FINISH;
            _responder.Finish(_reply, Status::OK, this);
        } else {
            GPR_ASSERT(_status == FINISH);
            // Once in the FINISH state, deallocate ourselves (CallData).
            delete this;
        }
    }

private:
    // The means of communication with the gRPC runtime for an asynchronous
    // server.
    DroneCoreRPC::AsyncService *_service;
    // The producer-consumer queue where for asynchronous server notifications.
    ServerCompletionQueue *_cq;
    // Context for the rpc, allowing to tweak aspects of it such as the use
    // of compression, authentication, as well as to send metadata back to the
    // client.
    ServerContext _ctx;

    // What we get from the client.
    Empty _request;
    // What we send back to the client.
    dronecorerpc::ActionResult _reply;

    // The means to get back to the client.
    ServerAsyncResponseWriter<dronecorerpc::ActionResult> _responder;

    // Let's implement a tiny state machine with the following states.
    enum CallStatus { CREATE, PROCESS, FINISH };
    CallStatus _status;  // The current serving state.
};

// This can be run in multiple threads if needed.
void HandleRpcs()
{
        // Spawn a new CallData instance to serve new clients.
        new CallData(&_service, _cq.get());
        void *tag;  // uniquely identifies a request.
        bool ok;
        while (true) {
            // Block waiting to read the next event from the completion queue. The
            // event is uniquely identified by its tag, which in this case is the
            // memory address of a CallData instance.
            // The return value of Next should always be checked. This return value
            // tells us whether there is any kind of event or cq_ is shutting down.
            GPR_ASSERT(_cq->Next(&tag, &ok));
            GPR_ASSERT(ok);
            static_cast<CallData *>(tag)->Proceed();
        }
    }

    std::unique_ptr<ServerCompletionQueue> _cq;
    DroneCoreRPC::AsyncService _service;
    std::unique_ptr<Server> _server;

    //Status TelemetryPositionSubscription(ServerContext *context, const Empty *request,
    //                                     ServerWriter<dronecorerpc::TelemetryPosition> *writer) override
    //{
    //    dc.device().telemetry().position_async([&writer](Telemetry::Position position) {
    //        dronecorerpc::TelemetryPosition rpc_position;
    //        rpc_position.set_latitude_deg(position.latitude_deg);
    //        rpc_position.set_longitude_deg(position.longitude_deg);
    //        rpc_position.set_relative_altitude_m(position.relative_altitude_m);
    //        rpc_position.set_absolute_altitude_m(position.absolute_altitude_m);
    //        writer->Write(rpc_position);
    //    });
    //    // TODO: This is probably not the best idea.
    //    while (true) {
    //        std::this_thread::sleep_for(std::chrono::seconds(1));
    //    }
    //    return Status::OK;
    //}


    //Status Arm(ServerContext *context, const Empty *request,
    //           dronecorerpc::ActionResult *response) override
    //{
    //    const Action::Result action_result = dc.device().action().arm();
    //    response->set_result(static_cast<dronecorerpc::ActionResult::Result>(action_result));
    //    response->set_result_str(Action::result_str(action_result));
    //    return Status::OK;
    //}

    //Status TakeOff(ServerContext *context, const Empty *request,
    //               dronecorerpc::ActionResult *response) override
    //{
    //    const Action::Result action_result = dc.device().action().takeoff();
    //    response->set_result(static_cast<dronecorerpc::ActionResult::Result>(action_result));
    //    response->set_result_str(Action::result_str(action_result));
    //    return Status::OK;
    //}

    //Status Land(ServerContext *context, const Empty *request,
    //            dronecorerpc::ActionResult *response) override
    //{
    //    const Action::Result action_result = dc.device().action().land();
    //    response->set_result(static_cast<dronecorerpc::ActionResult::Result>(action_result));
    //    response->set_result_str(Action::result_str(action_result));
    //    return Status::OK;
    //}

    //Status SendMission(ServerContext *context, const dronecorerpc::Mission *mission,
    //                   dronecorerpc::MissionResult *response) override
    //{
    //    // TODO: there has to be a beter way than using std::future.
    //    auto prom = std::make_shared<std::promise<void>>();
    //    auto future_result = prom->get_future();

    //    std::vector<std::shared_ptr<MissionItem>> mission_items;
    //    for (auto &mission_item : mission->mission_items()) {
    //        auto new_item = std::make_shared<MissionItem>();
    //        new_item->set_position(mission_item.latitude(), mission_item.longitude());
    //        new_item->set_relative_altitude(mission_item.altitude());
    //        new_item->set_speed(mission_item.speed());
    //        new_item->set_fly_through(mission_item.is_fly_through());
    //        new_item->set_gimbal_pitch_and_yaw(mission_item.pitch(), mission_item.yaw());
    //        new_item->set_camera_action(static_cast<dronecore::MissionItem::CameraAction>
    //                                    (mission_item.camera_action()));
    //        mission_items.push_back(new_item);
    //    }

    //    dc.device().mission().send_mission_async(
    //    mission_items, [prom, response](Mission::Result result) {
    //        response->set_result(static_cast<dronecorerpc::MissionResult::Result>(result));
    //        response->set_result_str(Mission::result_str(result));
    //        prom->set_value();
    //    }
    //    );

    //    future_result.wait();
    //    future_result.get();
    //    return Status::OK;
    //}

    //Status StartMission(ServerContext *context, const Empty *request,
    //                    dronecorerpc::MissionResult *response) override
    //{
    //    // TODO: there has to be a beter way than using std::future.
    //    auto prom = std::make_shared<std::promise<void>>();
    //    auto future_result = prom->get_future();

    //    dc.device().mission().start_mission_async(
    //    [prom, response](Mission::Result result) {
    //        response->set_result(static_cast<dronecorerpc::MissionResult::Result>(result));
    //        response->set_result_str(Mission::result_str(result));
    //        prom->set_value();
    //    }
    //    );

    //    future_result.wait();
    //    future_result.get();
    //    return Status::OK;
    //}

    DroneCore dc;

};

//void RunServer()
//{
//    std::string server_address("0.0.0.0:50051");
//    DroneCoreRPCImpl service;
//
//    bool discovered_device = false;
//    DroneCore::ConnectionResult connection_result = service.dc.add_udp_connection();
//    if (connection_result != DroneCore::ConnectionResult::SUCCESS) {
//        std::cout << "Connection failed: " << DroneCore::connection_result_str(
//                      connection_result) << std::endl;
//        return;
//    }
//    std::cout << "Waiting to discover device..." << std::endl;
//    service.dc.register_on_discover([&discovered_device](uint64_t uuid) {
//        std::cout << "Discovered device with UUID: " << uuid << std::endl;
//        discovered_device = true;
//    });
//    // We usually receive heartbeats at 1Hz, therefore we should find a device after around 2 seconds.
//    std::this_thread::sleep_for(std::chrono::seconds(2));
//    if (!discovered_device) {
//        std::cout << "No device found, exiting." << std::endl;
//        return;
//    }
//
//    ServerBuilder builder;
//    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
//    builder.RegisterService(&service);
//    std::unique_ptr<Server> server(builder.BuildAndStart());
//    std::cout << "Server listening on " << server_address << std::endl;
//    server->Wait();
//}

int main(int argc, char **argv)
{
    DroneCoreRPCImpl server;
    server.Run();

    return 0;
}
