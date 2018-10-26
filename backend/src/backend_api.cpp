#include "backend_api.h"

#include "backend.h"

void runBackend(const int mavlink_listen_port, void (*onServerStarted)(void *), void *context)
{
    dronecode_sdk::backend::DronecodeSDKBackend backend;
    backend.startGRPCServer();
    backend.connect(mavlink_listen_port);

    if (onServerStarted != nullptr) {
        onServerStarted(context);
    }

    backend.wait();
}
