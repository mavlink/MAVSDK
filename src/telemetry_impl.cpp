#include "telemetry_impl.h"
#include "math_conversions.h"
#include <math.h>

namespace dronelink {

TelemetryImpl::TelemetryImpl() :
    _absolute_altitude(NAN),
    _relative_altitude(NAN),
    _coordinates({NAN, NAN}),
    _in_air(false),
    _attitude_quaternion({NAN, NAN, NAN, NAN})
{
}

TelemetryImpl::~TelemetryImpl()
{
}

float TelemetryImpl::get_absolute_altitude_m() const
{
    return _absolute_altitude;
}

void TelemetryImpl::set_absolute_altitude_m(float absolute_altitude)
{
    _absolute_altitude = absolute_altitude;
}

float TelemetryImpl::get_relative_altitude_m() const
{
    return _relative_altitude;
}

void TelemetryImpl::set_relative_altitude_m(float relative_altitude)
{
    _relative_altitude = relative_altitude;
}

Telemetry::Coordinates TelemetryImpl::get_coordinates() const
{
    return _coordinates;
}

void TelemetryImpl::set_coordinates(Telemetry::Coordinates coordinates)
{
    _coordinates = coordinates;
}

bool TelemetryImpl::in_air() const
{
    return _in_air;
}

void TelemetryImpl::set_in_air(bool in_air)
{
    _in_air = in_air;
}

Telemetry::Quaternion TelemetryImpl::get_attitude_quaternion() const
{
    return _attitude_quaternion;
}

Telemetry::EulerAngle TelemetryImpl::get_attitude_euler_angle() const
{
    return to_euler_angle_from_quaternion(_attitude_quaternion);
}

void TelemetryImpl::set_attitude_quaternion(Telemetry::Quaternion quaternion)
{
    _attitude_quaternion = quaternion;
}

} // namespace dronelink
