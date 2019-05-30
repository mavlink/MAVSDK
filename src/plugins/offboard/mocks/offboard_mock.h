#include <gmock/gmock.h>

#include "plugins/offboard/offboard.h"

namespace dronecode_sdk {
namespace testing {

class MockOffboard {
public:
    MOCK_CONST_METHOD0(start, Offboard::Result()){};
    MOCK_CONST_METHOD0(stop, Offboard::Result()){};
    MOCK_CONST_METHOD0(is_active, bool()){};
    MOCK_CONST_METHOD1(set_attitude, void(Offboard::Attitude)){};
    MOCK_CONST_METHOD1(set_attitude_rate, void(Offboard::AttitudeRate)){};
    MOCK_CONST_METHOD1(set_position_ned, void(Offboard::PositionNEDYaw)){};
    MOCK_CONST_METHOD1(set_velocity_body, void(Offboard::VelocityBodyYawspeed)){};
    MOCK_CONST_METHOD1(set_velocity_ned, void(Offboard::VelocityNEDYaw)){};
};

} // namespace testing
} // namespace dronecode_sdk
