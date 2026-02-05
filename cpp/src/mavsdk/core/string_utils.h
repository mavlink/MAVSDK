#pragma once

#include <string>

namespace mavsdk {

bool starts_with(const std::string& str, const std::string& prefix);

std::string strip_prefix(const std::string& str, const std::string& prefix);

} // namespace mavsdk
