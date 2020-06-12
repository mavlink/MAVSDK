#include "backend_api.h"
#include "backend.h"
#include <string>

MavsdkBackend* mavsdk_server_run(const char* system_address, const int mavsdk_server_port)
{
    auto backend = new MavsdkBackend();

    auto grpc_port = backend->startGRPCServer(mavsdk_server_port);
    if (grpc_port == 0) {
        // Server failed to start
        return nullptr;
    }

    backend->connect(std::string(system_address));

    return backend;
}

int mavsdk_server_get_port(MavsdkBackend* backend)
{
    return backend->getPort();
}

void mavsdk_server_attach(MavsdkBackend* backend)
{
    backend->wait();
}

void mavsdk_server_stop(MavsdkBackend* backend)
{
    backend->stop();
    delete backend;
}
