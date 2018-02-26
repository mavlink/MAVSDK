#include <iostream>
#include <cmath>
#include "integration_test_helper.h"
#include "dronecore.h"
#include "plugins/action/action.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/offboard/offboard.h"
#include "plugins/mission/mission.h"

using namespace dronecore;


TEST_F(SitlTest, OffboardActuatorControlTarget)
{
    DroneCore dc;

    DroneCore::ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(DroneCore::ConnectionResult::SUCCESS, ret);

    // Wait for device to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    Device &device	= dc.device();
    auto telemetry	= std::make_shared<Telemetry>(&device);
    auto action		= std::make_shared<Action>(&device);
    auto offboard	= std::make_shared<Offboard>(&device);
    auto mission	= std::make_shared<Mission>(&device);

    while (!telemetry->health_all_ok()) {
        std::cout << "waiting for device to be ready" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    Action::Result action_ret = action->arm();
    ASSERT_EQ(Action::Result::SUCCESS, action_ret);

    // Send it once before starting offboard, otherwise it will be rejected.
    float val = 0.0f;
    float control_target[] = {val, val, val, val, val, val, val, val};
    uint8_t control_group  = 1; // Fixed wing control group
    offboard->set_actuator_control_target(control_target, control_group);

    Offboard::Result offboard_result = offboard->start();

    EXPECT_EQ(offboard_result, Offboard::Result::SUCCESS);

    offboard->set_actuator_control_target(control_target, control_group);
    {
        const float step_size = 0.01f;
        const float one_cycle = 2.0f * M_PI_F;
        const unsigned steps  = 2 * unsigned(one_cycle / step_size);

        for (unsigned i = 0; i < steps; ++i) {
            val = 1.0f * sinf(i * step_size);
            // This will move the flaps only
            float control_target_sine[] = {val, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
            offboard->set_actuator_control_target(control_target_sine, control_group);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    // Let's make sure that offboard knows it is active.
    EXPECT_TRUE(offboard->is_active());

    offboard_result = offboard->stop();
    EXPECT_EQ(offboard_result, Offboard::Result::SUCCESS);
}
