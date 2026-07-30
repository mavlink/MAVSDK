#include <gmock/gmock.h>
#include <cstdint>
#include <functional>
#include "compatibility_mode.hpp"
#include "mavlink_address.hpp"
#include <asio/io_context.hpp>

namespace mavsdk {
namespace testing {

class MockSender : public Sender {
public:
    MockSender() : Sender() {}
    MOCK_METHOD(bool, send_message, (mavlink_message_t&), (override));
    MOCK_METHOD(
        bool,
        queue_message,
        (std::function<mavlink_message_t(MavlinkAddress, uint8_t)>),
        (override));
    MOCK_METHOD(uint8_t, get_own_system_id, (), (const, override));
    MOCK_METHOD(uint8_t, get_own_component_id, (), (const, override));
    MOCK_METHOD(CompatibilityMode, compatibility_mode, (), (const, override));
    MOCK_METHOD(asio::io_context&, io_context, (), (override));
};

} // namespace testing
} // namespace mavsdk
