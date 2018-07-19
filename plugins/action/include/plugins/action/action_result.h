#pragma once

/**
 * @brief Namespace for all dronecode_sdk types.
 */
namespace dronecode_sdk {

/**
 * @brief Possible results returned for commanded actions.
 *
 * @note DronecodeSDK does not throw exceptions. Instead a result of this type will be
 * returned when you execute actions.
 */
enum class ActionResult {
    UNKNOWN, /**< @brief Unspecified error. */
    SUCCESS, /**< @brief Success. The action command was accepted by the vehicle. */
    NO_SYSTEM, /**< @brief No system is connected error. */
    CONNECTION_ERROR, /**< @brief %Connection error. */
    BUSY, /**< @brief Vehicle busy error. */
    COMMAND_DENIED, /**< @brief Command refused by vehicle. */
    COMMAND_DENIED_LANDED_STATE_UNKNOWN, /**< @brief Command refused because landed state is
                                            unknown. */
    COMMAND_DENIED_NOT_LANDED, /**< @brief Command refused because vehicle not landed. */
    TIMEOUT, /**< @brief Timeout waiting for command acknowledgement from vehicle. */
    VTOL_TRANSITION_SUPPORT_UNKNOWN, /**< @brief hybrid/VTOL transition refused because VTOL support
                                        is unknown. */
    NO_VTOL_TRANSITION_SUPPORT, /**< @brief Vehicle does not support hybrid/VTOL transitions. */
    PARAMETER_ERROR /**< @brief Error getting or setting parameter. */
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
        case ActionResult::PARAMETER_ERROR:
            return "Parameter error";
        case ActionResult::UNKNOWN:
        default:
            return "Unknown";
    }
}

} // namespace dronecode_sdk
