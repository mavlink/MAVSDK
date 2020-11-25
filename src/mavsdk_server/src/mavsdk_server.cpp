#include "mavsdk_server.h"

#include <memory>

#include "connection_initiator.h"
#include "mavsdk.h"
#include "grpc_server.h"

using namespace mavsdk::mavsdk_server;

class MavsdkServer::Impl {
public:
    Impl() {}
    ~Impl() {}

    void connect(const std::string& connection_url)
    {
        _connection_initiator.start(_dc, connection_url);
    }

    int startGRPCServer(const int port)
    {
        _server = std::make_unique<GRPCServer>(_dc);
        _server->set_port(port);
        _grpc_port = _server->run();
        return _grpc_port;
    }

    void wait() { _server->wait(); }

    void stop() { _server->stop(); }

    int getPort() { return _grpc_port; }

private:
    mavsdk::Mavsdk _dc;
    ConnectionInitiator<mavsdk::Mavsdk> _connection_initiator;
    std::unique_ptr<GRPCServer> _server;
    int _grpc_port;
};

MavsdkServer::MavsdkServer() : _impl(new Impl()) {}
MavsdkServer::~MavsdkServer() = default;

int MavsdkServer::startGRPCServer(const int port)
{
    return _impl->startGRPCServer(port);
}
void MavsdkServer::connect(const std::string& connection_url)
{
    return _impl->connect(connection_url);
}
void MavsdkServer::wait()
{
    _impl->wait();
}
void MavsdkServer::stop()
{
    _impl->stop();
}

int MavsdkServer::getPort()
{
    return _impl->getPort();
}
