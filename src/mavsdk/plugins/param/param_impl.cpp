#include <functional>
#include "param_impl.h"
#include "system.h"

namespace mavsdk {

ParamImpl::ParamImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

ParamImpl::ParamImpl(std::shared_ptr<System> system) : PluginImplBase(std::move(system))
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

std::pair<Param::Result, int32_t> ParamImpl::get_param_int(const std::string& name)
{
    std::pair<MAVLinkParameters::Result, int32_t> result = _parent->get_param_int(name);
    return std::make_pair<>(result_from_mavlink_parameters_result(result.first), result.second);
}

Param::Result ParamImpl::set_param_int(const std::string& name, int32_t value)
{
    MAVLinkParameters::Result result = _parent->set_param_int(name, value);
    return result_from_mavlink_parameters_result(result);
}

std::pair<Param::Result, float> ParamImpl::get_param_float(const std::string& name)
{
    std::pair<MAVLinkParameters::Result, float> result = _parent->get_param_float(name);
    return std::make_pair<>(result_from_mavlink_parameters_result(result.first), result.second);
}

Param::Result ParamImpl::set_param_float(const std::string& name, float value)
{
    MAVLinkParameters::Result result = _parent->set_param_float(name, value);
    return result_from_mavlink_parameters_result(result);
}

std::pair<Param::Result, std::string> ParamImpl::get_param_custom(const std::string& name)
{
    auto result = _parent->get_param_custom(name);
    return std::make_pair<>(result_from_mavlink_parameters_result(result.first), result.second);
}

Param::Result ParamImpl::set_param_custom(const std::string& name, const std::string& value)
{
    auto result = _parent->set_param_custom(name, value);
    return result_from_mavlink_parameters_result(result);
}

Param::AllParams ParamImpl::get_all_params()
{
    auto tmp = _parent->get_all_params();

    Param::AllParams res{};
    for (auto const& param_pair : tmp) {
        if (param_pair.second.is<float>()) {
            Param::FloatParam tmp_param;
            tmp_param.name = param_pair.first;
            tmp_param.value = param_pair.second.get<float>();
            res.float_params.push_back(tmp_param);
        } else if (param_pair.second.is<int32_t>()) {
            Param::IntParam tmp_param;
            tmp_param.name = param_pair.first;
            tmp_param.value = param_pair.second.get<int32_t>();
            res.int_params.push_back(tmp_param);
        } else if (param_pair.second.is<int16_t>()) {
            Param::IntParam tmp_param;
            tmp_param.name = param_pair.first;
            tmp_param.value = param_pair.second.get<int16_t>();
            res.int_params.push_back(tmp_param);
        } else if (param_pair.second.is<int8_t>()) {
            Param::IntParam tmp_param;
            tmp_param.name = param_pair.first;
            tmp_param.value = param_pair.second.get<int8_t>();
            res.int_params.push_back(tmp_param);
        }
    }

    return res;
}

Param::Result ParamImpl::result_from_mavlink_parameters_result(MAVLinkParameters::Result result)
{
    switch (result) {
        case MAVLinkParameters::Result::Success:
            return Param::Result::Success;
        case MAVLinkParameters::Result::Timeout:
            return Param::Result::Timeout;
        case MAVLinkParameters::Result::ParamNameTooLong:
            return Param::Result::ParamNameTooLong;
        case MAVLinkParameters::Result::WrongType:
            return Param::Result::WrongType;
        case MAVLinkParameters::Result::ConnectionError:
            return Param::Result::ConnectionError;
        case MAVLinkParameters::Result::ParamValueTooLong:
            return Param::Result::ParamValueTooLong;
        default:
            LogErr() << "Unknown param error";
            return Param::Result::Unknown;
    }
}

} // namespace mavsdk
