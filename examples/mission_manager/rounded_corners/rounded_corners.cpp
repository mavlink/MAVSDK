#include "rounded_corners.hpp"
#include <cassert>
#include <cmath>
#include <iostream>

namespace rounded_corners {

std::ostream& operator<<(std::ostream& os, const Point& point)
{
    os << "{x: " << point.x << ", y: " << point.y << ", z: " << point.z << "}";
    return os;
}

bool Point::operator==(const Point &other) const {
    return x==other.x && y==other.y && z==other.z;
}

Point operator+ (const Point& lhs, const Point& rhs) {
    return {lhs.x+rhs.x, lhs.y+rhs.y, lhs.z+rhs.z};
}

Point operator- (const Point& lhs, const Point& rhs) {
    return {lhs.x-rhs.x, lhs.y-rhs.y, lhs.z-rhs.z};
}

Point operator* (double factor, Point point) {
    return {factor*point.x, factor*point.y, factor*point.z};
}


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
        const double length = segment.length();
        assert(length > 0);
        const unsigned long num_points = std::ceil(length / resolution);

        const auto direction = segment.direction();

        for (unsigned long step = 0; step < num_points; ++step) {
            sampled.emplace_back(segment.a
                + (static_cast<double>(step)/static_cast<double>(num_points))*direction);
        }

        if (&segment == &segments_.back()) {
            sampled.push_back(segment.b);
        }
    }

    return sampled;
}

double RoundedCorners::Segment::length() const
{
    return sqrt((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) + (a.z-b.z)*(a.z-b.z));
}

Point RoundedCorners::Segment::direction() const
{
    return b-a;
}

} // namespace rounded_corners