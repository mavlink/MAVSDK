#include <gmock/gmock.h>

// TODO remove this include!
#include "action/action.h"

namespace dronecore {
namespace testing {

class MockAction
{
public:
    MOCK_CONST_METHOD0(arm, Action::Result());
    MOCK_CONST_METHOD0(takeoff, Action::Result());
    MOCK_CONST_METHOD0(land, Action::Result());
};

} // namespace testing
} // namespace dronecore
