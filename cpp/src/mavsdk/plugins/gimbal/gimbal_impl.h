#pragma once

#include <optional>
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

    Gimbal::Result set_angles(
        int32_t gimbal_id,
        float roll_deg,
        float pitch_deg,
        float yaw_deg,
        Gimbal::GimbalMode gimbal_mode,
        Gimbal::SendMode send_mode);
    void set_angles_async(
        int32_t gimbal_id,
        float roll_deg,
        float pitch_deg,
        float yaw_deg,
        Gimbal::GimbalMode gimbal_mode,
        Gimbal::SendMode send_mode,
        const Gimbal::ResultCallback& callback);

    Gimbal::Result set_angular_rates(
        int32_t gimbal_id,
        float roll_rate_deg,
        float pitch_rate_deg,
        float yaw_rate_deg,
        Gimbal::GimbalMode gimbal_mode,
        Gimbal::SendMode send_mode);
    void set_angular_rates_async(
        int32_t gimbal_id,
        float roll_rate_deg,
        float pitch_rate_deg,
        float yaw_rate_deg,
        Gimbal::GimbalMode gimbal_mode,
        Gimbal::SendMode send_mode,
        const Gimbal::ResultCallback& callback);

    Gimbal::Result set_roi_location(
        int32_t gimbal_id, double latitude_deg, double longitude_deg, float altitude_m);
    void set_roi_location_async(
        int32_t gimbal_id,
        double latitude_deg,
        double longitude_deg,
        float altitude_m,
        const Gimbal::ResultCallback& callback);

    Gimbal::Result take_control(int32_t gimbal_id, Gimbal::ControlMode control_mode);
    void take_control_async(
        int32_t gimbal_id,
        Gimbal::ControlMode control_mode,
        const Gimbal::ResultCallback& callback);

    Gimbal::Result release_control(int32_t gimbal_id);
    void release_control_async(int32_t gimbal_id, const Gimbal::ResultCallback& callback);

    Gimbal::GimbalListHandle subscribe_gimbal_list(const Gimbal::GimbalListCallback& callback);
    void unsubscribe_gimbal_list(Gimbal::GimbalListHandle handle);
    Gimbal::GimbalList gimbal_list();

    Gimbal::ControlStatusHandle
    subscribe_control_status(const Gimbal::ControlStatusCallback& callback);
    void unsubscribe_control_status(Gimbal::ControlStatusHandle handle);
    std::pair<Gimbal::Result, Gimbal::ControlStatus> get_control_status(int32_t gimbal_id);

    Gimbal::AttitudeHandle subscribe_attitude(const Gimbal::AttitudeCallback& callback);
    void unsubscribe_attitude(Gimbal::AttitudeHandle handle);
    std::pair<Gimbal::Result, Gimbal::Attitude> get_attitude(int32_t gimbal_id);

    static Gimbal::Result
    gimbal_result_from_command_result(MavlinkCommandSender::Result command_result);

    static void receive_command_result(
        MavlinkCommandSender::Result command_result, const Gimbal::ResultCallback& callback);

    // Non-copyable
    GimbalImpl(const GimbalImpl&) = delete;
    const GimbalImpl& operator=(const GimbalImpl&) = delete;

private:
    struct GimbalItem {
        Gimbal::ControlStatus control_status{0, Gimbal::ControlMode::None, 0, 0, 0, 0};
        Gimbal::Attitude attitude{};
        std::string vendor_name;
        std::string model_name;
        std::string custom_name;
        unsigned gimbal_manager_information_requests_left{5};
        unsigned gimbal_device_information_requests_left{5};
        bool gimbal_manager_information_received{false};
        bool gimbal_device_information_received{false};
        uint8_t gimbal_manager_compid{0};
        uint8_t gimbal_device_id{0};
        bool is_valid{false};
    };

    struct GimbalAddress {
        uint8_t gimbal_manager_compid{0};
        uint8_t gimbal_device_id{0};
    };

    void request_gimbal_manager_information(uint8_t target_component_id) const;
    void request_gimbal_device_information(uint8_t target_component_id) const;

    void process_heartbeat(const mavlink_message_t& message);
    void process_gimbal_manager_information(const mavlink_message_t& message);
    void process_gimbal_manager_status(const mavlink_message_t& message);
    void process_gimbal_device_information(const mavlink_message_t& message);
    void process_gimbal_device_attitude_status(const mavlink_message_t& message);
    void process_attitude(const mavlink_message_t& message);

    void check_is_gimbal_valid(GimbalItem* gimbal_item);

    void set_angles_async_internal(
        int32_t gimbal_id,
        float roll_deg,
        float pitch_deg,
        float yaw_deg,
        Gimbal::GimbalMode gimbal_mode,
        Gimbal::SendMode send_mode,
        const Gimbal::ResultCallback& callback);

    void set_angular_rates_async_internal(
        int32_t gimbal_id,
        float roll_rate_deg,
        float pitch_rate_deg,
        float yaw_rate_deg,
        Gimbal::GimbalMode gimbal_mode,
        Gimbal::SendMode send_mode,
        const Gimbal::ResultCallback& callback);

    Gimbal::GimbalList gimbal_list_with_lock();
    std::optional<GimbalAddress> maybe_address_for_gimbal_id(int32_t gimbal_id) const;
    GimbalItem* maybe_gimbal_item_for_gimbal_id(int32_t gimbal_id);

    std::mutex _mutex{};
    CallbackList<Gimbal::GimbalList> _gimbal_list_subscriptions{};
    CallbackList<Gimbal::ControlStatus> _control_status_subscriptions{};
    CallbackList<Gimbal::Attitude> _attitude_subscriptions{};

    std::vector<GimbalItem> _gimbals;
    float _vehicle_yaw_rad{NAN};

    bool _debugging{false};
};

} // namespace mavsdk
