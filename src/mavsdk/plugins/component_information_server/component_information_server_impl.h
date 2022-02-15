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

    ComponentInformationServer::Result set_our_ip(const std::string& ip);

    ComponentInformationServer::Result
    provide_float_param(ComponentInformationServer::FloatParam param);

    void subscribe_float_param(ComponentInformationServer::FloatParamCallback callback);

private:
    void update_json_files_with_lock();
    void param_update(const std::string& name, float new_value);

    std::string generate_parameter_file();
    std::string generate_meta_file();

    std::optional<MAV_RESULT> process_component_information_requested();

    std::mutex _mutex{};
    std::vector<ComponentInformationServer::FloatParam> _float_params{};
    std::string _our_ip{};
    ComponentInformationServer::FloatParamCallback _float_param_update_callback{};
};

} // namespace mavsdk
