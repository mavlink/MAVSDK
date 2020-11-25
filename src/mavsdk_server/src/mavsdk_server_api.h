#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WINDOWS
#define DLLExport __declspec(dllexport)
#else
#define DLLExport __attribute__((visibility("default")))
#endif

DLLExport struct MavsdkServer*
mavsdk_server_run(const char* system_address, const int mavsdk_server_port);

DLLExport int mavsdk_server_get_port(struct MavsdkServer* mavsdk_server);

DLLExport void mavsdk_server_attach(struct MavsdkServer* mavsdk_server);

DLLExport void mavsdk_server_stop(struct MavsdkServer* mavsdk_server);

#ifdef __cplusplus
}
#endif
