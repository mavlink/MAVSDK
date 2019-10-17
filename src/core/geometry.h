#pragma once

namespace mavsdk {
namespace geometry {

/**
 * @brief This is a utility class for coordinate transformations.
 *
 * The projections used to transform from global (lat/lon) to local (meter)
 * coordinates are taken from:
 * http://mathworld.wolfram.com/AzimuthalEquidistantProjection.html
 * and inspired by the implementations in:
 * https://github.com/PX4/ecl/blob/master/geo/geo.cpp
 */
class CoordinateTransformation {
public:
    /**
     * @brief Type for global coordinate in latitude/longitude in degrees.
     */
    struct GlobalCoordinate {
        double latitude_deg; /**< @brief Latitude in degrees. */
        double longitude_deg; /**< @brief Longitude in degrees. */
    };

    /**
     * @brief Type for local coordinate relative to reference in meters.
     */
    struct LocalCoordinate {
        double north_m; /**< @brief Position in North direction in meters. */
        double east_m; /**< @brief Position in East direction in meters. */
    };

    /**
     * @brief Default constructor not available.
     */
    CoordinateTransformation() = delete;

    /**
     * @brief Constructor to initialize projection reference.
     *
     * @param reference Reference coordinate to project from.
     */
    explicit CoordinateTransformation(GlobalCoordinate reference);

    /**
     * @brief Calculate local coordinates from global coordinates.
     *
     * @param global_coordinate The global coordinate to project from.
     */
    LocalCoordinate local_from_global(GlobalCoordinate global_coordinate) const;

    /**
     * @brief Calculate global coordinates from local coordinates.
     *
     * @param local_coordinate The local coordinate to project from.
     */
    GlobalCoordinate global_from_local(LocalCoordinate local_coordinate) const;

    /**
     * @brief Destructor.
     */
    ~CoordinateTransformation() = default;

private:
    static constexpr double rad(double deg);
    static constexpr double deg(double rad);
    static constexpr double constrain(double input, double min, double max);

    double _ref_lat_rad;
    double _ref_lon_rad;
    static constexpr double world_radius_m{6371000.0};
};

} // namespace geometry
} // namespace mavsdk
