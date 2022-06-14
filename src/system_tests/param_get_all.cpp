#include "log.h"
#include "mavsdk.h"
#include "system_tests_helper.h"
#include "plugins/param/param.h"
#include "plugins/param_server/param_server.h"
#include <chrono>
#include <utility>
#include <vector>
#include <thread>

using namespace mavsdk;

static constexpr unsigned num_params_per_type = 100;

std::vector<std::pair<std::string, float>> generate_float_params()
{
    std::vector<std::pair<std::string, float>> params;

    for (unsigned i = 0; i < num_params_per_type; ++i) {
        params.emplace_back(std::string("TEST_BLA") + std::to_string(i), 42.0f + i);
    }

    return params;
}

std::vector<std::pair<std::string, int>> generate_int_params()
{
    std::vector<std::pair<std::string, int>> params;

    for (unsigned i = 0; i < num_params_per_type; ++i) {
        params.emplace_back(std::string("TEST_FOO") + std::to_string(i), 42.0f + i);
    }

    return params;
}

TEST(SystemTest, ParamGetAll)
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

    auto param_server = ParamServer{
        mavsdk_autopilot.server_component_by_type(Mavsdk::ServerComponentType::Autopilot)};

    auto fut = wait_for_first_system_detected(mavsdk_groundstation);
    ASSERT_EQ(fut.wait_for(std::chrono::seconds(10)), std::future_status::ready);
    auto system = fut.get();

    ASSERT_TRUE(system->has_autopilot());

    auto param = Param{system};

	const auto test_float_params=generate_float_params();
	const auto test_int_params=generate_int_params();
    // Add many params
    for (const auto& float_param : test_float_params) {
        EXPECT_EQ(
            param_server.provide_param_float(float_param.first, float_param.second),
            ParamServer::Result::Success);
    }
    for (const auto& int_param :  test_int_params) {
        EXPECT_EQ(
            param_server.provide_param_int(int_param.first, int_param.second),
            ParamServer::Result::Success);
    }

    const auto all_params = param.get_all_params();

    EXPECT_EQ(all_params.float_params.size(), test_float_params.size());
    EXPECT_EQ(all_params.int_params.size(), test_int_params.size());
	// Note: the order is not necessarily a requirement
	for(int i=0;i<test_float_params.size();i++){
		EXPECT_EQ(all_params.float_params.at(i), test_float_params.at(i));
	}
	for(int i=0;i<test_int_params.size();i++){
		EXPECT_EQ(all_params.int_params.at(i), test_int_params.at(i));
	}
    std::this_thread::sleep_for(std::chrono::seconds(1));
}
