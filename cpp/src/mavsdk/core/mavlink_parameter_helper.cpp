#include "mavlink_parameter_helper.h"
#include <cstring>

namespace mavsdk {

std::string extract_safe_param_id(const char* param_id)
{
    // The param_id field of the MAVLink struct has length 16 and can not be null terminated.
    // Therefore, we make a 0 terminated copy first.
    char param_id_long_enough[PARAM_ID_LEN + 1] = {};
    std::memcpy(param_id_long_enough, param_id, PARAM_ID_LEN);
    return {param_id_long_enough};
}

std::array<char, PARAM_ID_LEN> param_id_to_message_buffer(const std::string& param_id)
{
    std::array<char, PARAM_ID_LEN> ret = {};
    std::memcpy(ret.data(), param_id.c_str(), param_id.length());
    return ret;
}

} // namespace mavsdk
