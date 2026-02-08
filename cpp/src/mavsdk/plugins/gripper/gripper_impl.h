#pragma once

#include "plugins/gripper/gripper.h"

#include "plugin_impl_base.h"

namespace mavsdk {

class GripperImpl : public PluginImplBase {
public:
    explicit GripperImpl(System& system);
    explicit GripperImpl(std::shared_ptr<System> system);

    ~GripperImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    void grab_async(uint32_t instance, const Gripper::ResultCallback callback);
    Gripper::Result grab(uint32_t instance);

    void release_async(uint32_t instance, const Gripper::ResultCallback callback);
    Gripper::Result release(uint32_t instance);

private:
    static Gripper::Result gripper_result_from_command_result(MavlinkCommandSender::Result result);

    void command_result_callback(
        MavlinkCommandSender::Result command_result, const Gripper::ResultCallback& callback) const;
};

} // namespace mavsdk
