#include <gtest/gtest.h>
#include <atomic>
#include "fake_system.h"
#include "action.h"
#include "log.h"
#include "dronecore.h"


using namespace dronecore;


TEST(PluginAction, HappyPath)
{
    auto fs = std::make_shared<FakeSystem>(FakeSystem::State::LOYAL);
    fs->start();

    DroneCore dc;
    System &system = dc.system();
    auto action = std::make_shared<Action>(system);
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // TODO: actually do things here.
    EXPECT_TRUE(false);
}

