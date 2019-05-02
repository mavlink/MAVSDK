#include <string>
#include <gmock/gmock.h>

#include "connection_result.h"

namespace dronecode_sdk {
namespace testing {

typedef std::function<void(uint64_t uuid)> event_callback_t;

class MockDronecodeSDK {
public:
    MOCK_CONST_METHOD1(add_any_connection, ConnectionResult(const std::string &)){};
    MOCK_CONST_METHOD1(register_on_discover, void(event_callback_t)){};
    MOCK_CONST_METHOD1(register_on_timeout, void(event_callback_t)){};
};

} // namespace testing
} // namespace dronecode_sdk
