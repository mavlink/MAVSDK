#include "log.h"
#include "mavsdk.h"
#include "plugins/param/param.h"
#include "plugins/param_server/param_server.h"
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;

static constexpr auto param_name = "TEST_BLA";

static constexpr double reduced_timeout_s = 0.1;

static std::string generate_uppercase_ascii(size_t length)
{
    auto data = std::string();
    data.reserve(length);

    static unsigned counter = 0;

    for (size_t i = 0; i < length; ++i) {
        data += static_cast<char>((counter++ % 26) + 65); // ASCII A is 65, A..Z are 25 chars
    }

    data.resize(length);

    return data;
}

TEST(SystemTest, ParamCustomSetAndGet)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{Mavsdk::ComponentType::Autopilot}};
    mavsdk_autopilot.set_timeout_s(reduced_timeout_s);

    ASSERT_EQ(mavsdk_groundstation.add_any_connection("udp://:17000"), ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udp://127.0.0.1:17000"), ConnectionResult::Success);

    auto param_server = ParamServer{mavsdk_autopilot.server_component()};

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    ASSERT_TRUE(system->has_autopilot());

    auto param = Param{system};
    param.select_component(1, Param::ProtocolVersion::Ext);

    const auto data = generate_uppercase_ascii(128);

    // Make it available.
    EXPECT_EQ(param_server.provide_param_custom(param_name, data), ParamServer::Result::Success);

    // Now it should be available
    auto result_pair = param.get_param_custom(param_name);
    EXPECT_EQ(result_pair.first, Param::Result::Success);
    EXPECT_EQ(result_pair.second, data);

    const auto data_shorter = generate_uppercase_ascii(55);

    // Let's now change the values
    auto result = param.set_param_custom(param_name, data_shorter);
    EXPECT_EQ(result, Param::Result::Success);

    // Check if it has been changed correctly
    result_pair = param.get_param_custom(param_name);
    EXPECT_EQ(result_pair.first, Param::Result::Success);
    EXPECT_EQ(result_pair.second, data_shorter);

    // Also check the server side
    auto server_result_pair = param_server.retrieve_param_custom(param_name);
    EXPECT_EQ(server_result_pair.first, ParamServer::Result::Success);
    EXPECT_EQ(server_result_pair.second, data_shorter);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(SystemTest, ParamCustomSetAndGetLossy)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{Mavsdk::ComponentType::Autopilot}};
    mavsdk_autopilot.set_timeout_s(reduced_timeout_s);

    // Drop every third message
    unsigned counter = 0;
    auto drop_some = [&counter](mavlink_message_t&) { return counter++ % 3; };

    mavsdk_groundstation.intercept_incoming_messages_async(drop_some);
    mavsdk_groundstation.intercept_incoming_messages_async(drop_some);

    ASSERT_EQ(mavsdk_groundstation.add_any_connection("udp://:17000"), ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udp://127.0.0.1:17000"), ConnectionResult::Success);

    auto param_server = ParamServer{mavsdk_autopilot.server_component()};

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    ASSERT_TRUE(system->has_autopilot());

    auto param = Param{system};
    param.select_component(1, Param::ProtocolVersion::Ext);

    const auto data = generate_uppercase_ascii(128);

    // Make it available.
    EXPECT_EQ(param_server.provide_param_custom(param_name, data), ParamServer::Result::Success);

    // Now it should be available
    auto result_pair = param.get_param_custom(param_name);
    EXPECT_EQ(result_pair.first, Param::Result::Success);
    EXPECT_EQ(result_pair.second, data);

    const auto data_shorter = generate_uppercase_ascii(55);

    // Let's now change the values
    auto result = param.set_param_custom(param_name, data_shorter);
    EXPECT_EQ(result, Param::Result::Success);

    // Check if it has been changed correctly
    result_pair = param.get_param_custom(param_name);
    EXPECT_EQ(result_pair.first, Param::Result::Success);
    EXPECT_EQ(result_pair.second, data_shorter);

    // Also check the server side
    auto server_result_pair = param_server.retrieve_param_custom(param_name);
    EXPECT_EQ(server_result_pair.first, ParamServer::Result::Success);
    EXPECT_EQ(server_result_pair.second, data_shorter);

    // Before going out of scope, we need to make sure to no longer access the
    // drop_some callback which accesses the local counter variable.
    mavsdk_groundstation.intercept_incoming_messages_async(nullptr);
    mavsdk_groundstation.intercept_incoming_messages_async(nullptr);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
