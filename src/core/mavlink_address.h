#pragma once

#include <cstdint>

struct MAVLinkAddress {
    uint8_t system_id;
    uint8_t component_id;
};
