#include <gmock/gmock.h>

#include "plugins/offboard/offboard.h"

namespace mavsdk {
namespace testing {

class MockOffboard {
public:
    MOCK_CONST_METHOD0(start, Offboard::Result()){};
    MOCK_CONST_METHOD0(stop, Offboard::Result()){};
    MOCK_CONST_METHOD0(is_active, std::pair<Offboard::Result, bool>()){};
    MOCK_CONST_METHOD1(set_attitude, Offboard::Result(Offboard::Attitude)){};
    MOCK_CONST_METHOD1(set_attitude_rate, Offboard::Result(Offboard::AttitudeRate)){};
    MOCK_CONST_METHOD1(set_position_ned, Offboard::Result(Offboard::PositionNedYaw)){};
    MOCK_CONST_METHOD1(set_velocity_body, Offboard::Result(Offboard::VelocityBodyYawspeed)){};
    MOCK_CONST_METHOD1(set_velocity_ned, Offboard::Result(Offboard::VelocityNedYaw)){};
    MOCK_CONST_METHOD1(set_actuator_control, Offboard::Result(Offboard::ActuatorControl)){};
};

} // namespace testing
} // namespace mavsdk
