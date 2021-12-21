#pragma once

namespace mavsdk {
namespace mavsdk_server {
namespace testing {

template<typename Plugin> class MockLazyPlugin {
public:
    MOCK_CONST_METHOD0(maybe_plugin, Plugin*()){};
};

} // namespace testing
} // namespace mavsdk_server
} // namespace mavsdk
