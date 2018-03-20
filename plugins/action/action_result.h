#pragma once

/**
 * @brief Namespace for all dronecore types.
 */
namespace dronecore {

/**
 * @brief Possible results returned for commanded actions.
 *
 * **Note**: DroneCore does not throw exceptions. Instead a result of this type will be
 * returned when you execute actions.
 */
enum class ActionResult {
    UNKNOWN, /**< @brief Unspecified error. */
    SUCCESS, /**< @brief Success. The action command was accepted by the vehicle. */
    NO_SYSTEM, /**< @brief No device is connected error. */
    CONNECTION_ERROR, /**< @brief %Connection error. */
    BUSY, /**< @brief Vehicle busy error. */
    COMMAND_DENIED, /**< @brief Command refused by vehicle. */
    COMMAND_DENIED_LANDED_STATE_UNKNOWN, /**< @brief Command refused because landed state is unknown. */
    COMMAND_DENIED_NOT_LANDED, /**< @brief Command refused because vehicle not landed. */
    TIMEOUT, /**< @brief Timeout waiting for command acknowledgement from vehicle. */
    VTOL_TRANSITION_SUPPORT_UNKNOWN, /**< @brief hybrid/VTOL transition refused because VTOL support is unknown. */
    NO_VTOL_TRANSITION_SUPPORT /**< @brief Vehicle does not support hybrid/VTOL transitions. */
};

/**
 * @brief Returns a human-readable English string for an ActionResult.
 *
 * @param result The enum value for which a human readable string is required.
 * @return Human readable string for the ActionResult.
 */
inline const char *action_result_str(ActionResult result)
{
    switch (result) {
        case ActionResult::SUCCESS:
            return "Success";
        case ActionResult::NO_SYSTEM:
            return "No system";
        case ActionResult::CONNECTION_ERROR:
            return "Connection error";
        case ActionResult::BUSY:
            return "Busy";
        case ActionResult::COMMAND_DENIED:
            return "Command denied";
        case ActionResult::COMMAND_DENIED_LANDED_STATE_UNKNOWN:
            return "Command denied, landed state is unknown";
        case ActionResult::COMMAND_DENIED_NOT_LANDED:
            return "Command denied, not landed";
        case ActionResult::TIMEOUT:
            return "Timeout";
        case ActionResult::VTOL_TRANSITION_SUPPORT_UNKNOWN:
            return "VTOL transition unknown";
        case ActionResult::NO_VTOL_TRANSITION_SUPPORT:
            return "No VTOL transition support";
        case ActionResult::UNKNOWN:
        default:
            return "Unknown";
    }
}

} // namespace dronecore
