#include "backend.h"

#include <memory>

#include "connection_initiator.h"
#include "dronecore.h"
#include "grpc_server.h"

namespace dronecore {
namespace backend {

class DroneCoreBackend::Impl
{
public:
    Impl() {}
    ~Impl() {}

    void connect(const int mavlink_listen_port)
    {
        _connection_initiator.start(_dc, 14540);
        _connection_initiator.wait();
    }

    void startGRPCServer()
    {
        _server = std::unique_ptr<GRPCServer>(new GRPCServer(_dc));
        _server->run();
    }

    void wait()
    {
        _server->wait();
    }

private:
    DroneCore _dc;
    ConnectionInitiator<dronecore::DroneCore> _connection_initiator;
    std::unique_ptr<GRPCServer> _server;
};

DroneCoreBackend::DroneCoreBackend() : _impl(new Impl()) {}
DroneCoreBackend::~DroneCoreBackend() = default;

void DroneCoreBackend::startGRPCServer() { _impl->startGRPCServer(); }
void DroneCoreBackend::connect(const int mavlink_listen_port) { return _impl->connect(mavlink_listen_port); }
void DroneCoreBackend::wait() { _impl->wait(); }

} // namespace backend
} // namespace dronecore
