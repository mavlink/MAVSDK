#include <functional>
#include "params_raw_impl.h"
#include "system.h"
#include "global_include.h"

namespace dronecode_sdk {

ParamsRawImpl::ParamsRawImpl(System &system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

ParamsRawImpl::~ParamsRawImpl()
{
    _parent->unregister_plugin(this);
}

void ParamsRawImpl::init() {}

void ParamsRawImpl::deinit() {}

void ParamsRawImpl::enable() {}

void ParamsRawImpl::disable() {}

std::pair<ParamsRaw::Result, int32_t> ParamsRawImpl::get_param_int(const std::string &name)
{
    std::pair<MAVLinkParameters::Result, int32_t> result = _parent->get_param_int(name);
    return std::make_pair<>(result_from_mavlink_parameters_result(result.first), result.second);
}

ParamsRaw::Result ParamsRawImpl::set_param_int(const std::string &name, int32_t value)
{
    MAVLinkParameters::Result result = _parent->set_param_int(name, value);
    return result_from_mavlink_parameters_result(result);
}

std::pair<ParamsRaw::Result, float> ParamsRawImpl::get_param_float(const std::string &name)
{
    std::pair<MAVLinkParameters::Result, float> result = _parent->get_param_float(name);
    return std::make_pair<>(result_from_mavlink_parameters_result(result.first), result.second);
}

ParamsRaw::Result ParamsRawImpl::set_param_float(const std::string &name, float value)
{
    MAVLinkParameters::Result result = _parent->set_param_float(name, value);
    return result_from_mavlink_parameters_result(result);
}

ParamsRaw::Result
ParamsRawImpl::result_from_mavlink_parameters_result(MAVLinkParameters::Result result)
{
    switch (result) {
        case MAVLinkParameters::Result::SUCCESS:
            return ParamsRaw::Result::SUCCESS;
        case MAVLinkParameters::Result::TIMEOUT:
            return ParamsRaw::Result::TIMEOUT;
        case MAVLinkParameters::Result::PARAM_NAME_TOO_LONG:
            return ParamsRaw::Result::PARAM_NAME_TOO_LONG;
        case MAVLinkParameters::Result::WRONG_TYPE:
            return ParamsRaw::Result::WRONG_TYPE;
        case MAVLinkParameters::Result::CONNECTION_ERROR:
            return ParamsRaw::Result::CONNECTION_ERROR;
        default:
            LogErr() << "Unknown param error";
            return ParamsRaw::Result::UNKNOWN;
    }
}

} // namespace dronecode_sdk
