#pragma once

#include "plugins/component_information_server/component_information_server.h"
#include "plugin_impl_base.h"

namespace mavsdk {

class ComponentInformationServerImpl : public PluginImplBase {
public:
    explicit ComponentInformationServerImpl(System& system);
    explicit ComponentInformationServerImpl(std::shared_ptr<System> system);
    ~ComponentInformationServerImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    ComponentInformationServer::Result provide_peripheral_file(const std::string& path);

private:
    std::optional<MAV_RESULT> process_component_information_requested();

    std::string _path{};
    uint32_t _crc32{0};
};

} // namespace mavsdk
