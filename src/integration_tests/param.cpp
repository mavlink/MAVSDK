#include <iostream>
#include "integration_test_helper.h"
#include "mavsdk.h"
#include "plugins/param/param.h"
#include <map>

using namespace mavsdk;

TEST(SitlTest, PX4ParamSad)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->has_autopilot());

    auto param = std::make_shared<Param>(system);

    {
        const std::pair<Param::Result, float> get_result = param->get_param_float("SYS_HITL");
        EXPECT_EQ(get_result.first, Param::Result::WrongType);
    }

    {
        const std::pair<Param::Result, int32_t> get_result = param->get_param_int("MPC_VEL_MANUAL");
        EXPECT_EQ(get_result.first, Param::Result::WrongType);
    }

    {
        const std::pair<Param::Result, int32_t> get_result =
            param->get_param_int("NAME_TOO_LOOOOONG");
        EXPECT_EQ(get_result.first, Param::Result::ParamNameTooLong);
    }

    {
        auto value = param->get_all_params();
        EXPECT_GT(value.float_params.size(), 0);
        EXPECT_GT(value.int_params.size(), 0);
    }
}

TEST(SitlTest, PX4ParamHappy)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->has_autopilot());

    auto param = std::make_shared<Param>(system);

    {
        const std::pair<Param::Result, int> get_result1 = param->get_param_int("SYS_HITL");

        // Get initial value.
        ASSERT_EQ(get_result1.first, Param::Result::Success);
        ASSERT_GE(get_result1.second, 0);
        ASSERT_LE(get_result1.second, 1);

        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Toggle the value.
        const Param::Result set_result1 = param->set_param_int("SYS_HITL", !get_result1.second);
        EXPECT_EQ(set_result1, Param::Result::Success);

        // Verify toggle.
        const std::pair<Param::Result, int> get_result2 = param->get_param_int("SYS_HITL");
        EXPECT_EQ(get_result2.first, Param::Result::Success);
        EXPECT_EQ(get_result2.second, !get_result1.second);

        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Reset back to initial value.
        const Param::Result set_result2 = param->set_param_int("SYS_HITL", get_result1.second);
        EXPECT_EQ(set_result2, Param::Result::Success);

        // Verify reset.
        const std::pair<Param::Result, int> get_result3 = param->get_param_int("SYS_HITL");
        EXPECT_EQ(get_result3.first, Param::Result::Success);
        EXPECT_EQ(get_result3.second, get_result1.second);
    }

    {
        const std::pair<Param::Result, float> get_result1 =
            param->get_param_float("MPC_VEL_MANUAL");

        // Get initial value.
        ASSERT_EQ(get_result1.first, Param::Result::Success);

        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Toggle the value.
        const Param::Result set_result1 =
            param->set_param_float("MPC_VEL_MANUAL", get_result1.second + 1.0f);
        EXPECT_EQ(set_result1, Param::Result::Success);

        // Verify toggle.
        const std::pair<Param::Result, float> get_result2 =
            param->get_param_float("MPC_VEL_MANUAL");
        EXPECT_EQ(get_result2.first, Param::Result::Success);
        EXPECT_FLOAT_EQ(get_result2.second, get_result1.second + 1.0f);

        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Reset back to initial value.
        const Param::Result set_result2 =
            param->set_param_float("MPC_VEL_MANUAL", get_result1.second);
        EXPECT_EQ(set_result2, Param::Result::Success);

        // Verify reset.
        const std::pair<Param::Result, float> get_result3 =
            param->get_param_float("MPC_VEL_MANUAL");
        EXPECT_EQ(get_result3.first, Param::Result::Success);
        EXPECT_FLOAT_EQ(get_result3.second, get_result1.second);
    }
}

TEST(SitlTest, GetAllParams)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    ASSERT_EQ(mavsdk.systems().size(), 1);

    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->has_autopilot());

    auto param = Param{system};

    auto all_params = param.get_all_params();

    std::map<std::string, float> all_mixed{};

    LogInfo() << "Int params: " << all_params.int_params.size();
    for (const auto& int_param : all_params.int_params) {
        std::cout << int_param.name << " : " << int_param.value << '\n';
        all_mixed[int_param.name] = static_cast<float>(int_param.value);
    }

    LogInfo() << "Float params: " << all_params.float_params.size();
    for (const auto& float_param : all_params.float_params) {
        std::cout << float_param.name << " : " << float_param.value << '\n';
        all_mixed[float_param.name] = float_param.value;
    }

    LogInfo() << "Combined params: " << all_mixed.size();
    for (const auto& mixed_param : all_mixed) {
        std::cout << mixed_param.first << " : " << mixed_param.second << '\n';
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));
}

TEST(SitlTest, APParam)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->has_autopilot());

    auto param = std::make_shared<Param>(system);

    Param::Result set_result1 = param->set_param_int("TERRAIN_ENABLE", 1);

    ASSERT_EQ(set_result1, Param::Result::Success);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    const std::pair<Param::Result, int> get_result2 = param->get_param_int("TERRAIN_ENABLE");
    EXPECT_EQ(get_result2.first, Param::Result::Success);
    EXPECT_EQ(get_result2.second, 1);
}
