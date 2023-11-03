#pragma once

#include "plugins/param_server/param_server.h"
#include "server_plugin_impl_base.h"
#include "mavlink_parameter_server.h"
#include "callback_list.h"

namespace mavsdk {

class ParamServerImpl : public ServerPluginImplBase {
public:
    explicit ParamServerImpl(std::shared_ptr<ServerComponent> server_component);
    ~ParamServerImpl() override;

    void init() override;
    void deinit() override;

    std::pair<ParamServer::Result, int32_t> retrieve_param_int(std::string name) const;

    ParamServer::Result provide_param_int(std::string name, int32_t value);

    std::pair<ParamServer::Result, float> retrieve_param_float(std::string name) const;

    ParamServer::Result provide_param_float(std::string name, float value);

    std::pair<ParamServer::Result, std::string> retrieve_param_custom(std::string name) const;

    ParamServer::Result provide_param_custom(std::string name, const std::string& value);

    ParamServer::AllParams retrieve_all_params() const;

    ParamServer::ChangedParamIntHandle
    subscribe_changed_param_int(const ParamServer::ChangedParamIntCallback& callback);
    void unsubscribe_changed_param_int(ParamServer::ChangedParamIntHandle handle);

    ParamServer::ChangedParamFloatHandle
    subscribe_changed_param_float(const ParamServer::ChangedParamFloatCallback& callback);
    void unsubscribe_changed_param_float(ParamServer::ChangedParamFloatHandle handle);

    ParamServer::ChangedParamCustomHandle
    subscribe_changed_param_custom(const ParamServer::ChangedParamCustomCallback& callback);
    void unsubscribe_changed_param_custom(ParamServer::ChangedParamCustomHandle handle);

    static ParamServer::Result
    result_from_mavlink_parameter_server_result(MavlinkParameterServer::Result result);

private:
    CallbackList<ParamServer::IntParam> _changed_param_int_callbacks{};
    CallbackList<ParamServer::FloatParam> _changed_param_float_callbacks{};
    CallbackList<ParamServer::CustomParam> _changed_param_custom_callbacks{};
};

} // namespace mavsdk
