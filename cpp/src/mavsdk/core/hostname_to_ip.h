#pragma once

#include <string>
#include <optional>

namespace mavsdk {

std::optional<std::string> resolve_hostname_to_ip(const std::string& hostname);

} // namespace mavsdk
