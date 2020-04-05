#pragma once

#include <cmath>
#include <vector>
#include <memory>
#include <functional>
#include <string>

#include "plugin_base.h"

#ifdef ERROR
#undef ERROR
#endif

namespace mavsdk {

class MissionImpl;
class System;

/**
 * @brief The Mission class enables waypoint missions.
 */
class Mission : public PluginBase {
public:
    /**
     * @brief Constructor. Creates the plugin for a specific System.
     *
     * The plugin is typically created as shown below:
     *
     *     ```cpp
     *     auto mission = std::make_shared<Mission>(system);
     *     ```
     *
     * @param system The specific system associated with this plugin.
     */
    explicit Mission(System& system);

    /**
     * @brief Destructor (internal use only).
     */
    ~Mission();

    /**
     * @brief Possible results returned for mission requests.
     */
    enum class Result {
        UNKNOWN, /**< @brief Unknown error. */
        SUCCESS, /**< @brief %Request succeeded. */
        ERROR, /**< @brief Error. */
        TOO_MANY_MISSION_ITEMS, /**< @brief Too many MissionItem objects in the mission. */
        BUSY, /**< @brief %Vehicle busy. */
        TIMEOUT, /**< @brief Request timed out. */
        INVALID_ARGUMENT, /**< @brief Invalid argument. */
        UNSUPPORTED, /**< @brief The mission downloaded from the system is not supported. */
        NO_MISSION_AVAILABLE, /**< @brief No mission available on system. */
        FAILED_TO_OPEN_QGC_PLAN, /**< @brief Failed to open QGroundControl plan. */
        FAILED_TO_PARSE_QGC_PLAN, /**< @brief Failed to parse QGroundControl plan. */
        UNSUPPORTED_MISSION_CMD, /**< @brief Unsupported mission command. */
        CANCELLED /**< @brief Mission upload or download has been cancelled. */
    };

    /**
     * @brief Possible camera actions at a mission item.
     * @sa to_str()
     */
    enum class CameraAction {
        NONE, /**< @brief No action. */
        TAKE_PHOTO, /**< @brief Take single photo. */
        START_PHOTO_INTERVAL, /**< @brief Start capturing photos at regular intervals - see
                                 set_camera_photo_interval(). */
        STOP_PHOTO_INTERVAL, /**< @brief Stop capturing photos at regular intervals. */
        START_VIDEO, /**< @brief Start capturing video. */
        STOP_VIDEO, /**< @brief Stop capturing video. */
    };

    /**
     * @brief A mission item containing a position and/or actions.
     *
     * Mission items are building blocks to assemble a mission.
     */
    struct MissionItem {
        double latitude_deg{double(NAN)}; /**< @brief Latitude in degrees (range: -90 to +90). */
        double longitude_deg{
            double(NAN)}; /**< @brief Longitude in degrees (range: -180 to +180). */
        float relative_altitude_m{
            NAN}; /**< @brief // Altitude relative to takeoff altitude in metres. */
        float speed_m_s{
            NAN}; /**< @brief Speed to use after this mission item (in metres/second). */
        bool fly_through{false}; /**< @brief True will make the drone fly through without stopping,
                                    while false will make the drone stop on the waypoint. */
        float gimbal_pitch_deg{NAN}; /**< @brief Gimbal pitch (in degrees). */
        float gimbal_yaw_deg{NAN}; /**< @brief Gimbal yaw (in degrees). */
        float loiter_time_s = {NAN}; /**< @brief Loiter time (in seconds). */
        CameraAction camera_action{}; /**< @brief Camera action to trigger at this mission item. */
        double camera_photo_interval_s =
            1.0; /**< @brief Camera photo interval to use after this mission item (in seconds). */
    };

    /**
     * @brief Gets a human-readable English string for an Mission::Result.
     *
     * @param result Enum for which string is required.
     * @return Human readable string for the Mission::Result.
     */
    static const char* result_str(Result result);

    /**
     * @brief Callback type for async mission calls.
     */
    typedef std::function<void(Result)> result_callback_t;

    /**
     * @brief Type for vector of mission items.
     */
    typedef std::vector<MissionItem> mission_items_t;

    /**
     * @brief Imports a **QGroundControl** (QGC) mission plan.
     *
     * The method composes the plan into a vector of MissionItem shared pointers that can
     * then be uploaded to a vehicle.
     * The method will fail if any of the imported mission items are not supported
     * by the MAVSDK API.
     *
     * @param[out] mission_items Vector of mission items imported from QGC plan.
     * @param qgc_plan_file File path of the QGC plan.
     * @sa [QGroundControl Plan file
     * format](https://dev.qgroundcontrol.com/en/file_formats/plan.html) (QGroundControl Dev Guide)
     * @return Result::SUCCESS if successful in importing QGC mission items.
     *     Otherwise one of the error codes: Result::FAILED_TO_OPEN_QGC_PLAN,
     *     Result::FAILED_TO_PARSE_QGC_PLAN, Result::UNSUPPORTED_MISSION_CMD.
     */
    static Result
    import_qgroundcontrol_mission(mission_items_t& mission_items, const std::string& qgc_plan_file);

    /**
     * @brief Uploads a vector of mission items to the system (asynchronous).
     *
     * The mission items are uploaded to a drone. Once uploaded the mission can be started and
     * executed even if a connection is lost.
     *
     * @param mission_items Reference to vector of mission items.
     * @param callback Callback to receive result of this request.
     */
    void
    upload_mission_async(const std::vector<MissionItem>& mission_items, result_callback_t callback);

    /**
     * @brief Cancel a mission upload (asynchronous).
     *
     * This cancels an ongoing mission upload. The mission upload will fail
     * with the result `Result::CANCELLED`.
     */
    void upload_mission_cancel();

    /**
     * @brief Callback type for `download_mission_async()` call to get mission items and result.
     */
    typedef std::function<void(Result, std::vector<MissionItem>)>
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
     * @brief Set whether to trigger Return-to-Launch (RTL) after mission is complete.
     *
     * This enables/disables to command RTL at the end of a mission.
     *
     * @note After setting this option, the mission needs to be re-uploaded.
     *
     * @param enable Enables RTL after mission is complete.
     */
    void set_return_to_launch_after_mission(bool enable);

    /**
     * @brief Get whether to trigger Return-to-Launch (RTL) after mission is complete.
     *
     * @note Before getting this option, it needs to be set, or a mission
     *       needs to be downloaded.
     *
     * @return True if RTL after mission is complete is enabled.
     */
    bool get_return_to_launch_after_mission();

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
     * @brief Sets the mission item index to go to (asynchronous).
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

    /**
     * @brief Equal operator to compare two `MissionItem` objects.
     *
     * @return `true` if items are equal.
     */
    friend bool operator==(const MissionItem& lhs, const MissionItem& rhs);

    // Non-copyable
    /**
     * @brief Copy constructor (object is not copyable).
     */
    Mission(const Mission&) = delete;
    /**
     * @brief Equality operator (object is not copyable).
     */
    const Mission& operator=(const Mission&) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    std::unique_ptr<MissionImpl> _impl;
};

} // namespace mavsdk
