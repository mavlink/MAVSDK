#pragma once

#include "plugins/component_information/component_information.h"
#include "plugin_impl_base.h"

namespace mavsdk {

class ComponentInformationImpl : public PluginImplBase {
public:
    explicit ComponentInformationImpl(System& system);
    explicit ComponentInformationImpl(std::shared_ptr<System> system);
    ~ComponentInformationImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    std::pair<ComponentInformation::Result, std::vector<ComponentInformation::FloatParam>>
    access_float_params();

    void subscribe_float_param(ComponentInformation::FloatParamCallback callback);

private:
    void receive_component_information(
        MavlinkCommandSender::Result result, const mavlink_message_t& message);
};

} // namespace mavsdk