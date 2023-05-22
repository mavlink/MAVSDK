#pragma once

#include <mutex>

#include "mavlink_include.h"
#include "plugins/param/param.h"
#include "plugin_impl_base.h"

namespace mavsdk {

class ParamImpl : public PluginImplBase {
public:
    explicit ParamImpl(System& system);
    explicit ParamImpl(std::shared_ptr<System> system);
    ~ParamImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    std::pair<Param::Result, int32_t> get_param_int(const std::string& name);

    Param::Result set_param_int(const std::string& name, int32_t value);

    std::pair<Param::Result, float> get_param_float(const std::string& name);

    Param::Result set_param_float(const std::string& name, float value);

    std::pair<Param::Result, std::string> get_param_custom(const std::string& name);

    Param::Result set_param_custom(const std::string& name, const std::string& value);

    Param::AllParams get_all_params();

    Param::Result select_component(int32_t component_id, Param::ProtocolVersion protocol_version);

private:
    static Param::Result
    result_from_mavlink_parameter_client_result(MavlinkParameterClient::Result result);

    uint8_t _component_id{MAV_COMP_ID_AUTOPILOT1};
    Param::ProtocolVersion _protocol_version{Param::ProtocolVersion::V1};
};

} // namespace mavsdk
