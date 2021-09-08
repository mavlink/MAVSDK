#include "mavsdk_server_api.h"
#include "mavsdk_server.h"
#include <string>

void mavsdk_server_init(MavsdkServer** mavsdk_server)
{
    *mavsdk_server = new MavsdkServer();
}

int mavsdk_server_run(
    MavsdkServer* mavsdk_server, const char* system_address, const int mavsdk_server_port)
{
    if (!mavsdk_server->connect(std::string(system_address))) {
        // Connection was cancelled
        return false;
    }

    auto grpc_port = mavsdk_server->startGrpcServer(mavsdk_server_port);
    if (grpc_port == 0) {
        // Server failed to start
        return false;
    }

    return true;
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
}

void mavsdk_server_destroy(MavsdkServer* mavsdk_server)
{
    delete mavsdk_server;
}
