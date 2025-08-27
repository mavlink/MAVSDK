#pragma once

#include "plugins/component_metadata/component_metadata.h"
#include "plugin_impl_base.h"
#include "handle_factory.h"

#include <set>

namespace mavsdk {

class ComponentMetadataImpl : public PluginImplBase {
public:
    explicit ComponentMetadataImpl(System& system);
    explicit ComponentMetadataImpl(std::shared_ptr<System> system);
    ~ComponentMetadataImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    void request_component(uint32_t compid);
    void request_autopilot_component() { request_component(MAV_COMP_ID_AUTOPILOT1); }

    std::pair<ComponentMetadata::Result, ComponentMetadata::MetadataData>
    get_metadata(uint32_t compid, ComponentMetadata::MetadataType type);

    ComponentMetadata::MetadataAvailableHandle
    subscribe_metadata_available(const ComponentMetadata::MetadataAvailableCallback& callback);
    void unsubscribe_metadata_available(ComponentMetadata::MetadataAvailableHandle handle);

private:
    void request_componenents();

    static ComponentMetadata::Result convert_result(MavlinkComponentMetadata::Result result);
    static ComponentMetadata::MetadataType
    convert_metadata_type(MavlinkComponentMetadata::MetadataType type);
    static MavlinkComponentMetadata::MetadataType
    convert_metadata_type(ComponentMetadata::MetadataType type);
    static ComponentMetadata::MetadataData
    convert_metadata_data(MavlinkComponentMetadata::MetadataData data);
    static ComponentMetadata::MetadataUpdate
    convert_metadata_update(MavlinkComponentMetadata::MetadataUpdate data);

    std::set<uint8_t> _components_to_request;
    bool _is_enabled{false};
    std::mutex _mutex{};

    HandleFactory<ComponentMetadata::MetadataUpdate> _handle_factory;
};

} // namespace mavsdk
