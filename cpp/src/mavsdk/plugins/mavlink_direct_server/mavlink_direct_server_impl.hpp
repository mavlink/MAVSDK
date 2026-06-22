#pragma once

#include "plugins/mavlink_direct_server/mavlink_direct_server.hpp"

#include "server_plugin_impl_base.hpp"

namespace mavsdk {

class MavlinkDirectServerImpl : public ServerPluginImplBase {
public:
    explicit MavlinkDirectServerImpl(std::shared_ptr<ServerComponent> server_component);

    ~MavlinkDirectServerImpl() override;

    void init() override;
    void deinit() override;

    MavlinkDirectServer::Result send_message(MavlinkDirectServer::MavlinkMessage message);

    MavlinkDirectServer::Result load_custom_xml(std::string xml_content);

private:
    bool _debugging = false;
};

} // namespace mavsdk
