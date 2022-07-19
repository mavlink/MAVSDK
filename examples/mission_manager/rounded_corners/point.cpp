#include "point.hpp"
#include <cmath>

namespace rounded_corners {

std::ostream& operator<<(std::ostream& os, const Point& point)
{
    os << "{x: " << point.x << ", y: " << point.y << ", z: " << point.z << "}";
    return os;
}

bool Point::operator==(const Point& other) const
{
    return x == other.x && y == other.y && z == other.z;
}

Point Point::normalized() const
{
    return *this / std::sqrt(x * x + y * y + z * z);
}

double Point::norm() const
{
    return std::sqrt(x * x + y * y + z * z);
}

Point operator+(const Point& lhs, const Point& rhs)
{
    return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
}

Point operator-(const Point& lhs, const Point& rhs)
{
    return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
}

Point operator*(double factor, Point point)
{
    return {factor * point.x, factor * point.y, factor * point.z};
}

Point operator/(Point point, double factor)
{
    return {point.x / factor, point.y / factor, point.z / factor};
}

Point cross_product(const Point& a, const Point& b)
{
    return Point{a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

double dot_product(const Point& a, const Point& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

} // namespace rounded_corners
