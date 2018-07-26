#include <gmock/gmock.h>

#include "plugins/action/action_result.h"

namespace dronecode_sdk {
namespace testing {

class MockAction {
public:
    MOCK_CONST_METHOD0(arm, ActionResult());
    MOCK_CONST_METHOD0(disarm, ActionResult());
    MOCK_CONST_METHOD0(takeoff, ActionResult());
    MOCK_CONST_METHOD0(land, ActionResult());
    MOCK_CONST_METHOD0(kill, ActionResult());
    MOCK_CONST_METHOD0(return_to_launch, ActionResult());
    MOCK_CONST_METHOD0(transition_to_fixedwing, ActionResult());
    MOCK_CONST_METHOD0(transition_to_multicopter, ActionResult());
    MOCK_CONST_METHOD0(get_takeoff_altitude, std::pair<ActionResult, float>());
    MOCK_CONST_METHOD1(set_takeoff_altitude, ActionResult(float));
    MOCK_CONST_METHOD0(get_max_speed, std::pair<ActionResult, float>());
    MOCK_CONST_METHOD1(set_max_speed, ActionResult(float));
};

} // namespace testing
} // namespace dronecode_sdk
