#include <gmock/gmock.h>

#include "action/action_result.h"

namespace dronecore {
namespace testing {

class MockAction
{
public:
    MOCK_CONST_METHOD0(arm, ActionResult());
    MOCK_CONST_METHOD0(takeoff, ActionResult());
    MOCK_CONST_METHOD0(land, ActionResult());
};

} // namespace testing
} // namespace dronecore
