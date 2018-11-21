#include <gmock/gmock.h>

#include "plugins/action/action.h"

namespace dronecode_sdk {
namespace testing {

class MockAction {
public:
    MOCK_CONST_METHOD0(arm, Action::Result()){};
    MOCK_CONST_METHOD0(disarm, Action::Result()){};
    MOCK_CONST_METHOD0(takeoff, Action::Result()){};
    MOCK_CONST_METHOD0(land, Action::Result()){};
    MOCK_CONST_METHOD0(kill, Action::Result()){};
    MOCK_CONST_METHOD0(return_to_launch, Action::Result()){};
    MOCK_CONST_METHOD0(transition_to_fixedwing, Action::Result()){};
    MOCK_CONST_METHOD0(transition_to_multicopter, Action::Result()){};
    MOCK_CONST_METHOD0(get_takeoff_altitude, std::pair<Action::Result, float>()){};
    MOCK_CONST_METHOD1(set_takeoff_altitude, Action::Result(float)){};
    MOCK_CONST_METHOD0(get_max_speed, std::pair<Action::Result, float>()){};
    MOCK_CONST_METHOD1(set_max_speed, Action::Result(float)){};
    MOCK_CONST_METHOD0(get_return_to_launch_return_altitude, std::pair<Action::Result, float>()){};
    MOCK_CONST_METHOD1(set_return_to_launch_return_altitude, Action::Result(float)){};
};

} // namespace testing
} // namespace dronecode_sdk
