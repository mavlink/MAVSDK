#include "backend_api.h"
#include "backend.h"
#include <string>

MavsdkBackend* runBackend(
    const char* system_address,
    const int mavsdk_server_port,
    void (*onServerStarted)(void*),
    void* context)
{
    auto backend = new MavsdkBackend();

    auto grpc_port = backend->startGRPCServer(mavsdk_server_port);
    if (grpc_port == 0) {
        // Server failed to start
        return nullptr;
    }

    backend->connect(std::string(system_address));

    if (onServerStarted != nullptr) {
        onServerStarted(context);
    }

    return backend;
}

int getPort(MavsdkBackend* backend)
{
    return backend->getPort();
}

void attach(MavsdkBackend* backend)
{
    backend->wait();
}

void stopBackend(MavsdkBackend* backend)
{
    backend->stop();
    delete backend;
}
