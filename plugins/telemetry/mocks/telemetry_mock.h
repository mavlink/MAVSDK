#include <gmock/gmock.h>

#include "telemetry/telemetry.h"

namespace dronecore {
namespace testing {

class MockTelemetry
{
public:
    MOCK_CONST_METHOD1(position_async, void(Telemetry::position_callback_t));
    MOCK_CONST_METHOD1(health_async, void(Telemetry::health_callback_t));
    MOCK_CONST_METHOD1(home_position_async, void(Telemetry::position_callback_t));
    MOCK_CONST_METHOD1(in_air_async, void(Telemetry::in_air_callback_t));
    MOCK_CONST_METHOD1(armed_async, void(Telemetry::armed_callback_t));
};

} // namespace testing
} // namespace dronecore
