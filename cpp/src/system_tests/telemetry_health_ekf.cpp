#include "log.hpp"
#include "mavsdk.hpp"
#include "plugins/telemetry/telemetry.hpp"
#include "plugins/mavlink_direct/mavlink_direct.hpp"
#include <chrono>
#include <functional>
#include <string>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;

namespace {

// MAV_SYS_STATUS_SENSOR_GPS
constexpr uint32_t gps_flag = 32;

// EKF_STATUS_FLAGS: attitude, velocity and vertical position, but no horizontal position.
constexpr uint16_t ekf_flags_no_position = 1 | 2 | 4 | 32;
// The same with EKF_PRED_POS_HORIZ_ABS, a position good enough while disarmed.
constexpr uint16_t ekf_flags_predicted_position = ekf_flags_no_position | 512;

MavlinkDirect::MavlinkMessage sys_status_with_healthy_gps()
{
    MavlinkDirect::MavlinkMessage message;
    message.message_name = "SYS_STATUS";
    message.system_id = 1;
    message.component_id = 1;
    message.target_system_id = 0;
    message.target_component_id = 0;
    message.fields_json =
        "{\"onboard_control_sensors_present\":" + std::to_string(gps_flag) +
        ",\"onboard_control_sensors_enabled\":" + std::to_string(gps_flag) +
        ",\"onboard_control_sensors_health\":" + std::to_string(gps_flag) +
        ",\"load\":0,\"voltage_battery\":0,\"current_battery\":0,"
        "\"battery_remaining\":-1,\"drop_rate_comm\":0,\"errors_comm\":0,"
        "\"errors_count1\":0,\"errors_count2\":0,\"errors_count3\":0,\"errors_count4\":0}";
    return message;
}

MavlinkDirect::MavlinkMessage ekf_status_report(uint16_t flags)
{
    MavlinkDirect::MavlinkMessage message;
    message.message_name = "EKF_STATUS_REPORT";
    message.system_id = 1;
    message.component_id = 1;
    message.target_system_id = 0;
    message.target_component_id = 0;
    message.fields_json =
        "{\"flags\":" + std::to_string(flags) +
        ",\"velocity_variance\":0,\"pos_horiz_variance\":0,\"pos_vert_variance\":0,"
        "\"compass_variance\":0,\"terrain_alt_variance\":0,\"airspeed_variance\":0}";
    return message;
}

bool wait_for(const std::function<bool()>& condition)
{
    for (unsigned i = 0; i < 50; ++i) {
        if (condition()) {
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return false;
}

} // namespace

// ArduPilot sets the GPS flag in SYS_STATUS as soon as the receiver is healthy, which is
// well before its estimator has a position. Once it reports the estimator, that decides.
TEST(Telemetry, HealthPositionFromEkfStatusReport)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:15224"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:15224"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    while (mavsdk_autopilot.systems().size() == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    auto gs_system = mavsdk_autopilot.systems().at(0);

    auto telemetry = Telemetry{system};
    auto sender = MavlinkDirect{gs_system};

    // Without a word about the estimator, the GPS flag is all there is to go by.
    EXPECT_EQ(sender.send_message(sys_status_with_healthy_gps()), MavlinkDirect::Result::Success);
    EXPECT_TRUE(wait_for([&]() { return telemetry.health().is_global_position_ok; }));

    EXPECT_EQ(
        sender.send_message(ekf_status_report(ekf_flags_no_position)),
        MavlinkDirect::Result::Success);
    EXPECT_TRUE(wait_for([&]() { return !telemetry.health().is_global_position_ok; }));
    EXPECT_FALSE(telemetry.health().is_local_position_ok);

    // A healthy GPS does not bring it back.
    EXPECT_EQ(sender.send_message(sys_status_with_healthy_gps()), MavlinkDirect::Result::Success);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_FALSE(telemetry.health().is_global_position_ok);

    EXPECT_EQ(
        sender.send_message(ekf_status_report(ekf_flags_predicted_position)),
        MavlinkDirect::Result::Success);
    EXPECT_TRUE(wait_for([&]() { return telemetry.health().is_global_position_ok; }));
    EXPECT_TRUE(telemetry.health().is_local_position_ok);
}
