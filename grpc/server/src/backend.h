#include <grpc++/server.h>
#include <memory>
#include <mutex>

#include "connection_initiator.h"
#include "dronecore.h"

namespace dronecore {
namespace backend {

class DroneCoreBackend
{
public:
    DroneCoreBackend() {}
    ~DroneCoreBackend() {}

    bool run(const int mavlink_listen_port = 14540);

private:
    bool run_server();
    void setup_port(grpc::ServerBuilder &builder);

    dronecore::DroneCore _dc;
    dronecore::backend::ConnectionInitiator<dronecore::DroneCore> _connection_initiator;
    std::unique_ptr<grpc::Server> _server;
};

} // namespace backend
} // namespace dronecore
