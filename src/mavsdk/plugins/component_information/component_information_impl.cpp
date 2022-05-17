#include "component_information_impl.h"
#include "fs.h"
#include "callback_list.tpp"

#include <utility>
#include <fstream>
#include <json/json.h>

namespace mavsdk {

template class CallbackList<ComponentInformation::FloatParamUpdate>;

ComponentInformationImpl::ComponentInformationImpl(System& system) : PluginImplBase(system)
{
    _system_impl->register_plugin(this);
}

ComponentInformationImpl::ComponentInformationImpl(std::shared_ptr<System> system) :
    PluginImplBase(std::move(system))
{
    _system_impl->register_plugin(this);
}

ComponentInformationImpl::~ComponentInformationImpl()
{
    _system_impl->unregister_plugin(this);
}

void ComponentInformationImpl::init() {}

void ComponentInformationImpl::deinit() {}

void ComponentInformationImpl::enable()
{
    // TODO: iterate through components!

    _system_impl->request_message().request(
        MAVLINK_MSG_ID_COMPONENT_INFORMATION,
        MAV_COMP_ID_PATHPLANNER,
        [this](auto&& result, auto&& message) { receive_component_information(result, message); });
}

void ComponentInformationImpl::disable() {}

void ComponentInformationImpl::receive_component_information(
    MavlinkCommandSender::Result result, const mavlink_message_t& message)
{
    if (result != MavlinkCommandSender::Result::Success) {
        LogWarn() << "Requesting component information failed with " << static_cast<int>(result);
        return;
    }

    mavlink_component_information_t component_information;
    mavlink_msg_component_information_decode(&message, &component_information);

    component_information
        .general_metadata_uri[sizeof(component_information.general_metadata_uri) - 1] = '\0';
    const auto general_metadata_uri = std::string(component_information.general_metadata_uri);

    download_file_async(
        general_metadata_uri, [this](std::string path) { parse_metadata_file(path); });
}

void ComponentInformationImpl::download_file_async(
    const std::string& uri, std::function<void(std::string path)> callback)
{
    // TODO: check CRC

    if (uri.empty()) {
        LogErr() << "No component information URI provided";
        return;

    } else if (uri.find("mftp://") == 0) {
        LogDebug() << "Found mftp URI, using MAVLink FTP to download file";

        const auto path = uri.substr(strlen("mftp://"));

        const auto maybe_tmp_path = create_tmp_directory("mavsdk-component-information-tmp-files");
        const auto path_to_download = maybe_tmp_path ? maybe_tmp_path.value() : "./";

        _system_impl->mavlink_ftp().download_async(
            path,
            path_to_download,
            [path_to_download, callback, path](
                MavlinkFtp::ClientResult download_result, MavlinkFtp::ProgressData progress_data) {
                if (download_result == MavlinkFtp::ClientResult::Next) {
                    LogDebug() << "File download progress: " << progress_data.bytes_transferred
                               << '/' << progress_data.total_bytes;
                } else {
                    LogDebug() << "File download ended with result " << download_result;
                    if (download_result == MavlinkFtp::ClientResult::Success) {
                        LogDebug() << "Received file " << path_to_download + "/" + path;
                        callback(path_to_download + "/" + path);
                    }
                }
            });
    } else if (uri.find("http://") == 0 || uri.find("https://") == 0) {
        LogWarn() << "Download using http(s) not implemented yet";
    } else {
        LogWarn() << "Unknown URI protocol";
    }
}

void ComponentInformationImpl::parse_metadata_file(const std::string& path)
{
    std::ifstream f(path);
    if (f.bad()) {
        LogErr() << "Could not open json metadata file.";
        return;
    }

    Json::Value metadata;
    f >> metadata;

    if (!metadata.isMember("version")) {
        LogErr() << "version not found";
        return;
    }

    if (metadata["version"].asInt() != 1) {
        LogWarn() << "version " << metadata["version"].asInt() << " not supported";
    }

    if (!metadata.isMember("metadataTypes")) {
        LogErr() << "metadataTypes not found";
        return;
    }

    for (auto& metadata_type : metadata["metadataTypes"]) {
        if (!metadata_type.isMember("type")) {
            LogErr() << "type missing";
            return;
        }
        if (!metadata_type.isMember("uri")) {
            LogErr() << "uri missing";
            return;
        }

        if (metadata_type["type"].asInt() == COMP_METADATA_TYPE_PARAMETER) {
            download_file_async(
                metadata_type["uri"].asString(), [this](const std::string& parameter_file_path) {
                    LogDebug() << "Found parameter file at: " << parameter_file_path;
                    parse_parameter_file(parameter_file_path);
                });
        }
    }
}

void ComponentInformationImpl::parse_parameter_file(const std::string& path)
{
    std::ifstream f(path);
    if (f.bad()) {
        LogErr() << "Could not open json parameter file.";
        return;
    }

    Json::Value parameters;
    f >> parameters;

    if (!parameters.isMember("version")) {
        LogErr() << "version not found";
        return;
    }

    if (parameters["version"].asInt() != 1) {
        LogWarn() << "version " << parameters["version"].asInt() << " not supported";
    }

    if (!parameters.isMember("parameters")) {
        LogErr() << "parameters not found";
        return;
    }

    std::lock_guard<std::mutex> lock(_mutex);
    _float_params.clear();

    for (auto& param : parameters["parameters"]) {
        if (!param.isMember("type")) {
            LogErr() << "type not found";
            return;
        }

        if (param["type"].asString() == "Float") {
            _float_params.push_back(ComponentInformation::FloatParam{
                param["name"].asString(),
                param["shortDesc"].asString(),
                param["longDesc"].asString(),
                param["units"].asString(),
                param["decimalPlaces"].asInt(),
                NAN,
                param["default"].asFloat(),
                param["min"].asFloat(),
                param["max"].asFloat()});

            const auto name = param["name"].asString();

            _system_impl->get_param_float_async(
                name,
                [this, name](MavlinkParameterClient::Result result, float value) {
                    get_float_param_result(name, result, value);
                },
                this);

            _system_impl->subscribe_param_float(
                name, [this, name](float value) { param_update(name, value); }, this);

        } else {
            LogWarn() << "Ignoring type " << param["type"].asString() << " for now.";
        }
    }
}

void ComponentInformationImpl::get_float_param_result(
    const std::string& name, MavlinkParameterClient::Result result, float value)
{
    if (result != MavlinkParameterClient::Result::Success) {
        LogWarn() << "Getting float param result: " << static_cast<int>(result);
        return;
    }

    std::lock_guard<std::mutex> lock(_mutex);
    for (auto& param : _float_params) {
        if (param.name == name) {
            param.start_value = value;
            LogDebug() << "Received value " << value << " for " << name;
        }
    }
}

void ComponentInformationImpl::param_update(const std::string& name, float new_value)
{
    std::lock_guard<std::mutex> lock(_mutex);

    for (auto& param : _float_params) {
        if (param.name == name) {
            param.start_value = new_value;
            LogDebug() << "Received value " << new_value << " for " << name;
        }
    }

    const auto param_update = ComponentInformation::FloatParamUpdate{name, new_value};

    _float_param_update_callbacks.queue(
        param_update, [this](const auto& func) { _system_impl->call_user_callback(func); });
}

std::pair<ComponentInformation::Result, std::vector<ComponentInformation::FloatParam>>
ComponentInformationImpl::access_float_params()
{
    return {ComponentInformation::Result::Success, _float_params};
}

ComponentInformation::FloatParamHandle ComponentInformationImpl::subscribe_float_param(
    const ComponentInformation::FloatParamCallback& callback)
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _float_param_update_callbacks.subscribe(callback);
}

void ComponentInformationImpl::unsubscribe_float_param(
    ComponentInformation::FloatParamHandle handle)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _float_param_update_callbacks.unsubscribe(handle);
}

} // namespace mavsdk
