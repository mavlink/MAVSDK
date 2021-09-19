#include "segment.hpp"
#include <cmath>
#include <cassert>

namespace rounded_corners {

double Segment::length() const {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
}

Point Segment::direction() const {
    return b - a;
}

std::vector <Point> Segment::sample(double resolution) const {
    auto sampled_points = std::vector < Point > {};

    const double len = length();
    assert(len > 0);
    const unsigned long num_points = std::ceil(len / resolution);

    const auto dir = direction();

    for (unsigned long step = 0; step < num_points; ++step) {
        sampled_points.emplace_back(a + (static_cast<double>(step) / static_cast<double>(num_points)) * dir);
    }

    return sampled_points;
}

} // namespace rounded_corners