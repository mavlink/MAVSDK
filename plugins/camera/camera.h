#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include "plugin_base.h"

namespace dronecore {

class CameraImpl;
class System;

/**
 * @brief The Camera class can be used to manage cameras that implement the
 * MAVLink Camera Protocol: https://mavlink.io/en/protocol/camera.html.
 *
 * Currently only a single camera is supported.
 * When multiple cameras are supported the plugin will need to be
 * instantiated separately for every camera.
 *
 * Synchronous and asynchronous variants of the camera methods are supplied.
 */
class Camera : public PluginBase
{
public:
    /**
     * @brief Constructor. Creates the plugin for a specific System.
     *
     * The plugin is typically created as shown below:
     *
     *     ```cpp
     *     auto camera = std::make_shared<Camera>(system);
     *     ```
     *
     * @param system The specific system associated with this plugin.
     */
    explicit Camera(System &system);

    /**
     * @brief Destructor (internal use only).
     */
    ~Camera();

    /**
     * @brief Possible results returned for camera commands.
     */
    enum class Result {
        SUCCESS = 0, /**< @brief Camera command executed successfully. */
        IN_PROGRESS, /**< @brief Camera command is in progress. */
        BUSY, /**< @brief Camera is busy and rejected command. */
        DENIED, /**< @brief Camera has denied the command. */
        ERROR, /**< @brief An error has occurred while executing the command. */
        TIMEOUT, /**< @brief Camera has not responded in time and the command has timed out. */
        WRONG_ARGUMENT, /**< @brief The command has wrong arguments. */
        UNKNOWN /**< @brief The result is unknown. */
    };

    /**
     * @brief Returns a human-readable English string for Camera::Result.
     *
     * @param result The enum value for which a human readable string is required.
     * @return Human readable string for the Camera::Result.
     */
    static std::string result_str(Result result);

    /**
     * @brief Callback type for asynchronous Camera calls.
     */
    typedef std::function<void(Result)> result_callback_t;

    /**
     * @brief Take photo (synchronous).
     *
     * This takes one photo.
     *
     * @return Result of request.
     */
    Result take_photo();

    /**
     * @brief Start photo interval (synchronous).
     *
     * Starts a photo timelapse with a given interval.
     * @sa stop_photo_interval()
     * @param interval_s The interval between photos in seconds.
     * @return Result of request.
     */
    Result start_photo_interval(float interval_s);

    /**
     * @brief Stop photo interval (synchronous).
     *
     * Stops a photo timelapse, previously started with start_photo_interval().
     *
     * @return Result of request.
     */
    Result stop_photo_interval();

    /**
     * @brief Start video capture (synchronous).
     *
     * This starts a video recording.
     *
     * @return Result of request.
     */
    Result start_video();

    /**
     * @brief Stop video capture (synchronous).
     *
     * This stops a video recording, previously started with start_video().
     *
     * @return Result of request.
     */
    Result stop_video();

    /**
     * @brief Take photo (asynchronous).
     *
     * This takes one photo.
     *
     * @param callback Function to call with result of request.
     */
    void take_photo_async(const result_callback_t &callback);

    /**
     * @brief Start photo interval (asynchronous).
     *
     * Starts a photo timelapse with a given interval.
     *
     * @sa stop_photo_interval_async
     * @param interval_s The interval between photos in seconds.
     * @param callback Function to call with result of request.
     */
    void start_photo_interval_async(float interval_s, const result_callback_t &callback);

    /**
     * @brief Stop photo interval (asynchronous).
     *
     * Stops a photo timelapse, previously started with start_photo_interval_async().
     *
     * @param callback Function to call with result of request.
     */
    void stop_photo_interval_async(const result_callback_t &callback);

    /**
     * @brief Start video capture (asynchronous).
     *
     * This starts a video recording.
     *
     * @param callback Function to call with result of request.
     */
    void start_video_async(const result_callback_t &callback);

    /**
     * @brief Stop video capture (asynchronous).
     *
     * This stops a video recording, previously started with start_video_async().
     *
     * @param callback Function to call with result of request.
     */
    void stop_video_async(const result_callback_t &callback);


    /**
     * @brief Camera mode type.
     */
    enum class Mode {
        PHOTO, /**< @brief Photo mode. */
        VIDEO, /**< @brief Video mode. */
        UNKNOWN /**< @brief Unknown mode. */
    };

    /**
     * @brief Callback type for asynchronous camera mode calls.
     */
    typedef std::function<void(Result, const Mode &)> mode_callback_t;

    /**
     * @brief Setter for camera mode (asynchronous).
     *
     * @param mode Camera mode to set.
     * @param callback Function to call with result of request.
     */
    void set_mode_async(Mode mode, const mode_callback_t &callback);

    /**
     * @brief Getter for camera mode (asynchronous).
     *
     * @param callback Function to call with result of request.
     */
    void get_mode_async(const mode_callback_t &callback);

    /**
     * @brief Information about a picture just captured.
     */
    struct CaptureInfo {
        /**
         * @brief Position type.
         */
        struct Position {
            double latitude_deg; /**< @brief Latitude in degrees (range: -90 to +90). */
            double longitude_deg; /**< @brief Longitude in degrees (range: -180 to 180). */
            float absolute_altitude_m; /**< @brief Altitude AMSL (above mean sea level) in metres. */
            float relative_altitude_m; /**< @brief Altitude relative to takeoff altitude in metres. */
        } position; /**< @brief Position of drone/camera when image was captured. */

        /**
         * @brief Quaternion type.
         *
         * All rotations and axis systems follow the right-hand rule.
         * The Hamilton quaternion product definition is used.
         * A zero-rotation quaternion is represented by (1,0,0,0).
         * The quaternion could also be written as w + xi + yj + zk.
         *
         * For more info see: https://en.wikipedia.org/wiki/Quaternion
         */
        struct Quaternion {
            float w; /**< @brief Quaternion entry 0 also denoted as a. */
            float x; /**< @brief Quaternion entry 1 also denoted as b. */
            float y; /**< @brief Quaternion entry 2 also denoted as c. */
            float z; /**< @brief Quaternion entry 3 also denoted as d. */
        } quaternion; /**< @brief Quaternion of camera orientation. */

        uint64_t time_utc_us; /**< @brief Timestamp in UTC (since UNIX epoch) in microseconds. */
        bool success; /**< @brief True if capture was successful. */
        int index; /**< @brief Zero-based index of this image since armed. */
        std::string file_url; /**< Download URL for captured image. */
    };

    /**
     * @brief Type for video stream settings.
     *
     * Application may call set_video_stream_settings() before starting
     * video streaming to tell Camera server to use these settings during video streaming.
     *
     * @sa set_video_stream_settings(), get_video_stream_info(), start_video_streaming().
     */
    struct VideoStreamSettings {
        float frame_rate_hz = 0.f; /**< @brief Frames per second. */
        uint16_t horizontal_resolution_pix = 0u; /**< @brief Horizontal resolution in pixels. */
        uint16_t vertical_resolution_pix = 0u; /**< @brief Vertical resolution in pixels. */
        uint32_t bit_rate_b_s = 0u; /**< @brief Bit rate in bits per second. */
        uint16_t rotation_deg = 0u; /**< @brief Video image rotation clockwise (0-359 degrees). */
        std::string uri {}; /**< @brief Video stream URI. */

        /**
         * @brief Sets to highest possible settings for Resolution, framerate.
         */
        void set_highest()
        {
            frame_rate_hz = FRAME_RATE_HIGHEST;
            horizontal_resolution_pix = RESOLUTION_H_HIGHEST;
            vertical_resolution_pix = RESOLUTION_V_HIGHEST;
            // FIXME: Should bit_rate_b_s be part of set_highest() ?
            bit_rate_b_s = BIT_RATE_AUTO;
        }

        constexpr static const float FRAME_RATE_HIGHEST = -1.0f; /**< @brief Highest possible framerate. */
        constexpr static const uint16_t RESOLUTION_H_HIGHEST =
            UINT16_MAX; /**< @brief Highest possible horizontal resolution. */
        constexpr static const uint16_t RESOLUTION_V_HIGHEST =
            UINT16_MAX; /**< @brief Highest possible vertical resolution. */
        constexpr static const uint32_t BIT_RATE_AUTO = 0; /**< @brief Auto settings for Bit rate. */
    };

    /**
     * @brief Sets video stream settings.
     *
     * @param settings Reference to custom video stream settings which include resolution,
     * framerate, bitrate, etc.
     */
    void set_video_stream_settings(const VideoStreamSettings &settings);

    /**
     * @brief Information about video stream.
     */
    struct VideoStreamInfo {
        VideoStreamSettings settings; /**< @brief Video stream settings. */
        enum class Status {
            NOT_RUNNING = 0, /**< @brief Video stream is not ongoing. */
            IN_PROGRESS /**< @brief Video stream in progress. */
        } status; /**< @brief Current status of video streaming. */
    };

    /**
     * @brief Get video stream information (synchronous).
     *
     * Application may use media player like VLC and feed `uri`
     * to play the ongoing video streaming.
     *
     * @param info Video stream information will be filled.
     *
     * @return SUCCESS if video stream info is received, error otherwise.
     */
    Result get_video_stream_info(VideoStreamInfo &info);

    /**
     * @brief Starts video streaming (synchronous).
     *
     * Sends a request to start video streaming.
     *
     * @sa stop_video_streaming()
     *
     * @return SUCCESS if video streaming is started, error otherwise.
     */
    Result start_video_streaming();

    /**
     * @brief Stop the current video streaming (synchronous).
     *
     * Sends a request to stop ongoing video streaming.
     *
     * @sa start_video_streaming()
     *
     * @return SUCCESS if video streaming is stopped, error otherwise.
     */
    Result stop_video_streaming();

    /**
     * @brief Callback type for capture info updates.
     */
    typedef std::function<void(CaptureInfo)> capture_info_callback_t;

    /**
     * @brief Subscribe to capture info updates (asynchronous).
     *
     * @param callback Function to call with updates.
     */
    void capture_info_async(capture_info_callback_t callback);

    /**
     * @brief Information about camera status.
     */
    struct Status {
        bool video_on; /**< @brief true if video capture is currently running. */
        bool photo_interval_on; /**< @brief true if video timelapse is currently active. */

        enum class StorageStatus {
            NOT_AVAILABLE, /**< @brief Storage status not available. */
            UNFORMATTED, /**< @brief Storage is not formatted (has no recognized file system). */
            FORMATTED /**< @brief Storage is formatted (has recognized a file system). */
        } storage_status; /**< @brief Storage status. */
        float used_storage_mib; /**< @brief Used storage in MiB. */
        float available_storage_mib; /**< @brief Available storage in MiB. */
        float total_storage_mib; /**< @brief Total storage in MiB. */
    };

    /**
     * @brief Callback type to get status.
     */
    typedef std::function<void(Result, Status)> get_status_callback_t;


    /**
     * @brief Get camera status (asynchronous).
     *
     * @param callback Function to call with camera status.
     */
    void get_status_async(get_status_callback_t callback);

    /**
     * @brief Get settings that can be changed.
     *
     * The list of settings consists of machine readable parameters,
     * for a human readable desription of the setting use get_setting_str().
     *
     * @sa `get_setting_str`
     *
     * @param settings List of settings that can be changed.
     * @return true request was successful.
     */
    bool get_possible_settings(std::vector<std::string> &settings);

    /**
     * @brief Get possible options for a setting that can be selected.
     *
     * The list of options consists of machine readable option values,
     * for a human readable description of the option use get_option_str().
     *
     * @sa `get_option_str`
     *
     * @param setting_name Name of setting (machine readable).
     * @param options List of options to select from.
     * @return true if request was successful.
     */
    bool get_possible_options(const std::string &setting_name, std::vector<std::string> &options);

    /**
     * @brief Callback type to get an option.
     */
    typedef std::function<void(Result, const std::string &)> get_option_callback_t;

    /**
     * @brief Get an option of a setting (asynchronous).
     *
     * @param setting The machine readable name of the setting.
     * @param callback The callback to get the result and selected option.
     */
    void get_option_async(const std::string &setting,
                          const get_option_callback_t &callback);

    /**
     * @brief Set an option of a setting (asynchronous).
     *
     * @param setting The machine readable name of the setting.
     * @param option The machine readable name of the option value.
     * @param callback The callback to get the result.
     */
    void set_option_async(const std::string &setting,
                          const std::string &option,
                          const result_callback_t &callback);
    /**
     * @brief Get the human readable string of a setting.
     *
     * @param setting_name The machine readable setting name.
     * @param description The human readable string of the setting to get.
     * @return true if call was successful and the description has been set.
     */
    bool get_setting_str(const std::string &setting_name,
                         std::string &description);

    /**
     * @brief Get the human readable string of an option.
     *
     * @param setting_name The machine readable setting name.
     * @param option_name The machine readable option value.
     * @param description The human readable string of the option to get.
     * @return true if call was successful and the description has been set.
     */
    bool get_option_str(const std::string &setting_name,
                        const std::string &option_name,
                        std::string &description);

    /**
     * @brief Copy constructor (object is not copyable).
     */
    Camera(const Camera &) = delete;

    /**
     * @brief Equality operator (object is not copyable).
     */
    const Camera &operator=(const Camera &) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    std::unique_ptr<CameraImpl> _impl;
};

} // namespace dronecore
