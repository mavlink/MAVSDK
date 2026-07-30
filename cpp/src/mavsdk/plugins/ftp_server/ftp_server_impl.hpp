#pragma once

#include "plugins/ftp_server/ftp_server.hpp"

#include <string>
#include <mutex>
#include "server_plugin_impl_base.hpp"

namespace mavsdk {

class FtpServerImpl : public ServerPluginImplBase {
public:
    explicit FtpServerImpl(std::shared_ptr<ServerComponent> server_component);

    ~FtpServerImpl() override;

    void init() override;
    void deinit() override;

    FtpServer::Result set_root_dir(const std::string& path);

private:
    std::mutex _root_dir_mutex{};
    std::string _root_dir{};
};

} // namespace mavsdk
