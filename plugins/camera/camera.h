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

    Result take_photo();
    Result start_photo_interval(float interval_s);
    Result stop_photo_interval();
    Result start_video();
    Result stop_video();

    void take_photo_async(const result_callback_t &callback);
    void start_photo_interval_async(float interval_s, const result_callback_t &callback);
    void stop_photo_interval_async(const result_callback_t &callback);
    void start_video_async(const result_callback_t &callback);
    void stop_video_async(const result_callback_t &callback);

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
