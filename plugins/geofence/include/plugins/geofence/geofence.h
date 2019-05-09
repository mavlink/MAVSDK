#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <dronecode_sdk/plugin_base.h>

namespace dronecode_sdk {

class GeofenceImpl;
class System;

class Geofence : public PluginBase {
public:
    explicit Geofence(System &system);
    ~Geofence();

    enum class Result {
        SUCCESS = 0,
        ERROR,
        TOO_MANY_GEOFENCE_ITEMS,
        BUSY,
        TIMEOUT,
        INVALID_ARGUMENT,
        UNKNOWN,
        VIOLATION
    };

    static const char *result_str(Result result);

    typedef std::function<void(Result)> result_callback_t;

    struct Polygon {
        struct Point {
            double latitude_deg;
            double longitude_deg;
        };

        enum class Type { INCLUSION, EXCLUSION };

        std::vector<Point> points;
        Type type;

        explicit Polygon();
        ~Polygon();
    };

    void send_geofence_async(const std::vector<std::shared_ptr<Polygon>> &polygons,
                             result_callback_t callback);

    // Non-copyable
    Geofence(const Geofence &) = delete;
    const Geofence &operator=(const Geofence &) = delete;

private:
    // Underlying implementation, set at instantiation
    GeofenceImpl *_impl;
};

} // namespace dronecode_sdk
