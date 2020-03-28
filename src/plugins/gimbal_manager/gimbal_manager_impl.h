#pragma once

#include "plugins/gimbal_manager/gimbal_manager.h"
#include "plugin_impl_base.h"
#include "system.h"

namespace mavsdk {

class GimbalManagerImpl : public PluginImplBase {
public:
    GimbalManagerImpl(System& system);
    ~GimbalManagerImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    void request_information();

    void subscribe_information_async(
            std::function<void(const GimbalManager::Information information)> callback);

    GimbalManager::Result set_attitude(float tilt_rate, float pan_rate,
            float tilt_deg, float pan_deg, int operation_flags,
            uint8_t id);

    void set_attitude_async(float tilt_rate, float pan_rate,
            float tilt_deg, float pan_deg, int operation_flags,
            uint8_t id, GimbalManager::result_callback_t callback);

    GimbalManager::Result track_point(float x, float y, uint8_t id);

    void track_point_async(float x, float y, uint8_t id, GimbalManager::result_callback_t callback);

    GimbalManager::Result track_rectangle(float x1, float y1, float x2, float y2, uint8_t id);

    void track_rectangle_async(float x1, float y1, float x2, float y2, uint8_t id, GimbalManager::result_callback_t callback);

    GimbalManager::Result set_roi_location(double latitude_deg, double longitude_deg, float altitude_m);

    void set_roi_location_async(double latitude_deg, double longitude_deg,
            float altitude_m, GimbalManager::result_callback_t callback);

    // Non-copyable
    GimbalManagerImpl(const GimbalManagerImpl&) = delete;
    const GimbalManagerImpl& operator=(const GimbalManagerImpl&) = delete;

private:
    static GimbalManager::Result gimbal_manager_result_from_command_result(MAVLinkCommands::Result command_result);

    static void receive_command_result(
        MAVLinkCommands::Result command_result, const GimbalManager::result_callback_t& callback);
};

} // namespace mavsdk
