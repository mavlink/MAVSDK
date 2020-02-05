#include <gmock/gmock.h>
#include "mavlink_mission_transfer.h"

namespace mavsdk {
namespace testing {

class MockSender : public Sender {
public:
    MockSender(MAVLinkAddress& own_address, MAVLinkAddress& target_address) :
        Sender(own_address, target_address)
    {}
    MOCK_METHOD(bool, send_message, (mavlink_message_t&), (override)){};
};

} // namespace testing
} // namespace mavsdk
