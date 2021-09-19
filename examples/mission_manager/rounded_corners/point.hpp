#pragma once

#include <iostream>

namespace rounded_corners {

struct Point {
    double x;
    double y;
    double z;

    Point(double x, double y, double z) : x(x), y(y), z(z) {}

    [[nodiscard]] Point normalized() const;
    [[nodiscard]] double norm() const;

    friend std::ostream &operator<<(std::ostream &os, const Point &point);

    bool operator==(const Point &other) const;
};

Point cross_product(const Point& a, const Point& b);

Point operator+ (const Point& lhs, const Point& rhs);
Point operator- (const Point& lhs, const Point& rhs);
Point operator* (double factor, Point point);
Point operator/ (Point point, double factor);

} // namespace rounded_corners