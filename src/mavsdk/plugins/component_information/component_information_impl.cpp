#include "component_information_impl.h"

namespace mavsdk {

ComponentInformationImpl::ComponentInformationImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

ComponentInformationImpl::ComponentInformationImpl(std::shared_ptr<System> system) :
    PluginImplBase(system)
{
    _parent->register_plugin(this);
}

ComponentInformationImpl::~ComponentInformationImpl()
{
    _parent->unregister_plugin(this);
}

void ComponentInformationImpl::init() {}

void ComponentInformationImpl::deinit() {}

void ComponentInformationImpl::enable()
{
    _parent->request_message().request(
        MAVLINK_MSG_ID_COMPONENT_INFORMATION,
        [this](auto&& result, auto&& message) { receive_component_information(result, message); });
}

void ComponentInformationImpl::disable() {}

void ComponentInformationImpl::receive_component_information(
    MavlinkCommandSender::Result result, const mavlink_message_t& message)
{
    if (result != MavlinkCommandSender::Result::Success) {
        LogWarn() << "Requesting component information failed with " << static_cast<int>(result);
        return;
    }

    mavlink_component_information_t component_information;
    mavlink_msg_component_information_decode(&message, &component_information);

    component_information
        .general_metadata_uri[sizeof(component_information.general_metadata_uri) - 1] = '\0';

    // TODO:
    // 1. Download the file if CRC is different, and save it.
    // 2. Parse the xml.
    // 3. Update params from it.
}

std::pair<ComponentInformation::Result, std::vector<ComponentInformation::FloatParam>>
ComponentInformationImpl::access_float_params()
{
    // TODO: go through params and return list of them.
    return {};
}

void ComponentInformationImpl::subscribe_float_param(
    ComponentInformation::FloatParamCallback callback)
{
    (void)(callback);
}

} // namespace mavsdk