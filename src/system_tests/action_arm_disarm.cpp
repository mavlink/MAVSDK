#include "log.h"
#include "mavsdk.h"
#include "system_tests_helper.h"
#include "plugins/action/action.h"
#include "plugins/action_server/action_server.h"

using namespace mavsdk;

TEST(SystemTest, ActionArmDisarm)
{
    Mavsdk mavsdk_groundstation;
    mavsdk_groundstation.set_configuration(
        Mavsdk::Configuration{Mavsdk::Configuration::UsageType::GroundStation});

    Mavsdk mavsdk_autopilot;
    mavsdk_autopilot.set_configuration(
        Mavsdk::Configuration{Mavsdk::Configuration::UsageType::Autopilot});

    ASSERT_EQ(mavsdk_groundstation.add_any_connection("udp://:17000"), ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udp://127.0.0.1:17000"), ConnectionResult::Success);

    auto action_server = ActionServer{
        mavsdk_autopilot.server_component_by_type(Mavsdk::ServerComponentType::Autopilot)};

    auto fut = wait_for_first_system_detected(mavsdk_groundstation);
    ASSERT_EQ(fut.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    auto system = fut.get();

    ASSERT_TRUE(system->has_autopilot());

    auto action = Action{system};

    // First we are not allowed to arm.
    EXPECT_EQ(action.arm(), Action::Result::CommandDenied);

    // Then we set it to armable.
    EXPECT_EQ(action_server.set_armable(true, true), ActionServer::Result::Success);

    // Now it should let us arm.
    EXPECT_EQ(action.arm(), Action::Result::Success);

    // At first, we can't disarm.
    EXPECT_EQ(action.disarm(), Action::Result::CommandDenied);

    // Then we set it to disarmable.
    EXPECT_EQ(action_server.set_disarmable(true, true), ActionServer::Result::Success);

    // And disarm again
    EXPECT_EQ(action.disarm(), Action::Result::Success);
}
