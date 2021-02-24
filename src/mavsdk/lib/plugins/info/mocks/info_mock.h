#include <gmock/gmock.h>

#include "plugins/info/info.h"

namespace mavsdk {
namespace testing {

class MockInfo {
public:
    MOCK_CONST_METHOD0(get_flight_information, std::pair<Info::Result, Info::FlightInfo>()){};
    MOCK_CONST_METHOD0(get_identification, std::pair<Info::Result, Info::Identification>()){};
    MOCK_CONST_METHOD0(get_product, std::pair<Info::Result, Info::Product>()){};
    MOCK_CONST_METHOD0(get_version, std::pair<Info::Result, Info::Version>()){};
    MOCK_CONST_METHOD0(get_speed_factor, std::pair<Info::Result, double>()){};
};

} // namespace testing
} // namespace mavsdk
