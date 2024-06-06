#pragma once

#include <gmock/gmock.h>

#include "plugins/action/action.h"
#include "mocks/system_mock.h"

namespace mavsdk {
namespace testing {

class MockAction {
public:
    MOCK_CONST_METHOD0(arm, Action::Result()){};
    MOCK_CONST_METHOD0(arm_force, Action::Result()){};
    MOCK_CONST_METHOD0(disarm, Action::Result()){};
    MOCK_CONST_METHOD0(takeoff, Action::Result()){};
    MOCK_CONST_METHOD0(land, Action::Result()){};
    MOCK_CONST_METHOD0(reboot, Action::Result()){};
    MOCK_CONST_METHOD0(shutdown, Action::Result()){};
    MOCK_CONST_METHOD4(goto_location, Action::Result(double, double, float, float)){};
    MOCK_CONST_METHOD6(
        do_orbit, Action::Result(float, float, Action::OrbitYawBehavior, double, double, double)){};
    MOCK_CONST_METHOD0(terminate, Action::Result()){};
    MOCK_CONST_METHOD2(set_actuator, Action::Result(int, float)){};
    MOCK_CONST_METHOD0(kill, Action::Result()){};
    MOCK_CONST_METHOD0(return_to_launch, Action::Result()){};
    MOCK_CONST_METHOD0(hold, Action::Result()){};
    MOCK_CONST_METHOD0(transition_to_fixedwing, Action::Result()){};
    MOCK_CONST_METHOD0(transition_to_multicopter, Action::Result()){};
    MOCK_CONST_METHOD0(get_takeoff_altitude, std::pair<Action::Result, float>()){};
    MOCK_CONST_METHOD1(set_takeoff_altitude, Action::Result(float)){};
    MOCK_CONST_METHOD0(get_maximum_speed, std::pair<Action::Result, float>()){};
    MOCK_CONST_METHOD1(set_maximum_speed, Action::Result(float)){};
    MOCK_CONST_METHOD0(get_return_to_launch_altitude, std::pair<Action::Result, float>()){};
    MOCK_CONST_METHOD1(set_return_to_launch_altitude, Action::Result(float)){};
    MOCK_CONST_METHOD1(set_current_speed, Action::Result(float)){};
};

} // namespace testing
} // namespace mavsdk
