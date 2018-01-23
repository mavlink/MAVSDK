#include "mission.h"
#include "mission_impl.h"
#include <vector>
#include "mavlink_include.h"
#include <fstream> // for `std::ifstream`
#include <sstream> // for `std::stringstream`
#include <json11.hpp>

using namespace json11;

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
        case Result::UNSUPPORTED:
            return "Mission downloaded from device is unsupported";
        case Result::FAILED_TO_OPEN_QGC_PLAN:
            return "Failed to open QGC plan";
        case Result::FAILED_TO_PARSE_QGC_PLAN:
            return "Failed to parse QGC plan";
        case Result::UNSUPPORTED_MISSION_CMD:
            return "Unsupported Mission command";
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

Mission::Result Mission::import_mission_items_from_QGC_plan(Mission::mission_items_t &items,
                                                            std::string qgc_plan_file)
{
    std::ifstream file(qgc_plan_file);
    if (!file) { // File open error
        return Mission::Result::FAILED_TO_OPEN_QGC_PLAN;
    }

    // Read QGC plan into a string stream
    std::stringstream ss;
    ss << file.rdbuf();
    file.close();

    std::string err;
    const auto parsed_plan = Json::parse(ss.str(), err); // parse QGC plan
    if (!err.empty()) { // Parse error
        return Mission::Result::FAILED_TO_PARSE_QGC_PLAN;
    }

    // Compose mission items
    Mission::Result result = [&items](const Json & mission_json) -> Mission::Result {
        for (auto &mission_item : mission_json["items"].array_items())
        {
            // Parameters of Mission item & MAV command of it.
            float speed_m_s = 0.f;
            float gimbal_pitch_deg = 0.f, gimbal_yaw_deg = 0.f;
            bool is_fly_through = false;
            double lat_deg = NAN, lon_deg = NAN, rel_alt_deg = NAN;
            size_t no_of_photos = 0;
            auto camera_action = MissionItem::CameraAction::NONE;
            auto command = mission_item["command"].int_value();

            // Extract parameters of each mission item
            std::vector<double> params;
            for (auto &p : mission_item["params"].array_items()) {
                params.push_back(p.number_value());
            }

            switch (command) {
                case MAV_CMD_IMAGE_START_CAPTURE:
                    no_of_photos = static_cast<size_t>(params[3]);
                    if (no_of_photos > 1) {
                        camera_action = MissionItem::CameraAction::START_PHOTO_INTERVAL;
                    } else if (no_of_photos == 1) {
                        camera_action = MissionItem::CameraAction::TAKE_PHOTO;
                    }
                    break;
                case MAV_CMD_VIDEO_START_STREAMING:
                    camera_action = MissionItem::CameraAction::START_VIDEO;
                    break;
                case MAV_CMD_VIDEO_STOP_STREAMING:
                    camera_action = MissionItem::CameraAction::START_VIDEO;
                    break;
                case MAV_CMD_DO_CHANGE_SPEED:
                    speed_m_s = params[2];
                    break;
                case MAV_CMD_DO_MOUNT_CONTROL:
                    // Possible bug in QGroundcontrol. It stores -ve values for pitch in deg.
                    gimbal_pitch_deg = -params[0];
                    gimbal_yaw_deg = params[3];
                    break;
                case MAV_CMD_NAV_WAYPOINT:
                    is_fly_through = (params[0] == 0.0) ? true : false;
                case MAV_CMD_NAV_TAKEOFF:
                    lat_deg = params[4];
                    lon_deg = params[5];
                    rel_alt_deg = params[6];
                    break;
                case MAV_CMD_NAV_RETURN_TO_LAUNCH:
                    break;
                default: // Unsupported mission command
                    LogErr() << "Mission: Unsupported mission command: " << command;
                    return Mission::Result::UNSUPPORTED_MISSION_CMD;
            }
            std::cout << "Cmd: " << command << " Lat: " << lat_deg << " Lon: " << lon_deg << " Alt: " <<
                      rel_alt_deg <<
                      " Speed " << speed_m_s << " Is fly thru: " << is_fly_through <<
                      " Gimbal pitch:  " << gimbal_pitch_deg << " Gimbal yaw: " << gimbal_yaw_deg <<
                      " Camera action: " << static_cast<int>(camera_action) << "\n";
            // Add a mission item
            items.push_back([&]() -> std::shared_ptr<MissionItem> const {
                std::shared_ptr<MissionItem> new_item(new MissionItem());
                new_item->set_position(lat_deg, lon_deg);
                new_item->set_relative_altitude(rel_alt_deg);
                new_item->set_speed(speed_m_s);
                new_item->set_fly_through(is_fly_through);
                new_item->set_gimbal_pitch_and_yaw(gimbal_pitch_deg, gimbal_yaw_deg);
                new_item->set_camera_action(camera_action);
                return new_item;
            }());
        }
        return Mission::Result::SUCCESS;
    }(parsed_plan["mission"]);

    return result;
}

} // namespace dronelin
