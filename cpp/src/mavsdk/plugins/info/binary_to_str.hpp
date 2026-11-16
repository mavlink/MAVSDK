#pragma once

#include <cstdint>
#include <string>
#include "mavsdk_export.h"

namespace mavsdk {

MAVSDK_TEST_EXPORT std::string
swap_and_translate_binary_to_str(const uint8_t* binary, unsigned binary_len);
MAVSDK_TEST_EXPORT std::string translate_binary_to_str(const uint8_t* binary, unsigned binary_len);

} // namespace mavsdk
