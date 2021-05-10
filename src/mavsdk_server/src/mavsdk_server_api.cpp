#include "mavsdk_server_api.h"
#include "mavsdk_server.h"
#include <string>

MavsdkServer* mavsdk_server_run(const char* system_address, const int mavsdk_server_port)
{
    auto mavsdk_server = new MavsdkServer();

    auto grpc_port = mavsdk_server->startGrpcServer(mavsdk_server_port);
    if (grpc_port == 0) {
        // Server failed to start
        return nullptr;
    }

    mavsdk_server->connect(std::string(system_address));

    return mavsdk_server;
}

int mavsdk_server_get_port(MavsdkServer* mavsdk_server)
{
    return mavsdk_server->getPort();
}

void mavsdk_server_attach(MavsdkServer* mavsdk_server)
{
    mavsdk_server->wait();
}

void mavsdk_server_stop(MavsdkServer* mavsdk_server)
{
    mavsdk_server->stop();
    delete mavsdk_server;
}
