#include "mission.h"
#include "mission_impl.h"
#include <vector>

namespace dronecore {

Mission::Mission(Device *device) :
    PluginBase()
{
    _impl = new MissionImpl(device);
}

Mission::~Mission()
{
    delete _impl;
}


void Mission::upload_mission_async(const std::vector<std::shared_ptr<MissionItem>> &mission_items,
                                   result_callback_t callback)
{
    _impl->upload_mission_async(mission_items, callback);
}

void Mission::download_mission_async(Mission::mission_items_and_result_callback_t callback)
{
    _impl->download_mission_async(callback);
}

void Mission::start_mission_async(result_callback_t callback)
{
    _impl->start_mission_async(callback);
}

void Mission::pause_mission_async(result_callback_t callback)
{
    _impl->pause_mission_async(callback);
}

void Mission::set_current_mission_item_async(int current, result_callback_t callback)
{
    _impl->set_current_mission_item_async(current, callback);
}

bool Mission::mission_finished() const
{
    return _impl->is_mission_finished();
}

const char *Mission::result_str(Result result)
{
    switch (result) {
        case Result::SUCCESS:
            return "Success";
        case Result::BUSY:
            return "Busy";
        case Result::ERROR:
            return "Error";
        case Result::TOO_MANY_MISSION_ITEMS:
            return "Too many mission items";
        case Result::INVALID_ARGUMENT:
            return "Invalid argument";
        case Result::TIMEOUT:
            return "Timeout";
        case Result::UNKNOWN:
        default:
            return "Unknown";
    }
}

int Mission::current_mission_item() const
{
    return _impl->current_mission_item();
}

int Mission::total_mission_items() const
{
    return _impl->total_mission_items();
}

void Mission::subscribe_progress(progress_callback_t callback)
{
    _impl->subscribe_progress(callback);
}

} // namespace dronelin
