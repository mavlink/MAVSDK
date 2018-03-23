#pragma once

#include <functional>
#include <memory>
#include "plugin_base.h"

namespace dronecore {

class CameraImpl;
class Device;

/**
 * @brief The Camera class allows to control generic mavlink cameras.
 *
 * Synchronous and asynchronous variants of the camera methods are supplied.
 */
class Camera : public PluginBase
{
public:
    /**
     * @brief Constructor. Creates the plugin for a specific Device.
     *
     * The plugin is typically created as shown below:
     *
     *     ```cpp
     *     auto camera = std::make_shared<Camera>(device);
     *     ```
     *
     * @param device The specific device associated with this plugin.
     */
    explicit Camera(Device &device);

    /**
     * @brief Destructor (internal use only).
     */
    ~Camera();

    /**
     * @brief Possible results returned for camera commands.
     */
    enum class Result {
        SUCCESS = 0,
        IN_PROGRESS,
        BUSY,
        DENIED,
        ERROR,
        TIMEOUT,
        WRONG_ARGUMENT,
        UNKNOWN
    };

    /**
     * @brief Returns a human-readable English string for Camera::Result.
     *
     * @param result The enum value for which a human readable string is required.
     * @return Human readable string for the Camera::Result.
     */
    static const char *result_str(Result result);

    /**
     * @brief Callback type for asynchronous Camera calls.
     */
    typedef std::function<void(Result)> result_callback_t;

    /**
     * @brief Take photo (synchronous).
     *
     * This takes one photo immediately.
     *
     * @return Result of request.
     */
    Result take_photo();

    /**
     * @brief Start photo interval (synchronous).
     *
     * Starts a photo timelapse given an interval.
     *
     * @param interval_s The interval between photos in seconds.
     * @return Result of request.
     */
    Result start_photo_interval(float interval_s);

    /**
     * @brief Stop photo interval (synchronous).
     *
     * Stops a photo timelapse again.
     *
     * @return Result of request.
     */
    Result stop_photo_interval();

    /**
     * @brief Start video capture (synchronous).
     *
     * This starts a video recording immediately.
     *
     * @return Result of request.
     */
    Result start_video();

    /**
     * @brief Stop video capture (synchronous).
     *
     * This stops a video recording again.
     *
     * @return Result of request.
     */
    Result stop_video();

    /**
     * @brief Take photo (asynchronous).
     *
     * This takes one photo immediately.
     *
     * @param callback Function to call with result of request.
     */
    void take_photo_async(const result_callback_t &callback);

    /**
     * @brief Start photo interval (asynchronous).
     *
     * Starts a photo timelapse given an interval.
     *
     * @param interval_s The interval between photos in seconds.
     * @param callback Function to call with result of request.
     */
    void start_photo_interval_async(float interval_s, const result_callback_t &callback);

    /**
     * @brief Stop photo interval (asynchronous).
     *
     * Stops a photo timelapse again.
     *
     * @param callback Function to call with result of request.
     */
    void stop_photo_interval_async(const result_callback_t &callback);

    /**
     * @brief Start video capture (asynchronous).
     *
     * This starts a video recording immediately.
     *
     * @param callback Function to call with result of request.
     */
    void start_video_async(const result_callback_t &callback);

    /**
     * @brief Stop video capture (asynchronous).
     *
     * This stops a video recording again.
     *
     * @param callback Function to call with result of request.
     */
    void stop_video_async(const result_callback_t &callback);


    /**
     * @brief Camera mode type.
     */
    enum class Mode {
        PHOTO,
        PHOTO_SURVEY,
        VIDEO,
        UNKNOWN
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
