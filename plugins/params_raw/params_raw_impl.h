#pragma once

#include "plugins/params_raw/params_raw.h"
#include "mavlink_include.h"
#include "plugin_impl_base.h"
#include <mutex>

namespace dronecode_sdk {

class ParamsRawImpl : public PluginImplBase {
public:
    ParamsRawImpl(System &system);
    ~ParamsRawImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    std::pair<ParamsRaw::Result, int32_t> get_param_int(const std::string &name);

    ParamsRaw::Result set_param_int(const std::string &name, int32_t value);

    std::pair<ParamsRaw::Result, float> get_param_float(const std::string &name);

    ParamsRaw::Result set_param_float(const std::string &name, float value);

private:
    static ParamsRaw::Result
    result_from_mavlink_parameters_result(MAVLinkParameters::Result result);
};

} // namespace dronecode_sdk
