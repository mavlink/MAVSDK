#pragma once

#include "plugins/manual_control/manual_control.h"
#include "plugin_impl_base.h"

namespace mavsdk {

class ManualControlImpl : public PluginImplBase {
public:
    explicit ManualControlImpl(System& system);
    explicit ManualControlImpl(std::shared_ptr<System> system);
    ~ManualControlImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    void start_position_control_async(const ManualControl::ResultCallback callback);

    ManualControl::Result start_position_control();

    void start_altitude_control_async(const ManualControl::ResultCallback callback);

    ManualControl::Result start_altitude_control();

    ManualControl::Result set_manual_control_input(float x, float y, float z, float r);

private:
    ManualControl::Result
    manual_control_result_from_command_result(MavlinkCommandSender::Result result);
    void command_result_callback(
        MavlinkCommandSender::Result command_result, const ManualControl::ResultCallback& callback);

    enum class Input { NotSet, Set } _input{Input::NotSet};
};

} // namespace mavsdk
