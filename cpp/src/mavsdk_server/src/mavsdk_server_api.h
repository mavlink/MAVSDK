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

struct MavsdkServer;

/*
 * Allocate and initialize MavsdkServer struct.
 *
 * @param mavsdk_server Pointer to pointer to MavsdkServer.
 */
DLLExport void mavsdk_server_init(struct MavsdkServer** mavsdk_server);

/*
 * Run MavsdkServer.
 *
 * @param mavsdk_server Pointer to initialized MavsdkServer
 * @param system_address Connection string for MAVLink as used by add_any_connection
 * @param mavsdk_server_port gRPC server port
 * @return 0 if successful
 */
DLLExport int mavsdk_server_run(
    struct MavsdkServer* mavsdk_server, const char* system_address, const int mavsdk_server_port);

/*
 * Run MavsdkServer.
 *
 * Note: This function immediately returns. To wait until the server exits,
 *       use mavsdk_server_attach.
 *
 * @param mavsdk_server Pointer to initialized MavsdkServer
 * @param system_address Connection string for MAVLink as used by add_any_connection
 * @param mavsdk_server_port gRPC server port
 * @param system_id MAVLink system ID for MAVSDK
 * @param component_id MAVLink component ID for MAVSDK
 * @return 0 if successful
 */
DLLExport int mavsdk_server_run_with_mavlink_ids(
    struct MavsdkServer* mavsdk_server,
    const char* system_address,
    const int mavsdk_server_port,
    const uint8_t system_id,
    const uint8_t component_id);

/*
 * Get gRPC port.
 *
 * @param mavsdk_server Pointer to initialized MavsdkServer
 * @return gRPC port used
 */
DLLExport int mavsdk_server_get_port(struct MavsdkServer* mavsdk_server);

/*
 * Attach to MavsdkServer. This only returns once the server is stopped.
 *
 * @param mavsdk_server Pointer to initialized MavsdkServer
 */
DLLExport void mavsdk_server_attach(struct MavsdkServer* mavsdk_server);

/*
 * Stop MavsdkServer.
 *
 * @param mavsdk_server Pointer to initialized MavsdkServer
 */
DLLExport void mavsdk_server_stop(struct MavsdkServer* mavsdk_server);

/*
 * Clean up MavsdkServer object. Counter part to mavsdk_server_init.
 *
 * @param mavsdk_server Pointer to initialized MavsdkServer
 */
DLLExport void mavsdk_server_destroy(struct MavsdkServer* mavsdk_server);

#ifdef __cplusplus
}
#endif
