#include "point.hpp"

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

} // namespace rounded_corners