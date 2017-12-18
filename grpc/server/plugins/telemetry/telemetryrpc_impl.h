#include "telemetry.h"
#include "telemetry.grpc.pb.h"


using grpc::Status;
using grpc::ServerContext;
using dronecorerpc::TelemetryRPC;
using grpc::ServerWriter;
using dronecorerpc::UUID;

using namespace dronecore;

class TelemetryRPCImpl final : public TelemetryRPC::Service
{
public:
    TelemetryRPCImpl(DroneCore *dc_obj)
    {
        dc = dc_obj;
    }

    Status TelemetryPositionSubscription(ServerContext *context,
                                         const UUID *request,
                                         ServerWriter<dronecorerpc::TelemetryPosition> *writer) override
    {
        dc->device(request->uuid()).telemetry().position_async([&writer](Telemetry::Position position) {
            dronecorerpc::TelemetryPosition rpc_position;
            rpc_position.set_latitude_deg(position.latitude_deg);
            rpc_position.set_longitude_deg(position.longitude_deg);
            rpc_position.set_relative_altitude_m(position.relative_altitude_m);
            rpc_position.set_absolute_altitude_m(position.absolute_altitude_m);
            writer->Write(rpc_position);
        });
        // TODO: This is probably not the best idea.
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        return Status::OK;
    }

private:
    DroneCore *dc;
};
