#pragma once

#include "plugins/arm_authorizer_server/arm_authorizer_server.h"
#include "server_plugin_impl_base.h"
#include "callback_list.h"

namespace mavsdk {

class ArmAuthorizerServerImpl : public ServerPluginImplBase {
public:
    explicit ArmAuthorizerServerImpl(std::shared_ptr<ServerComponent> server_component);

    ~ArmAuthorizerServerImpl() override;

    void init() override;
    void deinit() override;

    ArmAuthorizerServer::ArmAuthorizationHandle
    subscribe_arm_authorization(const ArmAuthorizerServer::ArmAuthorizationCallback& callback);

    void unsubscribe_arm_authorization(ArmAuthorizerServer::ArmAuthorizationHandle handle);

    ArmAuthorizerServer::Result accept_arm_authorization(int32_t valid_time_s) const;
    ArmAuthorizerServer::Result reject_arm_authorization(
        bool temporarily, ArmAuthorizerServer::RejectionReason reason, int32_t extra_info) const;

private:
    CallbackList<uint32_t> _arm_authorization_callbacks{};

    MavlinkCommandReceiver::CommandLong _last_arm_authorization_request_command;

    std::optional<mavlink_command_ack_t>
    process_arm_authorization_request(const MavlinkCommandReceiver::CommandLong& command);
};

} // namespace mavsdk