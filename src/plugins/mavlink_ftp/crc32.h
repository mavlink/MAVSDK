#pragma once

#include <cstdint>

namespace mavsdk {

class Crc32 {
public:
    uint32_t add(const uint8_t* src, uint32_t len);

    int32_t get() { return val; }

private:
    uint32_t val{0};
};

} // namespace mavsdk