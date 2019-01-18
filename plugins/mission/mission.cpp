#include "plugins/mission/mission.h"
#include "mission_impl.h"
#include <vector>
#include "mavlink_include.h"

namespace dronecode_sdk {

Mission::Mission(System &system) : PluginBase(), _impl{new MissionImpl(system)} {}

Mission::~Mission() {}

void Mission::upload_mission_async(const std::vector<std::shared_ptr<MissionItem>> &mission_items,
                                   result_callback_t callback)
{
    _impl->upload_mission_async(mission_items, callback);
}

void Mission::upload_mission_cancel()
{
    _impl->upload_mission_cancel();
}

void Mission::download_mission_async(Mission::mission_items_and_result_callback_t callback)
{
    _impl->download_mission_async(callback);
}

void Mission::download_mission_cancel()
{
    _impl->download_mission_cancel();
}

void Mission::set_return_to_launch_after_mission(bool enable)
{
    _impl->set_return_to_launch_after_mission(enable);
}

bool Mission::get_return_to_launch_after_mission()
{
    return _impl->get_return_to_launch_after_mission();
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
        case Result::ERROR:
            return "Error";
        case Result::TOO_MANY_MISSION_ITEMS:
            return "Too many mission items";
        case Result::BUSY:
            return "Busy";
        case Result::TIMEOUT:
            return "Timeout";
        case Result::INVALID_ARGUMENT:
            return "Invalid argument";
        case Result::UNSUPPORTED:
            return "Mission downloaded from system is unsupported";
        case Result::NO_MISSION_AVAILABLE:
            return "No mission available";
        case Result::FAILED_TO_OPEN_QGC_PLAN:
            return "Failed to open QGC plan";
        case Result::FAILED_TO_PARSE_QGC_PLAN:
            return "Failed to parse QGC plan";
        case Result::UNSUPPORTED_MISSION_CMD:
            return "Unsupported Mission command";
        case Result::CANCELLED:
            return "Cancelled";
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

Mission::Result Mission::import_qgroundcontrol_mission(Mission::mission_items_t &mission_items,
                                                       const std::string &qgc_plan_file)
{
    return MissionImpl::import_qgroundcontrol_mission(mission_items, qgc_plan_file);
}

} // namespace dronecode_sdk
