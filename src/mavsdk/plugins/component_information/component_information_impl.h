#pragma once

#include "plugins/component_information/component_information.h"
#include "plugin_impl_base.h"
#include "callback_list.h"

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

    ComponentInformation::FloatParamHandle
    subscribe_float_param(const ComponentInformation::FloatParamCallback& callback);
    void unsubscribe_float_param(ComponentInformation::FloatParamHandle handle);

private:
    void receive_component_information(
        MavlinkCommandSender::Result result, const mavlink_message_t& message);

    void
    download_file_async(const std::string& uri, std::function<void(std::string path)> callback);
    void parse_metadata_file(const std::string& path);
    void parse_parameter_file(const std::string& path);

    void get_float_param_result(
        const std::string& name, MavlinkParameterClient::Result result, float value);

    void param_update(const std::string& name, float new_value);

    std::mutex _mutex{};
    std::vector<ComponentInformation::FloatParam> _float_params{};

    CallbackList<ComponentInformation::FloatParamUpdate> _float_param_update_callbacks{};
};

} // namespace mavsdk
