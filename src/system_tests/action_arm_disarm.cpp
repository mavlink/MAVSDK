#include "log.h"
#include "mavsdk.h"
#include "plugins/action/action.h"
#include "plugins/action_server/action_server.h"
#include <gtest/gtest.h>
#include <thread>

using namespace mavsdk;

TEST(SystemTest, ActionArmDisarm)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{Mavsdk::ComponentType::Autopilot}};

    ASSERT_EQ(mavsdk_groundstation.add_any_connection("udp://:17000"), ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udp://127.0.0.1:17000"), ConnectionResult::Success);

    auto action_server = ActionServer{mavsdk_autopilot.server_component()};

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

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

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
