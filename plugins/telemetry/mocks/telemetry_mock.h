#include <gmock/gmock.h>

// TODO remove this include!
#include "telemetry/telemetry.h"

namespace dronecore {
namespace testing {

typedef std::function<void(Telemetry::Position)> position_callback_t;
typedef std::function<void(Telemetry::Health health)> health_callback_t;

class MockTelemetry
{
public:
    MOCK_CONST_METHOD1(position_async, void(position_callback_t));
    MOCK_CONST_METHOD1(health_async, void(health_callback_t));
};

} // namespace testing
} // namespace dronecore
