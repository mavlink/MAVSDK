#pragma once

#include <string>
#include <optional>
#include "mavsdk_export.h"

namespace mavsdk {

MAVSDK_TEST_EXPORT std::optional<std::string> resolve_hostname_to_ip(const std::string& hostname);

} // namespace mavsdk
