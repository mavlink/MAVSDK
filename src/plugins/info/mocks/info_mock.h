#include <gmock/gmock.h>

#include "plugins/info/info.h"

namespace dronecode_sdk {
namespace testing {

class MockInfo {
public:
    MOCK_CONST_METHOD0(get_version, std::pair<Info::Result, Info::Version>()){};
};

} // namespace testing
} // namespace dronecode_sdk
