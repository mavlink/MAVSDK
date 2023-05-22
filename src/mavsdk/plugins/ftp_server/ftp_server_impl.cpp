#include "ftp_server_impl.h"

namespace mavsdk {

FtpServerImpl::FtpServerImpl(std::shared_ptr<ServerComponent> server_component) :
    ServerPluginImplBase(server_component)
{
    _server_component_impl->register_plugin(this);
}

FtpServerImpl::~FtpServerImpl()
{
    _server_component_impl->unregister_plugin(this);
}

void FtpServerImpl::init() {}

void FtpServerImpl::deinit() {}

FtpServer::Result FtpServerImpl::set_root_dir(const std::string& path)
{
    // TODO: only do this when the server is not busy.
    // TODO: check if it exists

    _server_component_impl->mavlink_ftp_server().set_root_directory(path);

    return FtpServer::Result::Success;
}

} // namespace mavsdk
