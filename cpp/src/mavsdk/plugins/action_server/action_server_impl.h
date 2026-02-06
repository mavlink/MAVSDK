#pragma once

#include <atomic>

#include "plugins/action_server/action_server.h"
#include "server_plugin_impl_base.h"
#include "callback_list.h"

namespace mavsdk {

class ActionServerImpl : public ServerPluginImplBase {
public:
    explicit ActionServerImpl(std::shared_ptr<ServerComponent> server_component);
    ~ActionServerImpl() override;

    void init() override;
    void deinit() override;

    ActionServer::ArmDisarmHandle
    subscribe_arm_disarm(const ActionServer::ArmDisarmCallback& callback);
    void unsubscribe_arm_disarm(ActionServer::ArmDisarmHandle handle);

    ActionServer::FlightModeChangeHandle
    subscribe_flight_mode_change(const ActionServer::FlightModeChangeCallback& callback);
    void unsubscribe_flight_mode_change(ActionServer::FlightModeChangeHandle handle);

    ActionServer::TakeoffHandle subscribe_takeoff(const ActionServer::TakeoffCallback& callback);
    void unsubscribe_takeoff(ActionServer::TakeoffHandle handle);

    ActionServer::LandHandle subscribe_land(const ActionServer::LandCallback& callback);
    void unsubscribe_land(ActionServer::LandHandle handle);

    ActionServer::RebootHandle subscribe_reboot(const ActionServer::RebootCallback& callback);
    void unsubscribe_reboot(ActionServer::RebootHandle handle);

    ActionServer::ShutdownHandle subscribe_shutdown(const ActionServer::ShutdownCallback& callback);
    void unsubscribe_shutdown(ActionServer::ShutdownHandle handle);

    ActionServer::TerminateHandle
    subscribe_terminate(const ActionServer::TerminateCallback& callback);
    void unsubscribe_terminate(ActionServer::TerminateHandle handle);

    ActionServer::Result set_allow_takeoff(bool allow_takeoff);

    ActionServer::Result set_armable(bool armable, bool force_armable);

    ActionServer::Result set_disarmable(bool disarmable, bool force_disarmable);

    ActionServer::Result
    set_allowable_flight_modes(ActionServer::AllowableFlightModes flight_modes);

    ActionServer::AllowableFlightModes get_allowable_flight_modes();

    ActionServer::Result set_armed_state(bool armed);

    ActionServer::Result set_flight_mode(ActionServer::FlightMode flight_mode);

    ActionServer::Result set_flight_mode_internal(ActionServer::FlightMode flight_mode);

private:
    static ActionServer::FlightMode telemetry_flight_mode_from_flight_mode(FlightMode flight_mode);
    static uint32_t to_px4_mode_from_flight_mode(ActionServer::FlightMode flight_mode);

    void set_base_mode(uint8_t base_mode);
    uint8_t get_base_mode() const;

    void set_custom_mode(uint32_t custom_mode);
    uint32_t get_custom_mode() const;

    void set_server_armed(bool armed);

    std::mutex _callback_mutex;
    CallbackList<ActionServer::Result, ActionServer::ArmDisarm> _arm_disarm_callbacks{};
    CallbackList<ActionServer::Result, ActionServer::FlightMode> _flight_mode_change_callbacks{};
    CallbackList<ActionServer::Result, bool> _takeoff_callbacks{};
    CallbackList<ActionServer::Result, bool> _land_callbacks{};
    CallbackList<ActionServer::Result, bool> _reboot_callbacks{};
    CallbackList<ActionServer::Result, bool> _shutdown_callbacks{};
    CallbackList<ActionServer::Result, bool> _terminate_callbacks{};

    std::atomic<bool> _armable = false;
    std::atomic<bool> _force_armable = false;
    std::atomic<bool> _disarmable = false;
    std::atomic<bool> _force_disarmable = false;
    std::atomic<bool> _allow_takeoff = false;

    std::mutex _flight_mode_mutex;
    ActionServer::AllowableFlightModes _allowed_flight_modes{};
    std::atomic<ActionServer::FlightMode> _flight_mode{ActionServer::FlightMode::Unknown};

    CallEveryHandler::Cookie _send_version_cookie{};
};

} // namespace mavsdk
