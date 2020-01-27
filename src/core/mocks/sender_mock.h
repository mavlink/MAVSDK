#include <gmock/gmock.h>
#include "mavlink_mission_transfer.h"

namespace mavsdk {
namespace testing {

class MockSender : public Sender {
public:
    MOCK_METHOD(bool, send_message, (const mavlink_message_t&), (override)) {};
};

} // namespace testing
} // namespace mavsdk
