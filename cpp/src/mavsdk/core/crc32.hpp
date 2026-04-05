#pragma once

#include <cstdint>

namespace mavsdk {

// For more information about the CRC algorithm used, check the comment in the
// source file.

class Crc32 {
public:
    uint32_t add(const uint8_t* src, uint32_t len);

    [[nodiscard]] uint32_t get() const { return val; }

private:
    uint32_t val{0};
};

} // namespace mavsdk