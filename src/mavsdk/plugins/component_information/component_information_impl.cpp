#include "component_information_impl.h"
#include "fs.h"

namespace mavsdk {

ComponentInformationImpl::ComponentInformationImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

ComponentInformationImpl::ComponentInformationImpl(std::shared_ptr<System> system) :
    PluginImplBase(system)
{
    _parent->register_plugin(this);
}

ComponentInformationImpl::~ComponentInformationImpl()
{
    _parent->unregister_plugin(this);
}

void ComponentInformationImpl::init() {}

void ComponentInformationImpl::deinit() {}

void ComponentInformationImpl::enable()
{
    // TODO: iterate through components!

    _parent->request_message().request(
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

    // TODO:
    // 1. Download the file if CRC is different, and save it.
    // 2. Parse the xml.
    // 3. Update params from it.
    if (general_metadata_uri.empty()) {
        LogErr() << "No component information URI provided";

    } else if (general_metadata_uri.find("mftp://") == 0) {
        LogDebug() << "Found mftp URI, using MAVLink FTP to download file";

        const auto path = general_metadata_uri.substr(strlen("mftp://"));

        const auto maybe_tmp_path = create_tmp_directory("mavsdk-component-information-tmp-files");
        const auto path_to_download = maybe_tmp_path ? maybe_tmp_path.value() : "./";

        _parent->mavlink_ftp().download_async(
            path,
            path_to_download,
            [](MavlinkFtp::ClientResult download_result, MavlinkFtp::ProgressData progress_data) {
                if (download_result == MavlinkFtp::ClientResult::Next) {
                    LogDebug() << "File download progress: " << progress_data.bytes_transferred
                               << '/' << progress_data.total_bytes;
                } else {
                    LogDebug() << "File download ended with result " << download_result;
                    if (download_result == MavlinkFtp::ClientResult::Success) {
                        LogDebug() << "Received file";
                    }
                }
            });
    } else if (
        general_metadata_uri.find("http://") == 0 || general_metadata_uri.find("https://") == 0) {
        LogWarn() << "Download using http(s) not implemented yet";
    } else {
        LogWarn() << "Unknown URI protocol";
    }
}

std::pair<ComponentInformation::Result, std::vector<ComponentInformation::FloatParam>>
ComponentInformationImpl::access_float_params()
{
    // TODO: go through params and return list of them.
    return {};
}

void ComponentInformationImpl::subscribe_float_param(
    ComponentInformation::FloatParamCallback callback)
{
    (void)(callback);
}

} // namespace mavsdk