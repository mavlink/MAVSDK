#include "log.h"
#include "mavsdk.h"
#include "plugins/param/param.h"
#include "plugins/param_server/param_server.h"
#include <gtest/gtest.h>
#include <chrono>
#include <vector>
#include <thread>
#include <map>
#include <thread>

using namespace mavsdk;

static constexpr unsigned num_params_per_type = 3;

static constexpr double reduced_timeout_s = 0.1;

static std::map<std::string, float> generate_float_params()
{
    std::map<std::string, float> params;
    for (unsigned i = 0; i < num_params_per_type; ++i) {
        const auto id = std::string("TEST_FLOAT") + std::to_string(i);
        const float value = 42.0f + static_cast<float>(i);
        params[id] = value;
    }
    return params;
}

static std::map<std::string, int> generate_int_params()
{
    std::map<std::string, int> params;
    for (unsigned i = 0; i < num_params_per_type; ++i) {
        const auto id = std::string("TEST_INT") + std::to_string(i);
        const int value = 42 + i;
        params[id] = value;
    }
    return params;
}

static std::map<std::string, std::string> generate_string_params()
{
    std::map<std::string, std::string> params;
    for (unsigned i = 0; i < num_params_per_type; ++i) {
        const auto id = std::string("TEST_STRING") + std::to_string(i);
        const std::string value = "VAL" + std::to_string(i);
        params[id] = value;
    }
    return params;
}
// compare originally provided and received values - they have a slightly different data layout.
template<typename T1, typename T2>
static void assert_equal(const std::map<std::string, T1>& values, const std::vector<T2>& received)
{
    EXPECT_EQ(received.size(), values.size());
    // check that all the parameters we got have the right param value
    for (const auto& param : received) {
        EXPECT_EQ(param.value, values.find(param.name)->second);
    }
}

TEST(SystemTest, ParamGetAll)
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

    const auto test_float_params = generate_float_params();
    const auto test_int_params = generate_int_params();
    const auto test_string_params = generate_string_params();

    // Add many params (these don't need extended)
    for (auto const& [key, val] : test_float_params) {
        EXPECT_EQ(param_server.provide_param_float(key, val), ParamServer::Result::Success);
    }
    for (auto const& [key, val] : test_int_params) {
        EXPECT_EQ(param_server.provide_param_int(key, val), ParamServer::Result::Success);
    }

    for (auto const& [key, val] : test_string_params) {
        EXPECT_EQ(param_server.provide_param_custom(key, val), ParamServer::Result::Success);
    }

    {
        auto param_sender = Param{system};
        // Here we use the non-extended protocol
        param_sender.select_component(1, Param::ProtocolVersion::V1);
        const auto all_params = param_sender.get_all_params();
        assert_equal<int, Param::IntParam>(test_int_params, all_params.int_params);
        assert_equal<float, Param::FloatParam>(test_float_params, all_params.float_params);
    }
    {
        auto param_sender = Param{system};
        // now we do the same, but this time with the extended protocol
        param_sender.select_component(1, Param::ProtocolVersion::Ext);
        const auto all_params = param_sender.get_all_params();
        assert_equal<int, Param::IntParam>(test_int_params, all_params.int_params);
        assert_equal<float, Param::FloatParam>(test_float_params, all_params.float_params);
        assert_equal<std::string, Param::CustomParam>(test_string_params, all_params.custom_params);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(SystemTest, ParamGetAllLossy)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{Mavsdk::ComponentType::Autopilot}};
    mavsdk_autopilot.set_timeout_s(reduced_timeout_s);

    // Drop every third message
    unsigned counter = 0;
    auto drop_some = [&counter](mavlink_message_t&) { return counter++ % 5; };

    mavsdk_groundstation.intercept_incoming_messages_async(drop_some);
    mavsdk_groundstation.intercept_outgoing_messages_async(drop_some);

    ASSERT_EQ(mavsdk_groundstation.add_any_connection("udp://:17000"), ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udp://127.0.0.1:17000"), ConnectionResult::Success);

    auto param_server = ParamServer{mavsdk_autopilot.server_component()};

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    ASSERT_TRUE(system->has_autopilot());

    const auto test_float_params = generate_float_params();
    const auto test_int_params = generate_int_params();
    const auto test_string_params = generate_string_params();

    // Add many params (these don't need extended)
    for (auto const& [key, val] : test_float_params) {
        EXPECT_EQ(param_server.provide_param_float(key, val), ParamServer::Result::Success);
    }
    for (auto const& [key, val] : test_int_params) {
        EXPECT_EQ(param_server.provide_param_int(key, val), ParamServer::Result::Success);
    }

    for (auto const& [key, val] : test_string_params) {
        EXPECT_EQ(param_server.provide_param_custom(key, val), ParamServer::Result::Success);
    }

    {
        auto param_sender = Param{system};
        // Here we use the non-extended protocol
        param_sender.select_component(1, Param::ProtocolVersion::V1);
        const auto all_params = param_sender.get_all_params();
        assert_equal<int, Param::IntParam>(test_int_params, all_params.int_params);
        assert_equal<float, Param::FloatParam>(test_float_params, all_params.float_params);
    }
    {
        auto param_sender = Param{system};
        // now we do the same, but this time with the extended protocol
        param_sender.select_component(1, Param::ProtocolVersion::Ext);
        const auto all_params = param_sender.get_all_params();
        assert_equal<int, Param::IntParam>(test_int_params, all_params.int_params);
        assert_equal<float, Param::FloatParam>(test_float_params, all_params.float_params);
        assert_equal<std::string, Param::CustomParam>(test_string_params, all_params.custom_params);
    }

    // Before going out of scope, we need to make sure to no longer access the
    // drop_some callback which accesses the local counter variable.
    mavsdk_groundstation.intercept_incoming_messages_async(nullptr);
    mavsdk_groundstation.intercept_outgoing_messages_async(nullptr);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
