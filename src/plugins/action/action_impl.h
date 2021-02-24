#pragma once

#include "plugins/action/action.h"
#include "plugin_impl_base.h"

namespace mavsdk {

class ActionImpl : public PluginImplBase {
public:
    explicit ActionImpl(System& system);
    explicit ActionImpl(std::shared_ptr<System> system);
    explicit ActionImpl(SystemImpl* system_impl);
    ~ActionImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;



    void arm_async(const Action::ResultCallback callback);



    Action::Result arm();



    void disarm_async(const Action::ResultCallback callback);



    Action::Result disarm();



    void takeoff_async(const Action::ResultCallback callback);



    Action::Result takeoff();



    void land_async(const Action::ResultCallback callback);



    Action::Result land();



    void reboot_async(const Action::ResultCallback callback);



    Action::Result reboot();



    void shutdown_async(const Action::ResultCallback callback);



    Action::Result shutdown();



    void terminate_async(const Action::ResultCallback callback);



    Action::Result terminate();



    void kill_async(const Action::ResultCallback callback);



    Action::Result kill();



    void return_to_launch_async(const Action::ResultCallback callback);



    Action::Result return_to_launch();



    void goto_location_async(double latitude_deg, double longitude_deg, float absolute_altitude_m, float yaw_deg, const Action::ResultCallback callback);



    Action::Result goto_location(double latitude_deg, double longitude_deg, float absolute_altitude_m, float yaw_deg);



    void do_orbit_async(float radius_m, float velocity_ms, Action::OrbitYawBehavior yaw_behavior, double latitude_deg, double longitude_deg, double absolute_altitude_m, const Action::ResultCallback callback);



    Action::Result do_orbit(float radius_m, float velocity_ms, Action::OrbitYawBehavior yaw_behavior, double latitude_deg, double longitude_deg, double absolute_altitude_m);



    void transition_to_fixedwing_async(const Action::ResultCallback callback);



    Action::Result transition_to_fixedwing();



    void transition_to_multicopter_async(const Action::ResultCallback callback);



    Action::Result transition_to_multicopter();



    void get_takeoff_altitude_async(const Action::GetTakeoffAltitudeCallback callback);



    std::pair<Action::Result, float> get_takeoff_altitude();



    void set_takeoff_altitude_async(float altitude, const Action::ResultCallback callback);



    Action::Result set_takeoff_altitude(float altitude);



    void get_maximum_speed_async(const Action::GetMaximumSpeedCallback callback);



    std::pair<Action::Result, float> get_maximum_speed();



    void set_maximum_speed_async(float speed, const Action::ResultCallback callback);



    Action::Result set_maximum_speed(float speed);



    void get_return_to_launch_altitude_async(const Action::GetReturnToLaunchAltitudeCallback callback);



    std::pair<Action::Result, float> get_return_to_launch_altitude();



    void set_return_to_launch_altitude_async(float relative_altitude_m, const Action::ResultCallback callback);



    Action::Result set_return_to_launch_altitude(float relative_altitude_m);



private:
};

} // namespace mavsdk