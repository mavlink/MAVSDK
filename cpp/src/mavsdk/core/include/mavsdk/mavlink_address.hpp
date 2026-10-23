#pragma once

#include <cstdint>

/**
 * @brief Struct to represent a MAVLink address.
 */
struct MavlinkAddress {
    /**
     * @brief System ID, also called sysid.
     *
     * 32 bits wide to accommodate MAVLink's extended system IDs. Values above
     * 255 are only carried by peers that understand them.
     */
    uint32_t system_id;
    /**
     * @brief Component ID, also called compid.
     */
    uint8_t component_id;
};
