#pragma once

#include "plugins/gimbal/gimbal.h"
#include "plugin_impl_base.h"
#include "system.h"
#include "callback_list.h"

namespace mavsdk {

class GimbalImpl : public PluginImplBase {
public:
    explicit GimbalImpl(System& system);
    explicit GimbalImpl(std::shared_ptr<System> system);
    ~GimbalImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Gimbal::Result set_angles(float roll_deg, float pitch_deg, float yaw_deg);
    void set_angles_async(
        float roll_deg, float pitch_deg, float yaw_deg, Gimbal::ResultCallback callback);

    Gimbal::Result set_pitch_and_yaw(float pitch_deg, float yaw_deg);
    void set_pitch_and_yaw_async(float pitch_deg, float yaw_deg, Gimbal::ResultCallback callback);

    Gimbal::Result set_pitch_rate_and_yaw_rate(float pitch_rate_deg_s, float yaw_rate_deg_s);

    void set_pitch_rate_and_yaw_rate_async(
        float pitch_rate_deg_s, float yaw_rate_deg_s, Gimbal::ResultCallback callback);

    Gimbal::Result set_mode(const Gimbal::GimbalMode gimbal_mode);
    void set_mode_async(const Gimbal::GimbalMode gimbal_mode, Gimbal::ResultCallback callback);

    Gimbal::Result set_roi_location(double latitude_deg, double longitude_deg, float altitude_m);
    void set_roi_location_async(
        double latitude_deg,
        double longitude_deg,
        float altitude_m,
        Gimbal::ResultCallback callback);

    Gimbal::Result take_control(Gimbal::ControlMode control_mode);
    void take_control_async(Gimbal::ControlMode control_mode, Gimbal::ResultCallback callback);

    Gimbal::Result release_control();
    void release_control_async(Gimbal::ResultCallback callback);

    Gimbal::ControlStatus control();
    Gimbal::ControlHandle subscribe_control(const Gimbal::ControlCallback& callback);
    void unsubscribe_control(Gimbal::ControlHandle handle);

    Gimbal::AttitudeHandle subscribe_attitude(const Gimbal::AttitudeCallback& callback);
    void unsubscribe_attitude(Gimbal::AttitudeHandle handle);
    Gimbal::Attitude attitude();

    static Gimbal::Result
    gimbal_result_from_command_result(MavlinkCommandSender::Result command_result);

    static void receive_command_result(
        MavlinkCommandSender::Result command_result, const Gimbal::ResultCallback& callback);

    // Non-copyable
    GimbalImpl(const GimbalImpl&) = delete;
    const GimbalImpl& operator=(const GimbalImpl&) = delete;

private:
    void request_gimbal_information();

    void process_gimbal_manager_information(const mavlink_message_t& message);
    void process_gimbal_manager_status(const mavlink_message_t& message);
    void process_gimbal_device_attitude_status(const mavlink_message_t& message);
    void process_attitude(const mavlink_message_t& message);

    std::mutex _mutex{};
    CallbackList<Gimbal::ControlStatus> _control_subscriptions{};
    CallbackList<Gimbal::Attitude> _attitude_subscriptions{};

    uint8_t _gimbal_device_id{0};
    uint8_t _gimbal_manager_sysid{0};
    uint8_t _gimbal_manager_compid{0};

    Gimbal::GimbalMode _gimbal_mode{Gimbal::GimbalMode::YawFollow};
    Gimbal::ControlStatus _control_status{Gimbal::ControlMode::None, 0, 0, 0, 0};
    Gimbal::Attitude _attitude{};
    float _vehicle_yaw_rad{NAN};
};

} // namespace mavsdk
