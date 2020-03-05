#pragma once

//#include "mavlink_address.h"

namespace mavsdk {

class Configuration {
public:
    enum class UsageType;

    Configuration(uint8_t system_id, uint8_t component_id);
    Configuration(UsageType usage_type);
    ~Configuration();

    //Configuration(const Configuration&) = delete;
    //Configuration& operator=(const Configuration&) = delete;

    /*
     * @brief Get the system id of this configuration
     * @return `integer` system id, between 1 and 255
     */
    uint8_t get_system_id() const;

    /*
     * @brief Get the component id of this configuration
     * @return `integer` component id, between 1 and 255
     */
    uint8_t get_component_id() const;

    /**
     * @brief UsageTypes of configurations, used for automatic ID setting
     */
    enum class UsageType {
        Autopilot, /**< @brief SDK is used as an autopilot. */
        GroundStation, /**< @brief SDK is used as a ground station. */
        CompanionComputer, /**< @brief SDK is used as a companion computer on board the MAV. */
        Custom /**< @brief the SDK is used in a custom configuration, no automatic ID will be provided */
    };

    /**
     * @brief Usage type of this configuration
     * @return `UsageType` the usage type of this configuration
     */
    UsageType get_usage_type() const;

private:
    MAVLinkAddress _address;
    UsageType _usage_type;
};
} // namespace mavsdk
