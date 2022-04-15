#include "log.h"
#include "mavsdk.h"
#include "system_tests_helper.h"
#include "plugins/param/param.h"
#include "plugins/param_server/param_server.h"

using namespace mavsdk;

static constexpr auto param_name_int = "TEST_BLA";
static constexpr int param_value_int = 99;

static constexpr auto param_name_float = "TEST_FOO";
static constexpr float param_value_float = 42.0f;

TEST(SystemTest, ParamSetAndGet)
{
    Mavsdk mavsdk_groundstation;
    mavsdk_groundstation.set_configuration(
        Mavsdk::Configuration{Mavsdk::Configuration::UsageType::GroundStation});

    // We reduce the timeout for this test because some tests just trigger a timeout
    // and we don't want to wait forever.
    mavsdk_groundstation.set_timeout_s(0.01);

    Mavsdk mavsdk_autopilot;
    mavsdk_autopilot.set_configuration(
        Mavsdk::Configuration{Mavsdk::Configuration::UsageType::Autopilot});

    ASSERT_EQ(mavsdk_groundstation.add_any_connection("udp://:17000"), ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udp://127.0.0.1:17000"), ConnectionResult::Success);

    auto param_server = ParamServer{
        mavsdk_autopilot.server_component_by_type(Mavsdk::ServerComponentType::Autopilot)};

    auto fut = wait_for_first_system_detected(mavsdk_groundstation);
    ASSERT_EQ(fut.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    auto system = fut.get();

    ASSERT_TRUE(system->has_autopilot());

    auto param = Param{system};

    // First we try to get a param before it is available.
    auto result = param.get_param_float(param_name_float);
    EXPECT_EQ(result.first, Param::Result::Timeout);
    result = param.get_param_int(param_name_int);
    EXPECT_EQ(result.first, Param::Result::Timeout);

    // Then we make it available.
    EXPECT_EQ(param_server.provide_param_float(param_name_float, param_value_float), ParamServer::Result::Success);
    EXPECT_EQ(param_server.provide_param_int(param_name_int, param_value_int), ParamServer::Result::Success);

    // Now it should be available
    result = param.get_param_float(param_name_float);
    EXPECT_EQ(result.first, Param::Result::Success);
    EXPECT_EQ(result.second, param_value_float);

    result = param.get_param_int(param_name_int);
    //EXPECT_EQ(result.first, Param::Result::Success);
    //EXPECT_EQ(result.second, param_value_int);


}
