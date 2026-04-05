#pragma once

#include <string>
#include <memory>
#include <gmock/gmock.h>

#include "connection_result.hpp"
#include "system_mock.hpp"

namespace mavsdk {
namespace testing {

using NewSystemCallback = std::function<void()>;

class MockMavsdk {
public:
    MOCK_CONST_METHOD1(add_any_connection, ConnectionResult(const std::string&)) {};
    MOCK_CONST_METHOD1(subscribe_on_new_system, void(NewSystemCallback)) {};
    MOCK_CONST_METHOD0(systems, std::vector<std::shared_ptr<MockSystem>>()) {};
    MOCK_CONST_METHOD1(set_timeout_s, void(double)) {};
};

} // namespace testing
} // namespace mavsdk
