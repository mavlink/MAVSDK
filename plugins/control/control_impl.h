#pragma once

#include "plugin_impl_base.h"
#include "error_handling.h"
#include <cstdint>

namespace dronelink {

class ControlImpl : public PluginImplBase
{
public:
    ControlImpl();
    ~ControlImpl();

    void init() override;
    void deinit() override;

    Result arm() const;
    Result disarm() const;
    Result takeoff() const;
    Result land() const;
    Result return_to_land() const;

    void arm_async(result_callback_t callback);
    void disarm_async(result_callback_t callback);
    void takeoff_async(result_callback_t callback);
    void land_async(result_callback_t callback);
    void return_to_land_async(result_callback_t callback);

private:
    static constexpr uint8_t VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED = 1;

    // FIXME: these chould potentially change anytime
    static constexpr uint8_t PX4_CUSTOM_MAIN_MODE_AUTO = 4;
    static constexpr uint8_t PX4_CUSTOM_SUB_MODE_AUTO_RTL = 5;
};

} // namespace dronelink
