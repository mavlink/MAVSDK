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
    std::pair<bool, int32_t> result = _parent->get_param_int(name);
    if (result.first) {
        return std::make_pair<>(ParamsRaw::Result::SUCCESS, result.second);
    } else {
        return std::make_pair<>(ParamsRaw::Result::ERROR, result.second);
    }
}

ParamsRaw::Result ParamsRawImpl::set_param_int(const std::string &name, int32_t value)
{
    bool result = _parent->set_param_int(name, value);
    if (result) {
        return ParamsRaw::Result::SUCCESS;
    } else {
        return ParamsRaw::Result::ERROR;
    }
}

std::pair<ParamsRaw::Result, float> ParamsRawImpl::get_param_float(const std::string &name)
{
    std::pair<bool, float> result = _parent->get_param_float(name);
    if (result.first) {
        return std::make_pair<>(ParamsRaw::Result::SUCCESS, result.second);
    } else {
        return std::make_pair<>(ParamsRaw::Result::ERROR, result.second);
    }
}

ParamsRaw::Result ParamsRawImpl::set_param_float(const std::string &name, float value)
{
    bool result = _parent->set_param_float(name, value);
    if (result) {
        return ParamsRaw::Result::SUCCESS;
    } else {
        return ParamsRaw::Result::ERROR;
    }
}

} // namespace dronecode_sdk
