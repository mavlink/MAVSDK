#include <gtest/gtest.h>
#include <atomic>
#include <thread>
#include "fake_system.h"
#include "plugins/action/action.h"
#include "log.h"
#include "dronecode_sdk.h"

using namespace dronecode_sdk;

TEST(PluginAction, HappyPath)
{
    auto fs = std::make_shared<FakeSystem>(FakeSystem::State::LOYAL);
    fs->start();

    DronecodeSDK dc;
    dc.add_udp_connection(14540);
    System &system = dc.system();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    auto action = std::make_shared<Action>(system);

    EXPECT_EQ(action->arm(), ActionResult::SUCCESS);
    EXPECT_EQ(action->disarm(), ActionResult::SUCCESS);
    EXPECT_EQ(action->arm(), ActionResult::SUCCESS);
    EXPECT_EQ(action->takeoff(), ActionResult::SUCCESS);
    // Need to wait for landed state to catch up.
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(action->disarm(), ActionResult::COMMAND_DENIED_NOT_LANDED);

    // Need to land first.
    EXPECT_EQ(action->land(), ActionResult::SUCCESS);
    // Need to wait for landed state to catch up.
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(action->disarm(), ActionResult::SUCCESS);

    // But killing is possible.
    EXPECT_EQ(action->arm(), ActionResult::SUCCESS);
    EXPECT_EQ(action->takeoff(), ActionResult::SUCCESS);
    EXPECT_EQ(action->kill(), ActionResult::SUCCESS);
}
