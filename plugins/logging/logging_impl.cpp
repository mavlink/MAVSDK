#include "global_include.h"
#include "logging_impl.h"
#include "dronecore_impl.h"
#include "px4_custom_mode.h"

namespace dronecore {

LoggingImpl::LoggingImpl(Device *device) :
    PluginImplBase(device)
{
    _parent->register_plugin(this);
}

LoggingImpl::~LoggingImpl()
{
    _parent->unregister_plugin(this);
}

void LoggingImpl::init()
{
    using namespace std::placeholders; // for `_1`

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_LOGGING_DATA,
        std::bind(&LoggingImpl::process_logging_data, this, _1), this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_LOGGING_DATA_ACKED,
        std::bind(&LoggingImpl::process_logging_data_acked, this, _1), this);
}

void LoggingImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void LoggingImpl::enable() {}

void LoggingImpl::disable() {}

Logging::Result LoggingImpl::start_logging() const
{
    return logging_result_from_command_result(
               _parent->send_command_with_ack(
                   MAV_CMD_LOGGING_START,
                   MavlinkCommands::Params {0.0f, // Format: ULog
                                            0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}));
}

Logging::Result LoggingImpl::stop_logging() const
{
    return logging_result_from_command_result(
               _parent->send_command_with_ack(
                   MAV_CMD_LOGGING_STOP,
                   MavlinkCommands::Params {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}));
}

void LoggingImpl::start_logging_async(const Logging::result_callback_t &callback)
{
    _parent->send_command_with_ack_async(
        MAV_CMD_LOGGING_START,
        MavlinkCommands::Params {0.0f, // Format: ULog
                                 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
        std::bind(&LoggingImpl::command_result_callback,
                  std::placeholders::_1,
                  callback));
}

void LoggingImpl::stop_logging_async(const Logging::result_callback_t &callback)
{
    _parent->send_command_with_ack_async(
        MAV_CMD_LOGGING_STOP,
        MavlinkCommands::Params {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
        std::bind(&LoggingImpl::command_result_callback,
                  std::placeholders::_1,
                  callback));
}

void LoggingImpl::process_logging_data(const mavlink_message_t &message)
{
    mavlink_logging_data_t logging_data;
    mavlink_msg_logging_data_decode(&message, &logging_data);
}

void LoggingImpl::process_logging_data_acked(const mavlink_message_t &message)
{
    mavlink_logging_data_acked_t logging_data_acked;
    mavlink_msg_logging_data_acked_decode(&message, &logging_data_acked);

    mavlink_message_t answer;
    mavlink_msg_logging_ack_pack(
        _parent->get_own_system_id(), _parent->get_own_component_id(),
        &answer,
        _parent->get_target_system_id(), _parent->get_target_component_id(),
        logging_data_acked.sequence);

    _parent->send_message(answer);
}

Logging::Result
LoggingImpl::logging_result_from_command_result(MavlinkCommands::Result result)
{
    switch (result) {
        case MavlinkCommands::Result::SUCCESS:
            return Logging::Result::SUCCESS;
        case MavlinkCommands::Result::NO_DEVICE:
            return Logging::Result::NO_DEVICE;
        case MavlinkCommands::Result::CONNECTION_ERROR:
            return Logging::Result::CONNECTION_ERROR;
        case MavlinkCommands::Result::BUSY:
            return Logging::Result::BUSY;
        case MavlinkCommands::Result::COMMAND_DENIED:
            return Logging::Result::COMMAND_DENIED;
        case MavlinkCommands::Result::TIMEOUT:
            return Logging::Result::TIMEOUT;
        default:
            return Logging::Result::UNKNOWN;
    }
}

void LoggingImpl::command_result_callback(MavlinkCommands::Result command_result,
                                          const Logging::result_callback_t &callback)
{
    Logging::Result action_result = logging_result_from_command_result(command_result);

    callback(action_result);
}


} // namespace dronecore
