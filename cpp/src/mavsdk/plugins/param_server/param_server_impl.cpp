#include "param_server_impl.h"
#include "callback_list.tpp"
#include <thread>
#include <chrono>

namespace mavsdk {

template class CallbackList<ParamServer::IntParam>;
template class CallbackList<ParamServer::FloatParam>;
template class CallbackList<ParamServer::CustomParam>;

ParamServerImpl::ParamServerImpl(std::shared_ptr<ServerComponent> server_component) :
    ServerPluginImplBase(server_component)
{
    // FIXME: this plugin should support various component IDs
    _server_component_impl->register_plugin(this);
}

ParamServerImpl::~ParamServerImpl()
{
    _server_component_impl->unregister_plugin(this);
}

void ParamServerImpl::init() {}

void ParamServerImpl::deinit()
{
    // Ensure synchronous cleanup - keep trying until all callbacks are unregistered
    auto& param_server = _server_component_impl->mavlink_parameter_server();
    param_server.unsubscribe_all_params_changed(this);

    // Give a brief moment for any deferred unsubscriptions to be processed
    // This prevents use-after-free if callbacks are still executing
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // Clear our callback lists to ensure no pending callbacks exist
    _changed_param_int_callbacks.clear();
    _changed_param_float_callbacks.clear();
    _changed_param_custom_callbacks.clear();
}

ParamServer::Result ParamServerImpl::set_protocol(bool extended_protocol)
{
    _server_component_impl->mavlink_parameter_server().set_extended_protocol(extended_protocol);
    return ParamServer::Result::Success;
}

std::pair<ParamServer::Result, int32_t> ParamServerImpl::retrieve_param_int(std::string name) const
{
    auto result =
        _server_component_impl->mavlink_parameter_server().retrieve_server_param_int(name);

    if (result.first == MavlinkParameterServer::Result::Ok) {
        return {ParamServer::Result::Success, result.second};
    } else {
        return {ParamServer::Result::NotFound, -1};
    }
}

ParamServer::Result ParamServerImpl::provide_param_int(std::string name, int32_t value)
{
    if (name.size() > 16) {
        return ParamServer::Result::ParamNameTooLong;
    }

    if (_server_component_impl->mavlink_parameter_server().params_locked_down()) {
        return ParamServer::Result::ParamProvidedTooLate;
    }

    const auto ret =
        _server_component_impl->mavlink_parameter_server().provide_server_param_int(name, value);
    if (ret == MavlinkParameterServer::Result::Ok) {
        _server_component_impl->mavlink_parameter_server().subscribe_param_int_changed(
            name,
            [name, this](int32_t new_value) { _changed_param_int_callbacks({name, new_value}); },
            this);
    }
    return result_from_mavlink_parameter_server_result(ret);
}

std::pair<ParamServer::Result, float> ParamServerImpl::retrieve_param_float(std::string name) const
{
    const auto result =
        _server_component_impl->mavlink_parameter_server().retrieve_server_param_float(name);

    if (result.first == MavlinkParameterServer::Result::Ok) {
        return {ParamServer::Result::Success, result.second};
    } else {
        return {ParamServer::Result::NotFound, NAN};
    }
}

ParamServer::Result ParamServerImpl::provide_param_float(std::string name, float value)
{
    if (name.size() > 16) {
        return ParamServer::Result::ParamNameTooLong;
    }

    if (_server_component_impl->mavlink_parameter_server().params_locked_down()) {
        return ParamServer::Result::ParamProvidedTooLate;
    }

    const auto ret =
        _server_component_impl->mavlink_parameter_server().provide_server_param_float(name, value);
    if (ret == MavlinkParameterServer::Result::Ok) {
        _server_component_impl->mavlink_parameter_server().subscribe_param_float_changed(
            name,
            [name, this](float new_value) { _changed_param_float_callbacks({name, new_value}); },
            this);
    }
    return result_from_mavlink_parameter_server_result(ret);
}

std::pair<ParamServer::Result, std::string>
ParamServerImpl::retrieve_param_custom(std::string name) const
{
    const auto result =
        _server_component_impl->mavlink_parameter_server().retrieve_server_param_custom(name);

    if (result.first == MavlinkParameterServer::Result::Ok) {
        return {ParamServer::Result::Success, result.second};
    } else {
        return {ParamServer::Result::NotFound, {}};
    }
}

ParamServer::Result
ParamServerImpl::provide_param_custom(std::string name, const std::string& value)
{
    if (name.size() > 16) {
        return ParamServer::Result::ParamNameTooLong;
    }

    const auto ret =
        _server_component_impl->mavlink_parameter_server().provide_server_param_custom(name, value);
    if (ret == MavlinkParameterServer::Result::Ok) {
        _server_component_impl->mavlink_parameter_server().subscribe_param_custom_changed(
            name,
            [name, this](const std::string& new_value) {
                _changed_param_custom_callbacks({name, new_value});
            },
            this);
    }
    return result_from_mavlink_parameter_server_result(ret);
}

ParamServer::AllParams ParamServerImpl::retrieve_all_params() const
{
    auto tmp = _server_component_impl->mavlink_parameter_server().retrieve_all_server_params();

    ParamServer::AllParams res{};

    for (auto const& param_pair : tmp) {
        if (param_pair.second.is<float>()) {
            ParamServer::FloatParam tmp_param;
            tmp_param.name = param_pair.first;
            tmp_param.value = param_pair.second.get<float>();
            res.float_params.push_back(tmp_param);
        } else if (param_pair.second.is<int32_t>()) {
            ParamServer::IntParam tmp_param;
            tmp_param.name = param_pair.first;
            tmp_param.value = param_pair.second.get<int32_t>();
            res.int_params.push_back(tmp_param);
        }
    }

    return res;
}

ParamServer::ChangedParamIntHandle
ParamServerImpl::subscribe_changed_param_int(const ParamServer::ChangedParamIntCallback& callback)
{
    return _changed_param_int_callbacks.subscribe(callback);
}

void ParamServerImpl::unsubscribe_changed_param_int(ParamServer::ChangedParamIntHandle handle)
{
    _changed_param_int_callbacks.unsubscribe(handle);
}

ParamServer::ChangedParamFloatHandle ParamServerImpl::subscribe_changed_param_float(
    const ParamServer::ChangedParamFloatCallback& callback)
{
    return _changed_param_float_callbacks.subscribe(callback);
}

void ParamServerImpl::unsubscribe_changed_param_float(ParamServer::ChangedParamFloatHandle handle)
{
    _changed_param_float_callbacks.unsubscribe(handle);
}

ParamServer::ChangedParamCustomHandle ParamServerImpl::subscribe_changed_param_custom(
    const ParamServer::ChangedParamCustomCallback& callback)
{
    return _changed_param_custom_callbacks.subscribe(callback);
}

void ParamServerImpl::unsubscribe_changed_param_custom(ParamServer::ChangedParamCustomHandle handle)
{
    _changed_param_custom_callbacks.unsubscribe(handle);
}

ParamServer::Result
ParamServerImpl::result_from_mavlink_parameter_server_result(MavlinkParameterServer::Result result)
{
    switch (result) {
        case MavlinkParameterServer::Result::Ok:
        case MavlinkParameterServer::Result::OkExistsAlready:
            return ParamServer::Result::Success;
        case MavlinkParameterServer::Result::NotFound:
            return ParamServer::Result::NotFound;
        case MavlinkParameterServer::Result::ParamNameTooLong:
            return ParamServer::Result::ParamNameTooLong;
        case MavlinkParameterServer::Result::WrongType:
            return ParamServer::Result::WrongType;
        case MavlinkParameterServer::Result::ParamValueTooLong:
            return ParamServer::Result::ParamValueTooLong;
        default:
            LogErr() << "Unknown param error";
            return ParamServer::Result::Unknown;
    }
}

} // namespace mavsdk
