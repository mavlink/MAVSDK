#pragma once

#include <vector>
#include <iostream>
#include "point.hpp"
#include "segment.hpp"

namespace rounded_corners {

class RoundedCorners {
public:
    RoundedCorners() = delete;
    explicit RoundedCorners(double radius);
    void set_points(const std::vector<Point>& points) noexcept;
    [[nodiscard]] std::vector<Point> sample(double resolution) const noexcept;

private:
    [[nodiscard]] static double calculate_angle(const Point &a, const Point &b, const Point &c) ;
    [[nodiscard]] double calculate_distance_from_corner(double angle) const;
    [[nodiscard]] double calculated_adapted_radius(double distance, double angle);

    double radius_;
    std::vector<Segment> segments_{};

};

} // namespace rounded_corners