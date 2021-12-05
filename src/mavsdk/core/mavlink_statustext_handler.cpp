#include "mavlink_statustext_handler.h"
#include "log.h"

namespace mavsdk {

std::optional<MavlinkStatustextHandler::Statustext>
MavlinkStatustextHandler::process(const mavlink_statustext_t& statustext)
{
    char text_with_null[sizeof(statustext.text) + 1]{};
    strncpy(text_with_null, statustext.text, sizeof(text_with_null) - 1);
    auto severity = static_cast<MAV_SEVERITY>(statustext.severity);

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
            return std::nullopt;
        } else {
            return Statustext{_temp_multi_str, severity};
        }
    }

    return Statustext{text_with_null, severity};
}

std::string MavlinkStatustextHandler::severity_str(MAV_SEVERITY severity)
{
    switch (severity) {
        case MAV_SEVERITY_EMERGENCY:
            return "emergency";
        case MAV_SEVERITY_ALERT:
            return "alert";
        case MAV_SEVERITY_CRITICAL:
            return "critical";
        case MAV_SEVERITY_ERROR:
            return "error";
        case MAV_SEVERITY_WARNING:
            return "warning";
        case MAV_SEVERITY_NOTICE:
            return "notice";
        case MAV_SEVERITY_INFO:
            return "info";
        case MAV_SEVERITY_DEBUG:
            return "debug";
        default:
            return "unknown severity";
    }
}

} // namespace mavsdk
