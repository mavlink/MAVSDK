#include "mavsdk.hpp"
#include "plugins/mavlink_direct/mavlink_direct.hpp"
#include "plugins/param_server/param_server.hpp"
#include <chrono>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace mavsdk;

namespace {

constexpr auto param_name = "TEST_FOO";
constexpr float initial_value = 42.0f;
constexpr float changed_value = 43.0f;

// Collects the values seen for a given parameter from PARAM_VALUE / PARAM_EXT_VALUE broadcasts.
struct ValueCollector {
    std::mutex mutex{};
    std::vector<float> param_values{};
    std::vector<std::string> param_ext_values{};

    void add_param_value(const std::string& json)
    {
        nlohmann::json root;
        if (((root = nlohmann::json::parse(json, nullptr, false)).is_discarded())) {
            return;
        }
        if (root["param_id"].get<std::string>().rfind(param_name, 0) != 0) {
            return;
        }
        std::lock_guard<std::mutex> lock(mutex);
        param_values.push_back(root["param_value"].get<float>());
    }

    void add_param_ext_value(const std::string& json)
    {
        nlohmann::json root;
        if (((root = nlohmann::json::parse(json, nullptr, false)).is_discarded())) {
            return;
        }
        if (root["param_id"].get<std::string>().rfind(param_name, 0) != 0) {
            return;
        }
        std::lock_guard<std::mutex> lock(mutex);
        param_ext_values.push_back(root["param_value"].get<std::string>());
    }
};

bool contains_value(ValueCollector& collector, float value)
{
    std::lock_guard<std::mutex> lock(collector.mutex);
    for (const auto& v : collector.param_values) {
        if (v == value) {
            return true;
        }
    }
    return false;
}

// Sends a PARAM_(EXT_)REQUEST_READ for our parameter to the autopilot. This makes the server
// register a client using the given protocol (without locking the parameter set down, which a
// REQUEST_LIST would) and triggers a value response.
void request_read(MavlinkDirect& mavlink_direct, bool extended)
{
    MavlinkDirect::MavlinkMessage message;
    message.message_name = extended ? "PARAM_EXT_REQUEST_READ" : "PARAM_REQUEST_READ";
    message.fields_json = R"({"target_system":1,"target_component":1,"param_id":")" +
                          std::string(param_name) + R"(","param_index":-1})";
    EXPECT_EQ(mavlink_direct.send_message(message), MavlinkDirect::Result::Success);
}

} // namespace

// When only a non-extended client has been seen, a value change to a parameter that is
// representable in the classic protocol must be broadcast as PARAM_VALUE (not PARAM_EXT_VALUE),
// otherwise the non-extended client misses the update.
TEST(ParamChangeNotification, NonExtendedClient)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17010"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17010"), ConnectionResult::Success);

    auto param_server = ParamServer{mavsdk_autopilot.server_component()};
    ASSERT_EQ(
        param_server.provide_param_float(param_name, initial_value), ParamServer::Result::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    ValueCollector collector;
    auto mavlink_direct = MavlinkDirect{system};
    mavlink_direct.subscribe_message(
        "PARAM_VALUE", [&collector](MavlinkDirect::MavlinkMessage message) {
            collector.add_param_value(message.fields_json);
        });
    mavlink_direct.subscribe_message(
        "PARAM_EXT_VALUE", [&collector](MavlinkDirect::MavlinkMessage message) {
            collector.add_param_ext_value(message.fields_json);
        });

    // Give the subscriptions a moment to register.
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Announce ourselves as a non-extended client and get the initial value.
    request_read(mavlink_direct, false);

    // Wait for the initial value to arrive via PARAM_VALUE.
    for (int i = 0; i < 50 && !contains_value(collector, initial_value); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    EXPECT_TRUE(contains_value(collector, initial_value));

    // Change the value on the server.
    ASSERT_EQ(
        param_server.provide_param_float(param_name, changed_value), ParamServer::Result::Success);

    // The change must be broadcast as PARAM_VALUE so a non-extended client sees it.
    for (int i = 0; i < 50 && !contains_value(collector, changed_value); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    EXPECT_TRUE(contains_value(collector, changed_value));

    // And it must not have leaked out as PARAM_EXT_VALUE.
    std::lock_guard<std::mutex> lock(collector.mutex);
    EXPECT_TRUE(collector.param_ext_values.empty());
}

// When only an extended client has been seen, a value change must be broadcast as
// PARAM_EXT_VALUE, and must not go out as PARAM_VALUE.
TEST(ParamChangeNotification, ExtendedClient)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17011"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17011"), ConnectionResult::Success);

    auto param_server = ParamServer{mavsdk_autopilot.server_component()};
    ASSERT_EQ(
        param_server.provide_param_float(param_name, initial_value), ParamServer::Result::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    ValueCollector collector;
    auto mavlink_direct = MavlinkDirect{system};
    mavlink_direct.subscribe_message(
        "PARAM_VALUE", [&collector](MavlinkDirect::MavlinkMessage message) {
            collector.add_param_value(message.fields_json);
        });
    mavlink_direct.subscribe_message(
        "PARAM_EXT_VALUE", [&collector](MavlinkDirect::MavlinkMessage message) {
            collector.add_param_ext_value(message.fields_json);
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Announce ourselves as an extended client and get the initial value.
    request_read(mavlink_direct, true);

    // Wait for the initial value to arrive via PARAM_EXT_VALUE.
    auto ext_seen = [&collector]() {
        std::lock_guard<std::mutex> lock(collector.mutex);
        return !collector.param_ext_values.empty();
    };
    for (int i = 0; i < 50 && !ext_seen(); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    EXPECT_TRUE(ext_seen());

    const size_t ext_count_before = [&collector]() {
        std::lock_guard<std::mutex> lock(collector.mutex);
        return collector.param_ext_values.size();
    }();

    // Change the value on the server.
    ASSERT_EQ(
        param_server.provide_param_float(param_name, changed_value), ParamServer::Result::Success);

    // The change must be broadcast as PARAM_EXT_VALUE.
    auto ext_grew = [&collector, ext_count_before]() {
        std::lock_guard<std::mutex> lock(collector.mutex);
        return collector.param_ext_values.size() > ext_count_before;
    };
    for (int i = 0; i < 50 && !ext_grew(); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    EXPECT_TRUE(ext_grew());

    // The value change must not have leaked out as PARAM_VALUE.
    EXPECT_FALSE(contains_value(collector, changed_value));
}

// When both a non-extended and an extended client have been seen, a value change must be
// broadcast on both protocols. This is the actual regression: previously the notification went
// out on only the protocol of the most recent request, so a client on the other protocol missed
// the update.
TEST(ParamChangeNotification, BothProtocolsSeen)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17012"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17012"), ConnectionResult::Success);

    auto param_server = ParamServer{mavsdk_autopilot.server_component()};
    ASSERT_EQ(
        param_server.provide_param_float(param_name, initial_value), ParamServer::Result::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    ValueCollector collector;
    auto mavlink_direct = MavlinkDirect{system};
    mavlink_direct.subscribe_message(
        "PARAM_VALUE", [&collector](MavlinkDirect::MavlinkMessage message) {
            collector.add_param_value(message.fields_json);
        });
    mavlink_direct.subscribe_message(
        "PARAM_EXT_VALUE", [&collector](MavlinkDirect::MavlinkMessage message) {
            collector.add_param_ext_value(message.fields_json);
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // A non-extended client reads, then an extended client reads. The extended read is last,
    // which is what used to hide the update from the non-extended client.
    request_read(mavlink_direct, false);
    request_read(mavlink_direct, true);

    // Wait for the initial value to be reflected on both protocols.
    auto both_initial = [&collector]() {
        std::lock_guard<std::mutex> lock(collector.mutex);
        return !collector.param_values.empty() && !collector.param_ext_values.empty();
    };
    for (int i = 0; i < 50 && !both_initial(); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    ASSERT_TRUE(both_initial());

    const size_t ext_count_before = [&collector]() {
        std::lock_guard<std::mutex> lock(collector.mutex);
        return collector.param_ext_values.size();
    }();

    // Change the value on the server.
    ASSERT_EQ(
        param_server.provide_param_float(param_name, changed_value), ParamServer::Result::Success);

    // The change must reach both the non-extended and the extended client.
    auto ext_grew = [&collector, ext_count_before]() {
        std::lock_guard<std::mutex> lock(collector.mutex);
        return collector.param_ext_values.size() > ext_count_before;
    };
    for (int i = 0; i < 50 && !(contains_value(collector, changed_value) && ext_grew()); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    EXPECT_TRUE(contains_value(collector, changed_value));
    EXPECT_TRUE(ext_grew());
}
