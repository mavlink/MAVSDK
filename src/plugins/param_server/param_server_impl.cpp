#include "param_server_impl.h"

namespace mavsdk {

ParamServerImpl::ParamServerImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

ParamServerImpl::ParamServerImpl(std::shared_ptr<System> system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

ParamServerImpl::~ParamServerImpl()
{
    _parent->unregister_plugin(this);
}

void ParamServerImpl::init() {}

void ParamServerImpl::deinit() {}

void ParamServerImpl::enable() {}

void ParamServerImpl::disable() {}

std::pair<ParamServer::Result, int32_t> ParamServer::get_param_int(std::string name) const
{
    std::pair<MAVLinkParameters::Result, int32_t> result = _parent->get_server_param_int(name);

    if (result.first == MAVLinkParameters::Result::Success)
        return {ParamServer::Result::Success, result.second};
    else
        return {ParamServer::Result::NotFound, -1};
}

ParamServer::Result ParamServerImpl::set_param_int(std::string name, int32_t value)
{
    MAVLinkParameters::Result result = _parent->set_server_param_int(name, value);
    return result_from_mavlink_parameters_result(result);
}

std::pair<ParamServer::Result, float> ParamServer::get_param_float(std::string name) const
{
    std::pair<MAVLinkParameters::Result, float> result = _parent->get_server_param_float(name);

    if (result.first == MAVLinkParameters::Result::Success)
        return {ParamServer::Result::Success, result.second};
    else
        return {ParamServer::Result::NotFound, NAN};
}

ParamServer::Result ParamServerImpl::set_param_float(std::string name, float value)
{
    MAVLinkParameters::Result result = _parent->set_server_param_float(name, value);
    return result_from_mavlink_parameters_result(result);
}

ParamServer::AllParams ParamServer::get_all_params() const
{
    // TODO :)
    return {};
}

ParamServer::Result
ParamImpl::result_from_mavlink_parameters_result(MAVLinkParameters::Result result)
{
    switch (result) {
        case MAVLinkParameters::Result::Success:
            return ParamServer::Result::Success;
        case MAVLinkParameters::Result::Timeout:
            return ParamServer::Result::NotFound;
        case MAVLinkParameters::Result::ParamNameTooLong:
            return ParamServer::Result::ParamNameTooLong;
        case MAVLinkParameters::Result::WrongType:
            return ParamServer::Result::WrongType;
        default:
            LogErr() << "Unknown param error";
            return ParamServer::Result::Unknown;
    }
}

} // namespace mavsdk