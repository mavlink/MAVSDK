#include <string>
#include <gmock/gmock.h>

#include "connection_result.h"
#include "system.h"

namespace mavsdk {
namespace testing {

typedef std::function<void()> ChangeCallback;

class MockMavsdk {
public:
    MOCK_CONST_METHOD1(add_any_connection, ConnectionResult(const std::string&)){};
    MOCK_CONST_METHOD1(subscribe_on_change, void(ChangeCallback)){};
    MOCK_CONST_METHOD0(systems, std::vector<std::shared_ptr<System>>()){};
};

} // namespace testing
} // namespace mavsdk
