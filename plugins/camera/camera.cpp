#include <vector>
#include "camera.h"
#include "camera_impl.h"
#include "mavlink_include.h"

namespace dronecore {

Camera::Camera(System &system, uint8_t camera_id) :
    PluginBase(),
    _impl { new CameraImpl(system, camera_id) }
{
}

Camera::~Camera()
{
}

std::string Camera::result_str(Result result)
{
    switch (result) {
        case Result::SUCCESS:
            return "Success";
        case Result::BUSY:
            return "Busy";
        case Result::ERROR:
            return "Error";
        case Result::TIMEOUT:
            return "Timeout";
        case Result::UNSUPPORTED:
            return "Camera downloaded from system is unsupported";
        case Result::UNKNOWN:
        default:
            return "Unknown";
    }
}

} // namespace dronelin
