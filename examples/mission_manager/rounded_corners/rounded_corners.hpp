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
    double radius_;
    std::vector<Segment> segments_{};
};

} // namespace rounded_corners