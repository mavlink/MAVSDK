#include "mavlink_parameter_helper.hpp"
#include <algorithm>
#include <cstring>

namespace mavsdk {

std::string extract_safe_param_id(const char* param_id)
{
    if (param_id == nullptr) {
        return {};
    }

    // The param_id field of the MAVLink struct has length 16 and can not be null terminated.
    // Therefore, we make a 0 terminated copy first.
    char param_id_long_enough[PARAM_ID_LEN + 1] = {};
    std::memcpy(param_id_long_enough, param_id, PARAM_ID_LEN);
    return {param_id_long_enough};
}

std::array<char, PARAM_ID_LEN> param_id_to_message_buffer(const std::string& param_id)
{
    std::array<char, PARAM_ID_LEN> ret = {};
    // MAVLink param ids are at most 16 bytes; clamp to avoid writing past the fixed
    // buffer if a caller passes a longer string.
    const auto len = std::min(param_id.length(), PARAM_ID_LEN);
    std::memcpy(ret.data(), param_id.c_str(), len);
    return ret;
}

} // namespace mavsdk
