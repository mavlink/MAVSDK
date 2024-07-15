#include "mavlink_component_metadata.h"
#include "callback_list.tpp"
#include "fs_utils.h"
#include "inflate_lzma.h"
#include "unused.h"
#include "system_impl.h"

#include <utility>
#include <filesystem>
#include <fstream>
#include <random>
#include <regex>

#ifdef WINDOWS
#define strncasecmp(x, y, z) _strnicmp(x, y, z)
#endif

namespace mavsdk {

namespace fs = std::filesystem;

MavlinkComponentMetadata::MavlinkComponentMetadata(SystemImpl& system_impl) :
    _system_impl(system_impl)
{
    if (const char* env_p = std::getenv("MAVSDK_COMPONENT_METADATA_DEBUGGING")) {
        if (std::string(env_p) == "1") {
            LogDebug() << "Verbose component metadata logging is on";
            _verbose_debugging = true;
        }
    }

    const auto cache_dir_option = get_cache_directory();
    if (cache_dir_option) {
        _file_cache.emplace(
            cache_dir_option.value() / "component_metadata", 50, _verbose_debugging);
    } else {
        LogErr() << "Failed to get cache directory";
    }

    const auto tmp_option = create_tmp_directory("mavsdk-component-metadata");
    if (tmp_option) {
        _tmp_download_path = tmp_option.value();
    } else {
        _tmp_download_path = "./mavsdk-component-metadata";
        std::error_code err;
        std::filesystem::create_directory(_tmp_download_path, err);
    }
}

MavlinkComponentMetadata::~MavlinkComponentMetadata()
{
    std::error_code ec;
    std::filesystem::remove_all(_tmp_download_path, ec);
    if (ec) {
        LogErr() << "failed to remove directory: " << ec.message();
    }
}

void MavlinkComponentMetadata::request_component(uint32_t compid)
{
    const std::lock_guard lg{_mavlink_components_mutex};
    if (_mavlink_components.find(compid) == _mavlink_components.end()) {
        _mavlink_components[compid] = MavlinkComponent{};
        _system_impl.request_message().request(
            MAVLINK_MSG_ID_COMPONENT_METADATA, compid, [this](auto&& result, auto&& message) {
                receive_component_metadata(result, message);
            });
    }
}

void MavlinkComponentMetadata::receive_component_metadata(
    MavlinkCommandSender::Result result, const mavlink_message_t& message)
{
    const std::lock_guard lg{_mavlink_components_mutex};
    if (_mavlink_components.find(message.compid) == _mavlink_components.end()) {
        LogWarn() << "Unexpected component ID " << static_cast<int>(message.compid);
        return;
    }

    // Store the result if the command failed
    if (result != MavlinkCommandSender::Result::Success) {
        switch (result) {
            case MavlinkCommandSender::Result::ConnectionError:
                _mavlink_components[message.compid].result = Result::ConnectionError;
                break;
            case MavlinkCommandSender::Result::Denied:
                _mavlink_components[message.compid].result = Result::Denied;
                break;
            case MavlinkCommandSender::Result::Unsupported:
                _mavlink_components[message.compid].result = Result::Unsupported;
                break;
            case MavlinkCommandSender::Result::Timeout:
                _mavlink_components[message.compid].result = Result::Timeout;
                break;
            default:
                _mavlink_components[message.compid].result = Result::Failed;
                break;
        }
        LogWarn() << "Requesting component metadata failed with " << static_cast<int>(result);
        on_all_types_completed(message.compid);
        return;
    }

    mavlink_component_metadata_t component_metadata;
    mavlink_msg_component_metadata_decode(&message, &component_metadata);

    component_metadata.uri[sizeof(component_metadata.uri) - 1] = '\0';
    _mavlink_components[message.compid].components.insert(std::make_pair(
        COMP_METADATA_TYPE_GENERAL,
        MetadataComponent{{component_metadata.uri, component_metadata.file_crc}}));

    retrieve_metadata(message.compid, COMP_METADATA_TYPE_GENERAL);
}

std::string MavlinkComponentMetadata::get_file_cache_tag(
    uint8_t compid, int comp_info_type, uint32_t crc, bool is_translation)
{
    char buf[255];
    snprintf(
        buf,
        sizeof(buf),
        "compid-%03i_crc-%08x_type-%02i_trans-%i",
        compid,
        crc,
        comp_info_type,
        (int)is_translation);
    return buf;
}
bool MavlinkComponentMetadata::uri_is_mavlinkftp(
    const std::string& uri, std::string& download_path, uint8_t& target_compid)
{
    // Case insensitiv check if uri starts with "mftp://"
    if (strncasecmp(uri.c_str(), "mftp://", 7) == 0) {
        download_path = uri.substr(7);
        // Extract optional [;comp=<compid>] prefix
        std::regex compid_regex(R"(^(?:\[\;comp\=(\d+)\])(.*))");
        std::smatch match;
        if (std::regex_search(download_path, match, compid_regex)) {
            target_compid = std::stoi(match[1]);
            download_path = match[2];
        }
        return true;
    }
    return false;
}

void MavlinkComponentMetadata::retrieve_metadata(uint8_t compid, COMP_METADATA_TYPE type)
{
    if (_verbose_debugging) {
        LogDebug() << "MavlinkComponentMetadata::retrieve_metadata for compid "
                   << static_cast<int>(compid) << ", type " << static_cast<int>(type);
    }

    const std::lock_guard lg{_mavlink_components_mutex};
    auto& component = _mavlink_components[compid].components[type];
    bool crc_valid;
    uint32_t crc;
    bool is_translation;
    std::string uri;

    if (component.get_next_uri(crc_valid, crc, is_translation, uri)) {
        // If translation locale is not set, skip translations
        if (is_translation && !_translation_locale) {
            retrieve_metadata(compid, type);
            return;
        }

        std::optional<std::filesystem::path> cached_file_option{};
        std::string file_cache_tag{};
        if (_file_cache && crc_valid) {
            file_cache_tag = get_file_cache_tag(compid, type, crc, is_translation);
            cached_file_option = _file_cache->access(file_cache_tag);
        }

        if (cached_file_option) {
            if (_verbose_debugging) {
                LogDebug() << "Using cached file " << cached_file_option.value();
            }
            component.current_metadata_path() = cached_file_option.value();
            retrieve_metadata(compid, type);
        } else {
            if (_verbose_debugging) {
                LogDebug() << "Downloading json " << uri;
            }
            std::string download_path;
            uint8_t target_compid = compid;

            if (uri_is_mavlinkftp(uri, download_path, target_compid)) {
                // Create compid-specific tmp subdir, to ensure multiple components don't overwrite
                // files from each other
                const std::filesystem::path tmp_download_path =
                    _tmp_download_path / std::to_string(compid);
                if (!std::filesystem::exists(tmp_download_path)) {
                    std::error_code err;
                    std::filesystem::create_directory(tmp_download_path, err);
                }

                const std::filesystem::path local_path =
                    tmp_download_path / std::filesystem::path(download_path).filename();

                _system_impl.call_user_callback([this,
                                                 download_path,
                                                 tmp_download_path,
                                                 &component,
                                                 target_compid,
                                                 local_path,
                                                 compid,
                                                 type,
                                                 file_cache_tag]() {
                    _system_impl.mavlink_ftp_client().download_async(
                        download_path,
                        tmp_download_path.string(),
                        true,
                        [this, &component, local_path, compid, type, file_cache_tag](
                            MavlinkFtpClient::ClientResult download_result,
                            MavlinkFtpClient::ProgressData progress_data) {
                            if (download_result == MavlinkFtpClient::ClientResult::Next) {
                                if (_verbose_debugging) {
                                    LogDebug() << "File download progress: "
                                               << progress_data.bytes_transferred << '/'
                                               << progress_data.total_bytes;
                                }
                                // TODO: detect slow link (e.g. telemetry), and cancel download
                                // (fallback to http) e.g. by estimating the remaining download
                                // time, and cancel if >40s
                            } else {
                                if (_verbose_debugging) {
                                    LogDebug()
                                        << "File download ended with result " << download_result;
                                }
                                if (download_result == MavlinkFtpClient::ClientResult::Success) {
                                    component.current_metadata_path() =
                                        extract_and_cache_file(local_path, file_cache_tag);
                                }
                                // Move on to the next uri or type
                                retrieve_metadata(compid, type);
                            }
                        },
                        target_compid);
                });

            } else {
                // http(s) download
#if BUILD_WITHOUT_CURL == 1
                LogErr() << "HTTP disabled at build time, skipping download of " << uri;
                retrieve_metadata(compid, type);
#else
                const std::string base_filename = filename_from_uri(uri);
                const std::filesystem::path tmp_download_path =
                    _tmp_download_path / ("http-" + std::to_string(compid) + "-" +
                                          std::to_string(type) + "-" + base_filename);
                _http_loader.download_async(
                    uri,
                    tmp_download_path.string(),
                    [this, &component, tmp_download_path, compid, type, file_cache_tag](
                        int progress, HttpStatus status, CURLcode curl_code) -> int {
                        UNUSED(progress);
                        if (status == HttpStatus::Error) {
                            LogErr() << "File download failed with result " << curl_code;
                            // Move on to the next uri or type
                            retrieve_metadata(compid, type);
                        } else if (status == HttpStatus::Finished) {
                            if (_verbose_debugging) {
                                LogDebug() << "File download finished " << tmp_download_path;
                            }
                            component.current_metadata_path() =
                                extract_and_cache_file(tmp_download_path, file_cache_tag);
                            // Move on to the next uri or type
                            retrieve_metadata(compid, type);
                        }
                        return 0;
                    });
#endif
            }
        }
    } else {
        // Move on to next type
        handle_metadata_type_completed(compid, type);
    }
}

void MavlinkComponentMetadata::handle_metadata_type_completed(
    uint8_t compid, COMP_METADATA_TYPE type)
{
    const std::lock_guard lg{_mavlink_components_mutex};
    auto& component = _mavlink_components[compid].components[type];
    assert(component.is_completed());

    // TODO: handle translations. For that we need to parse the translation summary json file
    // component.json_translation() and then download the locale-specific translation file.
    // See
    // https://github.com/mavlink/qgroundcontrol/blob/master/src/Vehicle/ComponentInformationTranslation.cc

    if (type == COMP_METADATA_TYPE_GENERAL && component.json_metadata()) {
        parse_component_metadata_general(compid, *component.json_metadata());
    }

    // Call user callbacks
    if (component.json_metadata()) {
        auto metadata_type = get_metadata_type(type);
        if (metadata_type) {
            const std::lock_guard lg_callbacks{_notification_callbacks_mutex};
            _notification_callbacks.queue(
                MetadataUpdate{compid, metadata_type.value(), component.json_metadata().value()},
                [this](const auto& func) { _system_impl.call_user_callback(func); });
        }
    }

    // Retrieve next remaining metadata type
    bool all_completed = true;
    for (const auto& [next_type, next_component] : _mavlink_components[compid].components) {
        if (!next_component.is_completed()) {
            retrieve_metadata(compid, next_type);
            all_completed = false;
            break;
        }
    }
    if (all_completed) {
        LogDebug() << "All metadata types completed for compid " << static_cast<int>(compid);
        _mavlink_components[compid].result = Result::Success;
        on_all_types_completed(compid);
    }
}

std::optional<std::filesystem::path> MavlinkComponentMetadata::extract_and_cache_file(
    const std::filesystem::path& path, const std::string& file_cache_tag)
{
    std::filesystem::path returned_path = path;
    // Decompress if needed
    if (path.extension() == ".lzma" || path.extension() == ".xz") {
        returned_path.replace_extension(".extracted");
        if (InflateLZMA::inflateLZMAFile(path, returned_path)) {
            std::filesystem::remove(path);
        } else {
            LogErr() << "Inflate of compressed json failed " << path;
            return std::nullopt;
        }
    }

    if (_file_cache && !file_cache_tag.empty()) {
        // Cache the file (this will move/remove the temp file as well)
        returned_path = _file_cache->insert(file_cache_tag, returned_path).value_or(returned_path);
    }
    return returned_path;
}
std::string MavlinkComponentMetadata::filename_from_uri(const std::string& uri)
{
    // Extract the base name from an uri
    const auto last_slash = uri.find_last_of('/');
    std::string base_filename = "downloaded_file.json";
    if (last_slash != std::string::npos) {
        base_filename = uri.substr(last_slash + 1);
    }
    const auto parameter_index = base_filename.find('?');
    if (parameter_index != std::string::npos) {
        base_filename = base_filename.substr(0, parameter_index);
    }
    return base_filename;
}
std::optional<MavlinkComponentMetadata::MetadataType>
MavlinkComponentMetadata::get_metadata_type(COMP_METADATA_TYPE type)
{
    switch (type) {
        case COMP_METADATA_TYPE_PARAMETER:
            return MetadataType::Parameter;
        case COMP_METADATA_TYPE_EVENTS:
            return MetadataType::Events;
        case COMP_METADATA_TYPE_ACTUATORS:
            return MetadataType::Actuators;
        default:
            break;
    }

    return std::nullopt;
}
std::pair<MavlinkComponentMetadata::Result, MavlinkComponentMetadata::MetadataData>
MavlinkComponentMetadata::get_metadata(uint32_t compid, MetadataType type)
{
    COMP_METADATA_TYPE metadata_type{COMP_METADATA_TYPE_GENERAL};
    switch (type) {
        case MetadataType::Parameter:
            metadata_type = COMP_METADATA_TYPE_PARAMETER;
            break;
        case MetadataType::Events:
            metadata_type = COMP_METADATA_TYPE_EVENTS;
            break;
        case MetadataType::Actuators:
            metadata_type = COMP_METADATA_TYPE_ACTUATORS;
            break;
        case MetadataType::AllCompleted:
            return std::make_pair(Result::Failed, MetadataData{});
    }
    const std::lock_guard lg{_mavlink_components_mutex};
    const auto comp_iter = _mavlink_components.find(compid);
    if (comp_iter != _mavlink_components.end()) {
        const auto type_iter = comp_iter->second.components.find(metadata_type);
        if (type_iter != comp_iter->second.components.end() && type_iter->second.json_metadata()) {
            return std::make_pair(
                Result::Success, MetadataData{*type_iter->second.json_metadata()});
        }
        if (!comp_iter->second.result || *comp_iter->second.result == Result::Success) {
            return std::make_pair(Result::NotAvailable, MetadataData{""});
        } else {
            return std::make_pair(*comp_iter->second.result, MetadataData{""});
        }
    }
    return std::make_pair(Result::NotRequested, MetadataData{""});
}
void MavlinkComponentMetadata::parse_component_metadata_general(
    uint8_t compid, const std::string& json_metadata)
{
    Json::Value metadata;
    Json::Reader reader;
    bool parsing_successful = reader.parse(json_metadata, metadata);
    if (!parsing_successful) {
        LogErr() << "Failed to parse" << reader.getFormattedErrorMessages();
        return;
    }

    if (!metadata.isMember("version")) {
        LogErr() << "version not found";
        return;
    }

    if (metadata["version"].asInt() != 1) {
        LogWarn() << "version " << metadata["version"].asInt() << " not supported";
        return;
    }

    if (!metadata.isMember("metadataTypes")) {
        LogErr() << "metadataTypes not found";
        return;
    }

    for (const auto& metadata_type : metadata["metadataTypes"]) {
        if (!metadata_type.isMember("type")) {
            LogErr() << "type missing";
            continue;
        }
        auto type = static_cast<COMP_METADATA_TYPE>(metadata_type["type"].asInt());
        auto& components = _mavlink_components[compid].components;
        if (components.find(type) != components.end()) {
            LogErr() << "component type already added: " << type;
            continue;
        }
        if (!metadata_type.isMember("uri")) {
            LogErr() << "uri missing";
            continue;
        }

        components.emplace(type, MetadataComponent{MetadataComponentUris{metadata_type}});
    }
}
void MavlinkComponentMetadata::unsubscribe_metadata_available(MetadataAvailableHandle handle)
{
    const std::lock_guard lg{_notification_callbacks_mutex};
    _notification_callbacks.unsubscribe(handle);
}
MavlinkComponentMetadata::MetadataAvailableHandle
MavlinkComponentMetadata::subscribe_metadata_available(const MetadataAvailableCallback& callback)
{
    const std::lock_guard lg_components{_mavlink_components_mutex}; // Take this mutex first
    const std::lock_guard lg_callbacks{_notification_callbacks_mutex};
    const auto handle = _notification_callbacks.subscribe(callback);

    // Immediately call the callback for all already existing metadata (with the mutexes locked)
    for (const auto& [compid, component] : _mavlink_components) {
        for (const auto& [type, metadata] : component.components) {
            auto metadata_type = get_metadata_type(type);
            if (metadata.is_completed() && metadata.json_metadata() && metadata_type) {
                _system_impl.call_user_callback([temp_callback = callback,
                                                 metadata_type = metadata_type.value(),
                                                 temp_compid = compid,
                                                 json_metadata = metadata.json_metadata().value()] {
                    temp_callback(MetadataUpdate{temp_compid, metadata_type, json_metadata});
                });
            }
        }
        if (component.result) {
            _system_impl.call_user_callback([temp_callback = callback, temp_compid = compid] {
                temp_callback(MetadataUpdate{temp_compid, MetadataType::AllCompleted, ""});
            });
        }
    }
    return handle;
}

void MavlinkComponentMetadata::on_all_types_completed(uint8_t compid)
{
    const std::lock_guard lg_callbacks{_notification_callbacks_mutex};
    _notification_callbacks.queue(
        MetadataUpdate{compid, MetadataType::AllCompleted, ""},
        [this](const auto& func) { _system_impl.call_user_callback(func); });
}

MetadataComponentUris::MetadataComponentUris(const Json::Value& value)
{
    if (value["uri"].isString()) {
        _uri_metadata = value["uri"].asString();
    }
    if (value["fileCrc"].isUInt()) {
        _crc_metadata = value["fileCrc"].asUInt();
        _crc_metadata_valid = true;
    }
    if (value["uriFallback"].isString()) {
        _uri_metadata_fallback = value["uriFallback"].asString();
    }
    if (value["fileCrcFallback"].isUInt()) {
        _crc_metadata_fallback = value["fileCrcFallback"].asUInt();
        _crc_metadata_fallback_valid = true;
    }
    if (value["translationUri"].isString()) {
        _uri_translation = value["translationUri"].asString();
    }
    if (value["translationUriFallback"].isString()) {
        _uri_translation_fallback = value["translationUriFallback"].asString();
    }
}

bool MetadataComponent::get_next_uri(
    bool& crc_valid, uint32_t& crc, bool& is_translation, std::string& uri)
{
    // Skip fallback if we have valid data already
    switch (_state) {
        case State::Metadata:
            if (_metadata) {
                _state = State::MetadataFallback;
            }
            break;
        case State::Translation:
            if (_translation) {
                _state = State::TranslationFallback;
            }
            break;
        default:
            break;
    }

    uri = "";
    while (uri.empty() && _state != State::Done) {
        switch (_state) {
            case State::Init:
                crc_valid = _metadata_uris.crc_meta_data_valid();
                crc = _metadata_uris.crc_meta_data();
                uri = _metadata_uris.uri_metadata();
                is_translation = false;
                _state = State::Metadata;
                break;
            case State::Metadata:
                crc_valid = _metadata_uris.crc_meta_data_fallback_valid();
                crc = _metadata_uris.crc_meta_data_fallback();
                uri = _metadata_uris.uri_metadata_fallback();
                is_translation = false;
                _state = State::MetadataFallback;
                break;
            case State::MetadataFallback:
                crc_valid = false;
                crc = 0;
                uri = _metadata_uris.uri_translation();
                is_translation = true;
                _state = State::Translation;
                break;
            case State::Translation:
                crc_valid = false;
                crc = 0;
                uri = _metadata_uris.uri_translation_fallback();
                is_translation = true;
                _state = State::TranslationFallback;
                break;
            case State::TranslationFallback:
                // Read files if available
                if (_metadata) {
                    std::ifstream file(*_metadata);
                    if (file) {
                        std::stringstream buffer;
                        buffer << file.rdbuf();
                        _json_metadata.emplace(buffer.str());
                    }
                }
                if (_translation) {
                    std::ifstream file(*_translation);
                    if (file) {
                        std::stringstream buffer;
                        buffer << file.rdbuf();
                        _json_translation.emplace(buffer.str());
                    }
                }
                _state = State::Done;
                break;
            case State::Done:
                break;
        }
    }
    return _state != State::Done;
}
std::optional<std::filesystem::path>& MetadataComponent::current_metadata_path()
{
    switch (_state) {
        case State::Metadata:
        case State::MetadataFallback:
            return _metadata;
        case State::Translation:
        case State::TranslationFallback:
            return _translation;
        case State::Done:
        case State::Init:
            break;
    }
    LogErr() << "current_metadata_path() called in invalid state";
    return _metadata;
}
} // namespace mavsdk
