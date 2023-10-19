#pragma once

#include <gmock/gmock.h>

namespace mavsdk {
namespace testing {

class MockSystem {
public:
    MOCK_CONST_METHOD0(is_connected, bool()){};
    MOCK_CONST_METHOD0(has_autopilot, bool()){};
};

} // namespace testing
} // namespace mavsdk
