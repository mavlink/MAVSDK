#include "backend.h"

#include <memory>

#include "connection_initiator.h"
#include "mavsdk.h"
#include "grpc_server.h"

namespace mavsdk {
namespace backend {

class MavsdkBackend::Impl {
public:
    Impl() {}
    ~Impl() {}

    void connect(const std::string& connection_url)
    {
        _connection_initiator.start(_dc, connection_url);
        _connection_initiator.wait();
    }

    int startGRPCServer(const int port)
    {
        _server = std::unique_ptr<GRPCServer>(new GRPCServer(_dc));
        _server->set_port(port);
        return _server->run();
    }

    void wait() { _server->wait(); }

private:
    Mavsdk _dc;
    ConnectionInitiator<mavsdk::Mavsdk> _connection_initiator;
    std::unique_ptr<GRPCServer> _server;
};

MavsdkBackend::MavsdkBackend() : _impl(new Impl()) {}
MavsdkBackend::~MavsdkBackend() = default;

int MavsdkBackend::startGRPCServer(const int port)
{
    return _impl->startGRPCServer(port);
}
void MavsdkBackend::connect(const std::string& connection_url)
{
    return _impl->connect(connection_url);
}
void MavsdkBackend::wait()
{
    _impl->wait();
}

} // namespace backend
} // namespace mavsdk
