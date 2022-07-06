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

static constexpr unsigned num_params_per_type = 3;

static std::map<std::string, float> generate_float_params(){
    std::map<std::string, float> params;
    for (unsigned i = 0; i < num_params_per_type; ++i) {
        const auto id=std::string("TEST_FLOAT") + std::to_string(i);
        const float value=42.0f + i;
        params[id]=value;
    }
    return params;
}

static std::map<std::string,int> generate_int_params(){
    std::map<std::string,int> params;
    for (unsigned i = 0; i < num_params_per_type; ++i) {
        const auto id=std::string("TEST_INT") + std::to_string(i);
        const int value=42 + i;
        params[id]=value;
    }
    return params;
}

static std::map<std::string,std::string> generate_string_params(){
  std::map<std::string,std::string> params;
  for (unsigned i = 0; i < num_params_per_type; ++i) {
	const auto id=std::string("TEST_STRING") + std::to_string(i);
	const std::string value= "VAL"+std::to_string(i);
	params[id]=value;
  }
  return params;
}
// compare originally provided and received values - they have a slightly different data layout.
template<typename T1,typename T2>
static void assert_equal(const std::map<std::string,T1>& values,const std::vector<T2>& received){
    EXPECT_EQ(received.size(), values.size());
    // check that all the parameters we got have the right param value
    for(const auto& param: received){
        EXPECT_EQ(param.value, values.find(param.name)->second);
    }
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

    auto param_sender = Param{system};

    const auto test_float_params=generate_float_params();
    const auto test_int_params=generate_int_params();
    const auto test_string_params=generate_string_params();
    // Add many params (these don't need extended)
    for (auto const& [key, val] : test_float_params){
        EXPECT_EQ(
            param_server.provide_param_float(key,val),
            ParamServer::Result::Success);
    }
    // We also add a couple of std::string parameters - note that they won't show up in get_all_params() when using the non-extended version
    // also note that we add the string parameters before then adding the int params, which also tests if the server handles the "hiding" properly
    // (now the indices of the non-ext params are different to the extended ones).
    for (auto const& [key, val] : test_string_params){
        EXPECT_EQ(
            param_server.provide_param_custom(key,val),
            ParamServer::Result::Success);
    }
    for (auto const& [key, val] : test_int_params){
        EXPECT_EQ(
            param_server.provide_param_int(key,val),
            ParamServer::Result::Success);
    }

    {
        // Here we use the non-extended protocol
        param_sender.late_init(1, false);
        const auto all_params = param_sender.get_all_params();
        assert_equal<int,Param::IntParam>(test_int_params,all_params.int_params);
        assert_equal<float,Param::FloatParam>(test_float_params,all_params.float_params);
    }
    {
        // now we do the same, but this time with the extended protocol
        param_sender.late_init(1, true);
        const auto all_params = param_sender.get_all_params();
        assert_equal<int,Param::IntParam>(test_int_params,all_params.int_params);
        assert_equal<float,Param::FloatParam>(test_float_params,all_params.float_params);
        assert_equal<std::string,Param::CustomParam>(test_string_params,all_params.custom_params);
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
}
