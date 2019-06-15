#include "plugins/param/param.h"
#include "param_impl.h"

namespace mavsdk {

Param::Param(System &system) : PluginBase(), _impl{new ParamImpl(system)} {}

Param::~Param() {}

std::pair<Param::Result, int32_t> Param::get_param_int(const std::string &name)
{
    return _impl->get_param_int(name);
}

Param::Result Param::set_param_int(const std::string &name, int32_t value)
{
    return _impl->set_param_int(name, value);
}

std::pair<Param::Result, float> Param::get_param_float(const std::string &name)
{
    return _impl->get_param_float(name);
}

Param::Result Param::set_param_float(const std::string &name, float value)
{
    return _impl->set_param_float(name, value);
}

std::string Param::result_str(Result result)
{
    switch (result) {
        case Param::Result::SUCCESS:
            return "Success";
        case Param::Result::TIMEOUT:
            return "Timeout";
        case Param::Result::CONNECTION_ERROR:
            return "Connection error";
        case Param::Result::WRONG_TYPE:
            return "Wrong type";
        case Param::Result::PARAM_NAME_TOO_LONG:
            return "Param name too long";
        default:
            // FALLTHROUGH
        case Param::Result::UNKNOWN:
            return "Unknown";
    }
}

} // namespace mavsdk
