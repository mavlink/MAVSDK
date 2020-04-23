#pragma once

#include <vector>
#include <memory>
#include <functional>

#include "plugin_base.h"

#ifdef ERROR
#undef ERROR
#endif

namespace mavsdk {

class MissionRawImpl;
class System;

/**
 * @brief The MissionRaw class enables direct access to MAVLink mission items.
 *
 * This plugin gives direct access to the MAVLink mission protocol which
 * means full control over waypoints and waypoint actions/commands. However,
 * this means that not all functionality provided is actually implemented or
 * correctly supported by a flight controller.
 *
 * For a tested, simpler subset in mission functionality, it is recommended
 * to use the `Mission` plugin.
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
    explicit MissionRaw(System& system);

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
        UNSUPPORTED, /**< @brief The mission downloaded from the system is not supported. */
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
    static const char* result_str(Result result);

    /**
     * @brief Mission item exactly identical to MAVLink MISSION_ITEM_INT.
     */
    struct MavlinkMissionItemInt {
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

        /**
         * @brief Equality between two MavlinkMissionItemInt
         *
         * @param other Another MavlinkMissionItemInt object
         *
         * @return 'true'  If the two MavlinkMissionItemInts are equal
         *         'false' Otherwise
         */
        bool operator==(const MavlinkMissionItemInt& other) const
        {
            return (
                seq == other.seq && frame == other.frame && command == other.command &&
                current == other.current && autocontinue == other.autocontinue &&
                param1 == other.param1 && param2 == other.param2 && param3 == other.param3 &&
                param4 == other.param4 && x == other.x && y == other.y && z == other.z &&
                mission_type == other.mission_type);
        }
    };

    /**
     * @brief Callback type for async mission calls.
     */
    typedef std::function<void(Result)> result_callback_t;

    /**
     * @brief Type for vector of mission items.
     */
    typedef std::function<void(Result, std::vector<std::shared_ptr<MavlinkMissionItemInt>>)>
        mission_items_and_result_callback_t;

    /**
     * @brief Downloads a vector of mission items from the system (asynchronous).
     *
     * The method will fail if any of the downloaded mission items are not supported
     * by the MAVSDK API.
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
     * @brief Uploads a vector of mission raw to the system (asynchronous).
     *
     * The mission raw are uploaded to a drone. Once uploaded the mission can be started and
     * executed even if a connection is lost.
     *
     * @param mission_raw Reference to vector of mission raw.
     * @param callback Callback to receive result of this request.
     */
    void upload_mission_async(
        const std::vector<std::shared_ptr<MissionRaw::MavlinkMissionItemInt>>& mission_raw,
        result_callback_t callback);

    /**
     * @brief Cancel a mission upload (asynchronous).
     *
     * This cancels an ongoing mission upload. The mission upload will fail
     * with the result `Result::CANCELLED`.
     */
    void upload_mission_cancel();

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
     * @brief Starts the mission (asynchronous).
     *
     * Note that the mission must be uploaded to the vehicle using `upload_mission_async()` before
     * this method is called.
     *
     * @param callback callback to receive result of this request.
     */
    void start_mission_async(result_callback_t callback);

    /**
     * @brief Pauses the mission (asynchronous).
     *
     * Pausing the mission puts the vehicle into
     * [HOLD mode](https://docs.px4.io/en/flight_modes/hold.html).
     * A multicopter should just hover at the spot while a fixedwing vehicle should loiter
     * around the location where it paused.
     *
     * @param callback Callback to receive result of this request.
     */
    void pause_mission_async(result_callback_t callback);

    /**
     * @brief Clears the mission saved on the vehicle (asynchronous).
     *
     * @param callback Callback to receive result of this request.
     */
    void clear_mission_async(result_callback_t callback);

    /**
     * @brief Sets the mavlink mission item index to go to (asynchronous).
     *
     * By setting the current index to 0, the mission is restarted from the beginning. If it is set
     * to a specific index of a mission item, the mission will be set to this item.
     *
     * Note that this is not necessarily true for general missions using MAVLink if loop counters
     * are used.
     *
     * Also not that the mission items need to be uploaded or downloaded before calling this
     * method. The exception is current == 0 to reset to the beginning
     *
     * @param current_mavlink Index for mission index to go to next (0 based).
     * @param callback Callback to receive result of this request.
     */
    void set_current_mavlink_mission_item_async(int current_mavlink, result_callback_t callback);

    /**
     * @brief Returns the reached mavlink mission item index (synchronous).
     *
     * The reached mavlink mission item is N and current mavlink mission is N+1
     * Note that reached and current are equal at the end of the mission.
     *
     * @return reached mavlink mission item index (0 based).
     */
    int reached_mavlink_mission_item() const;

    /**
     * @brief Returns the current mavlink mission item index (synchronous).
     *
     * The reached mavlink mission item is N and current mavlink mission is N+1
     * Note that reached and current are equal at the end of the mission.
     *
     * @return current mavlink mission item index (0 based).
     */
    int current_mavlink_mission_item() const;

    /**
     * @brief Returns the total number of mavlink mission items (synchronous).
     *
     * @return total number of mavlink mission items
     */
    int total_mavlink_mission_items() const;

    /**
     * @brief Callback type to receive current mission progress.
     *
     * The mission is finished if (reached + 1) == total.
     * Note that we never receive a reached for RTL.
     *
     * @param current_mavlink Current mavlink mission item index (0 based).
     * @param total Total number of mavlink mission items.
     */
    typedef std::function<void(int current_mavlink, int total)> progress_current_callback_t;

    /**
     * @brief Subscribes to mission progress current mavlink item(asynchronous).
     *
     * @param callback Callback to receive mission progress of current item.
     */
    void subscribe_progress_current(progress_current_callback_t callback);

    /**
     * @brief Callback type to receive reached mission progress.
     *
     * The mission is finished if (reached + 1) == total.
     * Note that we never receive a reached for RTL.
     *
     * @param reached_mavlink Reached mavlink mission item index (0 based).
     * @param total Total number of mavlink mission items.
     */
    typedef std::function<void(int reached_mavlink, int total)> progress_reached_callback_t;

    /**
     * @brief Subscribes to reached mission progress (asynchronous).
     *
     * @param callback Callback to receive reached mission progress.
     */
    void subscribe_progress_reached(progress_reached_callback_t callback);

    /**
     * @brief Copy constructor (object is not copyable).
     */
    MissionRaw(const MissionRaw&) = delete;
    /**
     * @brief Equality operator (object is not copyable).
     */
    const MissionRaw& operator=(const MissionRaw&) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    std::unique_ptr<MissionRawImpl> _impl;
};

} // namespace mavsdk
