#pragma once

#include "plugins/action_server/action_server.h"
#include "plugin_impl_base.h"

namespace mavsdk {

class ActionServerImpl : public PluginImplBase {
public:
    explicit ActionServerImpl(System& system);
    explicit ActionServerImpl(std::shared_ptr<System> system);
    ~ActionServerImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;



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





    ActionServer::Result set_allowable_flight_modes(ActionServer::AllowableFlightModes flight_modes);





    ActionServer::AllowableFlightModes get_allowable_flight_modes();



private:
    ActionServer::ArmDisarmCallback _arm_disarm_callback;
    ActionServer::FlightModeChangeCallback _flight_mode_change_callback;
    std::mutex _callback_mutex;

    std::atomic<bool> _armable = false;
    std::atomic<bool> _force_armable = false;
    std::atomic<bool> _disarmable = false;
    std::atomic<bool> _force_disarmable = false;

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
};

} // namespace mavsdk