#include <gmock/gmock.h>

#include "action/action_result.h"

namespace dronecore {
namespace testing {

class MockAction
{
public:
    MOCK_CONST_METHOD0(arm, ActionResult());
    MOCK_CONST_METHOD0(disarm, ActionResult());
    MOCK_CONST_METHOD0(takeoff, ActionResult());
    MOCK_CONST_METHOD0(land, ActionResult());
    MOCK_CONST_METHOD0(kill, ActionResult());
    MOCK_CONST_METHOD0(return_to_launch, ActionResult());
    MOCK_CONST_METHOD0(transition_to_fixedwing, ActionResult());
    MOCK_CONST_METHOD0(transition_to_multicopter, ActionResult());
};

} // namespace testing
} // namespace dronecore
