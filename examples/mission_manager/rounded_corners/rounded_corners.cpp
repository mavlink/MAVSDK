#include "rounded_corners.hpp"
#include "segment.hpp"
#include <cmath>
#include <iostream>
#include <limits>

namespace rounded_corners {

RoundedCorners::RoundedCorners(double radius) : radius_(radius) {}

void RoundedCorners::set_points(const std::vector<Point>& points) noexcept
{
    points_ = points;
    segments_.clear();
}

RoundedCorners::Result RoundedCorners::calculate() noexcept
{
    segments_.clear();

    if (points_.size() < 2) {
        return Result::NotEnoughPoints;
    }

    if (points_.size() == 2) {
        segments_.emplace_back(LineSegment{points_[0], points_[1]});
        return Result::Ok;
    }

    std::optional<Point> last_point;

    for (auto a = points_.cbegin(); a != points_.cend(); ++a) {
        const auto b = a + 1;
        if (b == points_.cend()) {
            break;
        }

        const auto c = b + 1;
        if (c == points_.cend()) {
            break;
        }

        if (!last_point) {
            last_point = *a;
        }

        const auto ab = *b - last_point.value();
        const auto bc = *c - *b;

        const double angle = calculate_angle(last_point.value(), *b, *c);
        if (angle < std::numeric_limits<double>::epsilon()) {
            continue;
        }

        const double distance_from_corner = calculate_distance_from_corner(angle);

        const double available_straight = ab.norm();

        if (available_straight > distance_from_corner) {
            segments_.emplace_back(
                LineSegment{last_point.value(), *b - distance_from_corner * ab.normalized()});

        } else if (available_straight < distance_from_corner) {
            return Result::CornersTooTight;

        } else {
            // Exactly no line segment required.
        }

        const auto plane = cross_product(ab, bc);
        auto norm12 = cross_product(plane, ab).normalized();

        if (dot_product(norm12, bc) < 0) {
            norm12 = -1.0 * norm12;
        }

        const auto center = *b - distance_from_corner * ab.normalized() + radius_ * norm12;

        segments_.emplace_back(CornerSegment{
            *b - distance_from_corner * ab.normalized(),
            *b + distance_from_corner * bc.normalized(),
            center,
            angle});

        last_point = *b + distance_from_corner * bc.normalized();

        if (c + 1 == points_.cend()) {
            segments_.emplace_back(LineSegment{last_point.value(), *c});
        }
    }

    return Result::Ok;
}

std::vector<Point> RoundedCorners::sample(double resolution) const noexcept
{
    auto sampled = std::vector<Point>{};

    for (const auto& segment : segments_) {
        auto caller = [resolution](const auto& obj) { return obj.sample(resolution); };
        const auto segment_sampled = std::visit(caller, segment);
        if (&segment == &segments_.back()) {
            sampled.insert(sampled.end(), segment_sampled.begin(), segment_sampled.end());
        } else {
            sampled.insert(sampled.end(), segment_sampled.begin(), segment_sampled.end() - 1);
        }
    }

    return sampled;
}

double RoundedCorners::calculate_angle(const Point& a, const Point& b, const Point& c)
{
    const auto l1 = b - a;
    const auto l2 = c - b;

    const double alpha = std::acos(
        (l1.x * l2.x + l1.y * l2.y + l1.z * l2.z) /
        (sqrt(l1.x * l1.x + l1.y * l1.y + l1.z * l1.z) *
         sqrt(l2.x * l2.x + l2.y * l2.y + l2.z * l2.z)));

    return alpha;
}

double RoundedCorners::calculate_distance_from_corner(const double angle) const
{
    const double distance_from_corner = tan(angle / 2) * radius_;
    return distance_from_corner;
}

double RoundedCorners::calculated_adapted_radius(const double distance, const double angle)
{
    return distance / tan(angle / 2);
}

} // namespace rounded_corners
