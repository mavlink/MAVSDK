#include "server_utility_impl.h"

namespace mavsdk {

ServerUtilityImpl::ServerUtilityImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

ServerUtilityImpl::ServerUtilityImpl(std::shared_ptr<System> system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

ServerUtilityImpl::~ServerUtilityImpl()
{
    _parent->unregister_plugin(this);
}

void ServerUtilityImpl::init() {}

void ServerUtilityImpl::deinit() {}

void ServerUtilityImpl::enable() {}

void ServerUtilityImpl::disable() {}





ServerUtility::Result ServerUtilityImpl::send_status_text(ServerUtility::StatusTextType type, std::string text)
{
    
    UNUSED(type);
    
    UNUSED(text);
    

    // TODO :)
    return {};
}



} // namespace mavsdk