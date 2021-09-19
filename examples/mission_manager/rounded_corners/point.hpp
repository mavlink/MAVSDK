#pragma once

#include <iostream>

namespace rounded_corners {

struct Point {
    double x;
    double y;
    double z;

    Point(double x, double y, double z) : x(x), y(y), z(z) {}

    friend std::ostream &operator<<(std::ostream &os, const Point &point);

    bool operator==(const Point &other) const;
};

Point operator+ (const Point& lhs, const Point& rhs);
Point operator- (const Point& lhs, const Point& rhs);
Point operator* (double factor, Point point);

} // namespace rounded_corners