#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WINDOWS
#define DLLExport __declspec(dllexport)
#else
#define DLLExport __attribute__((visibility("default")))
#endif

DLLExport struct MavsdkBackend* runBackend(
    const char* system_address,
    const int mavsdk_server_port,
    void (*onServerStarted)(void*),
    void* context);

DLLExport int getPort(struct MavsdkBackend* backend);

DLLExport void attach(struct MavsdkBackend* backend);

DLLExport void stopBackend(struct MavsdkBackend* backend);

#ifdef __cplusplus
}
#endif
