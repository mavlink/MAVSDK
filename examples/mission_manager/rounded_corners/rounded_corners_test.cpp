#include <cassert>
#include "rounded_corners.hpp"
#include <iostream>

using namespace rounded_corners;

void sample_straight_line()
{
    auto rc = RoundedCorners{1.0};
    const auto line = std::vector<Point>{{2.0, 0.0, 0.0}, {5.0, 0.0, 0.0}};
    rc.set_points(line);
    auto sampled = rc.sample(0.1);

    assert(sampled.size() == 31);
    assert(sampled.at(0) == line.at(0));
    assert(sampled.at(30) == line.at(1));
    assert(sampled.at(1) == Point(2.1, 0.0, 0.0));
    assert(sampled.at(15) == Point(3.5, 0.0, 0.0));
    assert(sampled.at(29) == Point(4.9, 0.0, 0.0));
}

int main()
{
    sample_straight_line();


    return 0;
}