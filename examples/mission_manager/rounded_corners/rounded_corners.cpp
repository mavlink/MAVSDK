#include "rounded_corners.hpp"
#include "segment.hpp"
#include <cassert>
#include <cmath>
#include <iostream>

namespace rounded_corners {

RoundedCorners::RoundedCorners(double radius) : radius_(radius) {}

void RoundedCorners::set_points(const std::vector<Point>& points) noexcept
{
    assert(points.size() > 1);

    for (auto it = points.cbegin(); it != points.cend(); ++it) {
        const auto it_next = it + 1;

        if (it == points.cbegin()) {
            const auto full_first_line = *it_next-*it;
            segments_.emplace_back(LineSegment{*it, *it_next - 0.5*full_first_line});

        } else if (it_next == points.cend()) {
            const auto it_previous = it - 1;
            const auto full_first_line = *it-*it_previous;
            segments_.emplace_back(LineSegment{*it - 0.5*full_first_line, *it});
            break;
        }

        const auto it_next_next = it_next + 1;

        if (it_next_next != points.cend()) {
            const double angle = calculate_angle(*it, *it_next, *it_next_next);
            double distance_from_corner = calculate_distance_from_corner(angle);

            const auto full_first_line = *it_next - *it;
            const double max_distance = full_first_line.norm() / 2.0;

            auto radius = radius_;
            if (distance_from_corner > max_distance) {
                radius = calculated_adapted_radius(max_distance, angle);
                distance_from_corner = max_distance;
            }
            const auto first_line_end = *it_next - distance_from_corner * full_first_line.normalized();
            const auto full_second_line = *it_next_next - *it_next;
            const auto second_line_start = *it_next + distance_from_corner * full_second_line.normalized();

            const auto line12 = *it_next - *it;
            const auto line23 = *it_next_next - *it_next;

            const auto line = LineSegment{*it + 0.5 * full_first_line, first_line_end};
            if (line.length() > 0) {
                segments_.push_back(line);
            }

            const auto c = cross_product(line12, line23);

            const auto norm12 = cross_product(c, line12).normalized();
            const auto norm23 = cross_product(c, line23).normalized();

            const auto center = first_line_end + radius * norm12;
            const auto center_check = second_line_start + radius * norm23;

            //assert(center == center_check);

            segments_.emplace_back(CornerSegment{first_line_end, second_line_start, center, angle});

            const auto line2 = LineSegment{second_line_start, second_line_start + 0.5 * full_second_line};
            if (line2.length() > 0) {
                segments_.emplace_back(line2);
            }
        } else {
        }
    }
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

double RoundedCorners::calculate_angle(const Point &a, const Point &b, const Point &c)
{
    const auto l1 = b - a;
    const auto l2 = c -b;

    const double alpha = std::acos(
            (l1.x*l2.x + l1.y*l2.y + l1.z*l2.z) /
            (sqrt(l1.x*l1.x + l1.y*l1.y + l1.z*l1.z) *
             sqrt(l2.x*l2.x + l2.y*l2.y + l2.z*l2.z))
    );

    return alpha;
}

double RoundedCorners::calculate_distance_from_corner(const double angle) const
{
    const double distance_from_corner = tan(angle/2)*radius_;
    return distance_from_corner;
}

double RoundedCorners::calculated_adapted_radius(const double distance, const double angle) {
    return distance/tan(angle/2);
}

} // namespace rounded_corners