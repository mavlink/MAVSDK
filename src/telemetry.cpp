#include "telemetry.h"
#include "telemetry_impl.h"

namespace dronelink {

Telemetry::Telemetry(TelemetryImpl *impl) :
    _impl(impl)
{
}

Telemetry::~Telemetry()
{
}

float Telemetry::absolute_altitude_m() const
{
    return _impl->get_absolute_altitude_m();
}

float Telemetry::relative_altitude_m() const
{
    return _impl->get_relative_altitude_m();
}

Telemetry::Coordinates Telemetry::coordinates() const
{
    return _impl->get_coordinates();
}

bool Telemetry::in_air() const
{
    return _impl->in_air();
}

Telemetry::Quaternion Telemetry::attitude_quaternion() const
{
    return _impl->get_attitude_quaternion();
}

Telemetry::EulerAngle Telemetry::attitude_euler_angle() const
{
    return _impl->get_attitude_euler_angle();
}


} // namespace dronelink
