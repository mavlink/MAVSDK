#include "log.hpp"
#include "mavsdk.hpp"
#include "plugins/param/param.hpp"
#include "plugins/param_server/param_server.hpp"
#include <atomic>
#include <chrono>
#include <memory>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;

static constexpr auto param_name_int = "TEST_BLA";
static constexpr int param_value_int = 99;

static constexpr auto param_name_float = "TEST_FOO";
static constexpr float param_value_float = 42.0f;

static constexpr double reduced_timeout_s = 0.1;

TEST(Param, SetAndGet)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    mavsdk_autopilot.set_timeout_s(reduced_timeout_s);

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    auto param_server = ParamServer{mavsdk_autopilot.server_component()};

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    ASSERT_TRUE(system->has_autopilot());

    auto param = Param{system};

    // First we try to get a param before it is available.
    auto result_pair_float = param.get_param_float(param_name_float);
    EXPECT_EQ(result_pair_float.first, Param::Result::DoesNotExist);
    auto result_pair_int = param.get_param_int(param_name_int);
    EXPECT_EQ(result_pair_int.first, Param::Result::DoesNotExist);

    // Then we make it available.
    EXPECT_EQ(
        param_server.provide_param_float(param_name_float, param_value_float),
        ParamServer::Result::Success);
    EXPECT_EQ(
        param_server.provide_param_int(param_name_int, param_value_int),
        ParamServer::Result::Success);

    // Now it should be available
    result_pair_float = param.get_param_float(param_name_float);
    EXPECT_EQ(result_pair_float.first, Param::Result::Success);
    EXPECT_EQ(result_pair_float.second, param_value_float);

    result_pair_int = param.get_param_int(param_name_int);
    EXPECT_EQ(result_pair_int.first, Param::Result::Success);
    EXPECT_EQ(result_pair_int.second, param_value_int);

    // Let's now change the values
    auto result = param.set_param_float(param_name_float, param_value_float + 1.0f);
    EXPECT_EQ(result, Param::Result::Success);

    result = param.set_param_int(param_name_int, param_value_int + 2);
    EXPECT_EQ(result, Param::Result::Success);

    // Check if it has been changed correctly
    result_pair_float = param.get_param_float(param_name_float);
    EXPECT_EQ(result_pair_float.first, Param::Result::Success);
    EXPECT_EQ(result_pair_float.second, param_value_float + 1.0f);

    result_pair_int = param.get_param_int(param_name_int);
    EXPECT_EQ(result_pair_int.first, Param::Result::Success);
    EXPECT_EQ(result_pair_int.second, param_value_int + 2);

    // Also check the server side
    auto server_result_pair_float = param_server.retrieve_param_float(param_name_float);
    EXPECT_EQ(server_result_pair_float.first, ParamServer::Result::Success);
    EXPECT_FLOAT_EQ(server_result_pair_float.second, param_value_float + 1.0f);

    auto server_result_pair_int = param_server.retrieve_param_int(param_name_int);
    EXPECT_EQ(server_result_pair_int.first, ParamServer::Result::Success);
    EXPECT_EQ(server_result_pair_int.second, param_value_int + 2);

    // Also try to retrieve them all at once
    auto server_result_all_params = param_server.retrieve_all_params();
    EXPECT_EQ(server_result_all_params.int_params.size(), 1);
    EXPECT_EQ(server_result_all_params.float_params.size(), 1);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(Param, SetAndGetLossy)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    // Drop every third message to simulate a lossy link
    auto counter = std::make_shared<std::atomic<unsigned>>(0);
    auto drop_handle = mavsdk_groundstation.subscribe_incoming_messages_json(
        [counter](Mavsdk::MavlinkMessage) -> bool { return (*counter)++ % 3 != 0; });

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    mavsdk_autopilot.set_timeout_s(reduced_timeout_s);

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    auto param_server = ParamServer{mavsdk_autopilot.server_component()};

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
    auto result_pair_float = param.get_param_float(param_name_float);
    EXPECT_EQ(result_pair_float.first, Param::Result::Success);
    EXPECT_EQ(result_pair_float.second, param_value_float);

    auto result_pair_int = param.get_param_int(param_name_int);
    EXPECT_EQ(result_pair_int.first, Param::Result::Success);
    EXPECT_EQ(result_pair_int.second, param_value_int);

    // Let's now change the values
    auto result = param.set_param_float(param_name_float, param_value_float + 1.0f);
    EXPECT_EQ(result, Param::Result::Success);

    result = param.set_param_int(param_name_int, param_value_int + 2);
    EXPECT_EQ(result, Param::Result::Success);

    // Check if it has been changed correctly
    result_pair_float = param.get_param_float(param_name_float);
    EXPECT_EQ(result_pair_float.first, Param::Result::Success);
    EXPECT_EQ(result_pair_float.second, param_value_float + 1.0f);

    result_pair_int = param.get_param_int(param_name_int);
    EXPECT_EQ(result_pair_int.first, Param::Result::Success);
    EXPECT_EQ(result_pair_int.second, param_value_int + 2);

    // Also check the server side
    auto server_result_pair_float = param_server.retrieve_param_float(param_name_float);
    EXPECT_EQ(server_result_pair_float.first, ParamServer::Result::Success);
    EXPECT_FLOAT_EQ(server_result_pair_float.second, param_value_float + 1.0f);

    auto server_result_pair_int = param_server.retrieve_param_int(param_name_int);
    EXPECT_EQ(server_result_pair_int.first, ParamServer::Result::Success);
    EXPECT_EQ(server_result_pair_int.second, param_value_int + 2);

    // Also try to retrieve them all at once
    auto server_result_all_params = param_server.retrieve_all_params();
    EXPECT_EQ(server_result_all_params.int_params.size(), 1);
    EXPECT_EQ(server_result_all_params.float_params.size(), 1);

    // Stop dropping
    mavsdk_groundstation.unsubscribe_incoming_messages_json(drop_handle);
}

TEST(Param, GetAndChange)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    mavsdk_autopilot.set_timeout_s(reduced_timeout_s);

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    auto param_server = ParamServer{mavsdk_autopilot.server_component()};

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    ASSERT_TRUE(system->has_autopilot());

    auto param = Param{system};

    // First we try to get a param before it is available.
    auto result_pair_float = param.get_param_float(param_name_float);
    EXPECT_EQ(result_pair_float.first, Param::Result::DoesNotExist);
    auto result_pair_int = param.get_param_int(param_name_int);
    EXPECT_EQ(result_pair_int.first, Param::Result::DoesNotExist);

    // Then we make it available.
    EXPECT_EQ(
        param_server.provide_param_float(param_name_float, param_value_float),
        ParamServer::Result::Success);
    EXPECT_EQ(
        param_server.provide_param_int(param_name_int, param_value_int),
        ParamServer::Result::Success);

    // Now it should be available
    result_pair_float = param.get_param_float(param_name_float);
    EXPECT_EQ(result_pair_float.first, Param::Result::Success);
    EXPECT_EQ(result_pair_float.second, param_value_float);

    result_pair_int = param.get_param_int(param_name_int);
    EXPECT_EQ(result_pair_int.first, Param::Result::Success);
    EXPECT_EQ(result_pair_int.second, param_value_int);

    // Now, the server changes the values
    EXPECT_EQ(
        param_server.provide_param_float(param_name_float, param_value_float + 1.0f),
        ParamServer::Result::Success);
    EXPECT_EQ(
        param_server.provide_param_int(param_name_int, param_value_int + 1),
        ParamServer::Result::Success);

    // Check if it has been changed correctly
    result_pair_float = param.get_param_float(param_name_float);
    EXPECT_EQ(result_pair_float.first, Param::Result::Success);
    EXPECT_EQ(result_pair_float.second, param_value_float + 1.0f);

    result_pair_int = param.get_param_int(param_name_int);
    EXPECT_EQ(result_pair_int.first, Param::Result::Success);
    EXPECT_EQ(result_pair_int.second, param_value_int + 1);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(Param, GetAndChangeTooLate)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    mavsdk_autopilot.set_timeout_s(reduced_timeout_s);

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    auto param_server = ParamServer{mavsdk_autopilot.server_component()};

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    ASSERT_TRUE(system->has_autopilot());

    auto param = Param{system};

    // Then we make it available.
    EXPECT_EQ(
        param_server.provide_param_float(param_name_float, param_value_float),
        ParamServer::Result::Success);
    EXPECT_EQ(
        param_server.provide_param_int(param_name_int, param_value_int),
        ParamServer::Result::Success);

    // Once we use all params, the indices are set and we can't provide more params.
    const auto all_params = param.get_all_params();
    ASSERT_EQ(all_params.float_params.size(), 1);
    ASSERT_EQ(all_params.int_params.size(), 1);
    EXPECT_EQ(all_params.float_params[0].value, param_value_float);
    EXPECT_EQ(all_params.int_params[0].value, param_value_int);

    // Now that the params have be used, we are not able to "provide" more parameters. We can only
    // do so initially.
    EXPECT_EQ(
        param_server.provide_param_int("ANOTHER_ONE", 67),
        ParamServer::Result::ParamProvidedTooLate);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(Param, ChangeExistingAfterLockdown)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    mavsdk_autopilot.set_timeout_s(reduced_timeout_s);

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    auto param_server = ParamServer{mavsdk_autopilot.server_component()};

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    ASSERT_TRUE(system->has_autopilot());

    auto param = Param{system};

    EXPECT_EQ(
        param_server.provide_param_float(param_name_float, param_value_float),
        ParamServer::Result::Success);
    EXPECT_EQ(
        param_server.provide_param_int(param_name_int, param_value_int),
        ParamServer::Result::Success);

    // Requesting all params locks down the set: indices and count are now fixed.
    const auto all_params = param.get_all_params();
    ASSERT_EQ(all_params.float_params.size(), 1);
    ASSERT_EQ(all_params.int_params.size(), 1);

    // Adding a new parameter is no longer allowed.
    EXPECT_EQ(
        param_server.provide_param_int("ANOTHER_ONE", 67),
        ParamServer::Result::ParamProvidedTooLate);

    // But changing the value of an already-provided parameter still works, since it doesn't
    // change the indices or count.
    EXPECT_EQ(
        param_server.provide_param_float(param_name_float, param_value_float + 1.0f),
        ParamServer::Result::Success);
    EXPECT_EQ(
        param_server.provide_param_int(param_name_int, param_value_int + 2),
        ParamServer::Result::Success);

    // And the ground station reads back the changed values.
    auto result_pair_float = param.get_param_float(param_name_float);
    EXPECT_EQ(result_pair_float.first, Param::Result::Success);
    EXPECT_EQ(result_pair_float.second, param_value_float + 1.0f);

    auto result_pair_int = param.get_param_int(param_name_int);
    EXPECT_EQ(result_pair_int.first, Param::Result::Success);
    EXPECT_EQ(result_pair_int.second, param_value_int + 2);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(Param, SelectNonAutopilotComponent)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    mavsdk_autopilot.set_timeout_s(reduced_timeout_s);

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    auto autopilot_param_server = ParamServer{mavsdk_autopilot.server_component()};
    constexpr uint8_t peripheral_component_id = 51;
    auto peripheral_param_server =
        ParamServer{mavsdk_autopilot.server_component_by_id(peripheral_component_id)};

    ASSERT_EQ(
        autopilot_param_server.provide_param_int("SHARED_PARAM", 10), ParamServer::Result::Success);
    ASSERT_EQ(
        autopilot_param_server.provide_param_int("AUTO_ONLY", 111), ParamServer::Result::Success);
    ASSERT_EQ(
        peripheral_param_server.provide_param_int("SHARED_PARAM", 51),
        ParamServer::Result::Success);
    ASSERT_EQ(
        peripheral_param_server.provide_param_int("COMP51_ONLY", 5151),
        ParamServer::Result::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();
    ASSERT_TRUE(system->has_autopilot());

    auto param = Param{system};

    // Out-of-range component IDs must be rejected without corrupting the active selection.
    EXPECT_EQ(
        param.select_component(-1, Param::ProtocolVersion::V1), Param::Result::ComponentNotFound);
    EXPECT_EQ(
        param.select_component(256, Param::ProtocolVersion::V1), Param::Result::ComponentNotFound);

    ASSERT_EQ(
        param.select_component(peripheral_component_id, Param::ProtocolVersion::V1),
        Param::Result::Success);

    // Rejected calls after a valid selection must preserve both component ID and protocol.
    EXPECT_EQ(
        param.select_component(300, Param::ProtocolVersion::Ext), Param::Result::ComponentNotFound);
    EXPECT_EQ(
        param.select_component(-1, Param::ProtocolVersion::Ext), Param::Result::ComponentNotFound);

    auto peripheral_shared = param.get_param_int("SHARED_PARAM");
    EXPECT_EQ(peripheral_shared.first, Param::Result::Success);
    EXPECT_EQ(peripheral_shared.second, 51);

    EXPECT_EQ(param.set_param_int("SHARED_PARAM", 77), Param::Result::Success);

    const auto peripheral_all = param.get_all_params();
    EXPECT_EQ(peripheral_all.int_params.size(), 2u);

    // Switching back to autopilot must return autopilot parameters unaffected by component 51.
    ASSERT_EQ(
        param.select_component(MAV_COMP_ID_AUTOPILOT1, Param::ProtocolVersion::V1),
        Param::Result::Success);

    auto autopilot_shared = param.get_param_int("SHARED_PARAM");
    EXPECT_EQ(autopilot_shared.first, Param::Result::Success);
    EXPECT_EQ(autopilot_shared.second, 10);

    auto autopilot_only = param.get_param_int("AUTO_ONLY");
    EXPECT_EQ(autopilot_only.first, Param::Result::Success);
    EXPECT_EQ(autopilot_only.second, 111);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
