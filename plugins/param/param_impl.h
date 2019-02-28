#pragma once

#include "plugins/param/param.h"
#include "mavlink_include.h"
#include "plugin_impl_base.h"
#include <mutex>

namespace dronecode_sdk {

class ParamImpl : public PluginImplBase {
public:
    ParamImpl(System &system);
    ~ParamImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    std::pair<Param::Result, int32_t> get_param_int(const std::string &name);

    Param::Result set_param_int(const std::string &name, int32_t value);

    std::pair<Param::Result, float> get_param_float(const std::string &name);

    Param::Result set_param_float(const std::string &name, float value);

private:
    static Param::Result
    result_from_mavlink_parameters_result(MAVLinkParameters::Result result);
};

} // namespace dronecode_sdk
