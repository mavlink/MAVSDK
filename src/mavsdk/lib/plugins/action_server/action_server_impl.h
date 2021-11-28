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
};

} // namespace mavsdk