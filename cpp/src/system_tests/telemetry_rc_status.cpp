#include "log.hpp"
#include "mavsdk.hpp"
#include "plugins/telemetry/telemetry.hpp"
#include "plugins/mavlink_direct/mavlink_direct.hpp"
#include <atomic>
#include <chrono>
#include <future>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;

// Test that RC status updates arrive via SYS_STATUS messages.
// This is the path used by ArduPilot where RC receiver health
// is reported through the SYS_STATUS sensor flags.
TEST(Telemetry, RcStatusViaSysStatus)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:15220"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:15220"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    while (mavsdk_autopilot.systems().size() == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    auto gs_system = mavsdk_autopilot.systems().at(0);

    auto telemetry = Telemetry{system};
    auto sender = MavlinkDirect{gs_system};

    auto prom = std::promise<Telemetry::RcStatus>{};
    auto fut = prom.get_future();
    std::atomic<bool> received{false};

    auto handle = telemetry.subscribe_rc_status([&](const Telemetry::RcStatus& rc_status) {
        if (!received.exchange(true)) {
            prom.set_value(rc_status);
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Send SYS_STATUS with RC receiver present and healthy
    // MAV_SYS_STATUS_SENSOR_RC_RECEIVER = 0x10000 = 65536
    const uint32_t rc_receiver_flag = 65536;
    MavlinkDirect::MavlinkMessage msg;
    msg.message_name = "SYS_STATUS";
    msg.system_id = 1;
    msg.component_id = 1;
    msg.target_system_id = 0;
    msg.target_component_id = 0;
    msg.fields_json =
        "{\"onboard_control_sensors_present\":" + std::to_string(rc_receiver_flag) +
        ",\"onboard_control_sensors_enabled\":" + std::to_string(rc_receiver_flag) +
        ",\"onboard_control_sensors_health\":" + std::to_string(rc_receiver_flag) +
        ",\"load\":0,\"voltage_battery\":0,\"current_battery\":0,"
        "\"battery_remaining\":-1,\"drop_rate_comm\":0,\"errors_comm\":0,"
        "\"errors_count1\":0,\"errors_count2\":0,\"errors_count3\":0,\"errors_count4\":0}";

    auto result = sender.send_message(msg);
    EXPECT_EQ(result, MavlinkDirect::Result::Success);

    ASSERT_EQ(fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);

    auto rc_status = fut.get();
    // RC receiver is healthy → is_available should be true
    EXPECT_TRUE(rc_status.is_available);

    telemetry.unsubscribe_rc_status(handle);
}

// Test that RC status updates arrive via RC_CHANNELS messages.
// This is the path used by PX4 where RC signal strength is
// reported through the RC_CHANNELS message rssi field.
TEST(Telemetry, RcStatusViaRcChannels)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:15221"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:15221"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    while (mavsdk_autopilot.systems().size() == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    auto gs_system = mavsdk_autopilot.systems().at(0);

    auto telemetry = Telemetry{system};
    auto sender = MavlinkDirect{gs_system};

    auto prom = std::promise<Telemetry::RcStatus>{};
    auto fut = prom.get_future();
    std::atomic<bool> received{false};

    auto handle = telemetry.subscribe_rc_status([&](const Telemetry::RcStatus& rc_status) {
        // Wait for a callback that has signal_strength set (from RC_CHANNELS)
        if (rc_status.signal_strength_percent > 0.0f && !received.exchange(true)) {
            prom.set_value(rc_status);
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Send RC_CHANNELS with rssi = 200 (out of 255)
    MavlinkDirect::MavlinkMessage msg;
    msg.message_name = "RC_CHANNELS";
    msg.system_id = 1;
    msg.component_id = 1;
    msg.target_system_id = 0;
    msg.target_component_id = 0;
    msg.fields_json = R"({"time_boot_ms":1000,"chancount":8,"chan1_raw":1500,"chan2_raw":1500,)"
                      R"("chan3_raw":1500,"chan4_raw":1500,"chan5_raw":0,"chan6_raw":0,)"
                      R"("chan7_raw":0,"chan8_raw":0,"chan9_raw":0,"chan10_raw":0,)"
                      R"("chan11_raw":0,"chan12_raw":0,"chan13_raw":0,"chan14_raw":0,)"
                      R"("chan15_raw":0,"chan16_raw":0,"chan17_raw":0,"chan18_raw":0,"rssi":200})";

    auto result = sender.send_message(msg);
    EXPECT_EQ(result, MavlinkDirect::Result::Success);

    ASSERT_EQ(fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);

    auto rc_status = fut.get();
    // rssi is stored directly as signal_strength_percent (raw value 0-254)
    EXPECT_FLOAT_EQ(rc_status.signal_strength_percent, 200.0f);

    telemetry.unsubscribe_rc_status(handle);
}

// Test that RC status correctly reports unavailable when
// SYS_STATUS reports RC receiver not healthy.
TEST(Telemetry, RcStatusUnavailableViaSysStatus)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:15222"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:15222"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    while (mavsdk_autopilot.systems().size() == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    auto gs_system = mavsdk_autopilot.systems().at(0);

    auto telemetry = Telemetry{system};
    auto sender = MavlinkDirect{gs_system};

    auto prom = std::promise<Telemetry::RcStatus>{};
    auto fut = prom.get_future();
    std::atomic<bool> received{false};

    auto handle = telemetry.subscribe_rc_status([&](const Telemetry::RcStatus& rc_status) {
        if (!received.exchange(true)) {
            prom.set_value(rc_status);
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Send SYS_STATUS with RC receiver present but NOT healthy
    // MAV_SYS_STATUS_SENSOR_RC_RECEIVER = 0x10000 = 65536
    const uint32_t rc_receiver_flag = 65536;
    MavlinkDirect::MavlinkMessage msg;
    msg.message_name = "SYS_STATUS";
    msg.system_id = 1;
    msg.component_id = 1;
    msg.target_system_id = 0;
    msg.target_component_id = 0;
    msg.fields_json =
        "{\"onboard_control_sensors_present\":" + std::to_string(rc_receiver_flag) +
        ",\"onboard_control_sensors_enabled\":" + std::to_string(rc_receiver_flag) +
        ",\"onboard_control_sensors_health\":0"
        ",\"load\":0,\"voltage_battery\":0,\"current_battery\":0,"
        "\"battery_remaining\":-1,\"drop_rate_comm\":0,\"errors_comm\":0,"
        "\"errors_count1\":0,\"errors_count2\":0,\"errors_count3\":0,\"errors_count4\":0}";

    auto result = sender.send_message(msg);
    EXPECT_EQ(result, MavlinkDirect::Result::Success);

    ASSERT_EQ(fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);

    auto rc_status = fut.get();
    // RC receiver present but not healthy → is_available should be false
    EXPECT_FALSE(rc_status.is_available);

    telemetry.unsubscribe_rc_status(handle);
}
