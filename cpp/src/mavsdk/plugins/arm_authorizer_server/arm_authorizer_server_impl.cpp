#include "arm_authorizer_server_impl.h"

#include <utility>
#include "log.h"

namespace mavsdk {

ArmAuthorizerServerImpl::ArmAuthorizerServerImpl(
    std::shared_ptr<ServerComponent> server_component) :
    ServerPluginImplBase(std::move(server_component))
{
    _server_component_impl->register_plugin(this);
}

ArmAuthorizerServerImpl::~ArmAuthorizerServerImpl()
{
    _server_component_impl->unregister_plugin(this);
}

void ArmAuthorizerServerImpl::init()
{
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_ARM_AUTHORIZATION_REQUEST,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            return process_arm_authorization_request(command);
        },
        this);
}

void ArmAuthorizerServerImpl::deinit() {}

ArmAuthorizerServer::ArmAuthorizationHandle ArmAuthorizerServerImpl::subscribe_arm_authorization(
    const ArmAuthorizerServer::ArmAuthorizationCallback& callback)
{
    return _arm_authorization_callbacks.subscribe(callback);
}

void ArmAuthorizerServerImpl::unsubscribe_arm_authorization(
    ArmAuthorizerServer::ArmAuthorizationHandle handle)
{
    _arm_authorization_callbacks.unsubscribe(handle);
}

std::optional<mavlink_command_ack_t> ArmAuthorizerServerImpl::process_arm_authorization_request(
    const MavlinkCommandReceiver::CommandLong& command)
{
    if (_arm_authorization_callbacks.empty()) {
        LogDebug() << "Set mode requested with no user callback";
        return _server_component_impl->make_command_ack_message(
            command, MAV_RESULT::MAV_RESULT_UNSUPPORTED);
    }
    auto const system_id = static_cast<uint32_t>(command.params.param1);

    _last_arm_authorization_request_command = command;
    _arm_authorization_callbacks(system_id);

    return std::nullopt;
}

ArmAuthorizerServer::Result
ArmAuthorizerServerImpl::accept_arm_authorization(int32_t valid_time_s) const
{
    auto command_ack = _server_component_impl->make_command_ack_message(
        _last_arm_authorization_request_command, MAV_RESULT_ACCEPTED);

    // If the arm is authorized, param2 is set to the time in seconds through which the
    // authorization is valid
    command_ack.result_param2 = valid_time_s;

    if (!_server_component_impl->send_command_ack(command_ack)) {
        return ArmAuthorizerServer::Result::Failed;
    }
    return ArmAuthorizerServer::Result::Success;
}

ArmAuthorizerServer::Result ArmAuthorizerServerImpl::reject_arm_authorization(
    bool temporarily, ArmAuthorizerServer::RejectionReason reason, int32_t extra_info) const
{
    MAV_RESULT result = temporarily ? MAV_RESULT_TEMPORARILY_REJECTED : MAV_RESULT_DENIED;
    auto command_ack = _server_component_impl->make_command_ack_message(
        _last_arm_authorization_request_command, result);

    // Fill in progress, which is the reason for arm rejected or 0 if arm was allowed
    switch (reason) {
        case ArmAuthorizerServer::RejectionReason::Generic:
            command_ack.progress = static_cast<uint8_t>(MAV_ARM_AUTH_DENIED_REASON_GENERIC);
            break;
        case ArmAuthorizerServer::RejectionReason::Timeout:
            command_ack.progress = static_cast<uint8_t>(MAV_ARM_AUTH_DENIED_REASON_TIMEOUT);
            break;
        case ArmAuthorizerServer::RejectionReason::BadWeather:
            command_ack.progress = static_cast<uint8_t>(MAV_ARM_AUTH_DENIED_REASON_BAD_WEATHER);
            break;
        case ArmAuthorizerServer::RejectionReason::InvalidWaypoint:
            command_ack.progress =
                static_cast<uint8_t>(MAV_ARM_AUTH_DENIED_REASON_INVALID_WAYPOINT);
            break;
        case ArmAuthorizerServer::RejectionReason::AirspaceInUse:
            command_ack.progress = static_cast<uint8_t>(MAV_ARM_AUTH_DENIED_REASON_AIRSPACE_IN_USE);
            break;
        case ArmAuthorizerServer::RejectionReason::None:
            [[fallthrough]];
        default:
            command_ack.progress = static_cast<uint8_t>(MAV_ARM_AUTH_DENIED_REASON_NONE);
            break;
    }

    // Fill in the extra information. It is dependent on the decision, and and reason
    command_ack.result_param2 = extra_info;

    if (!_server_component_impl->send_command_ack(command_ack)) {
        return ArmAuthorizerServer::Result::Failed;
    }

    return ArmAuthorizerServer::Result::Success;
}

} // namespace mavsdk
