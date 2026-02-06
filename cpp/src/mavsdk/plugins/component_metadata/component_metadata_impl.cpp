#include "component_metadata_impl.h"

namespace mavsdk {

namespace fs = std::filesystem;

ComponentMetadataImpl::ComponentMetadataImpl(System& system) : PluginImplBase(system)
{
    _system_impl->register_plugin(this);
}

ComponentMetadataImpl::ComponentMetadataImpl(std::shared_ptr<System> system) :
    PluginImplBase(std::move(system))
{
    _system_impl->register_plugin(this);
}

ComponentMetadataImpl::~ComponentMetadataImpl()
{
    _system_impl->unregister_plugin(this);
}

void ComponentMetadataImpl::init() {}

void ComponentMetadataImpl::deinit() {}

void ComponentMetadataImpl::enable()
{
    const std::lock_guard lg{_mutex};
    request_componenents();
    _is_enabled = true;
}

void ComponentMetadataImpl::disable()
{
    _is_enabled = false;
}

void ComponentMetadataImpl::request_componenents()
{
    // Assumes _mutex is taken
    for (const auto& component : _components_to_request) {
        _system_impl->component_metadata().request_component(component);
    }
}

void ComponentMetadataImpl::request_component(uint32_t compid)
{
    const std::lock_guard lg{_mutex};
    _components_to_request.insert(compid);
    if (_is_enabled) {
        request_componenents();
    }
}

std::pair<ComponentMetadata::Result, ComponentMetadata::MetadataData>
ComponentMetadataImpl::get_metadata(uint32_t compid, ComponentMetadata::MetadataType type)
{
    auto result =
        _system_impl->component_metadata().get_metadata(compid, convert_metadata_type(type));
    return std::make_pair(
        convert_result(result.first), convert_metadata_data(std::move(result.second)));
}

void ComponentMetadataImpl::unsubscribe_metadata_available(
    ComponentMetadata::MetadataAvailableHandle handle)
{
    const auto converted_handle =
        _handle_factory.convert_to<MavlinkComponentMetadata::MetadataUpdate>(handle);
    _system_impl->component_metadata().unsubscribe_metadata_available(converted_handle);
}
ComponentMetadata::MetadataAvailableHandle ComponentMetadataImpl::subscribe_metadata_available(
    const ComponentMetadata::MetadataAvailableCallback& callback)
{
    const auto handle = _system_impl->component_metadata().subscribe_metadata_available(
        [temp_callback = callback](MavlinkComponentMetadata::MetadataUpdate update) {
            temp_callback(convert_metadata_update(std::move(update)));
        });
    return _handle_factory.convert_from(handle);
}
ComponentMetadata::Result
ComponentMetadataImpl::convert_result(MavlinkComponentMetadata::Result result)
{
    switch (result) {
        case MavlinkComponentMetadata::Result::Success:
            return ComponentMetadata::Result::Success;
        case MavlinkComponentMetadata::Result::NotAvailable:
            return ComponentMetadata::Result::NotAvailable;
        case MavlinkComponentMetadata::Result::ConnectionError:
            return ComponentMetadata::Result::ConnectionError;
        case MavlinkComponentMetadata::Result::Unsupported:
            return ComponentMetadata::Result::Unsupported;
        case MavlinkComponentMetadata::Result::Denied:
            return ComponentMetadata::Result::Denied;
        case MavlinkComponentMetadata::Result::Failed:
            return ComponentMetadata::Result::Failed;
        case MavlinkComponentMetadata::Result::Timeout:
            return ComponentMetadata::Result::Timeout;
        case MavlinkComponentMetadata::Result::NoSystem:
            return ComponentMetadata::Result::NoSystem;
        case MavlinkComponentMetadata::Result::NotRequested:
            return ComponentMetadata::Result::NotRequested;
    }
    return ComponentMetadata::Result::Failed;
}
ComponentMetadata::MetadataType
ComponentMetadataImpl::convert_metadata_type(MavlinkComponentMetadata::MetadataType type)
{
    switch (type) {
        case MavlinkComponentMetadata::MetadataType::AllCompleted:
            return ComponentMetadata::MetadataType::AllCompleted;
        case MavlinkComponentMetadata::MetadataType::Parameter:
            return ComponentMetadata::MetadataType::Parameter;
        case MavlinkComponentMetadata::MetadataType::Events:
            return ComponentMetadata::MetadataType::Events;
        case MavlinkComponentMetadata::MetadataType::Actuators:
            return ComponentMetadata::MetadataType::Actuators;
    }
    return ComponentMetadata::MetadataType::Parameter;
}
MavlinkComponentMetadata::MetadataType
ComponentMetadataImpl::convert_metadata_type(ComponentMetadata::MetadataType type)
{
    switch (type) {
        case ComponentMetadata::MetadataType::AllCompleted:
            return MavlinkComponentMetadata::MetadataType::AllCompleted;
        case ComponentMetadata::MetadataType::Parameter:
            return MavlinkComponentMetadata::MetadataType::Parameter;
        case ComponentMetadata::MetadataType::Events:
            return MavlinkComponentMetadata::MetadataType::Events;
        case ComponentMetadata::MetadataType::Actuators:
            return MavlinkComponentMetadata::MetadataType::Actuators;
    }
    return MavlinkComponentMetadata::MetadataType::Parameter;
}
ComponentMetadata::MetadataData
ComponentMetadataImpl::convert_metadata_data(MavlinkComponentMetadata::MetadataData data)
{
    return ComponentMetadata::MetadataData{std::move(data.json_metadata)};
}
ComponentMetadata::MetadataUpdate
ComponentMetadataImpl::convert_metadata_update(MavlinkComponentMetadata::MetadataUpdate data)
{
    return ComponentMetadata::MetadataUpdate{
        data.compid, convert_metadata_type(data.type), std::move(data.json_metadata)};
}

} // namespace mavsdk
