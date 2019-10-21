#include "plugins/geofence/geofence.h"
#include "geofence_impl.h"
#include <vector>

namespace mavsdk {

Geofence::Geofence(System& system) : PluginBase(), _impl(std::make_unique<GeofenceImpl>(system)) {}

Geofence::~Geofence() {}

Geofence::Polygon::Polygon() : points(), type(Polygon::Type::INCLUSION) {}

Geofence::Polygon::~Polygon()
{
    points.clear();
}

void Geofence::send_geofence_async(
    const std::vector<std::shared_ptr<Geofence::Polygon>>& polygons, result_callback_t callback)
{
    _impl->send_geofence_async(polygons, callback);
}

const char* Geofence::result_str(Result result)
{
    switch (result) {
        case Result::SUCCESS:
            return "Success";
        case Result::BUSY:
            return "Busy";
        case Result::ERROR:
            return "Error";
        case Result::TOO_MANY_GEOFENCE_ITEMS:
            return "Too many geofence items";
        case Result::INVALID_ARGUMENT:
            return "Invalid argument";
        case Result::TIMEOUT:
            return "Timeout";
        case Result::UNKNOWN:
        default:
            return "Unknown";
    }
}

} // namespace mavsdk
