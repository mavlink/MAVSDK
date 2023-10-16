#pragma once

#include <cstdint>

struct MavlinkAddress {
    uint8_t system_id;
    uint8_t component_id;
};
