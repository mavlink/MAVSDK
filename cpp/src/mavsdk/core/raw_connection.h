#pragma once

#include "connection.h"
#include <vector>
#include <cstdint>

namespace mavsdk {

/**
 * @brief Virtual connection for raw MAVLink bytes API
 *
 * This connection doesn't have actual I/O - it's used to:
 * 1. Distribute raw bytes to all real connections when sending
 * 2. Enable heartbeat sending when the raw bytes API is used
 */
class RawConnection : public Connection {
public:
    explicit RawConnection(
        ReceiverCallback receiver_callback,
        LibmavReceiverCallback libmav_receiver_callback,
        MavsdkImpl& mavsdk_impl,
        ForwardingOption forwarding_option = ForwardingOption::ForwardingOff);
    ~RawConnection() override;

    ConnectionResult start() override;
    ConnectionResult stop() override;

    std::pair<bool, std::string> send_message(const mavlink_message_t& message) override;
    std::pair<bool, std::string> send_raw_bytes(const char* bytes, size_t length) override;

    void receive(const char* bytes, size_t length);

    // Non-copyable
    RawConnection(const RawConnection&) = delete;
    const RawConnection& operator=(const RawConnection&) = delete;
};

} // namespace mavsdk
