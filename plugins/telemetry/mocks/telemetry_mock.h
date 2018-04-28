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
    MOCK_CONST_METHOD1(gps_info_async, void(Telemetry::gps_info_callback_t));
    MOCK_CONST_METHOD1(battery_async, void(Telemetry::battery_callback_t));
    MOCK_CONST_METHOD1(flight_mode_async, void(Telemetry::flight_mode_callback_t));
    MOCK_CONST_METHOD1(attitude_quaternion_async, void(Telemetry::attitude_quaternion_callback_t));
    MOCK_CONST_METHOD1(camera_attitude_quaternion_async,
                       void(Telemetry::attitude_quaternion_callback_t));
};

} // namespace testing
} // namespace dronecore
