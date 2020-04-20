#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "plugin_base.h"

#ifdef ERROR
#undef ERROR
#endif

namespace mavsdk {

class GeofenceImpl;
class System;

/**
 * @brief The Geofence class enables setting a geofence.
 */
class Geofence : public PluginBase {
public:
    /**
     * @brief Constructor. Creates the plugin for a specific System.
     *
     * The plugin is typically created as shown below:
     *
     *     ```cpp
     *     auto geofence = std::make_shared<Geofence>(system);
     *     ```
     *
     * @param system The specific system associated with this plugin.
     */
    explicit Geofence(System& system);

    /**
     * @brief Destructor (internal use only).
     */
    ~Geofence();

    /**
     * @brief Possible results returned for geofence requests.
     */
    enum class Result {
        UNKNOWN = 0, /**< @brief %Unknown result. */
        SUCCESS, /**< @brief %Request succeeded. */
        ERROR, /**< @brief Error. */
        TOO_MANY_GEOFENCE_ITEMS, /**< @brief Too many Polygon objects in the geofence. */
        BUSY, /**< @brief %Vehicle busy. */
        TIMEOUT, /**< @brief Request timed out. */
        INVALID_ARGUMENT, /**< @brief Invalid argument. */
    };

    /**
     * @brief Gets a human-readable English string for an Geofence::Result.
     *
     * @param result Enum for which string is required.
     * @return Human readable string for the Geofence::Result.
     */
    static const char* result_str(Result result);

    /**
     * @brief Callback type for async geofence calls.
     */
    typedef std::function<void(Result)> result_callback_t;

    /**
     * @brief Polygon type.
     */
    struct Polygon {
        /**
         * @brief Point type.
         */
        struct Point {
            double latitude_deg; /**< @brief Latitude in degrees (range: -90 to +90) */
            double longitude_deg; /**< @brief Longitude in degrees (range: -180 to 180) */
        };

        /**
         * @brief Polygon Fence Types.
         *
         * @note MAVLink supports inclusion and exclusion polygon fences.
         */
        enum class Type { INCLUSION, EXCLUSION };

        std::vector<Point> points; /**< @brief Vector of points that define the polygon. */
        Type type; /**< @brief Polygon fence type. */

        /**
         * @brief Constructor. Creates the Polygon for a specific geometry.
         */
        explicit Polygon();

        /**
         * @brief Destructor (internal use only).
         */
        ~Polygon();
    };

    /**
     * @brief Uploads a geofence to the system (asynchronous).
     *
     * The polygons are uploaded to a drone. Once uploaded the geofence will remain
     * on the drone even if a connection is lost.
     *
     * @param polygons Reference to vector of polygons.
     * @param callback Callback to receive result of this request.
     */
    void send_geofence_async(
        const std::vector<std::shared_ptr<Polygon>>& polygons, result_callback_t callback);

    // Non-copyable
    /**
     * @brief Copy constructor (object is not copyable).
     */
    Geofence(const Geofence&) = delete;
    /**
     * @brief Equality operator (object is not copyable).
     */
    const Geofence& operator=(const Geofence&) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    std::unique_ptr<GeofenceImpl> _impl;
};

} // namespace mavsdk
