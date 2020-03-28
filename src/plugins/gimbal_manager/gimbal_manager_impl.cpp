#include "gimbal_manager_impl.h"
#include "global_include.h"
#include <functional>
#include <cmath>

namespace mavsdk {

GimbalManagerImpl::GimbalManagerImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

GimbalManagerImpl::~GimbalManagerImpl()
{
    _parent->unregister_plugin(this);
}

void GimbalManagerImpl::init() {}

void GimbalManagerImpl::deinit() {}

void GimbalManagerImpl::enable() {}

void GimbalManagerImpl::disable() {}

void GimbalManagerImpl::request_information()
{
    //MAVLinkCommands::CommandLong command{};

    //command.command = MAV_CMD_REQUEST_MESSAGE;
    //command.params.param1 = MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION;
    //command.params.param7 = 1;

    //_parent->send_command_async(command, NULL);
}

void GimbalManagerImpl::subscribe_information_async(
        std::function<void(const GimbalManager::Information information)> callback)
{
    callback = NULL;
}

GimbalManager::Result GimbalManagerImpl::set_attitude(float tilt_rate, float pan_rate,
        float tilt_deg, float pan_deg, int operation_flags,
        uint8_t id)
{
    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_DO_GIMBAL_MANAGER_ATTITUDE;
    command.params.param1 = tilt_rate;
    command.params.param2 = pan_rate;
    command.params.param3 = tilt_deg;
    command.params.param4 = pan_deg;
    command.params.param5 = operation_flags;
    command.params.param7 = id;

    // TODO probably keep track of gimbal manager ID, its not guranteed to be on the autopilot
    command.target_component_id = _parent->get_autopilot_id();

    return gimbal_manager_result_from_command_result(_parent->send_command(command));
}

void GimbalManagerImpl::set_attitude_async(float tilt_rate, float pan_rate,
        float tilt_deg, float pan_deg, int operation_flags,
        uint8_t id, GimbalManager::result_callback_t callback)
{
    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_DO_GIMBAL_MANAGER_ATTITUDE;
    command.params.param1 = tilt_rate;
    command.params.param2 = pan_rate;
    command.params.param3 = tilt_deg;
    command.params.param4 = pan_deg;
    command.params.param5 = operation_flags;
    command.params.param7 = id;

    // TODO probably keep track of gimbal manager ID, its not guranteed to be on the autopilot
    command.target_component_id = _parent->get_autopilot_id();

    _parent->send_command_async(
            command, std::bind(&GimbalManagerImpl::receive_command_result, std::placeholders::_1, callback));
}

GimbalManager::Result GimbalManagerImpl::track_point(float x, float y, uint8_t id)
{
    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_DO_GIMBAL_MANAGER_TRACK_POINT;
    command.params.param1 = x;
    command.params.param2 = y;
    command.params.param7 = id;

    // TODO probably keep track of gimbal manager ID, its not guranteed to be on the autopilot
    command.target_component_id = _parent->get_autopilot_id();

    return gimbal_manager_result_from_command_result(_parent->send_command(command));
}

void GimbalManagerImpl::track_point_async(float x, float y, uint8_t id, GimbalManager::result_callback_t callback)
{
    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_DO_GIMBAL_MANAGER_TRACK_POINT;
    command.params.param1 = x;
    command.params.param2 = y;
    command.params.param7 = id;

    // TODO probably keep track of gimbal manager ID, its not guranteed to be on the autopilot
    command.target_component_id = _parent->get_autopilot_id();

    _parent->send_command_async(
            command, std::bind(&GimbalManagerImpl::receive_command_result, std::placeholders::_1, callback));
}

GimbalManager::Result GimbalManagerImpl::track_rectangle(float x1, float y1,
        float x2, float y2, uint8_t id)
{
    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_DO_GIMBAL_MANAGER_TRACK_RECTANGLE;
    command.params.param1 = x1;
    command.params.param2 = y1;
    command.params.param3 = x2;
    command.params.param4 = y2;
    command.params.param7 = id;

    // TODO probably keep track of gimbal manager ID, its not guranteed to be on the autopilot
    command.target_component_id = _parent->get_autopilot_id();

    return gimbal_manager_result_from_command_result(_parent->send_command(command));
}

void GimbalManagerImpl::track_rectangle_async(float x1, float y1, float x2, float y2, uint8_t id, GimbalManager::result_callback_t callback)
{
    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_DO_GIMBAL_MANAGER_TRACK_RECTANGLE;
    command.params.param1 = x1;
    command.params.param2 = y1;
    command.params.param3 = x2;
    command.params.param4 = y2;
    command.params.param7 = id;

    // TODO probably keep track of gimbal manager ID, its not guranteed to be on the autopilot
    command.target_component_id = _parent->get_autopilot_id();

    _parent->send_command_async(
            command, std::bind(&GimbalManagerImpl::receive_command_result, std::placeholders::_1, callback));
}

GimbalManager::Result GimbalManagerImpl::set_roi_location(double latitude_deg, double longitude_deg, float altitude_m)
{
    MAVLinkCommands::CommandInt command{};

    command.command = MAV_CMD_DO_SET_ROI_LOCATION;
    command.params.x = int32_t(std::round(latitude_deg * 1e7));
    command.params.y = int32_t(std::round(longitude_deg * 1e7));
    command.params.z = altitude_m;

    // TODO probably keep track of gimbal manager ID, its not guranteed to be on the autopilot
    // we definetely DO NOT want to set the ROI of the autopilot
    command.target_component_id = _parent->get_autopilot_id();

    return gimbal_manager_result_from_command_result(_parent->send_command(command));
}

void GimbalManagerImpl::set_roi_location_async(double latitude_deg, double longitude_deg, float altitude_m, GimbalManager::result_callback_t callback)
{
    MAVLinkCommands::CommandInt command{};

    command.command = MAV_CMD_DO_SET_ROI_LOCATION;
    command.params.x = int32_t(std::round(latitude_deg * 1e7));
    command.params.y = int32_t(std::round(longitude_deg * 1e7));
    command.params.z = altitude_m;

    // TODO probably keep track of gimbal manager ID, its not guranteed to be on the autopilot
    // we definetely DO NOT want to set the ROI of the autopilot
    command.target_component_id = _parent->get_autopilot_id();

    _parent->send_command_async(
            command, std::bind(&GimbalManagerImpl::receive_command_result, std::placeholders::_1, callback));
}

void GimbalManagerImpl::receive_command_result(
        MAVLinkCommands::Result command_result, const GimbalManager::result_callback_t& callback)
{
    GimbalManager::Result gimbal_manager_result = gimbal_manager_result_from_command_result(command_result);

    if (callback) {
        callback(gimbal_manager_result);
    }
}

GimbalManager::Result GimbalManagerImpl::gimbal_manager_result_from_command_result(MAVLinkCommands::Result command_result)
{
    switch (command_result) {
        case MAVLinkCommands::Result::SUCCESS:
            return GimbalManager::Result::SUCCESS;
        case MAVLinkCommands::Result::TIMEOUT:
            return GimbalManager::Result::TIMEOUT;
        case MAVLinkCommands::Result::NO_SYSTEM:
        case MAVLinkCommands::Result::CONNECTION_ERROR:
        case MAVLinkCommands::Result::BUSY:
        case MAVLinkCommands::Result::COMMAND_DENIED:
        default:
            return GimbalManager::Result::ERROR;
    }
}

} // namespace mavsdk
