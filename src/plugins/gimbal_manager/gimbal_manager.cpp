#include "plugins/gimbal_manager/gimbal_manager.h"
#include "gimbal_manager_impl.h"

namespace mavsdk {

GimbalManager::GimbalManager(System& system) : PluginBase(), _impl{new GimbalManagerImpl(system)}, _information{} {}

GimbalManager::~GimbalManager() {}

GimbalManager::Result GimbalManager::request_information()
{
    return _impl->request_information();
}

void GimbalManager::subscribe_information_async(
        std::function<void(const GimbalManager::Information information)> callback)
{
    _impl->subscribe_information_async(callback);
}

GimbalManager::Result GimbalManager::set_attitude(float tilt_rate, float pan_rate,
        float tilt_deg, float pan_deg, int operation_flags,
        uint8_t id)
{
    return _impl->set_attitude(tilt_rate, pan_rate, tilt_deg, pan_deg, operation_flags, id);
}

void GimbalManager::set_attitude_async(float tilt_rate, float pan_rate,
        float tilt_deg, float pan_deg, int operation_flags,
        uint8_t id, result_callback_t callback)
{
    _impl->set_attitude_async(tilt_rate, pan_rate, tilt_deg, pan_deg, operation_flags, id, callback);
}

void GimbalManager::bind(GimbalManager::Information information)
{
    _information = information;
    _bound = true;
}

void GimbalManager::unbind()
{
    _bound = false;
}

//GimbalManager::Result GimbalManager::track_point(float x, float y, uint8_t id)
//{
    //return _impl->track_point(x, y, id);
//}

//void GimbalManager::track_point_async(float x, float y, uint8_t id, result_callback_t callback)
//{
    //_impl->track_point_async(x, y, id, callback);
//}

//GimbalManager::Result GimbalManager::track_rectangle(float x1, float y1, float x2, float y2, uint8_t id)
//{
    //return _impl->track_rectangle(x1, y1, x2, y2, id);
//}

//void GimbalManager::track_rectangle_async(float x1, float y1, float x2, float y2, uint8_t id, result_callback_t callback)
//{
    //_impl->track_rectangle_async(x1, y1, x2, y2, id, callback);
//}

GimbalManager::Result GimbalManager::set_roi_location(double latitude_deg, double longitude_deg, float altitude_m)
{
    return _impl->set_roi_location(latitude_deg, longitude_deg, altitude_m);
}

void GimbalManager::set_roi_location_async(double latitude_deg, double longitude_deg, float altitude_m, result_callback_t callback)
{
    _impl->set_roi_location_async(latitude_deg, longitude_deg, altitude_m, callback);
}

const char* GimbalManager::result_str(Result result)
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
};

} // namespace mavsdk
