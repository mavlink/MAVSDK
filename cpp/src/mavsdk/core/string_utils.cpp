#include "string_utils.h"

namespace mavsdk {

bool starts_with(const std::string& str, const std::string& prefix)
{
    return str.compare(0, prefix.size(), prefix) == 0;
}

std::string strip_prefix(const std::string& str, const std::string& prefix)
{
    if (starts_with(str, prefix)) {
        return str.substr(prefix.size());
    }
    return str; // If no known prefix is found, return the original string
}

} // namespace mavsdk
