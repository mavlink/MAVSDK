#pragma once

#include "plugins/mocap/mocap.h"
#include "mavlink_include.h"
#include "plugin_impl_base.h"
#include "system.h"

namespace mavsdk {

class System;

class MocapImpl : public PluginImplBase {
public:
    explicit MocapImpl(System& system);
    explicit MocapImpl(std::shared_ptr<System> system);
    ~MocapImpl() override;

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
    Mocap::Result
    send_vision_position_estimate(const Mocap::VisionPositionEstimate& vision_position_estimate);
    Mocap::Result
    send_attitude_position_mocap(const Mocap::AttitudePositionMocap& attitude_position_mocap);
    Mocap::Result send_odometry(const Mocap::Odometry& odometry);
};
} // namespace mavsdk
