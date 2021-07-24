#include <gmock/gmock.h>
#include "mavlink_mission_transfer.h"

namespace mavsdk {
namespace testing {

class MockSender : public Sender {
public:
    MockSender() : Sender() {}
    MOCK_METHOD(bool, send_message, (mavlink_message_t&), (override));
    MOCK_METHOD(uint8_t, get_own_system_id, (), (const, override));
    MOCK_METHOD(uint8_t, get_own_component_id, (), (const, override));
    MOCK_METHOD(uint8_t, get_system_id, (), (const, override));
    MOCK_METHOD(Autopilot, autopilot, (), (const, override));
};

} // namespace testing
} // namespace mavsdk
