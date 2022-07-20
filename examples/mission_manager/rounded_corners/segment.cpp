#include "segment.hpp"
#include <cmath>
#include <cassert>

namespace rounded_corners {

double LineSegment::length() const
{
    return sqrt(
        (begin_.x - end_.x) * (begin_.x - end_.x) + (begin_.y - end_.y) * (begin_.y - end_.y) +
        (begin_.z - end_.z) * (begin_.z - end_.z));
}

Point LineSegment::direction() const
{
    return end_ - begin_;
}

std::vector<Point> LineSegment::sample(double resolution) const
{
    auto sampled_points = std::vector<Point>{};

    const double len = length();
    // assert(len > 0);
    if (len == 0) {
        return {};
    }
    const unsigned long num_points = std::ceil(len / resolution);
    const auto dir = direction();

    for (unsigned long step = 0; step < num_points; ++step) {
        auto point =
            Point{begin_ + (static_cast<double>(step) / static_cast<double>(num_points)) * dir};
        sampled_points.emplace_back(point);
    }

    return sampled_points;
}

std::vector<Point> CornerSegment::sample(double resolution) const
{
    auto sampled_points = std::vector<Point>{};

    const auto len = length();
    // assert(len > 0);
    if (len == 0) {
        return {};
    }
    const auto num_points = 2 * len * M_PI / resolution;

    for (unsigned long step = 0; step < num_points; ++step) {
        // Slerp
        const auto t = static_cast<double>(step) / static_cast<double>(num_points);
        const auto slerp = std::sin((1 - t) * angle_) / std::sin(angle_) * (begin_ - center_) +
                           std::sin(t * angle_) / std::sin(angle_) * (end_ - center_);

        sampled_points.emplace_back(center_ + slerp);
    }

    return sampled_points;
}

double CornerSegment::length() const
{
    const auto radius = (begin_ - center_).norm();
    return angle_ * radius;
}

} // namespace rounded_corners
