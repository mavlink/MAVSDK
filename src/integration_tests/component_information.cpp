#include <iostream>
#include <fstream>
#include <vector>
#include <atomic>
#include <future>

#include "integration_test_helper.h"
#include "mavsdk.h"
#include "plugins/component_information_server/component_information_server.h"
#include "plugins/mavlink_passthrough/mavlink_passthrough.h"
#include "plugins/param/param.h"

using namespace mavsdk;

TEST(ComponentInformation, Connect)
{
    Mavsdk mavsdk_groundstation;
    mavsdk_groundstation.set_configuration(
        Mavsdk::Configuration{Mavsdk::Configuration::UsageType::CompanionComputer});
    ASSERT_EQ(mavsdk_groundstation.add_any_connection("udp://:24550"), ConnectionResult::Success);

    Mavsdk mavsdk_companion;
    mavsdk_companion.set_configuration(
        Mavsdk::Configuration{Mavsdk::Configuration::UsageType::CompanionComputer});
    ASSERT_EQ(
        mavsdk_companion.add_any_connection("udp://127.0.0.1:24550"), ConnectionResult::Success);

    // Time to discover each other.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    ASSERT_EQ(mavsdk_groundstation.systems().size(), 1);
    ASSERT_EQ(mavsdk_companion.systems().size(), 1);

    auto groundstation = mavsdk_groundstation.systems().at(0);
    auto companion = mavsdk_companion.systems().at(0);

    auto server = ComponentInformationServer{companion};

    auto param = ComponentInformationServer::FloatParam{};
    param.name = "ANG_RATE_ACC_MAX";
    param.short_description = "Angular rate acceleration maximum.";
    param.long_description = "The lower the maximum angular rate acceleration, "
                             "the smoother the gimbal will react to user input.";
    param.unit = "rad/s^2";
    param.min_value = 1.0f;
    param.max_value = 10.0f;
    param.default_value = 3.0f;
    param.start_value = 5.0f;
    param.decimal_places = 1;

    server.provide_float_param(param);

    server.subscribe_float_param([](ComponentInformationServer::FloatParamUpdate param_update) {
        LogInfo() << "Param " << param_update.name << " changed to " << param_update.value;
    });

    auto passthrough_client = MavlinkPassthrough{groundstation};
    auto param_client = Param{groundstation};

    const auto request = MavlinkPassthrough::CommandLong{
        companion->get_system_id(),
        0,
        MAV_CMD_REQUEST_MESSAGE,
        static_cast<float>(MAVLINK_MSG_ID_COMPONENT_INFORMATION),
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f};

    passthrough_client.send_command_long(request);

    param_client.set_param_float("ANG_RATE_ACC_MAX", 4.0f);

    std::this_thread::sleep_for(std::chrono::seconds(2));
}
