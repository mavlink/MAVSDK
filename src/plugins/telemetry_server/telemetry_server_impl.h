#pragma once

#include "plugins/telemetry_server/telemetry_server.h"
#include "plugin_impl_base.h"

#include <chrono>

namespace mavsdk {

class TelemetryServerImpl : public PluginImplBase {
public:
    explicit TelemetryServerImpl(System& system);
    explicit TelemetryServerImpl(std::shared_ptr<System> system);
    ~TelemetryServerImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    void publish_position_async(
        TelemetryServer::Position position,
        TelemetryServer::VelocityNed velocity_ned,
        const TelemetryServer::ResultCallback callback);

    TelemetryServer::Result
    publish_position(TelemetryServer::Position position, TelemetryServer::VelocityNed velocity_ned);

    void publish_home_async(
        TelemetryServer::Position home, const TelemetryServer::ResultCallback callback);

    TelemetryServer::Result publish_home(TelemetryServer::Position home);

    void publish_armed_async(bool is_armed, const TelemetryServer::ResultCallback callback);

    TelemetryServer::Result publish_armed(bool is_armed);

    void publish_raw_gps_async(
        TelemetryServer::RawGps raw_gps,
        TelemetryServer::GpsInfo gps_info,
        const TelemetryServer::ResultCallback callback);

    TelemetryServer::Result
    publish_raw_gps(TelemetryServer::RawGps raw_gps, TelemetryServer::GpsInfo gps_info);

    void publish_battery_async(
        TelemetryServer::Battery battery, const TelemetryServer::ResultCallback callback);

    TelemetryServer::Result publish_battery(TelemetryServer::Battery battery);

    void publish_flight_mode_async(
        TelemetryServer::FlightMode flight_mode, const TelemetryServer::ResultCallback callback);

    TelemetryServer::Result publish_flight_mode(TelemetryServer::FlightMode flight_mode);

    void publish_health_async(
        TelemetryServer::Health health, const TelemetryServer::ResultCallback callback);

    TelemetryServer::Result publish_health(TelemetryServer::Health health);

    void publish_status_text_async(
        TelemetryServer::StatusText status_text, const TelemetryServer::ResultCallback callback);

    TelemetryServer::Result publish_status_text(TelemetryServer::StatusText status_text);

    void publish_odometry_async(
        TelemetryServer::Odometry odometry, const TelemetryServer::ResultCallback callback);

    TelemetryServer::Result publish_odometry(TelemetryServer::Odometry odometry);

    void publish_position_velocity_ned_async(
        TelemetryServer::PositionVelocityNed position_velocity_ned,
        const TelemetryServer::ResultCallback callback);

    TelemetryServer::Result
    publish_position_velocity_ned(TelemetryServer::PositionVelocityNed position_velocity_ned);

    void publish_ground_truth_async(
        TelemetryServer::GroundTruth ground_truth, const TelemetryServer::ResultCallback callback);

    TelemetryServer::Result publish_ground_truth(TelemetryServer::GroundTruth ground_truth);

    void
    publish_imu_async(TelemetryServer::Imu imu, const TelemetryServer::ResultCallback callback);

    TelemetryServer::Result publish_imu(TelemetryServer::Imu imu);

    void publish_scaled_imu_async(
        TelemetryServer::Imu imu, const TelemetryServer::ResultCallback callback);

    TelemetryServer::Result publish_scaled_imu(TelemetryServer::Imu imu);

    void
    publish_raw_imu_async(TelemetryServer::Imu imu, const TelemetryServer::ResultCallback callback);

    TelemetryServer::Result publish_raw_imu(TelemetryServer::Imu imu);

    void publish_health_all_ok_async(
        bool is_health_all_ok, const TelemetryServer::ResultCallback callback);

    TelemetryServer::Result publish_health_all_ok(bool is_health_all_ok);

    void
    publish_unix_epoch_time_async(uint64_t time_us, const TelemetryServer::ResultCallback callback);

    TelemetryServer::Result publish_unix_epoch_time(uint64_t time_us);

    void subscribe_arm_disarm(TelemetryServer::ArmDisarmCallback callback);

    TelemetryServer::ArmDisarm arm_disarm() const;

    void set_armable_async(
        bool armable, bool force_armable, const TelemetryServer::ResultCallback callback);

    TelemetryServer::Result set_armable(bool armable, bool force_armable);

    void set_disarmable_async(
        bool disarmable, bool force_disarmable, const TelemetryServer::ResultCallback callback);

    TelemetryServer::Result set_disarmable(bool disarmable, bool force_disarmable);

    TelemetryServer::Result
    set_allowable_flight_modes(TelemetryServer::AllowableFlightModes flight_modes);

    TelemetryServer::AllowableFlightModes get_allowable_flight_modes();

    void subscribe_do_set_mode(TelemetryServer::DoSetModeCallback callback);

    void publish_sys_status_async(
        TelemetryServer::Battery battery,
        bool rc_receiver_status,
        bool gyro_status,
        bool accel_status,
        bool mag_status,
        bool gps_status,
        const TelemetryServer::ResultCallback callback);

    TelemetryServer::Result publish_sys_status(
        TelemetryServer::Battery battery,
        bool rc_receiver_status,
        bool gyro_status,
        bool accel_status,
        bool mag_status,
        bool gps_status) const;

    void publish_extended_sys_state_async(
        TelemetryServer::VTOLState vtol_state,
        TelemetryServer::LandedState landed_state,
        const TelemetryServer::ResultCallback callback);

    TelemetryServer::Result publish_extended_sys_state(
        TelemetryServer::VTOLState vtol_state, TelemetryServer::LandedState landed_state) const;

private:
    std::chrono::time_point<std::chrono::steady_clock> _start_time;
    TelemetryServer::ArmDisarmCallback _arm_disarm_callback;
    TelemetryServer::DoSetModeCallback _do_set_mode_callback;
    std::atomic<bool> _armable = false;
    std::atomic<bool> _force_armable = false;
    std::atomic<bool> _disarmable = false;
    std::atomic<bool> _force_disarmable = false;

    std::mutex _flight_mode_mutex;
    std::mutex _callback_mutex;

    union px4_custom_mode {
        struct {
            uint16_t reserved;
            uint8_t main_mode;
            uint8_t sub_mode;
        };
        uint32_t data;
        float data_float;
    };

    TelemetryServer::AllowableFlightModes _allowed_flight_modes{};

    uint64_t get_boot_time()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::steady_clock::now() - _start_time)
            .count();
    }

    uint64_t get_epoch_time()
    {
        return std::chrono::steady_clock::now().time_since_epoch().count();
    }
};

} // namespace mavsdk