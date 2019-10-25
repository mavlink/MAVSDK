#pragma once

#include <atomic>
#include <mutex>

#include "plugins/mocap/mocap.h"
#include "mavlink_include.h"
#include "plugin_impl_base.h"
#include "system.h"

namespace mavsdk {

class System;

class MocapImpl : public PluginImplBase {
public:
    MocapImpl(System& system);
    ~MocapImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Mocap::Result
    set_vision_position_estimate(const Mocap::VisionPositionEstimate& vision_position_estimate);
    Mocap::Result
    set_attitude_position_mocap(const Mocap::AttitudePositionMocap& attitude_position_mocap);
    Mocap::Result set_odometry(const Mocap::Odometry& odometry);

    MocapImpl(const MocapImpl&) = delete;
    MocapImpl& operator=(const MocapImpl&) = delete;

private:
    bool send_vision_position_estimate();
    bool send_attitude_position_mocap();
    bool send_odometry();

    mutable std::mutex _visual_position_estimate_mutex{};
    Mocap::VisionPositionEstimate _vision_position_estimate{};

    mutable std::mutex _attitude_position_mocap_mutex{};
    Mocap::AttitudePositionMocap _attitude_position_mocap{};

    mutable std::mutex _odometry_mutex{};
    Mocap::Odometry _odometry{};
};
} // namespace mavsdk
