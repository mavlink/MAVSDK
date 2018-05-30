#include "global_include.h"
#include "logging_impl.h"
#include "dronecore_impl.h"
#include "px4_custom_mode.h"

namespace dronecore {

LoggingImpl::LoggingImpl(System &system) : PluginImplBase(system)
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
        MAVLINK_MSG_ID_LOGGING_DATA, std::bind(&LoggingImpl::process_logging_data, this, _1), this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_LOGGING_DATA_ACKED,
        std::bind(&LoggingImpl::process_logging_data_acked, this, _1),
        this);
}

void LoggingImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void LoggingImpl::enable() {}

void LoggingImpl::disable() {}

Logging::Result LoggingImpl::start_logging() const
{
    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_LOGGING_START;
    MAVLinkCommands::CommandLong::set_as_reserved(command.params, 0.f);
    command.target_component_id = _parent->get_autopilot_id();

    return logging_result_from_command_result(_parent->send_command(command));
}

Logging::Result LoggingImpl::stop_logging() const
{
    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_LOGGING_STOP;
    MAVLinkCommands::CommandLong::set_as_reserved(command.params, 0.f);
    command.target_component_id = _parent->get_autopilot_id();

    return logging_result_from_command_result(_parent->send_command(command));
}

void LoggingImpl::start_logging_async(const Logging::result_callback_t &callback)
{
    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_LOGGING_START;
    MAVLinkCommands::CommandLong::set_as_reserved(command.params, 0.f);
    command.target_component_id = _parent->get_autopilot_id();

    _parent->send_command_async(
        command, std::bind(&LoggingImpl::command_result_callback, std::placeholders::_1, callback));
}

void LoggingImpl::stop_logging_async(const Logging::result_callback_t &callback)
{
    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_LOGGING_STOP;
    MAVLinkCommands::CommandLong::set_as_reserved(command.params, 0.f);
    command.target_component_id = _parent->get_autopilot_id();

    _parent->send_command_async(
        command, std::bind(&LoggingImpl::command_result_callback, std::placeholders::_1, callback));
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
    mavlink_msg_logging_ack_pack(GCSClient::system_id,
                                 GCSClient::component_id,
                                 &answer,
                                 _parent->get_system_id(),
                                 _parent->get_autopilot_id(),
                                 logging_data_acked.sequence);

    _parent->send_message(answer);
}

Logging::Result LoggingImpl::logging_result_from_command_result(MAVLinkCommands::Result result)
{
    switch (result) {
        case MAVLinkCommands::Result::SUCCESS:
            return Logging::Result::SUCCESS;
        case MAVLinkCommands::Result::NO_SYSTEM:
            return Logging::Result::NO_SYSTEM;
        case MAVLinkCommands::Result::CONNECTION_ERROR:
            return Logging::Result::CONNECTION_ERROR;
        case MAVLinkCommands::Result::BUSY:
            return Logging::Result::BUSY;
        case MAVLinkCommands::Result::COMMAND_DENIED:
            return Logging::Result::COMMAND_DENIED;
        case MAVLinkCommands::Result::TIMEOUT:
            return Logging::Result::TIMEOUT;
        default:
            return Logging::Result::UNKNOWN;
    }
}

void LoggingImpl::command_result_callback(MAVLinkCommands::Result command_result,
                                          const Logging::result_callback_t &callback)
{
    Logging::Result action_result = logging_result_from_command_result(command_result);

    callback(action_result);
}

} // namespace dronecore
