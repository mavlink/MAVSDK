#pragma once

#include "mission_item.h"
#include <vector>
#include <memory>
#include <functional>

namespace dronecore {

class MissionImpl;

/**
 * The Mission class enables waypoint missions.
 */
class Mission
{
public:
    /**
     * Constructor for Mission called internally.
     */
    explicit Mission(MissionImpl *impl);

    /**
     * Destructor for Mission called internally.
     */
    ~Mission();

    /**
     * Possible results returned for mission requests.
     */
    enum class Result {
        SUCCESS = 0,
        ERROR,
        TOO_MANY_MISSION_ITEMS,
        BUSY,
        TIMEOUT,
        INVALID_ARGUMENT,
        UNKNOWN
    };

    /**
     * Returns a human-readable English string for an Mission::Result.
     */
    static const char *result_str(Result result);

    /**
     * Callback type for async mission calls.
     */
    typedef std::function<void(Result)> result_callback_t;

    /**
     * Sends a vector of mission items to the device (asynchronous).
     *
     * The mission items are uploaded to a drone and can then be started and executed even if a
     * connection is lost.
     *
     * @param mission_items reference to vector of mission items.
     * @param callback callback to receive result of this request
     */
    void send_mission_async(const std::vector<std::shared_ptr<MissionItem>> &mission_items,
                            result_callback_t callback);

    /**
     * Starts the mission (asynchronous).
     *
     * This needs the mission to be sent beforehand using send_mission_async.
     *
     * @param callback callback to receive result of this request
     */
    void start_mission_async(result_callback_t callback);

    /**
     * Pauses the mission (asynchronous).
     *
     * When a mission is paused a drone should just hover at the spot and a fixedwing vehicle
     * should loiter around the spot where it paused.
     *
     * @param callback callback to receive result of this request
     */
    void pause_mission_async(result_callback_t callback);

    /**
     * Sets the mission item index to go to (asynchronous).
     *
     * By setting the current index to 0, the mission is restarted from the beginning. If it is set
     * to a specific index of a mission item, the mission will be set to this item.
     *
     * @param current index for mission index to go to next (0 based)
     * @param callback callback to receive result of this request.
     */
    void set_current_mission_item_async(int current, result_callback_t callback);

    /**
     * Checks if mission has been finished (synchronous).
     *
     * @return true if mission is finished and the last mission item has been reached.
     */
    bool mission_finished() const;

    /**
     * Returns the current mission item index (synchronous).
     *
     * If the mission is finished, the current mission item will be the total number of mission
     * items (so the last mission item index + 1).
     *
     * @return current mission item index (0 based).
     */
    int current_mission_item() const;

    /**
     * Returns the total number of mission items (synchronous).
     *
     * @return total number of mission items
     */
    int total_mission_items() const;

    /**
     * Callback type to receive mission progress.
     *
     * The mission is finished if current == total.
     *
     * @param current current mission item index (0 based)
     * @param total total number of mission items
     */
    typedef std::function<void(int current, int total)> progress_callback_t;

    /**
     * Subscribes to mission progress (asynchronous).
     *
     * @param callback callback to receive mission progress
     */
    void subscribe_progress(progress_callback_t callback);

    // Non-copyable
    Mission(const Mission &) = delete;
    const Mission &operator=(const Mission &) = delete;

private:
    // Underlying implementation, set at instantiation
    MissionImpl *_impl;
};

} // namespace dronecore
