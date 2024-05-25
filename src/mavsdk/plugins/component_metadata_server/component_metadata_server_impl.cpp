#include "component_metadata_server_impl.h"
#include "mavlink_address.h"
#include "mavlink_request_message_handler.h"
#include "callback_list.tpp"
#include "unused.h"
#include "fs_utils.h"
#include "crc32.h"

#include <string>
#include <json/json.h>

namespace mavsdk {

ComponentMetadataServerImpl::ComponentMetadataServerImpl(
    std::shared_ptr<ServerComponent> server_component) :
    ServerPluginImplBase(server_component)
{
    if (const char* env_p = std::getenv("MAVSDK_COMPONENT_METADATA_DEBUGGING")) {
        if (std::string(env_p) == "1") {
            LogDebug() << "Verbose component metadata logging is on";
            _verbose_debugging = true;
        }
    }

    _server_component_impl->register_plugin(this);
}

ComponentMetadataServerImpl::~ComponentMetadataServerImpl()
{
    _server_component_impl->unregister_plugin(this);
    if (_metadata_set) {
        std::filesystem::remove_all(_tmp_path);
    }
}

void ComponentMetadataServerImpl::init()
{
    _server_component_impl->mavlink_request_message_handler().register_handler(
        MAVLINK_MSG_ID_COMPONENT_METADATA,
        [this](
            uint8_t source_sys_id, uint8_t source_comp_id, MavlinkRequestMessageHandler::Params) {
            UNUSED(source_sys_id);
            UNUSED(source_comp_id);
            return process_component_metadata_requested();
        },
        this);
}

void ComponentMetadataServerImpl::deinit()
{
    _server_component_impl->mavlink_request_message_handler().unregister_all_handlers(this);
}

std::optional<MAV_RESULT> ComponentMetadataServerImpl::process_component_metadata_requested()
{
    if (_verbose_debugging) {
        LogDebug() << "MAVLINK_MSG_ID_COMPONENT_METADATA request received";
    }

    const std::lock_guard lg{_mutex};

    if (!_metadata_set) {
        return MAV_RESULT_TEMPORARILY_REJECTED;
    }

    char general_metadata_uri[MAVLINK_MSG_COMPONENT_METADATA_FIELD_URI_LEN];
    snprintf(
        general_metadata_uri, sizeof(general_metadata_uri), "mftp://%s", kComponentGeneralFilename);
    _server_component_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message;
        mavlink_msg_component_metadata_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            _server_component_impl->get_time().elapsed_ms(),
            _comp_info_general_crc,
            general_metadata_uri);
        return message;
    });

    return MAV_RESULT_ACCEPTED;
}
void ComponentMetadataServerImpl::set_metadata(
    const std::vector<ComponentMetadataServer::Metadata>& metadata)
{
    const std::lock_guard lg{_mutex};
    if (_metadata_set) {
        LogErr() << "metadata already set";
        return;
    }

    // Create tmp directory as ftp root directory
    const auto tmp_option = create_tmp_directory("mavsdk-component-metadata-server");
    if (!tmp_option) {
        LogErr() << "Failed to create tmp directory";
        return;
    }
    _tmp_path = *tmp_option;

    if (_verbose_debugging) {
        LogDebug() << "Storing metadata under " << _tmp_path;
    }

    // Write files
    for (const auto& single_metadata : metadata) {
        Crc32 crc{};
        crc.add(
            reinterpret_cast<const uint8_t*>(single_metadata.json_metadata.data()),
            single_metadata.json_metadata.length());

        _metadata.emplace_back(single_metadata.type, crc.get());
        const std::filesystem::path path = _tmp_path / _metadata.back().filename;
        std::ofstream file(path, std::fstream::trunc | std::fstream::binary | std::fstream::out);
        if (!file) {
            LogErr() << "Failed to open " << path;
            continue;
        }
        file.write(single_metadata.json_metadata.data(), single_metadata.json_metadata.length());
    }

    if (!generate_component_metadata_general_file()) {
        return;
    }

    _metadata_set = true;
    _server_component_impl->mavlink_ftp_server().set_root_directory(_tmp_path.string());
}

bool ComponentMetadataServerImpl::generate_component_metadata_general_file()
{
    Json::Value root;
    root["version"] = 1;
    Json::Value metadata_types = Json::arrayValue;
    for (const auto& metadata : _metadata) {
        Json::Value metadata_type;
        metadata_type["type"] = Json::Int{metadata.type};
        metadata_type["uri"] = "mftp://" + metadata.filename;
        metadata_type["fileCrc"] = Json::UInt{metadata.crc};
        metadata_types.append(metadata_type);
    }
    root["metadataTypes"] = metadata_types;

    const std::filesystem::path path = _tmp_path / kComponentGeneralFilename;
    std::ofstream file(path, std::fstream::trunc | std::fstream::binary | std::fstream::out);
    if (!file) {
        LogErr() << "Failed to open " << path;
        return false;
    }
    const std::string json_data = root.toStyledString();
    Crc32 crc{};
    crc.add(reinterpret_cast<const uint8_t*>(json_data.data()), json_data.length());
    _comp_info_general_crc = crc.get();
    file.write(json_data.data(), json_data.length());
    return true;
}

ComponentMetadataServerImpl::Metadata::Metadata(
    ComponentMetadataServer::MetadataType metadata_type, uint32_t crc_data) :
    crc(crc_data)
{
    switch (metadata_type) {
        case ComponentMetadataServer::MetadataType::Parameter:
            type = COMP_METADATA_TYPE_PARAMETER;
            filename = "parameters.json";
            break;
        case ComponentMetadataServer::MetadataType::Events:
            type = COMP_METADATA_TYPE_EVENTS;
            filename = "events.json";
            break;
        case ComponentMetadataServer::MetadataType::Actuators:
            type = COMP_METADATA_TYPE_ACTUATORS;
            filename = "actuators.json";
            break;
    }
}
} // namespace mavsdk
