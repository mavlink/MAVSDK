#pragma once

#include <array>
#include <string>
#include "mavlink_include.h"

namespace mavsdk {

static constexpr size_t PARAM_ID_LEN = 16;

[[nodiscard]] std::string extract_safe_param_id(const char* param_id);

[[nodiscard]] std::array<char, PARAM_ID_LEN>
param_id_to_message_buffer(const std::string& param_id);

} // namespace mavsdk