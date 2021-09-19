#pragma once

#include <vector>
#include <iostream>

namespace rounded_corners {

struct Point {
    double x;
    double y;
    double z;

    Point(double x, double y, double z) : x(x), y(y), z(z) {}

    friend std::ostream& operator<<(std::ostream& os, const Point& point);
    bool operator==(const Point& other) const;
};

class RoundedCorners {
public:
    RoundedCorners() = delete;
    explicit RoundedCorners(double radius);
    void set_points(const std::vector<Point>& points) noexcept;
    std::vector<Point> sample(double resolution) const noexcept;

private:
    class Segment {
    public:
        Segment(Point a, Point b) : a(a), b(b) {}

        double length() const;
        Point direction() const;

        Point a;
        Point b;
    };
    double radius_;
    std::vector<Segment> segments_{};
};

} // namespace rounded_corners