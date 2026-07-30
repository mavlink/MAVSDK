#pragma once

#include "plugins/mavlink_direct_server/mavlink_direct_server.hpp"

#include "server_plugin_impl_base.hpp"
#include "callback_list.hpp"

namespace mavsdk {

class MavlinkDirectServerImpl : public ServerPluginImplBase {
public:
    explicit MavlinkDirectServerImpl(std::shared_ptr<ServerComponent> server_component);

    ~MavlinkDirectServerImpl() override;

    void init() override;
    void deinit() override;

    MavlinkDirectServer::Result send_message(MavlinkDirectServer::MavlinkMessage message);

    MavlinkDirectServer::MessageHandle subscribe_message(
        std::string message_name, const MavlinkDirectServer::MessageCallback& callback);

    void unsubscribe_message(MavlinkDirectServer::MessageHandle handle);

    MavlinkDirectServer::Result load_custom_xml(std::string xml_content);

private:
    // Internal callback management
    CallbackList<MavlinkDirectServer::MavlinkMessage> _callbacks{
        _server_component_impl->io_context()};
    Handle<Mavsdk::MavlinkMessage> _server_subscription{};

    bool _debugging = false;
};

} // namespace mavsdk
