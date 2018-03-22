#pragma once

#include <vector>
#include <memory>
#include <functional>
#include "plugin_base.h"

namespace dronecore {

class CameraImpl;
class System;

/**
 * @brief The Camera class that can control Onboard camera.
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
    explicit Camera(System &system, uint8_t camera_id = 1);

    /**
     * @brief Destructor (internal use only).
     */
    ~Camera();

    /**
     * @brief Possible results returned for mission requests.
     */
    enum class Result {
        SUCCESS = 0, /**< @brief %Request succeeded. */
        ERROR, /**< @brief Error. */
        BUSY, /**< @brief %System busy. */
        TIMEOUT, /**< @brief Request timed out. */
        COMMAND_DENIED, /**< @brief Command denied. */
        UNSUPPORTED, /**< @brief __ not supported. */
        UNKNOWN /**< @brief Unknown error. */
    };

    /**
     * @brief Camera Information.
     */
    struct CameraInformation {
        std::string vendor;
        std::string model;
        uint32_t firmware_version;
        float focal_len_mm;
        float sensor_size_h_mm;
        float sensor_size_v_mm;
        uint16_t resolution_h_pix;
        uint16_t resolution_v_pix;
        uint8_t lens_id;
        uint32_t flags;
        uint16_t camera_definition_ver;
        std::string camera_definition_uri;
    };

    /**
     * @brief get_camera_ids
     * @return
     */
    static std::vector<uint8_t> get_camera_ids();

    /**
     * @brief Gets a human-readable English string for an Camera::Result.
     *
     * @param result Enum for which string is required.
     * @return Human readable string for the Camera::Result.
     */
    static std::string result_str(Result result);

    /**
     * @brief Type for result callback of async methods.
     */
    using result_callback_t = std::function<void(Result)>;

    /**
     * @brief Type for progress callback.
     */
    using progress_callback_t = std::function<void(int current, int total)> ;

    // Non-copyable
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
