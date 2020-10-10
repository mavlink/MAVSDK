#pragma once

#include <gmock/gmock.h>

namespace mavsdk {
namespace testing {

class MockSystem {
public:
    MOCK_CONST_METHOD0(is_connected, bool()){};
};

} // namespace testing
} // namespace mavsdk
