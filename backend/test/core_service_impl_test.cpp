#include <gmock/gmock.h>

#include "core/core_service_impl.h"
#include "core/mocks/dronecore_mock.h"

namespace {

using testing::NiceMock;

using MockDroneCore = NiceMock<dronecore::testing::MockDroneCore>;
using CoreServiceImpl = dronecore::backend::CoreServiceImpl<MockDroneCore>;

void checkPluginIsRunning(const std::string plugin_name);

TEST(CoreServiceImplTest, actionPluginIsRunning)
{
    checkPluginIsRunning("action");
}

void checkPluginIsRunning(const std::string plugin_name)
{
    MockDroneCore dc;
    CoreServiceImpl coreService(dc);
    dronecore::rpc::core::ListRunningPluginsResponse response;

    coreService.ListRunningPlugins(nullptr, nullptr, &response);

    bool exists = false;
    for (int i = 0; i < response.plugin_info_size(); i++) {
        if (response.plugin_info(i).name() == plugin_name) {
            exists = true;
        }
    }

    EXPECT_TRUE(exists);
}

TEST(CoreServiceImplTest, missionPluginIsRunning)
{
    checkPluginIsRunning("mission");
}

TEST(CoreServiceImplTest, telemetryPluginIsRunning)
{
    checkPluginIsRunning("telemetry");
}

TEST(CoreServiceImplTest, addressIsLocalhostInPluginInfos)
{
    MockDroneCore dc;
    CoreServiceImpl coreService(dc);
    dronecore::rpc::core::ListRunningPluginsResponse response;

    coreService.ListRunningPlugins(nullptr, nullptr, &response);

    for (int i = 0; i < response.plugin_info_size(); i++) {
        EXPECT_EQ(response.plugin_info(i).address(), "localhost");
    }
}

TEST(CoreServiceImplTest, portIsDefaultInPluginInfos)
{
    MockDroneCore dc;
    CoreServiceImpl coreService(dc);
    dronecore::rpc::core::ListRunningPluginsResponse response;

    coreService.ListRunningPlugins(nullptr, nullptr, &response);

    for (int i = 0; i < response.plugin_info_size(); i++) {
        EXPECT_EQ(response.plugin_info(i).port(), 50051);
    }
}

} // namespace
