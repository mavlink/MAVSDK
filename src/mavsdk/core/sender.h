#pragma once

#include "autopilot.h"
#include "mavlink_include.h"
#include "mavlink_address.h"
#include <cstdint>
#include <functional>

namespace mavsdk {

class Sender {
public:
    Sender() = default;
    virtual ~Sender() = default;
    virtual bool send_message(mavlink_message_t& message) = 0;
    virtual bool queue_message(
        std::function<mavlink_message_t(MavlinkAddress mavlink_address, uint8_t channel)> fun) = 0;
    [[nodiscard]] virtual uint8_t get_own_system_id() const = 0;
    [[nodiscard]] virtual uint8_t get_own_component_id() const = 0;
    [[nodiscard]] virtual Autopilot autopilot() const = 0;
};

} // namespace mavsdk
