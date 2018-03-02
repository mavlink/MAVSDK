#include "telemetry/telemetry.h"
#include "telemetry/telemetry.grpc.pb.h"

using grpc::Status;
using grpc::ServerContext;
using grpc::ServerWriter;

using namespace dronecore;

class TelemetryServiceImpl final : public rpc::telemetry::TelemetryService::Service
{
public:
    TelemetryServiceImpl(Telemetry &telemetry)
        : telemetry(telemetry) {}

    Status SubscribePosition(ServerContext *context,
                             const rpc::telemetry::SubscribePositionRequest *request,
                             ServerWriter<rpc::telemetry::Position> *writer) override
    {
        telemetry.position_async([&writer](
        Telemetry::Position position) {
            rpc::telemetry::Position rpc_position;
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
    Telemetry &telemetry;
};
