#include "component_information_server_impl.h"
#include "mavlink_request_message_handler.h"
#include <algorithm>
#include <string>
#include <json/json.h>

namespace mavsdk {

ComponentInformationServerImpl::ComponentInformationServerImpl(
    std::shared_ptr<ServerComponent> server_component) :
    ServerPluginImplBase(server_component)
{
    // FIXME: allow other component IDs
    _server_component_impl->register_plugin(this);
}

ComponentInformationServerImpl::~ComponentInformationServerImpl()
{
    _server_component_impl->register_plugin(this);
}

void ComponentInformationServerImpl::init()
{
    // FIXME: needs refactoring
    //_server_component_impl->register_mavlink_request_message_handler(
    //    MAVLINK_MSG_ID_COMPONENT_INFORMATION,
    //    [this](MavlinkRequestMessageHandler::Params) {
    //        return process_component_information_requested();
    //    },
    //    this);
}

void ComponentInformationServerImpl::deinit()
{
    // FIXME: needs refactoring
    //_server_component_impl->unregister_all_mavlink_request_message_handlers(this);
}

ComponentInformationServer::Result
ComponentInformationServerImpl::provide_float_param(ComponentInformationServer::FloatParam param)
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (std::find_if(
            _float_params.begin(),
            _float_params.end(),
            [&](ComponentInformationServer::FloatParam& existing_param) {
                return existing_param.name == param.name;
            }) != std::end(_float_params)) {
        return ComponentInformationServer::Result::DuplicateParam;
    }

    if (param.start_value > param.max_value || param.start_value < param.min_value) {
        return ComponentInformationServer::Result::InvalidParamStartValue;
    }

    if (param.default_value > param.max_value || param.default_value < param.min_value) {
        return ComponentInformationServer::Result::InvalidParamDefaultValue;
    }

    if (param.name.size() > 16) {
        return ComponentInformationServer::Result::InvalidParamName;
    }

    _float_params.push_back(param);

    update_json_files_with_lock();

    // FIXME: requires refactoring
    //_parent->provide_server_param_float(param.name, param.start_value);
    //_parent->subscribe_param_float(
    //    param.name,
    //    [this, name = param.name](float new_value) { param_update(name, new_value); },
    //    this);

    return ComponentInformationServer::Result::Success;
}

void ComponentInformationServerImpl::subscribe_float_param(
    ComponentInformationServer::FloatParamCallback callback)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _float_param_update_callback = callback;
}

void ComponentInformationServerImpl::param_update(const std::string& name, float new_value)
{
    std::lock_guard<std::mutex> lock(_mutex);
    ComponentInformationServer::FloatParamUpdate param_update{name, new_value};
    // FIXME: requires refactoring
    // if (_float_param_update_callback) {
    //    _parent->call_user_callback(
    //        [this, param_update]() { _float_param_update_callback(param_update); });
    //}
}

std::optional<MAV_RESULT> ComponentInformationServerImpl::process_component_information_requested()
{
    mavlink_message_t message;
    mavlink_msg_component_information_pack(
        _server_component_impl->get_own_system_id(),
        _server_component_impl->get_own_component_id(),
        &message,
        _server_component_impl->get_time().elapsed_ms(),
        0,
        "mftp://general.json",
        0,
        "");
    _server_component_impl->send_message(message);

    // FIXME: REMOVE again
    update_json_files_with_lock();

    return MAV_RESULT_ACCEPTED;
}

void ComponentInformationServerImpl::update_json_files_with_lock()
{
    auto parameter_file = generate_parameter_file();
    auto meta_file = generate_meta_file();

    // std::cout << "parameter: " << parameter_file << '\n';
    // std::cout << "meta: " << meta_file << '\n';

    // FIXME: needs refactoring
    //_server_component_impl->mavlink_ftp().write_tmp_file("general.json",
    // meta_file);
    //_server_component_impl->mavlink_ftp().write_tmp_file("parameter.json",
    // parameter_file);
}

std::string ComponentInformationServerImpl::generate_parameter_file()
{
    Json::Value root;
    root["version"] = 1;
    Json::Value parameters = Json::arrayValue;
    for (const auto& param : _float_params) {
        Json::Value parameter;
        parameter["name"] = param.name;
        parameter["type"] = "Float";
        parameter["shortDesc"] = param.short_description;
        parameter["longDesc"] = param.long_description;
        parameter["units"] = param.unit;
        parameter["decimalPlaces"] = param.decimal_places;
        parameter["min"] = param.min_value;
        parameter["max"] = param.max_value;
        parameter["default"] = param.default_value;
        parameters.append(parameter);
    }
    root["parameters"] = parameters;

    return root.toStyledString();
}

std::string ComponentInformationServerImpl::generate_meta_file()
{
    Json::Value root;
    root["version"] = 1;
    root["vendorName"] = "Vendor";
    root["modelName"] = "Model";
    root["firmwareVersion"] = "1.0.0.0";
    root["hardwareVersion"] = "1.0.0.0";
    Json::Value metadata_types = Json::arrayValue;
    Json::Value metadata_type;
    metadata_type["type"] = Json::Int{COMP_METADATA_TYPE_PARAMETER};
    metadata_type["uri"] = "mftp://parameter.json";
    metadata_type["fileCrc"] = 0; // TODO
    metadata_type["uriFallback"] = ""; // TODO
    metadata_type["fileCrcFallback"] = 0;
    metadata_types.append(metadata_type);
    root["metadataTypes"] = metadata_types;

    return root.toStyledString();
}

} // namespace mavsdk
