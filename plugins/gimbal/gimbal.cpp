#include "gimbal.h"
#include "gimbal_impl.h"

namespace dronecore {

Gimbal::Gimbal(System &system) :
    PluginBase(),
    _impl { new GimbalImpl(system) }
{
}

Gimbal::~Gimbal()
{
}

Gimbal::Result Gimbal::set_pitch_and_yaw(float pitch_deg, float yaw_deg)
{
    return _impl->set_pitch_and_yaw(pitch_deg, yaw_deg);
}

void Gimbal::set_pitch_and_yaw_async(float pitch_deg, float yaw_deg, result_callback_t callback)
{
    _impl->set_pitch_and_yaw_async(pitch_deg, yaw_deg, callback);
}

Gimbal::Result Gimbal::set_roi_location(double latitude_deg, double longitude_deg, float altitude_m)
{
    return _impl->set_roi_location(latitude_deg, longitude_deg, altitude_m);
}

void Gimbal::set_roi_location_async(double latitude_deg, double longitude_deg, float altitude_m,
                                    result_callback_t callback)
{
    _impl->set_roi_location_async(latitude_deg, longitude_deg, altitude_m, callback);
}

const char *Gimbal::result_str(Result result)
{
    switch (result) {
        case Result::SUCCESS:
            return "Success";
        case Result::ERROR:
            return "Error";
        case Result::TIMEOUT:
            return "Timeout";
        case Result::UNKNOWN:
        default:
            return "Unknown";
    }
}

} // namespace dronecore
