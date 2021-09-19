#pragma once

#include "point.hpp"
#include <vector>
#include <variant>

namespace rounded_corners {

class LineSegment {
public:
    LineSegment(Point begin, Point end) :
        begin_(begin), end_(end) {
        //std::cout << "Add edge from " << begin_ << " to " << end_ << '\n';
    }

    [[nodiscard]] std::vector<Point> sample(double resolution) const;
    [[nodiscard]] double length() const;
private:
    [[nodiscard]] Point direction() const;

    Point begin_;
    Point end_;
};

class CornerSegment {
public:
    CornerSegment(Point begin, Point end, Point center, double angle) :
        begin_(begin), end_(end), center_(center), angle_(angle) {
        //std::cout << "Add corner from " << begin_ << " to " << end_ << '\n';
    }

    [[nodiscard]] double length() const;
    [[nodiscard]] std::vector<Point> sample(double resolution) const;

private:
    Point begin_;
    Point end_;
    Point center_;
    double angle_;
};

using Segment = std::variant<LineSegment, CornerSegment>;

struct Sample {
    [[nodiscard]] std::vector<Point> operator()(const LineSegment& segment) { return segment.sample(resolution); }
    [[nodiscard]] std::vector<Point> operator()(const CornerSegment& segment) { return segment.sample(resolution); }

    double resolution;
};

} // namespace rounded_corners