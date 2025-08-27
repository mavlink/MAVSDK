#pragma once

namespace mavsdk {

/**
 * @brief ComponentType of configurations, used for automatic ID setting
 */
enum class ComponentType {
    Autopilot, /**< @brief SDK is used as an autopilot. */
    GroundStation, /**< @brief SDK is used as a ground station. */
    CompanionComputer, /**< @brief SDK is used as a companion computer on board the MAV. */
    Camera, /** < @brief SDK is used as a camera. */
    Gimbal, /** < @brief SDK is used as a gimbal. */
    RemoteId, /** < @brief SDK is used as a RemoteId system. */
    Custom /**< @brief the SDK is used in a custom configuration, no automatic ID will be
              provided */
};

} // namespace mavsdk
