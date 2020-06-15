#include "autopilot_interface_impl.h"
#include "node.h"
#include "mavlink_commands.h"


namespace mavsdk {

using namespace std::placeholders;

AutopilotInterfaceImpl::AutopilotInterfaceImpl(std::shared_ptr<NodeImpl> node_impl) :
    _uuid{node_impl->get_system_id()},
    _node_impl{node_impl}
{
    _node_impl->register_mavlink_message_handler(
            MAVLINK_MSG_ID_AUTOPILOT_VERSION, 
            std::bind(&AutopilotInterfaceImpl::process_autopilot_version, this, _1),
            this);
    request_autopilot_version();
}

uint64_t AutopilotInterfaceImpl::get_uuid() const
{
    //return 1234;
    return _uuid;
};

void AutopilotInterfaceImpl::request_autopilot_version()
{
    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_REQUEST_AUTOPILOT_CAPABILITIES;
    command.params.param1 = 1.0f;

    _node_impl->send_command_async(command, nullptr);
}

void AutopilotInterfaceImpl::process_autopilot_version(const mavlink_message_t& message)
{
    mavlink_autopilot_version_t autopilot_version;
    mavlink_msg_autopilot_version_decode(&message, &autopilot_version);

    if (autopilot_version.uid != 0) {
        _uuid = autopilot_version.uid;
    }
}

} // namespace mavsdk
