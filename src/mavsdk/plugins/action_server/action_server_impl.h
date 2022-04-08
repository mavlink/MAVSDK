#pragma once

#include <atomic>

#include "plugins/action_server/action_server.h"
#include "server_plugin_impl_base.h"

namespace mavsdk {

class ActionServerImpl : public ServerPluginImplBase {
public:
    explicit ActionServerImpl(std::shared_ptr<ServerComponent> server_component);
    ~ActionServerImpl() override;

    void init() override;
    void deinit() override;

    void subscribe_arm_disarm(ActionServer::ArmDisarmCallback callback);

    void subscribe_flight_mode_change(ActionServer::FlightModeChangeCallback callback);

    void subscribe_takeoff(ActionServer::TakeoffCallback callback);

    void subscribe_land(ActionServer::LandCallback callback);

    void subscribe_reboot(ActionServer::RebootCallback callback);

    void subscribe_shutdown(ActionServer::ShutdownCallback callback);

    void subscribe_terminate(ActionServer::TerminateCallback callback);

    ActionServer::Result set_allow_takeoff(bool allow_takeoff);

    ActionServer::Result set_armable(bool armable, bool force_armable);

    ActionServer::Result set_disarmable(bool disarmable, bool force_disarmable);

    ActionServer::Result
    set_allowable_flight_modes(ActionServer::AllowableFlightModes flight_modes);

    ActionServer::AllowableFlightModes get_allowable_flight_modes();

private:
    void set_custom_mode(uint32_t custom_mode);
    uint32_t get_custom_mode() const;

    void set_base_mode(uint8_t base_mode);
    uint8_t get_base_mode() const;

    // Used when acting as autopilot!
    void set_server_armed(bool armed);
    bool is_server_armed() const;

    void send_autopilot_version();
    void enable_sending_autopilot_version();

    ActionServer::ArmDisarmCallback _arm_disarm_callback{nullptr};
    ActionServer::FlightModeChangeCallback _flight_mode_change_callback{nullptr};
    ActionServer::TakeoffCallback _takeoff_callback{nullptr};

    std::mutex _callback_mutex;

    std::atomic<bool> _armable = false;
    std::atomic<bool> _force_armable = false;
    std::atomic<bool> _disarmable = false;
    std::atomic<bool> _force_disarmable = false;
    std::atomic<bool> _allow_takeoff = false;

    union px4_custom_mode {
        struct {
            uint16_t reserved;
            uint8_t main_mode;
            uint8_t sub_mode;
        };
        uint32_t data;
        float data_float;
    };

    std::mutex _flight_mode_mutex;
    ActionServer::AllowableFlightModes _allowed_flight_modes{};

    // std::atomic<bool> _should_send_autopilot_version{false};

    std::atomic<ActionServer::FlightMode> _flight_mode{ActionServer::FlightMode::Unknown};

    // std::mutex _autopilot_version_mutex{};
    // AutopilotVersion _autopilot_version{MAV_PROTOCOL_CAPABILITY_COMMAND_INT, 0, 0, 0, 0, 0, 0,
    // {0}};
};

} // namespace mavsdk
