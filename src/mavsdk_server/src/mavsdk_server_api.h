#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

#ifdef WINDOWS
#define DLLExport __declspec(dllexport)
#else
#define DLLExport __attribute__((visibility("default")))
#endif

DLLExport void mavsdk_server_init(struct MavsdkServer** mavsdk_server);

DLLExport int mavsdk_server_run(
    struct MavsdkServer* mavsdk_server, const char* system_address, const int mavsdk_server_port);

DLLExport int mavsdk_server_run_with_mavlink_ids(
    struct MavsdkServer* mavsdk_server,
    const char* system_address,
    const int mavsdk_server_port,
    const uint8_t system_id,
    const uint8_t component_id);

DLLExport int mavsdk_server_get_port(struct MavsdkServer* mavsdk_server);

DLLExport void mavsdk_server_attach(struct MavsdkServer* mavsdk_server);

DLLExport void mavsdk_server_stop(struct MavsdkServer* mavsdk_server);

DLLExport void mavsdk_server_destroy(struct MavsdkServer* mavsdk_server);

#ifdef __cplusplus
}
#endif
