#include <iostream>
#include "integration_test_helper.h"
#include "dronecode_sdk.h"
#include "plugins/params_raw/params_raw.h"

using namespace dronecode_sdk;

TEST_F(SitlTest, ParamsRawSad)
{
    DronecodeSDK dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    auto &system = dc.system();
    ASSERT_TRUE(system.has_autopilot());

    auto params_raw = std::make_shared<ParamsRaw>(system);

    {
        const std::pair<ParamsRaw::Result, float> get_result =
            params_raw->get_param_float("SYS_HITL");
        EXPECT_EQ(get_result.first, ParamsRaw::Result::WRONG_TYPE);
    }

    {
        const std::pair<ParamsRaw::Result, int32_t> get_result =
            params_raw->get_param_int("MPC_VEL_MANUAL");
        EXPECT_EQ(get_result.first, ParamsRaw::Result::WRONG_TYPE);
    }

    {
        const std::pair<ParamsRaw::Result, int32_t> get_result =
            params_raw->get_param_int("NAME_TOO_LOOOOONG");
        EXPECT_EQ(get_result.first, ParamsRaw::Result::PARAM_NAME_TOO_LONG);
    }
}

TEST_F(SitlTest, ParamsRawHappy)
{
    DronecodeSDK dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    auto &system = dc.system();
    ASSERT_TRUE(system.has_autopilot());

    auto params_raw = std::make_shared<ParamsRaw>(system);

    {
        const std::pair<ParamsRaw::Result, int> get_result1 = params_raw->get_param_int("SYS_HITL");

        // Get initial value.
        ASSERT_EQ(get_result1.first, ParamsRaw::Result::SUCCESS);
        ASSERT_GE(get_result1.second, 0);
        ASSERT_LE(get_result1.second, 1);

        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Toggle the value.
        const ParamsRaw::Result set_result1 =
            params_raw->set_param_int("SYS_HITL", !get_result1.second);
        EXPECT_EQ(set_result1, ParamsRaw::Result::SUCCESS);

        // Verify toggle.
        const std::pair<ParamsRaw::Result, int> get_result2 = params_raw->get_param_int("SYS_HITL");
        EXPECT_EQ(get_result2.first, ParamsRaw::Result::SUCCESS);
        EXPECT_EQ(get_result2.second, !get_result1.second);

        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Reset back to initial value.
        const ParamsRaw::Result set_result2 =
            params_raw->set_param_int("SYS_HITL", get_result1.second);
        EXPECT_EQ(set_result2, ParamsRaw::Result::SUCCESS);

        // Verify reset.
        const std::pair<ParamsRaw::Result, int> get_result3 = params_raw->get_param_int("SYS_HITL");
        EXPECT_EQ(get_result3.first, ParamsRaw::Result::SUCCESS);
        EXPECT_EQ(get_result3.second, get_result1.second);
    }

    {
        const std::pair<ParamsRaw::Result, float> get_result1 =
            params_raw->get_param_float("MPC_VEL_MANUAL");

        // Get initial value.
        ASSERT_EQ(get_result1.first, ParamsRaw::Result::SUCCESS);
        ASSERT_GE(get_result1.second, 0);
        ASSERT_LE(get_result1.second, 1);

        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Toggle the value.
        const ParamsRaw::Result set_result1 =
            params_raw->set_param_float("MPC_VEL_MANUAL", get_result1.second + 1.0f);
        EXPECT_EQ(set_result1, ParamsRaw::Result::SUCCESS);

        // Verify toggle.
        const std::pair<ParamsRaw::Result, float> get_result2 =
            params_raw->get_param_float("MPC_VEL_MANUAL");
        EXPECT_EQ(get_result2.first, ParamsRaw::Result::SUCCESS);
        EXPECT_FLOAT_EQ(get_result2.second, get_result1.second + 1.0f);

        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Reset back to initial value.
        const ParamsRaw::Result set_result2 =
            params_raw->set_param_float("MPC_VEL_MANUAL", get_result1.second);
        EXPECT_EQ(set_result2, ParamsRaw::Result::SUCCESS);

        // Verify reset.
        const std::pair<ParamsRaw::Result, float> get_result3 =
            params_raw->get_param_float("MPC_VEL_MANUAL");
        EXPECT_EQ(get_result3.first, ParamsRaw::Result::SUCCESS);
        EXPECT_FLOAT_EQ(get_result3.second, get_result1.second);
    }
}
