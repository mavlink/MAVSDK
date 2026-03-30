#pragma once

#include <string>
#include "mavsdk_export.h"

namespace mavsdk {

MAVSDK_TEST_EXPORT bool starts_with(const std::string& str, const std::string& prefix);

MAVSDK_TEST_EXPORT std::string strip_prefix(const std::string& str, const std::string& prefix);

} // namespace mavsdk
