#pragma once

#include <cstdint>

/**
 * @brief Struct to represent a MAVLink address.
 */
struct MavlinkAddress {
    /**
     * @brief System ID, also called sysid.
     *
     * 32 bits wide to accommodate MAVLink's extended system IDs, which are
     * not supported yet.
     */
    uint32_t system_id;
    /**
     * @brief Component ID, also called compid.
     */
    uint8_t component_id;
};
