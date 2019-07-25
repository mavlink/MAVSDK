#include <gmock/gmock.h>
#include <memory>
#include <vector>

#include "plugins/geofence/geofence.h"

namespace mavsdk {
namespace testing {

class MockGeofence {
public:
    MOCK_CONST_METHOD2(
        send_geofence_async,
        void(const std::vector<std::shared_ptr<Polygon>>&, Geofence::result_callback_t)){};
};

} // namespace testing
} // namespace mavsdk
