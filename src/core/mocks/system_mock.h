#pragma once

#include <gmock/gmock.h>

namespace mavsdk {
namespace testing {

class MockSystem {
public:
    MOCK_CONST_METHOD0(is_connected, bool()){};
    MOCK_CONST_METHOD0(get_uuid, uint64_t()){};
};

} // namespace testing
} // namespace mavsdk
