#pragma once

#include "plugins/mavlink_direct_server/mavlink_direct_server.hpp"

#include "server_plugin_impl_base.hpp"
#include "callback_list.hpp"

#include <map>
#include <mutex>

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

    MavlinkDirectServer::CommandHandle
    subscribe_command(uint32_t command, const MavlinkDirectServer::CommandCallback& callback);

    void unsubscribe_command(MavlinkDirectServer::CommandHandle handle);

private:
    // Internal callback management
    CallbackList<MavlinkDirectServer::MavlinkMessage> _callbacks{
        _server_component_impl->io_context()};
    Handle<Mavsdk::MavlinkMessage> _server_subscription{};

    void register_command_handlers(uint16_t command_id);
    void queue_command(const mavlink_message_t& message);

    // Commands are delivered separately, only to subscribers of that command ID.
    CallbackList<MavlinkDirectServer::MavlinkMessage> _command_callbacks{
        _server_component_impl->io_context()};
    std::mutex _command_subscriptions_mutex{};
    std::map<MavlinkDirectServer::CommandHandle, uint16_t> _command_subscriptions{};
    std::map<uint16_t, unsigned> _command_subscription_counts{};

    bool _debugging = false;
};

} // namespace mavsdk
