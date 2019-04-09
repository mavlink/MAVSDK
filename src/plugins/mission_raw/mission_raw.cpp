#include "plugins/mission_raw/mission_raw.h"
#include "mission_raw_impl.h"
#include <vector>

namespace dronecode_sdk {

MissionRaw::MissionRaw(System &system) : PluginBase(), _impl{new MissionRawImpl(system)} {}

MissionRaw::~MissionRaw() {}

void MissionRaw::download_mission_async(MissionRaw::mission_items_and_result_callback_t callback)
{
    _impl->download_mission_async(callback);
}

void MissionRaw::download_mission_cancel()
{
    _impl->download_mission_cancel();
}

const char *MissionRaw::result_str(Result result)
{
    switch (result) {
        case Result::SUCCESS:
            return "Success";
        case Result::BUSY:
            return "Busy";
        case Result::ERROR:
            return "Error";
        case Result::INVALID_ARGUMENT:
            return "Invalid argument";
        case Result::TIMEOUT:
            return "Timeout";
        case Result::CANCELLED:
            return "Cancelled";
        case Result::UNKNOWN:
        default:
            return "Unknown";
    }
}

void MissionRaw::subscribe_mission_changed(mission_changed_callback_t callback)
{
    _impl->subscribe_mission_changed(callback);
}

} // namespace dronecode_sdk
