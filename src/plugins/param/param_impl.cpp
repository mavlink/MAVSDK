#include <functional>
#include "param_impl.h"
#include "system.h"
#include "global_include.h"

namespace mavsdk {

ParamImpl::ParamImpl(System &system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

ParamImpl::~ParamImpl()
{
    _parent->unregister_plugin(this);
}

void ParamImpl::init() {}

void ParamImpl::deinit() {}

void ParamImpl::enable() {}

void ParamImpl::disable() {}

std::pair<Param::Result, int32_t> ParamImpl::get_param_int(const std::string &name)
{
    std::pair<MAVLinkParameters::Result, int32_t> result = _parent->get_param_int(name);
    return std::make_pair<>(result_from_mavlink_parameters_result(result.first), result.second);
}

Param::Result ParamImpl::set_param_int(const std::string &name, int32_t value)
{
    MAVLinkParameters::Result result = _parent->set_param_int(name, value);
    return result_from_mavlink_parameters_result(result);
}

std::pair<Param::Result, float> ParamImpl::get_param_float(const std::string &name)
{
    std::pair<MAVLinkParameters::Result, float> result = _parent->get_param_float(name);
    return std::make_pair<>(result_from_mavlink_parameters_result(result.first), result.second);
}

Param::Result ParamImpl::set_param_float(const std::string &name, float value)
{
    MAVLinkParameters::Result result = _parent->set_param_float(name, value);
    return result_from_mavlink_parameters_result(result);
}

Param::Result ParamImpl::result_from_mavlink_parameters_result(MAVLinkParameters::Result result)
{
    switch (result) {
        case MAVLinkParameters::Result::SUCCESS:
            return Param::Result::SUCCESS;
        case MAVLinkParameters::Result::TIMEOUT:
            return Param::Result::TIMEOUT;
        case MAVLinkParameters::Result::PARAM_NAME_TOO_LONG:
            return Param::Result::PARAM_NAME_TOO_LONG;
        case MAVLinkParameters::Result::WRONG_TYPE:
            return Param::Result::WRONG_TYPE;
        case MAVLinkParameters::Result::CONNECTION_ERROR:
            return Param::Result::CONNECTION_ERROR;
        default:
            LogErr() << "Unknown param error";
            return Param::Result::UNKNOWN;
    }
}

} // namespace mavsdk
