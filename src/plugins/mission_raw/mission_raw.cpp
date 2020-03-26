#include <vector>

#include "plugins/mission_raw/mission_raw.h"
#include "mission_raw_impl.h"

namespace mavsdk {

MissionRaw::MissionRaw(System& system) : PluginBase(), _impl{new MissionRawImpl(system)} {}

MissionRaw::~MissionRaw() {}

void MissionRaw::download_mission_async(MissionRaw::mission_items_and_result_callback_t callback)
{
    _impl->download_mission_async(callback);
}

void MissionRaw::download_mission_cancel()
{
    _impl->download_mission_cancel();
}

void MissionRaw::upload_mission_async(
    const std::vector<std::shared_ptr<MavlinkMissionItemInt>>& mission_raw,
    result_callback_t callback)
{
    _impl->upload_mission_async(mission_raw, callback);
}

void MissionRaw::upload_mission_cancel()
{
    _impl->upload_mission_cancel();
}

const char* MissionRaw::result_str(Result result)
{
    switch (result) {
        case Result::SUCCESS:
            return "Success";
        case Result::BUSY:
            return "Busy";
        case Result::ERROR:
            return "Error";
        case Result::UNSUPPORTED:
            return "Unsupported";
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

void MissionRaw::start_mission_async(result_callback_t callback)
{
    _impl->start_mission_async(callback);
}

void MissionRaw::pause_mission_async(result_callback_t callback)
{
    _impl->pause_mission_async(callback);
}

void MissionRaw::clear_mission_async(result_callback_t callback)
{
    _impl->clear_mission_async(callback);
}

void MissionRaw::set_current_mavlink_mission_item_async(
    int current_mavlink, result_callback_t callback)
{
    _impl->set_current_mavlink_mission_item_async(current_mavlink, callback);
}

int MissionRaw::reached_mavlink_mission_item() const
{
    return _impl->reached_mavlink_mission_item();
}

int MissionRaw::current_mavlink_mission_item() const
{
    return _impl->current_mavlink_mission_item();
}

int MissionRaw::total_mavlink_mission_items() const
{
    return _impl->total_mavlink_mission_items();
}

void MissionRaw::subscribe_progress_current(progress_current_callback_t callback)
{
    _impl->subscribe_progress_current(callback);
}

void MissionRaw::subscribe_progress_reached(progress_reached_callback_t callback)
{
    _impl->subscribe_progress_reached(callback);
}

} // namespace mavsdk
