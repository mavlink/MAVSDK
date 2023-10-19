#pragma once

#include <cstdint>

/**
 * @brief Struct to represent a MAVLink address.
 */
struct MavlinkAddress {
    /**
     * @brief System ID, also called sysid.
     */
    uint8_t system_id;
    /**
     * @brief Component ID, also called compid.
     */
    uint8_t component_id;
};
