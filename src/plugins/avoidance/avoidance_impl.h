#pragma once

#include "mavlink_include.h"
#include "plugins/avoidance/avoidance.h"
#include "plugin_impl_base.h"
#include "system.h"

namespace mavsdk {

class AvoidanceImpl : public PluginImplBase {
public:
    AvoidanceImpl(System& system);
    ~AvoidanceImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    void receive_trajectory_representation_waypoints_async(
        trajectory_representation_waypoints_callback_t callback);

    // Non-copyable
    AvoidanceImpl(const AvoidanceImpl&) = delete;
    const AvoidanceImpl& operator=(const AvoidanceImpl&) = delete;

private:
    void timeout_happened();
};

} // namespace mavsdk
