#include "global_include.h"
#include "offboard_impl.h"
#include "dronecore_impl.h"
#include "px4_custom_mode.h"

namespace dronecore {

OffboardImpl::OffboardImpl() :
    _offboard_mode_active(false),
    _result_callback(nullptr)
{
}

OffboardImpl::~OffboardImpl()
{

}

void OffboardImpl::init()
{
    // We need the system state.
    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_HEARTBEAT,
        std::bind(&OffboardImpl::process_heartbeat, this, std::placeholders::_1),
        (void *)this);
}

void OffboardImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers((void *)this);
}

Offboard::Result OffboardImpl::start() const
{
    // Note: the safety flag is not needed in future versions of the PX4 Firmware
    //       but want to be rather safe than sorry.
    uint8_t flag_safety_armed = _parent->is_armed() ? MAV_MODE_FLAG_SAFETY_ARMED : 0;

    uint8_t mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | flag_safety_armed;
    uint8_t custom_mode = px4::PX4_CUSTOM_MAIN_MODE_OFFBOARD;
    uint8_t custom_sub_mode = 0;

    return offboard_result_from_command_result(
               _parent->send_command_with_ack(
                   MAV_CMD_DO_SET_MODE,
                   MavlinkCommands::Params {float(mode),
                                            float(custom_mode),
                                            float(custom_sub_mode),
                                            NAN, NAN, NAN, NAN},
                   MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT));
}

Offboard::Result OffboardImpl::stop() const
{
    // Note: the safety flag is not needed in future versions of the PX4 Firmware
    //       but want to be rather safe than sorry.
    uint8_t flag_safety_armed = _parent->is_armed() ? MAV_MODE_FLAG_SAFETY_ARMED : 0;

    uint8_t mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | flag_safety_armed;
    uint8_t custom_mode = px4::PX4_CUSTOM_MAIN_MODE_AUTO;
    uint8_t custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_LOITER;

    return offboard_result_from_command_result(
               _parent->send_command_with_ack(
                   MAV_CMD_DO_SET_MODE,
                   MavlinkCommands::Params {float(mode),
                                            float(custom_mode),
                                            float(custom_sub_mode),
                                            NAN, NAN, NAN, NAN},
                   MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT));
}

void OffboardImpl::start_async(Offboard::result_callback_t callback)
{
    // Note: the safety flag is not needed in future versions of the PX4 Firmware
    //       but want to be rather safe than sorry.
    uint8_t flag_safety_armed = _parent->is_armed() ? MAV_MODE_FLAG_SAFETY_ARMED : 0;

    uint8_t mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | flag_safety_armed;
    uint8_t custom_mode = px4::PX4_CUSTOM_MAIN_MODE_OFFBOARD;
    uint8_t custom_sub_mode = 0;

    _parent->send_command_with_ack_async(
        MAV_CMD_DO_SET_MODE,
        MavlinkCommands::Params {float(mode),
                                 float(custom_mode),
                                 float(custom_sub_mode),
                                 NAN, NAN, NAN, NAN},
        std::bind(&OffboardImpl::receive_command_result, this,
                  std::placeholders::_1, callback),
        MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT);

    _result_callback = callback;

    _parent->register_timeout_handler(std::bind(&OffboardImpl::timeout_happened, this), 1.0,
                                      &_timeout_cookie);
}

void OffboardImpl::stop_async(Offboard::result_callback_t callback)
{
    // Note: the safety flag is not needed in future versions of the PX4 Firmware
    //       but want to be rather safe than sorry.
    uint8_t flag_safety_armed = _parent->is_armed() ? MAV_MODE_FLAG_SAFETY_ARMED : 0;

    uint8_t mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | flag_safety_armed;
    uint8_t custom_mode = px4::PX4_CUSTOM_MAIN_MODE_AUTO;
    uint8_t custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_LOITER;

    _parent->send_command_with_ack_async(
        MAV_CMD_DO_SET_MODE,
        MavlinkCommands::Params {float(mode),
                                 float(custom_mode),
                                 float(custom_sub_mode),
                                 NAN, NAN, NAN, NAN},
        std::bind(&OffboardImpl::receive_command_result, this,
                  std::placeholders::_1, callback),
        MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT);

    _result_callback = callback;

    _parent->register_timeout_handler(std::bind(&OffboardImpl::timeout_happened, this), 1.0,
                                      &_timeout_cookie);
}

void OffboardImpl::receive_command_result(MavlinkCommands::Result result,
                                          const Offboard::result_callback_t &callback)
{
    // We got a command back, so we can get rid of the timeout handler.
    _parent->unregister_timeout_handler(_timeout_cookie);

    Offboard::Result offboard_result = offboard_result_from_command_result(result);

    callback(offboard_result);
}

void OffboardImpl::timeout_happened()
{
    report_offboard_result(_result_callback, Offboard::Result::TIMEOUT);
}

void OffboardImpl::report_offboard_result(const Offboard::result_callback_t &callback,
                                          Offboard::Result result)
{
    if (callback == nullptr) {
        LogWarn() << "Callback is not set";
        return;
    }

    callback(result);
}

void OffboardImpl::set_velocity_ned(Offboard::VelocityNEDYaw velocity_ned_yaw)
{
    const static uint16_t IGNORE_X = (1 << 0);
    const static uint16_t IGNORE_Y = (1 << 1);
    const static uint16_t IGNORE_Z = (1 << 2);
    //const static uint16_t IGNORE_VX = (1 << 3);
    //const static uint16_t IGNORE_VY = (1 << 4);
    //const static uint16_t IGNORE_VZ = (1 << 5);
    const static uint16_t IGNORE_AX = (1 << 6);
    const static uint16_t IGNORE_AY = (1 << 7);
    const static uint16_t IGNORE_AZ = (1 << 8);
    //const static uint16_t IS_FORCE = (1 << 9);
    //const static uint16_t IGNORE_YAW = (1 << 10);
    const static uint16_t IGNORE_YAW_RATE = (1 << 11);

    const float yaw = (float)to_rad_from_deg(velocity_ned_yaw.yaw_deg);
    const float yaw_rate = 0.0f;
    const float x = 0.0f;
    const float y = 0.0f;
    const float z = 0.0f;
    const float vx = velocity_ned_yaw.north_m_s;
    const float vy = velocity_ned_yaw.east_m_s;
    const float vz = velocity_ned_yaw.down_m_s;
    const float afx = 0.0f;
    const float afy = 0.0f;
    const float afz = 0.0f;

    mavlink_message_t message;
    mavlink_msg_set_position_target_local_ned_pack(_parent->get_own_system_id(),
                                                   _parent->get_own_component_id(),
                                                   &message,
                                                   (uint32_t)(elapsed_s() * 1e3f),
                                                   _parent->get_target_system_id(),
                                                   _parent->get_target_component_id(),
                                                   MAV_FRAME_LOCAL_NED,
                                                   IGNORE_X | IGNORE_Y | IGNORE_Z |
                                                   IGNORE_AX | IGNORE_AY | IGNORE_AZ |
                                                   IGNORE_YAW_RATE,
                                                   x, y, z, vx, vy, vz, afx, afy, afz,
                                                   yaw, yaw_rate);
    _parent->send_message(message);
}

void OffboardImpl::set_velocity_body(Offboard::VelocityBodyYawspeed velocity_body_yawspeed)
{
    const static uint16_t IGNORE_X = (1 << 0);
    const static uint16_t IGNORE_Y = (1 << 1);
    const static uint16_t IGNORE_Z = (1 << 2);
    //const static uint16_t IGNORE_VX = (1 << 3);
    //const static uint16_t IGNORE_VY = (1 << 4);
    //const static uint16_t IGNORE_VZ = (1 << 5);
    const static uint16_t IGNORE_AX = (1 << 6);
    const static uint16_t IGNORE_AY = (1 << 7);
    const static uint16_t IGNORE_AZ = (1 << 8);
    //const static uint16_t IS_FORCE = (1 << 9);
    const static uint16_t IGNORE_YAW = (1 << 10);
    //const static uint16_t IGNORE_YAW_RATE = (1 << 11);

    const float yaw = 0.0f;
    const float yaw_rate = (float)to_rad_from_deg(velocity_body_yawspeed.yawspeed_deg_s);
    const float x = 0.0f;
    const float y = 0.0f;
    const float z = 0.0f;
    const float vx = velocity_body_yawspeed.forward_m_s;
    const float vy = velocity_body_yawspeed.right_m_s;
    const float vz = velocity_body_yawspeed.down_m_s;
    const float afx = 0.0f;
    const float afy = 0.0f;
    const float afz = 0.0f;

    mavlink_message_t message;
    mavlink_msg_set_position_target_local_ned_pack(_parent->get_own_system_id(),
                                                   _parent->get_own_component_id(),
                                                   &message,
                                                   (uint32_t)(elapsed_s() * 1e3f),
                                                   _parent->get_target_system_id(),
                                                   _parent->get_target_component_id(),
                                                   MAV_FRAME_BODY_NED,
                                                   IGNORE_X | IGNORE_Y | IGNORE_Z |
                                                   IGNORE_AX | IGNORE_AY | IGNORE_AZ |
                                                   IGNORE_YAW,
                                                   x, y, z, vx, vy, vz, afx, afy, afz,
                                                   yaw, yaw_rate);
    _parent->send_message(message);
}

void OffboardImpl::process_heartbeat(const mavlink_message_t &message)
{
    mavlink_heartbeat_t heartbeat;
    mavlink_msg_heartbeat_decode(&message, &heartbeat);

    bool offboard_mode_active = false;
    if (heartbeat.base_mode & MAV_MODE_FLAG_CUSTOM_MODE_ENABLED) {

        px4::px4_custom_mode px4_custom_mode;
        px4_custom_mode.data = heartbeat.custom_mode;

        if (px4_custom_mode.main_mode == px4::PX4_CUSTOM_MAIN_MODE_OFFBOARD) {
            offboard_mode_active = true;
        }
    }
    _offboard_mode_active = offboard_mode_active;
}

Offboard::Result
OffboardImpl::offboard_result_from_command_result(MavlinkCommands::Result result)
{
    switch (result) {
        case MavlinkCommands::Result::SUCCESS:
            return Offboard::Result::SUCCESS;
        case MavlinkCommands::Result::NO_DEVICE:
            return Offboard::Result::NO_DEVICE;
        case MavlinkCommands::Result::CONNECTION_ERROR:
            return Offboard::Result::CONNECTION_ERROR;
        case MavlinkCommands::Result::BUSY:
            return Offboard::Result::BUSY;
        case MavlinkCommands::Result::COMMAND_DENIED:
            return Offboard::Result::COMMAND_DENIED;
        case MavlinkCommands::Result::TIMEOUT:
            return Offboard::Result::TIMEOUT;
        default:
            return Offboard::Result::UNKNOWN;
    }
}


} // namespace dronecore
