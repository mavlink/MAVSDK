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

std::string ParamsRaw::result_str(Result result)
{
    switch (result) {
        case ParamsRaw::Result::SUCCESS:
            return "Success";
        case ParamsRaw::Result::TIMEOUT:
            return "Timeout";
        case ParamsRaw::Result::CONNECTION_ERROR:
            return "Connection error";
        case ParamsRaw::Result::WRONG_TYPE:
            return "Wrong type";
        case ParamsRaw::Result::PARAM_NAME_TOO_LONG:
            return "Param name too long";
        default:
            // FALLTHROUGH
        case ParamsRaw::Result::UNKNOWN:
            return "Unknown";
    }
}

} // namespace dronecode_sdk
