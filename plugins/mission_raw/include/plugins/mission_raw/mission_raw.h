#pragma once

#include <vector>
#include <memory>
#include <functional>
#include "plugin_base.h"

namespace dronecode_sdk {

class MissionRawImpl;
class System;

/**
 * @brief The MissionRaw class enables direct direct access to MAVLink
 *        mission items.
 */
class MissionRaw : public PluginBase {
public:
    /**
     * @brief Constructor. Creates the plugin for a specific System.
     *
     * The plugin is typically created as shown below:
     *
     *     ```cpp
     *     auto mission_raw = std::make_shared<MissionRaw>(system);
     *     ```
     *
     * @param system The specific system associated with this plugin.
     */
    explicit MissionRaw(System &system);

    /**
     * @brief Destructor (internal use only).
     */
    ~MissionRaw();

    /**
     * @brief Possible results returned for mission requests.
     */
    enum class Result {
        UNKNOWN, /**< @brief Unknown error. */
        SUCCESS, /**< @brief %Request succeeded. */
        ERROR, /**< @brief Error. */
        BUSY, /**< @brief %Vehicle busy. */
        TIMEOUT, /**< @brief Request timed out. */
        INVALID_ARGUMENT, /**< @brief Invalid argument. */
        NO_MISSION_AVAILABLE, /**< @brief No mission available on system. */
        CANCELLED /**< @brief Mission upload or download has been cancelled. */
    };

    /**
     * @brief Gets a human-readable English string for an MissionRaw::Result.
     *
     * @param result Enum for which string is required.
     * @return Human readable string for the MissionRaw::Result.
     */
    static const char *result_str(Result result);

    /**
     * @brief Mission item identical to MAVLink MISSION_ITEM_INT.
     */
    struct MavlinkMissionItemInt {
        uint8_t target_system; /**< @brief System ID. */
        uint8_t target_component; /**< @brief Component ID. */
        uint16_t seq; /**< @brief Sequence. */
        uint8_t frame; /**< @brief The coordinate system of the waypoint. */
        uint16_t command; /**< @brief The scheduled action for the waypoint. */
        uint8_t current; /**< @brief false:0, true:1. */
        uint8_t autocontinue; /**< @brief Autocontinue to next waypoint. */
        float param1; /**< @brief PARAM1, see MAV_CMD enum. */
        float param2; /**< @brief PARAM2, see MAV_CMD enum. */
        float param3; /**< @brief PARAM3, see MAV_CMD enum. */
        float param4; /**< @brief PARAM4, see MAV_CMD enum. */
        int32_t x; /**< @brief PARAM5 / local: x position in meters * 1e4, global: latitude in
                      degrees * 10^7. */
        int32_t y; /**< @brief PARAM6 / y position: local: x position in meters * 1e4, global:
                      longitude in degrees *10^7. */
        float z; /**< @brief PARAM7 / local: Z coordinate, global: altitude (relative or absolute,
                    depending on frame). */
        uint8_t mission_type; /**< @brief Mission type. */
    };

    /**
     * @brief Type for vector of mission items.
     */
    typedef std::function<void(Result, std::vector<std::shared_ptr<MavlinkMissionItemInt>>)>
        mission_items_and_result_callback_t;

    /**
     * @brief Downloads a vector of mission items from the system (asynchronous).
     *
     * The method will fail if any of the downloaded mission items are not supported
     * by the Dronecode SDK API.
     *
     * @param callback Callback to receive mission items and result of this request.
     */
    void download_mission_async(mission_items_and_result_callback_t callback);

    /**
     * @brief Cancel a mission download (asynchronous).
     *
     * This cancels an ongoing mission download. The mission download will fail
     * with the result `Result::CANCELLED`.
     */
    void download_mission_cancel();

    /**
     * @brief Callback type to signal if the mission has changed.
     */
    typedef std::function<void()> mission_changed_callback_t;

    /**
     * @brief Subscribes to mission progress (asynchronous).
     *
     * @param callback Callback to receive mission progress.
     */
    void subscribe_mission_changed(mission_changed_callback_t callback);

    /**
     * @brief Copy constructor (object is not copyable).
     */
    MissionRaw(const MissionRaw &) = delete;
    /**
     * @brief Equality operator (object is not copyable).
     */
    const MissionRaw &operator=(const MissionRaw &) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    std::unique_ptr<MissionRawImpl> _impl;
};

} // namespace dronecode_sdk
