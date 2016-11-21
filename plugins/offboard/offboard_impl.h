#pragma once

#include "plugin_impl_base.h"
#include "mavlink_include.h"
#include "device_impl.h"
#include "offboard.h"

namespace dronelink {

class OffboardImpl : public PluginImplBase
{
public:
    OffboardImpl();
    ~OffboardImpl();

    void init() override;
    void deinit() override;

    Offboard::Result start() const;
    Offboard::Result stop() const;

    void start_async(Offboard::result_callback_t callback);
    void stop_async(Offboard::result_callback_t callback);

    void set_velocity_ned(Offboard::VelocityNEDYaw velocity_ned_yaw);
    void set_velocity_body(Offboard::VelocityBodyYawspeed velocity_body_yawspeed);

private:
    void process_heartbeat(const mavlink_message_t &message);
    void receive_command_result(DeviceImpl::CommandResult result,
                                const Offboard::result_callback_t &callback);

    static void report_offboard_result(const Offboard::result_callback_t &callback,
                                       Offboard::Result result);

    static Offboard::Result offboard_result_from_command_result(
        DeviceImpl::CommandResult result);


    void timeout_happened();

    bool _offboard_mode_active;
    Offboard::result_callback_t _result_callback;
};

} // namespace dronelink
