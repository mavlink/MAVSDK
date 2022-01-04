#include "geometry.h"
#include "mavsdk_math.h"
#include <cmath>

namespace mavsdk::geometry {

CoordinateTransformation::CoordinateTransformation(GlobalCoordinate reference) :
    _ref_lat_rad(to_rad_from_deg(reference.latitude_deg)),
    _ref_lon_rad(to_rad_from_deg(reference.longitude_deg))
{}

CoordinateTransformation::LocalCoordinate
CoordinateTransformation::local_from_global(GlobalCoordinate global_coordinate) const
{
    const double lat_rad = to_rad_from_deg(global_coordinate.latitude_deg);
    const double lon_rad = to_rad_from_deg(global_coordinate.longitude_deg);

    const double sin_lat = sin(lat_rad);
    const double cos_lat = cos(lat_rad);

    const double cos_d_lon = cos(lon_rad - _ref_lon_rad);

    const double ref_sin_lat = sin(_ref_lat_rad);
    const double ref_cos_lat = cos(_ref_lat_rad);

    const double arg =
        constrain(ref_sin_lat * sin_lat + ref_cos_lat * cos_lat * cos_d_lon, -1.0, 1.0);
    const double c = acos(arg);

    const double k = (fabs(c) > 0) ? (c / sin(c)) : 1.0;

    return LocalCoordinate{
        k * (ref_cos_lat * sin_lat - ref_sin_lat * cos_lat * cos_d_lon) * world_radius_m,
        k * cos_lat * sin(lon_rad - _ref_lon_rad) * world_radius_m};
}

CoordinateTransformation::GlobalCoordinate
CoordinateTransformation::global_from_local(LocalCoordinate local_coordinate) const
{
    const double x_rad = local_coordinate.north_m / world_radius_m;
    const double y_rad = local_coordinate.east_m / world_radius_m;
    const double c = sqrt(x_rad * x_rad + y_rad * y_rad);

    GlobalCoordinate global{};

    if (fabs(c) > 0) {
        const double sin_c = sin(c);
        const double cos_c = cos(c);

        const double ref_sin_lat = sin(_ref_lat_rad);
        const double ref_cos_lat = cos(_ref_lat_rad);

        const double lat_rad = asin(cos_c * ref_sin_lat + (x_rad * sin_c * ref_cos_lat) / c);
        const double lon_rad =
            (_ref_lon_rad +
             atan2(y_rad * sin_c, c * ref_cos_lat * cos_c - x_rad * ref_sin_lat * sin_c));

        global.latitude_deg = to_deg_from_rad(lat_rad);
        global.longitude_deg = to_deg_from_rad(lon_rad);

    } else {
        global.latitude_deg = to_deg_from_rad(_ref_lat_rad);
        global.longitude_deg = to_deg_from_rad(_ref_lon_rad);
    }

    return global;
}

} // namespace mavsdk::geometry
