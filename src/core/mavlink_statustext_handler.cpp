#include "mavlink_statustext_handler.h"
#include "log.h"

namespace mavsdk {

std::pair<bool, std::string>
MavlinkStatustextHandler::process_severity(const mavlink_statustext_t& statustext)
{
    switch (statustext.severity) {
        case MAV_SEVERITY_EMERGENCY:
            return {true, "emergency"};
        case MAV_SEVERITY_ALERT:
            return {true, "alert"};
        case MAV_SEVERITY_CRITICAL:
            return {true, "critical"};
        case MAV_SEVERITY_ERROR:
            return {true, "error"};
        case MAV_SEVERITY_WARNING:
            return {true, "warning"};
        case MAV_SEVERITY_NOTICE:
            return {true, "notice"};
        case MAV_SEVERITY_INFO:
            return {true, "info"};
        case MAV_SEVERITY_DEBUG:
            return {true, "debug"};
        default:
            return {false, ""};
    }
}

std::pair<bool, std::string>
MavlinkStatustextHandler::process_text(const mavlink_statustext_t& statustext)
{
    char text_with_null[sizeof(statustext.text) + 1]{};
    strncpy(text_with_null, statustext.text, sizeof(text_with_null) - 1);

    if (statustext.id > 0) {
        if (statustext.id != _last_id) {
            _temp_multi_str = "";
            _last_chunk_seq = 0;
            _last_id = statustext.id;
        }

        // We can recover from missing chunks in-between but not if the first or last one is lost.
        if (_last_chunk_seq + 1 < statustext.chunk_seq) {
            _temp_multi_str += "[ missing ... ]";
        }

        _last_chunk_seq = statustext.chunk_seq;

        _temp_multi_str += text_with_null;

        if (strlen(text_with_null) == sizeof(statustext.text)) {
            // No zero termination yet, keep going.
            return {false, ""};
        } else {
            return {true, _temp_multi_str};
        }
    }

    return std::make_pair<bool, std::string>(true, text_with_null);
}

} // namespace mavsdk
