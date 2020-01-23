#include "backend_api.h"
#include "backend.h"
#include <string>

void runBackend(
    const char* connection_url,
    const int mavsdk_server_port,
    void (*onServerStarted)(void* context, int grpc_port),
    void* context)
{
    mavsdk::backend::MavsdkBackend backend;

    auto grpc_port = backend.startGRPCServer(mavsdk_server_port);
    if (grpc_port == 0) {
        // Server failed to start
        return;
    }

    backend.connect(std::string(connection_url));

    if (onServerStarted != nullptr) {
        onServerStarted(context, grpc_port);
    }

    backend.wait();
}
