#include "log.h"
#include "mavsdk.h"
#include "plugins/param/param.h"
#include "plugins/param_server/param_server.h"
#include <chrono>
#include <gtest/gtest.h>

using namespace mavsdk;

static constexpr auto param_name_int = "TEST_BLA";
static constexpr int param_value_int = 99;

static constexpr auto param_name_float = "TEST_FOO";
static constexpr float param_value_float = 42.0f;

static constexpr double reduced_timeout_s = 0.1;

TEST(SystemTest, ParamSetAndGet)
{
    Mavsdk mavsdk_groundstation;
    mavsdk_groundstation.set_configuration(
        Mavsdk::Configuration{Mavsdk::Configuration::UsageType::GroundStation});
    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    Mavsdk mavsdk_autopilot;
    mavsdk_autopilot.set_configuration(
        Mavsdk::Configuration{Mavsdk::Configuration::UsageType::Autopilot});
    mavsdk_autopilot.set_timeout_s(reduced_timeout_s);

    ASSERT_EQ(mavsdk_groundstation.add_any_connection("udp://:17000"), ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udp://127.0.0.1:17000"), ConnectionResult::Success);

    auto param_server = ParamServer{
        mavsdk_autopilot.server_component_by_type(Mavsdk::ServerComponentType::Autopilot)};

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    ASSERT_TRUE(system->has_autopilot());

    auto param = Param{system};

    // First we try to get a param before it is available.
    auto result_pair = param.get_param_float(param_name_float);
    EXPECT_EQ(result_pair.first, Param::Result::Timeout);
    result_pair = param.get_param_int(param_name_int);
    EXPECT_EQ(result_pair.first, Param::Result::Timeout);

    // Then we make it available.
    EXPECT_EQ(
        param_server.provide_param_float(param_name_float, param_value_float),
        ParamServer::Result::Success);
    EXPECT_EQ(
        param_server.provide_param_int(param_name_int, param_value_int),
        ParamServer::Result::Success);

    // Now it should be available
    result_pair = param.get_param_float(param_name_float);
    EXPECT_EQ(result_pair.first, Param::Result::Success);
    EXPECT_EQ(result_pair.second, param_value_float);

    result_pair = param.get_param_int(param_name_int);
    EXPECT_EQ(result_pair.first, Param::Result::Success);
    EXPECT_EQ(result_pair.second, param_value_int);

    // Let's now change the values
    auto result = param.set_param_float(param_name_float, param_value_float + 1.0f);
    EXPECT_EQ(result, Param::Result::Success);

    result = param.set_param_int(param_name_int, param_value_int + 2);
    EXPECT_EQ(result, Param::Result::Success);

    // Check if it has been changed correctly
    result_pair = param.get_param_float(param_name_float);
    EXPECT_EQ(result_pair.first, Param::Result::Success);
    EXPECT_EQ(result_pair.second, param_value_float + 1.0f);

    result_pair = param.get_param_int(param_name_int);
    EXPECT_EQ(result_pair.first, Param::Result::Success);
    EXPECT_EQ(result_pair.second, param_value_int + 2);

    // Also check the server side
    auto server_result_pair = param_server.retrieve_param_float(param_name_float);
    EXPECT_EQ(server_result_pair.first, ParamServer::Result::Success);
    EXPECT_FLOAT_EQ(server_result_pair.second, param_value_float + 1.0f);

    server_result_pair = param_server.retrieve_param_int(param_name_int);
    EXPECT_EQ(server_result_pair.first, ParamServer::Result::Success);
    EXPECT_FLOAT_EQ(server_result_pair.second, param_value_int + 2);

    // Also try to retrieve them all at once
    auto server_result_all_params = param_server.retrieve_all_params();
    EXPECT_EQ(server_result_all_params.int_params.size(), 1);
    EXPECT_EQ(server_result_all_params.float_params.size(), 1);
}

TEST(SystemTest, ParamSetAndGetLossy)
{
    Mavsdk mavsdk_groundstation;
    mavsdk_groundstation.set_configuration(
        Mavsdk::Configuration{Mavsdk::Configuration::UsageType::GroundStation});
    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    // Drop every third message
    unsigned counter = 0;
    auto drop_some = [&counter](mavlink_message_t&) { return counter++ % 3; };

    mavsdk_groundstation.intercept_incoming_messages_async(drop_some);
    mavsdk_groundstation.intercept_incoming_messages_async(drop_some);

    Mavsdk mavsdk_autopilot;
    mavsdk_autopilot.set_configuration(
        Mavsdk::Configuration{Mavsdk::Configuration::UsageType::Autopilot});
    mavsdk_autopilot.set_timeout_s(reduced_timeout_s);

    ASSERT_EQ(mavsdk_groundstation.add_any_connection("udp://:17000"), ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udp://127.0.0.1:17000"), ConnectionResult::Success);

    auto param_server = ParamServer{
        mavsdk_autopilot.server_component_by_type(Mavsdk::ServerComponentType::Autopilot)};

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    ASSERT_TRUE(system->has_autopilot());

    auto param = Param{system};

    // Make it available.
    EXPECT_EQ(
        param_server.provide_param_float(param_name_float, param_value_float),
        ParamServer::Result::Success);
    EXPECT_EQ(
        param_server.provide_param_int(param_name_int, param_value_int),
        ParamServer::Result::Success);

    // Now it should be available
    auto result_pair = param.get_param_float(param_name_float);
    EXPECT_EQ(result_pair.first, Param::Result::Success);
    EXPECT_EQ(result_pair.second, param_value_float);

    result_pair = param.get_param_int(param_name_int);
    EXPECT_EQ(result_pair.first, Param::Result::Success);
    EXPECT_EQ(result_pair.second, param_value_int);

    // Let's now change the values
    auto result = param.set_param_float(param_name_float, param_value_float + 1.0f);
    EXPECT_EQ(result, Param::Result::Success);

    result = param.set_param_int(param_name_int, param_value_int + 2);
    EXPECT_EQ(result, Param::Result::Success);

    // Check if it has been changed correctly
    result_pair = param.get_param_float(param_name_float);
    EXPECT_EQ(result_pair.first, Param::Result::Success);
    EXPECT_EQ(result_pair.second, param_value_float + 1.0f);

    result_pair = param.get_param_int(param_name_int);
    EXPECT_EQ(result_pair.first, Param::Result::Success);
    EXPECT_EQ(result_pair.second, param_value_int + 2);

    // Also check the server side
    auto server_result_pair = param_server.retrieve_param_float(param_name_float);
    EXPECT_EQ(server_result_pair.first, ParamServer::Result::Success);
    EXPECT_FLOAT_EQ(server_result_pair.second, param_value_float + 1.0f);

    server_result_pair = param_server.retrieve_param_int(param_name_int);
    EXPECT_EQ(server_result_pair.first, ParamServer::Result::Success);
    EXPECT_FLOAT_EQ(server_result_pair.second, param_value_int + 2);

    // Also try to retrieve them all at once
    auto server_result_all_params = param_server.retrieve_all_params();
    EXPECT_EQ(server_result_all_params.int_params.size(), 1);
    EXPECT_EQ(server_result_all_params.float_params.size(), 1);

    // Before going out of scope, we need to make sure to no longer access the
    // drop_some callback which accesses the local counter variable.
    mavsdk_groundstation.intercept_incoming_messages_async(nullptr);
    mavsdk_groundstation.intercept_incoming_messages_async(nullptr);
}
