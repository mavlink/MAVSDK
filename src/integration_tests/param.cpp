#include <iostream>
#include "integration_test_helper.h"
#include "mavsdk.h"
#include "plugins/param/param.h"

using namespace mavsdk;

TEST_F(SitlTest, ParamSad)
{
    Mavsdk dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    auto& system = dc.system();
    ASSERT_TRUE(system.has_autopilot());

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
}

TEST_F(SitlTest, ParamHappy)
{
    Mavsdk dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    auto& system = dc.system();
    ASSERT_TRUE(system.has_autopilot());

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
