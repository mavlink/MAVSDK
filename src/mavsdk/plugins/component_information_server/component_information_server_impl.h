#pragma once

#include "plugins/component_information_server/component_information_server.h"
#include "server_plugin_impl_base.h"
#include "callback_list.h"

namespace mavsdk {

class ComponentInformationServerImpl : public ServerPluginImplBase {
public:
    explicit ComponentInformationServerImpl(std::shared_ptr<ServerComponent> server_component);
    ~ComponentInformationServerImpl() override;

    void init() override;
    void deinit() override;

    ComponentInformationServer::Result set_our_ip(const std::string& ip);

    ComponentInformationServer::Result
    provide_float_param(ComponentInformationServer::FloatParam param);

    ComponentInformationServer::FloatParamHandle
    subscribe_float_param(const ComponentInformationServer::FloatParamCallback& callback);
    void unsubscribe_float_param(ComponentInformationServer::FloatParamHandle handle);

private:
    void update_json_files_with_lock();
    void param_update(const std::string& name, float new_value);

    std::string generate_parameter_file();
    std::string generate_meta_file();

    std::optional<MAV_RESULT> process_component_information_requested();

    std::mutex _mutex{};
    std::vector<ComponentInformationServer::FloatParam> _float_params{};
    std::string _our_ip{};
    CallbackList<ComponentInformationServer::FloatParamUpdate> _float_param_update_callbacks{};
};

} // namespace mavsdk
