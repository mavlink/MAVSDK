#pragma once

#include "plugins/param_server/param_server.h"
#include "plugin_impl_base.h"

namespace mavsdk {

class ParamServerImpl : public PluginImplBase {
public:
    explicit ParamServerImpl(System& system);
    explicit ParamServerImpl(std::shared_ptr<System> system);
    ~ParamServerImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    std::pair<ParamServer::Result, int32_t> retrieve_param_int(std::string name) const;

    ParamServer::Result provide_param_int(std::string name, int32_t value);

    std::pair<ParamServer::Result, float> retrieve_param_float(std::string name) const;

    ParamServer::Result provide_param_float(std::string name, float value);

    ParamServer::AllParams retrieve_all_params() const;

    ParamServer::Result result_from_mavlink_parameters_result(MAVLinkParameters::Result result);

private:
};

} // namespace mavsdk
