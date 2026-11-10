#include "log.hpp"
#include "mavsdk.hpp"
#include "plugins/action_server/action_server.hpp"
#include "plugins/modes/modes.hpp"
#include "plugins/modes_server/modes_server.hpp"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>
#include <gtest/gtest.h>

using namespace mavsdk;
using namespace std::chrono_literals;

namespace {

std::optional<Modes::Mode> find_mode(const std::vector<Modes::Mode>& modes, const std::string& name)
{
    const auto it = std::find_if(
        modes.begin(), modes.end(), [&](const Modes::Mode& mode) { return mode.name == name; });
    if (it == modes.end()) {
        return std::nullopt;
    }
    return *it;
}

template<typename Predicate> bool wait_until(Predicate predicate, std::chrono::milliseconds timeout)
{
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    while (std::chrono::steady_clock::now() < deadline) {
        if (predicate()) {
            return true;
        }
        std::this_thread::sleep_for(10ms);
    }
    return predicate();
}

// PX4 custom modes (main mode in the third byte, sub mode in the fourth), so that ActionServer,
// which only understands PX4 modes, can set them.
const std::vector<ModesServer::Mode> px4_server_modes{
    {"Manual", 0x00010000, false, true, false, false},
    {"Stabilized", 0x00070000, false, true, true, false},
    {"Takeoff", 0x02040000, true, true, false, true},
    {"Mission", 0x04040000, true, true, false, true},
    {"Land", 0x06040000, true, true, false, true},
    {"Offboard", 0x00060000, false, false, true, false},
};

Mavsdk::Configuration px4_autopilot_configuration()
{
    Mavsdk::Configuration configuration{ComponentType::Autopilot};
    configuration.set_autopilot(Autopilot::Px4);
    return configuration;
}

} // namespace

TEST(SystemTest, ModesServerInvalidModes)
{
    Mavsdk mavsdk_autopilot{px4_autopilot_configuration()};
    ModesServer modes_server{mavsdk_autopilot.server_component()};

    // Duplicate name.
    EXPECT_EQ(
        modes_server.set_available_modes(
            {{"Manual", 1, false, true, false, false}, {"Manual", 2, false, true, false, false}}),
        ModesServer::Result::InvalidModes);
    // Duplicate custom mode.
    EXPECT_EQ(
        modes_server.set_available_modes(
            {{"Manual", 1, false, true, false, false}, {"Acro", 1, false, true, false, false}}),
        ModesServer::Result::InvalidModes);
    // Unknown standard mode.
    EXPECT_EQ(
        modes_server.set_available_modes({{"Acro", 1, true, true, false, false}}),
        ModesServer::Result::InvalidModes);
    // Empty and too long names.
    EXPECT_EQ(
        modes_server.set_available_modes({{"", 1, false, true, false, false}}),
        ModesServer::Result::InvalidModes);
    EXPECT_EQ(
        modes_server.set_available_modes({{std::string(36, 'a'), 1, false, true, false, false}}),
        ModesServer::Result::InvalidModes);

    EXPECT_EQ(modes_server.set_available_modes(px4_server_modes), ModesServer::Result::Success);
}

TEST(SystemTest, ModesServerAvailableModes)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{px4_autopilot_configuration()};

    ModesServer modes_server{mavsdk_autopilot.server_component()};
    ASSERT_EQ(modes_server.set_available_modes(px4_server_modes), ModesServer::Result::Success);

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17085"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17085"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto modes = Modes{maybe_system.value()};

    // The fallback list for PX4 might show up first, so wait for the announced one.
    ASSERT_TRUE(wait_until(
        [&]() {
            const auto available_modes = modes.available_modes();
            return available_modes.size() == px4_server_modes.size() &&
                   find_mode(available_modes, "Stabilized");
        },
        5s));

    const auto available_modes = modes.available_modes();
    for (const auto& server_mode : px4_server_modes) {
        const auto mode = find_mode(available_modes, server_mode.name);
        ASSERT_TRUE(mode) << server_mode.name;
        EXPECT_EQ(mode->custom_mode, server_mode.custom_mode) << server_mode.name;
        EXPECT_EQ(mode->is_standard, server_mode.is_standard) << server_mode.name;
        EXPECT_EQ(mode->is_user_selectable, server_mode.is_user_selectable) << server_mode.name;
        EXPECT_EQ(mode->is_advanced, server_mode.is_advanced) << server_mode.name;
        EXPECT_EQ(mode->is_auto_mode, server_mode.is_auto_mode) << server_mode.name;
    }

    // ModesServer keeps sending AVAILABLE_MODES_MONITOR, but with the same seq that doesn't
    // trigger a download.
    std::atomic<unsigned> num_updates{0};
    auto handle = modes.subscribe_available_modes([&](std::vector<Modes::Mode>) { ++num_updates; });
    std::this_thread::sleep_for(200ms);
    const unsigned updates_before = num_updates;
    std::this_thread::sleep_for(2500ms);
    EXPECT_EQ(num_updates, updates_before);

    // A new list is picked up through AVAILABLE_MODES_MONITOR.
    auto new_modes = px4_server_modes;
    new_modes.push_back({"Altitude", 0x00020000, true, true, false, false});
    ASSERT_EQ(modes_server.set_available_modes(new_modes), ModesServer::Result::Success);

    ASSERT_TRUE(wait_until(
        [&]() {
            const auto altitude = find_mode(modes.available_modes(), "Altitude");
            return altitude && altitude->is_standard && altitude->custom_mode == 0x00020000;
        },
        5s));
    EXPECT_EQ(modes.available_modes().size(), new_modes.size());
    EXPECT_GT(num_updates, updates_before);
    modes.unsubscribe_available_modes(handle);
}

TEST(SystemTest, ModesServerCurrentMode)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{px4_autopilot_configuration()};

    ModesServer modes_server{mavsdk_autopilot.server_component()};
    ASSERT_EQ(modes_server.set_available_modes(px4_server_modes), ModesServer::Result::Success);
    ASSERT_EQ(modes_server.set_current_mode("Manual"), ModesServer::Result::Success);
    auto requested_handle = modes_server.subscribe_mode_requested([&](ModesServer::Mode) {
        EXPECT_EQ(
            modes_server.respond_mode_requested(ModesServer::ModeFeedback::Accepted),
            ModesServer::Result::Success);
    });

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17080"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17080"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto modes = Modes{maybe_system.value()};

    ASSERT_TRUE(wait_until(
        [&]() {
            const auto available_modes = modes.available_modes();
            return available_modes.size() == px4_server_modes.size() &&
                   find_mode(available_modes, "Stabilized");
        },
        5s));
    ASSERT_TRUE(wait_until([&]() { return modes.current_mode().name == "Manual"; }, 2s));

    // Names are matched exactly, and only modes the vehicle has can be set.
    EXPECT_EQ(modes.set_mode("manual"), Modes::Result::ModeNotFound);
    EXPECT_EQ(modes.set_mode("Does not exist"), Modes::Result::ModeNotFound);
    EXPECT_EQ(modes.set_mode("Orbit"), Modes::Result::ModeNotFound);

    std::mutex mutex;
    std::vector<std::string> current_modes;
    auto current_handle = modes.subscribe_current_mode([&](Modes::Mode mode) {
        std::lock_guard<std::mutex> lock(mutex);
        current_modes.push_back(mode.name);
    });

    EXPECT_EQ(modes.set_mode("Stabilized"), Modes::Result::Success);
    ASSERT_TRUE(wait_until([&]() { return modes.current_mode().name == "Stabilized"; }, 2s));
    EXPECT_EQ(modes.set_mode("Mission"), Modes::Result::Success);
    ASSERT_TRUE(wait_until([&]() { return modes.current_mode().name == "Mission"; }, 2s));

    // ModesServer keeps sending CURRENT_MODE, but only changes are reported.
    std::this_thread::sleep_for(2500ms);

    modes.unsubscribe_current_mode(current_handle);
    modes_server.unsubscribe_mode_requested(requested_handle);
    {
        std::lock_guard<std::mutex> lock(mutex);
        ASSERT_GE(current_modes.size(), 2u);
        EXPECT_EQ(current_modes.back(), "Mission");
        EXPECT_EQ(current_modes.at(current_modes.size() - 2), "Stabilized");
        EXPECT_EQ(
            std::adjacent_find(current_modes.begin(), current_modes.end()), current_modes.end());
    }
}

// These tests use deprecated APIs for packet interception and ActionServer interoperability.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4996)
#endif
// The message interception API is used here to control packet ordering.
class ModesDownloadTest : public testing::TestWithParam<bool> {};

TEST_P(ModesDownloadTest, MonitorWhileFirstReplyIsOutstanding)
{
    const bool omit_sequence = GetParam();
    const auto port = std::to_string(17180 + omit_sequence);
    std::atomic<bool> reply_withheld{false};
    std::atomic<bool> monitor_received{false};
    Mavsdk groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    ModesServer server{autopilot.server_component()};
    const std::vector<ModesServer::Mode> modes{
        {"Hover", 1, false, true, false, false}, {"Position", 2, false, true, false, false}};
    ASSERT_EQ(server.set_available_modes(modes), ModesServer::Result::Success);
    ASSERT_EQ(server.set_current_mode("Hover"), ModesServer::Result::Success);

    autopilot.intercept_outgoing_messages_async([&](mavlink_message_t& message) {
        if (message.msgid != MAVLINK_MSG_ID_AVAILABLE_MODES) {
            return true;
        }
        if (!monitor_received) {
            reply_withheld = true;
            return false;
        }
        if (omit_sequence) {
            mavlink_available_modes_t mode;
            mavlink_msg_available_modes_decode(&message, &mode);
            mode.seq = 0;
            mavlink_status_t status{};
            status.current_tx_seq = message.seq;
            mavlink_msg_available_modes_encode_status(
                message.sysid, message.compid, &status, &message, &mode);
        }
        return true;
    });
    groundstation.intercept_incoming_messages_async([&](mavlink_message_t& message) {
        if (message.msgid == MAVLINK_MSG_ID_AVAILABLE_MODES_MONITOR) {
            if (!reply_withheld) {
                return false;
            }
            monitor_received = true;
        }
        return true;
    });

    ASSERT_EQ(
        groundstation.add_any_connection("udpin://0.0.0.0:" + port), ConnectionResult::Success);
    ASSERT_EQ(
        autopilot.add_any_connection("udpout://127.0.0.1:" + port), ConnectionResult::Success);
    auto system = groundstation.first_autopilot(10.0);
    ASSERT_TRUE(system);
    Modes client{system.value()};

    ASSERT_TRUE(wait_until([&]() { return client.available_modes().size() == modes.size(); }, 8s));
    ASSERT_TRUE(monitor_received);
    const auto received = client.available_modes();
    EXPECT_TRUE(find_mode(received, "Hover"));
    EXPECT_TRUE(find_mode(received, "Position"));
}

INSTANTIATE_TEST_SUITE_P(SequencedAndUnsequencedReplies, ModesDownloadTest, testing::Bool());

TEST(SystemTest, ModesServerChangesModes)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{px4_autopilot_configuration()};

    // ActionServer's flight mode API is deprecated: once ModesServer has modes, it leaves mode
    // changes alone.
    ModesServer modes_server{mavsdk_autopilot.server_component()};
    ActionServer action_server{mavsdk_autopilot.server_component()};
    ASSERT_EQ(modes_server.set_available_modes(px4_server_modes), ModesServer::Result::Success);

    std::atomic<unsigned> action_server_flight_mode_changes{0};
    auto flight_mode_handle = action_server.subscribe_flight_mode_change(
        [&](ActionServer::Result, ActionServer::FlightMode) {
            ++action_server_flight_mode_changes;
        });

    EXPECT_EQ(modes_server.set_current_mode("Manual"), ModesServer::Result::Success);
    EXPECT_EQ(modes_server.set_current_mode("Acro"), ModesServer::Result::ModeNotFound);

    // There is nothing to respond to yet.
    EXPECT_EQ(
        modes_server.respond_mode_requested(ModesServer::ModeFeedback::Accepted),
        ModesServer::Result::NoRequest);
    EXPECT_EQ(
        modes_server.respond_mode_requested(ModesServer::ModeFeedback::Unknown),
        ModesServer::Result::InvalidFeedback);

    // The vehicle switches to any requested mode but Takeoff, which it can't do right now, and
    // Land, which it doesn't allow.
    std::mutex requested_mutex;
    std::vector<std::string> requested;
    auto requested_handle = modes_server.subscribe_mode_requested([&](ModesServer::Mode mode) {
        {
            std::lock_guard<std::mutex> lock(requested_mutex);
            requested.push_back(mode.name);
        }
        auto feedback = ModesServer::ModeFeedback::Accepted;
        if (mode.name == "Takeoff") {
            feedback = ModesServer::ModeFeedback::Busy;
        } else if (mode.name == "Land") {
            feedback = ModesServer::ModeFeedback::Denied;
        }
        EXPECT_EQ(modes_server.respond_mode_requested(feedback), ModesServer::Result::Success);
    });

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17086"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17086"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto modes = Modes{maybe_system.value()};

    ASSERT_TRUE(wait_until(
        [&]() {
            const auto available_modes = modes.available_modes();
            return available_modes.size() == px4_server_modes.size() &&
                   find_mode(available_modes, "Stabilized");
        },
        5s));
    ASSERT_TRUE(wait_until([&]() { return modes.current_mode().name == "Manual"; }, 2s));

    // A standard mode, through MAV_CMD_DO_SET_STANDARD_MODE.
    EXPECT_EQ(modes.set_mode("Mission"), Modes::Result::Success);
    ASSERT_TRUE(wait_until([&]() { return modes.current_mode().name == "Mission"; }, 2s));
    EXPECT_TRUE(modes.current_mode().is_standard);

    // A custom mode, through MAV_CMD_DO_SET_MODE.
    EXPECT_EQ(modes.set_mode("Stabilized"), Modes::Result::Success);
    ASSERT_TRUE(wait_until([&]() { return modes.current_mode().name == "Stabilized"; }, 2s));
    EXPECT_FALSE(modes.current_mode().is_standard);

    // Rejected requests leave the vehicle where it is.
    EXPECT_EQ(modes.set_mode("Takeoff"), Modes::Result::Busy);
    EXPECT_EQ(modes.set_mode("Land"), Modes::Result::Denied);
    std::this_thread::sleep_for(300ms);
    EXPECT_EQ(modes.current_mode().name, "Stabilized");

    modes_server.unsubscribe_mode_requested(requested_handle);
    action_server.unsubscribe_flight_mode_change(flight_mode_handle);
    {
        std::lock_guard<std::mutex> lock(requested_mutex);
        EXPECT_EQ(
            requested, (std::vector<std::string>{"Mission", "Stabilized", "Takeoff", "Land"}));
    }
    EXPECT_EQ(action_server_flight_mode_changes, 0u);
}
// Without ModesServer, a PX4 autopilot is what older PX4 versions look like: no AVAILABLE_MODES,
// and mode changes go to ActionServer.
TEST(SystemTest, ModesFallback)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{px4_autopilot_configuration()};

    ActionServer action_server{mavsdk_autopilot.server_component()};
    ActionServer::AllowableFlightModes allowable_flight_modes{};
    allowable_flight_modes.can_auto_mode = true;
    allowable_flight_modes.can_auto_rtl_mode = true;
    ASSERT_EQ(
        action_server.set_allowable_flight_modes(allowable_flight_modes),
        ActionServer::Result::Success);

    std::mutex mutex;
    std::vector<ActionServer::FlightMode> flight_modes;
    auto flight_mode_handle = action_server.subscribe_flight_mode_change(
        [&](ActionServer::Result result, ActionServer::FlightMode flight_mode) {
            EXPECT_EQ(result, ActionServer::Result::Success);
            std::lock_guard<std::mutex> lock(mutex);
            flight_modes.push_back(flight_mode);
        });

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17084"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17084"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto modes = Modes{maybe_system.value()};

    // The request for AVAILABLE_MODES fails, so the known PX4 modes are used.
    ASSERT_TRUE(wait_until([&]() { return !modes.available_modes().empty(); }, 10s));
    const auto available_modes = modes.available_modes();
    ASSERT_TRUE(find_mode(available_modes, "Mission"));
    ASSERT_TRUE(find_mode(available_modes, "Safe Recovery"));

    // Standard modes are set as custom modes, without trying MAV_CMD_DO_SET_STANDARD_MODE,
    // and the current mode follows the custom mode in HEARTBEAT.
    EXPECT_EQ(modes.set_mode("Mission"), Modes::Result::Success);
    ASSERT_TRUE(wait_until([&]() { return modes.current_mode().name == "Mission"; }, 2s));
    EXPECT_EQ(modes.set_mode("Safe Recovery"), Modes::Result::Success);
    ASSERT_TRUE(wait_until([&]() { return modes.current_mode().name == "Safe Recovery"; }, 2s));

    action_server.unsubscribe_flight_mode_change(flight_mode_handle);
    {
        std::lock_guard<std::mutex> lock(mutex);
        EXPECT_EQ(
            flight_modes,
            (std::vector<ActionServer::FlightMode>{
                ActionServer::FlightMode::Mission, ActionServer::FlightMode::ReturnToLaunch}));
    }
}
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif

TEST(SystemTest, ModesServerGenericAutopilot)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    // An autopilot with its own custom modes, not PX4.
    const std::vector<ModesServer::Mode> server_modes{
        {"Hover", 1, false, true, false, false},
        {"Position", 2, true, true, false, false},
        {"Survey", 3, false, true, false, true},
        {"Land", 4, true, true, false, true},
    };

    ModesServer modes_server{mavsdk_autopilot.server_component()};
    ASSERT_EQ(modes_server.set_available_modes(server_modes), ModesServer::Result::Success);
    ASSERT_EQ(modes_server.set_current_mode("Hover"), ModesServer::Result::Success);

    auto requested_handle = modes_server.subscribe_mode_requested([&](ModesServer::Mode) {
        EXPECT_EQ(
            modes_server.respond_mode_requested(ModesServer::ModeFeedback::Accepted),
            ModesServer::Result::Success);
    });

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17087"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17087"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto modes = Modes{maybe_system.value()};

    ASSERT_TRUE(
        wait_until([&]() { return modes.available_modes().size() == server_modes.size(); }, 5s));
    ASSERT_TRUE(wait_until([&]() { return modes.current_mode().name == "Hover"; }, 2s));

    for (const auto& name : {"Survey", "Position", "Land", "Hover"}) {
        EXPECT_EQ(modes.set_mode(name), Modes::Result::Success) << name;
        ASSERT_TRUE(wait_until([&]() { return modes.current_mode().name == name; }, 2s)) << name;
    }

    modes_server.unsubscribe_mode_requested(requested_handle);
}
