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
    System &system = dc.system();
    auto action = std::make_shared<Action>(system);
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // TODO: actually do things here.
    EXPECT_TRUE(false);
}
