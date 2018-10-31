#include "plugins/params_raw/params_raw.h"
#include "params_raw_impl.h"

namespace dronecode_sdk {

ParamsRaw::ParamsRaw(System &system) : PluginBase(), _impl{new ParamsRawImpl(system)} {}

ParamsRaw::~ParamsRaw() {}

std::pair<ParamsRaw::Result, int32_t> ParamsRaw::get_param_int(const std::string &name)
{
    return _impl->get_param_int(name);
}

ParamsRaw::Result ParamsRaw::set_param_int(const std::string &name, int32_t value)
{
    return _impl->set_param_int(name, value);
}

std::pair<ParamsRaw::Result, float> ParamsRaw::get_param_float(const std::string &name)
{
    return _impl->get_param_float(name);
}

ParamsRaw::Result ParamsRaw::set_param_float(const std::string &name, float value)
{
    return _impl->set_param_float(name, value);
}

} // namespace dronecode_sdk
