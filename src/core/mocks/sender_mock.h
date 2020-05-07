#include <gmock/gmock.h>
#include "mavlink_mission_transfer.h"

namespace mavsdk {
namespace testing {

class MockSender : public Sender {
public:
    MockSender(MAVLinkAddress& new_own_address, MAVLinkAddress& new_target_address) :
        Sender(new_own_address, new_target_address)
    {}
    MOCK_METHOD(bool, send_message, (mavlink_message_t&), (override)){};
};

} // namespace testing
} // namespace mavsdk
