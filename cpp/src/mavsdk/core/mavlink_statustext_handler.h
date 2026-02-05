#pragma once

#include "mavlink_include.h"
#include <string>
#include <optional>

namespace mavsdk {

class MavlinkStatustextHandler {
public:
    MavlinkStatustextHandler() = default;
    ~MavlinkStatustextHandler() = default;

    struct Statustext {
        std::string text;
        MAV_SEVERITY severity;
    };

    std::optional<Statustext> process(const mavlink_statustext_t& statustext);

    static std::string severity_str(MAV_SEVERITY severity);

private:
    std::string _temp_multi_str{};
    uint16_t _last_id{0};
    uint8_t _last_chunk_seq{0};
};

} // namespace mavsdk
