#pragma once

#include "mission_item.h"
#include <vector>
#include <memory>
#include <functional>
#include "plugin_base.h"

namespace dronecore {

class MissionImpl;
class Device;

/**
 * @brief The Mission class enables waypoint missions.
 */
class Mission : public PluginBase
{
public:
    /**
     * @brief Constructor (internal use only).
     */
    explicit Mission(Device *device);

    /**
     * @brief Destructor (internal use only).
     */
    ~Mission();

    /**
     * @brief Possible results returned for mission requests.
     */
    enum class Result {
        SUCCESS = 0, /**< @brief %Request succeeded. */
        ERROR, /**< @brief Error. */
        TOO_MANY_MISSION_ITEMS, /**< @brief Too many MissionItem objects in the mission. */
        BUSY, /**< @brief %Vehicle busy. */
        TIMEOUT, /**< @brief Request timed out. */
        INVALID_ARGUMENT, /**< @brief Invalid argument. */
        UNSUPPORTED, /**< @brief The mission downloaded from the device is not supported. */
        NO_MISSION_AVAILABLE, /**< @brief No mission available on device. */
        UNKNOWN /**< @brief Unknown error. */
    };

    /**
     * @brief Gets a human-readable English string for an Mission::Result.
     *
     * @param result Enum for which string is required.
     * @return Human readable string for the Mission::Result.
     */
    static const char *result_str(Result result);

    /**
     * @brief Callback type for async mission calls.
     */
    typedef std::function<void(Result)> result_callback_t;

    /**
     * @brief Uploads a vector of mission items to the device (asynchronous).
     *
     * The mission items are uploaded to a drone. Once uploaded the mission can be started and
     * executed even if a connection is lost.
     *
     * @param mission_items Reference to vector of mission items.
     * @param callback Callback to receive result of this request.
     */
    void upload_mission_async(const std::vector<std::shared_ptr<MissionItem>> &mission_items,
                              result_callback_t callback);

    /**
     * @brief Callback type for `download_mission_async()` call to get mission items and result.
     */
    typedef std::function<void(Result, std::vector<std::shared_ptr<MissionItem>>)>
    mission_items_and_result_callback_t;

    /**
     * @brief Downloads a vector of mission items from the device (asynchronous).
     *
     * The method will fail if any of the downloaded mission items are not supported
     * by the DroneCore API.
     *
     * @param callback Callback to receive mission items and result of this request.
     */
    void download_mission_async(mission_items_and_result_callback_t callback);

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
     * @brief Sets the mission item index to go to (asynchronous).
     *
     * By setting the current index to 0, the mission is restarted from the beginning. If it is set
     * to a specific index of a mission item, the mission will be set to this item.
     *
     * Note that this is not necessarily true for general missions using MAVLink if loop counters
     * are used.
     *
     * @param current Index for mission index to go to next (0 based).
     * @param callback Callback to receive result of this request.
     */
    void set_current_mission_item_async(int current, result_callback_t callback);

    /**
     * @brief Checks if mission has been finished (synchronous).
     *
     * @return true if mission is finished and the last mission item has been reached.
     */
    bool mission_finished() const;

    /**
     * @brief Returns the current mission item index (synchronous).
     *
     * If the mission is finished, the current mission item will be the total number of mission
     * items (so the last mission item index + 1).
     *
     * @return current mission item index (0 based).
     */
    int current_mission_item() const;

    /**
     * @brief Returns the total number of mission items (synchronous).
     *
     * @return total number of mission items
     */
    int total_mission_items() const;

    /**
     * @brief Callback type to receive mission progress.
     *
     * The mission is finished if current == total.
     *
     * @param current Current mission item index (0 based).
     * @param total Total number of mission items.
     */
    typedef std::function<void(int current, int total)> progress_callback_t;

    /**
     * @brief Subscribes to mission progress (asynchronous).
     *
     * @param callback Callback to receive mission progress.
     */
    void subscribe_progress(progress_callback_t callback);

    // Non-copyable
    /**
     * @brief Copy constructor (object is not copyable).
     */
    Mission(const Mission &) = delete;
    /**
    * @brief Equality operator (object is not copyable).
    */
    const Mission &operator=(const Mission &) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    MissionImpl *_impl;
};

} // namespace dronecore
