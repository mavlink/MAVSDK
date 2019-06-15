#include "backend.h"

#include <memory>

#include "connection_initiator.h"
#include "mavsdk.h"
#include "grpc_server.h"

namespace mavsdk {
namespace backend {

class DronecodeSDKBackend::Impl {
public:
    Impl() {}
    ~Impl() {}

    void connect(const std::string &connection_url)
    {
        _connection_initiator.start(_dc, connection_url);
        _connection_initiator.wait();
    }

    void startGRPCServer()
    {
        _server = std::unique_ptr<GRPCServer>(new GRPCServer(_dc));
        _server->run();
    }

    void wait() { _server->wait(); }

private:
    DronecodeSDK _dc;
    ConnectionInitiator<mavsdk::DronecodeSDK> _connection_initiator;
    std::unique_ptr<GRPCServer> _server;
};

DronecodeSDKBackend::DronecodeSDKBackend() : _impl(new Impl()) {}
DronecodeSDKBackend::~DronecodeSDKBackend() = default;

void DronecodeSDKBackend::startGRPCServer()
{
    _impl->startGRPCServer();
}
void DronecodeSDKBackend::connect(const std::string &connection_url)
{
    return _impl->connect(connection_url);
}
void DronecodeSDKBackend::wait()
{
    _impl->wait();
}

} // namespace backend
} // namespace mavsdk
