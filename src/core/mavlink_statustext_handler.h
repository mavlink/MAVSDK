#pragma once

#include "mavlink_include.h"
#include <string>
#include <utility>

namespace mavsdk {

class MavlinkStatustextHandler {
public:
    MavlinkStatustextHandler() = default;
    ~MavlinkStatustextHandler() = default;

    std::pair<bool, std::string> process_severity(const mavlink_statustext_t& statustext);
    std::pair<bool, std::string> process_text(const mavlink_statustext_t& statustext);

private:
    std::string _temp_multi_str{};
    uint16_t _last_id{0};
    uint8_t _last_chunk_seq{0};
};

} // namespace mavsdk
