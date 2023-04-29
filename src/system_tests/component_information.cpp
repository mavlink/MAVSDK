#include <iostream>
#include <fstream>
#include <vector>
#include <atomic>
#include <future>

#include "mavsdk.h"
#include "log.h"
#include "plugins/component_information/component_information.h"
#include "plugins/component_information_server/component_information_server.h"
#include "plugins/param/param.h"
#include <gtest/gtest.h>

using namespace mavsdk;

// Disabled for now because the mavlink ftp plugin is not available properly on
// the server side right now. It needs to be split into client and server and
// split across system_impl, and server_component_impl.
TEST(SystemTest, DISABLED_ComponentInformationConnect)
{
    Mavsdk mavsdk_groundstation;
    mavsdk_groundstation.set_configuration(
        Mavsdk::Configuration{Mavsdk::Configuration::UsageType::GroundStation});
    ASSERT_EQ(mavsdk_groundstation.add_any_connection("udp://:17000"), ConnectionResult::Success);

    Mavsdk mavsdk_companion;
    mavsdk_companion.set_configuration(
        Mavsdk::Configuration{Mavsdk::Configuration::UsageType::CompanionComputer});
    ASSERT_EQ(
        mavsdk_companion.add_any_connection("udp://127.0.0.1:17000"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    auto server = ComponentInformationServer{
        mavsdk_companion.server_component_by_type(Mavsdk::ServerComponentType::CompanionComputer)};

    auto param = ComponentInformationServer::FloatParam{};
    param.name = "ANG_RATE_ACC_MAX";
    param.short_description = "Angular rate acceleration maximum.";
    param.long_description = "The lower the maximum angular rate acceleration, "
                             "the smoother the gimbal will react to user input.";
    param.unit = "rad/s^2";
    param.min_value = 1.0f;
    param.max_value = 10.0f;
    param.default_value = 3.0f;
    param.start_value = 4.0f;
    param.decimal_places = 1;

    server.provide_float_param(param);

    server.subscribe_float_param([](ComponentInformationServer::FloatParamUpdate param_update) {
        LogInfo() << "Param " << param_update.name << " changed to " << param_update.value
                  << " on server side";
    });

    auto param_client = Param{system};

    param_client.set_param_float("ANG_RATE_ACC_MAX", 5.0f);

    auto client = ComponentInformation{system};
    client.subscribe_float_param([](ComponentInformation::FloatParamUpdate param_update) {
        LogInfo() << "Param " << param_update.name << " changed to " << param_update.value
                  << " on client side";
    });

    // Use another parameter to trigger the second callback.
    param_client.set_param_float("ANG_RATE_ACC_MAX", 6.0f);
}
