#include "rounded_corners.hpp"
#include "segment.hpp"
#include <cassert>
#include <cmath>
#include <iostream>

namespace rounded_corners {

RoundedCorners::RoundedCorners(double radius) : radius_(radius) {}

void RoundedCorners::set_points(const std::vector<Point>& points) noexcept
{
    for (auto it = points.cbegin(); it != points.cend(); ++it) {
        const auto it_next = (it+1);

        if (it_next == points.end()) {
            break;
        }
        segments_.emplace_back(*it, *it_next);
    }
}

std::vector<Point> RoundedCorners::sample(double resolution) const noexcept
{
    auto sampled = std::vector<Point>{};

    for (const auto& segment : segments_) {
        const auto segment_sampled = segment.sample(resolution);
        sampled.insert(sampled.end(), segment_sampled.begin(), segment_sampled.end());

        if (&segment == &segments_.back()) {
            sampled.push_back(segment.b);
        }
    }

    return sampled;
}

} // namespace rounded_corners