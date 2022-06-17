#include "log.h"
#include "mavsdk.h"
#include "system_tests_helper.h"
#include "plugins/param/param.h"
#include "plugins/param_server/param_server.h"
#include <chrono>
#include <utility>
#include <vector>
#include <thread>
#include <map>

using namespace mavsdk;

static constexpr unsigned num_params_per_type = 100;

std::map<std::string, float> generate_float_params()
{
  	std::map<std::string, float> params;
    for (unsigned i = 0; i < num_params_per_type; ++i) {
	    const auto id=std::string("TEST_FLOAT") + std::to_string(i);
		const float value=42.0f + i;
	  	params[id]=value;
    }
    return params;
}

std::map<std::string,int> generate_int_params()
{
  	std::map<std::string,int> params;
    for (unsigned i = 0; i < num_params_per_type; ++i) {
	  	const auto id=std::string("TEST_INT") + std::to_string(i);
		const int value=42 + i;
	  	params[id]=value;
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
	for (auto const& [key, val] : test_float_params){
		EXPECT_EQ(
			param_server.provide_param_float(key,val),
			ParamServer::Result::Success);
	}
	for (auto const& [key, val] : test_int_params){
        EXPECT_EQ(
            param_server.provide_param_int(key,val),
            ParamServer::Result::Success);
    }

    const auto all_params = param.get_all_params();

    EXPECT_EQ(all_params.float_params.size(), test_float_params.size());
    EXPECT_EQ(all_params.int_params.size(), test_int_params.size());
	// check that all the parameters we got have the right param value
	for(const auto& param: all_params.float_params){
		EXPECT_EQ(param.value, test_float_params.find(param.name)->second);
	}
	for(const auto& param: all_params.int_params){
	  	EXPECT_EQ(param.value, test_int_params.find(param.name)->second);
	}
    std::this_thread::sleep_for(std::chrono::seconds(1));
}
