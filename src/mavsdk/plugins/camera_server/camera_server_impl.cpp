#include "camera_server_impl.h"
#include "callback_list.tpp"

namespace mavsdk {

template class CallbackList<int32_t>;
template class CallbackList<CameraServer::TrackPoint>;
template class CallbackList<CameraServer::TrackRectangle>;

CameraServerImpl::CameraServerImpl(std::shared_ptr<ServerComponent> server_component) :
    ServerPluginImplBase(server_component)
{
    _server_component_impl->register_plugin(this);
}

CameraServerImpl::~CameraServerImpl()
{
    _server_component_impl->unregister_plugin(this);
}

void CameraServerImpl::init()
{
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_REQUEST_CAMERA_INFORMATION,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            return process_camera_information_request(command);
        },
        this);
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_REQUEST_CAMERA_SETTINGS,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            return process_camera_settings_request(command);
        },
        this);
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_REQUEST_STORAGE_INFORMATION,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            return process_storage_information_request(command);
        },
        this);
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_STORAGE_FORMAT,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            return process_storage_format(command);
        },
        this);
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_REQUEST_CAMERA_CAPTURE_STATUS,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            return process_camera_capture_status_request(command);
        },
        this);
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_RESET_CAMERA_SETTINGS,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            return process_reset_camera_settings(command);
        },
        this);
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_SET_CAMERA_MODE,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            return process_set_camera_mode(command);
        },
        this);
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_SET_CAMERA_ZOOM,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            return process_set_camera_zoom(command);
        },
        this);
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_SET_CAMERA_FOCUS,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            return process_set_camera_focus(command);
        },
        this);
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_SET_STORAGE_USAGE,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            return process_set_storage_usage(command);
        },
        this);
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_IMAGE_START_CAPTURE,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            return process_image_start_capture(command);
        },
        this);
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_IMAGE_STOP_CAPTURE,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            return process_image_stop_capture(command);
        },
        this);
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_REQUEST_CAMERA_IMAGE_CAPTURE,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            return process_camera_image_capture_request(command);
        },
        this);
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_VIDEO_START_CAPTURE,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            return process_video_start_capture(command);
        },
        this);
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_VIDEO_STOP_CAPTURE,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            return process_video_stop_capture(command);
        },
        this);
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_VIDEO_START_STREAMING,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            return process_video_start_streaming(command);
        },
        this);
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_VIDEO_STOP_STREAMING,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            return process_video_stop_streaming(command);
        },
        this);
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_REQUEST_VIDEO_STREAM_INFORMATION,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            return process_video_stream_information_request(command);
        },
        this);
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_REQUEST_VIDEO_STREAM_STATUS,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            return process_video_stream_status_request(command);
        },
        this);
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_CAMERA_TRACK_POINT,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            return process_track_point_command(command);
        },
        this);

    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_CAMERA_TRACK_RECTANGLE,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            return process_track_rectangle_command(command);
        },
        this);

    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_CAMERA_STOP_TRACKING,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            return process_track_off_command(command);
        },
        this);
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_SET_MESSAGE_INTERVAL,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            return process_set_message_interval(command);
        },
        this);
}

void CameraServerImpl::deinit()
{
    stop_image_capture_interval();
    _server_component_impl->unregister_all_mavlink_command_handlers(this);
}

bool CameraServerImpl::is_command_sender_ok(const MavlinkCommandReceiver::CommandLong& command)
{
    if (command.target_system_id != 0 &&
        command.target_system_id != _server_component_impl->get_own_system_id()) {
        return false;
    } else {
        return true;
    }
}

void CameraServerImpl::set_tracking_point_status(CameraServer::TrackPoint tracked_point)
{
    std::lock_guard<std::mutex> lg{_tracking_status_mutex};
    _tracking_mode = TrackingMode::POINT;
    _tracked_point = tracked_point;
}

void CameraServerImpl::set_tracking_rectangle_status(CameraServer::TrackRectangle tracked_rectangle)
{
    std::lock_guard<std::mutex> lg{_tracking_status_mutex};
    _tracking_mode = TrackingMode::RECTANGLE;
    _tracked_rectangle = tracked_rectangle;
}

void CameraServerImpl::set_tracking_off_status()
{
    std::lock_guard<std::mutex> lg{_tracking_status_mutex};
    _tracking_mode = TrackingMode::NONE;
}

bool CameraServerImpl::parse_version_string(const std::string& version_str)
{
    uint32_t unused;

    return parse_version_string(version_str, unused);
}

bool CameraServerImpl::parse_version_string(const std::string& version_str, uint32_t& version)
{
    // empty string means no version
    if (version_str.empty()) {
        version = 0;

        return true;
    }

    uint8_t major{}, minor{}, patch{}, dev{};

    auto ret = sscanf(version_str.c_str(), "%hhu.%hhu.%hhu.%hhu", &major, &minor, &patch, &dev);

    if (ret == EOF) {
        return false;
    }

    // pack version according to MAVLINK spec
    version = dev << 24 | patch << 16 | minor << 8 | major;

    return true;
}

CameraServer::Result CameraServerImpl::set_information(CameraServer::Information information)
{
    if (!parse_version_string(information.firmware_version)) {
        LogDebug() << "incorrectly formatted firmware version string: "
                   << information.firmware_version;
        return CameraServer::Result::WrongArgument;
    }

    // TODO: validate information.definition_file_uri

    _is_information_set = true;
    _information = information;

    return CameraServer::Result::Success;
}

CameraServer::Result
CameraServerImpl::set_video_streaming(CameraServer::VideoStreaming video_streaming)
{
    // TODO: validate uri length

    _is_video_streaming_set = true;
    _video_streaming = video_streaming;

    return CameraServer::Result::Success;
}

CameraServer::Result CameraServerImpl::set_in_progress(bool in_progress)
{
    _is_image_capture_in_progress = in_progress;

    send_capture_status();
    return CameraServer::Result::Success;
}

CameraServer::TakePhotoHandle
CameraServerImpl::subscribe_take_photo(const CameraServer::TakePhotoCallback& callback)
{
    return _take_photo_callbacks.subscribe(callback);
}

void CameraServerImpl::unsubscribe_take_photo(CameraServer::TakePhotoHandle handle)
{
    _take_photo_callbacks.unsubscribe(handle);
}

CameraServer::Result CameraServerImpl::respond_take_photo(
    CameraServer::CameraFeedback take_photo_feedback, CameraServer::CaptureInfo capture_info)
{
    // If capture_info.index == INT32_MIN, it means this was an interval
    // capture rather than a single image capture.
    if (capture_info.index != INT32_MIN) {
        // We expect each capture to be the next sequential number.
        // If _image_capture_count == 0, we ignore since it means that this is
        // the first photo since the plugin was initialized.
        if (_image_capture_count != 0 && capture_info.index != _image_capture_count + 1) {
            LogErr() << "unexpected image index, expecting " << +(_image_capture_count + 1)
                     << " but was " << +capture_info.index;
        }

        _image_capture_count = capture_info.index;
    }

    switch (take_photo_feedback) {
        default:
            // Fallthrough
        case CameraServer::CameraFeedback::Unknown:
            return CameraServer::Result::Error;
        case CameraServer::CameraFeedback::Ok: {
            // Check for error above
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_take_photo_command, MAV_RESULT_ACCEPTED);
            _server_component_impl->send_command_ack(command_ack);
            // Only break and send the captured below.
            break;
        }
        case CameraServer::CameraFeedback::Busy: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_take_photo_command, MAV_RESULT_TEMPORARILY_REJECTED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }

        case CameraServer::CameraFeedback::Failed: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_take_photo_command, MAV_RESULT_FAILED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
    }

    // REVISIT: Should we cache all CaptureInfo in memory for single image
    // captures so that we can respond to requests for lost CAMERA_IMAGE_CAPTURED
    // messages without calling back to user code?

    static const uint8_t camera_id = 0; // deprecated unused field

    const float attitude_quaternion[] = {
        capture_info.attitude_quaternion.w,
        capture_info.attitude_quaternion.x,
        capture_info.attitude_quaternion.y,
        capture_info.attitude_quaternion.z,
    };

    // There needs to be enough data to be copied mavlink internal.
    capture_info.file_url.resize(205);

    // TODO: this should be a broadcast message
    _server_component_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message{};
        mavlink_msg_camera_image_captured_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            static_cast<uint32_t>(_server_component_impl->get_time().elapsed_s() * 1e3),
            capture_info.time_utc_us,
            camera_id,
            static_cast<int32_t>(capture_info.position.latitude_deg * 1e7),
            static_cast<int32_t>(capture_info.position.longitude_deg * 1e7),
            static_cast<int32_t>(capture_info.position.absolute_altitude_m * 1e3f),
            static_cast<int32_t>(capture_info.position.relative_altitude_m * 1e3f),
            attitude_quaternion,
            capture_info.index,
            capture_info.is_success,
            capture_info.file_url.c_str());
        return message;
    });
    LogDebug() << "sent camera image captured msg - index: " << +capture_info.index;

    return CameraServer::Result::Success;
}

CameraServer::StartVideoHandle
CameraServerImpl::subscribe_start_video(const CameraServer::StartVideoCallback& callback)
{
    return _start_video_callbacks.subscribe(callback);
}

void CameraServerImpl::unsubscribe_start_video(CameraServer::StartVideoHandle handle)
{
    _start_video_callbacks.unsubscribe(handle);
}

CameraServer::Result
CameraServerImpl::respond_start_video(CameraServer::CameraFeedback start_video_feedback)
{
    switch (start_video_feedback) {
        default:
            // Fallthrough
        case CameraServer::CameraFeedback::Unknown:
            return CameraServer::Result::Error;
        case CameraServer::CameraFeedback::Ok: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_start_video_command, MAV_RESULT_ACCEPTED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
        case CameraServer::CameraFeedback::Busy: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_start_video_command, MAV_RESULT_TEMPORARILY_REJECTED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
        case CameraServer::CameraFeedback::Failed: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_start_video_command, MAV_RESULT_FAILED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
    }
}

CameraServer::StopVideoHandle
CameraServerImpl::subscribe_stop_video(const CameraServer::StopVideoCallback& callback)
{
    return _stop_video_callbacks.subscribe(callback);
}

void CameraServerImpl::unsubscribe_stop_video(CameraServer::StopVideoHandle handle)
{
    return _stop_video_callbacks.unsubscribe(handle);
}

CameraServer::Result
CameraServerImpl::respond_stop_video(CameraServer::CameraFeedback stop_video_feedback)
{
    switch (stop_video_feedback) {
        default:
            // Fallthrough
        case CameraServer::CameraFeedback::Unknown:
            return CameraServer::Result::Error;
        case CameraServer::CameraFeedback::Ok: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_stop_video_command, MAV_RESULT_ACCEPTED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
        case CameraServer::CameraFeedback::Busy: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_stop_video_command, MAV_RESULT_TEMPORARILY_REJECTED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
        case CameraServer::CameraFeedback::Failed: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_stop_video_command, MAV_RESULT_TEMPORARILY_REJECTED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
    }
}

CameraServer::StartVideoStreamingHandle CameraServerImpl::subscribe_start_video_streaming(
    const CameraServer::StartVideoStreamingCallback& callback)
{
    return _start_video_streaming_callbacks.subscribe(callback);
}

void CameraServerImpl::unsubscribe_start_video_streaming(
    CameraServer::StartVideoStreamingHandle handle)
{
    return _start_video_streaming_callbacks.unsubscribe(handle);
}

CameraServer::Result CameraServerImpl::respond_start_video_streaming(
    CameraServer::CameraFeedback start_video_streaming_feedback)
{
    switch (start_video_streaming_feedback) {
        default:
            // Fallthrough
        case CameraServer::CameraFeedback::Unknown:
            return CameraServer::Result::Error;
        case CameraServer::CameraFeedback::Ok: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_start_video_streaming_command, MAV_RESULT_ACCEPTED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
        case CameraServer::CameraFeedback::Busy: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_start_video_streaming_command, MAV_RESULT_TEMPORARILY_REJECTED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
        case CameraServer::CameraFeedback::Failed: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_start_video_streaming_command, MAV_RESULT_FAILED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
    }
}

CameraServer::StopVideoStreamingHandle CameraServerImpl::subscribe_stop_video_streaming(
    const CameraServer::StopVideoStreamingCallback& callback)
{
    return _stop_video_streaming_callbacks.subscribe(callback);
}

void CameraServerImpl::unsubscribe_stop_video_streaming(
    CameraServer::StopVideoStreamingHandle handle)
{
    return _stop_video_streaming_callbacks.unsubscribe(handle);
}

CameraServer::Result CameraServerImpl::respond_stop_video_streaming(
    CameraServer::CameraFeedback stop_video_streaming_feedback)
{
    switch (stop_video_streaming_feedback) {
        default:
            // Fallthrough
        case CameraServer::CameraFeedback::Unknown:
            return CameraServer::Result::Error;
        case CameraServer::CameraFeedback::Ok: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_stop_video_streaming_command, MAV_RESULT_ACCEPTED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
        case CameraServer::CameraFeedback::Busy: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_stop_video_streaming_command, MAV_RESULT_TEMPORARILY_REJECTED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
        case CameraServer::CameraFeedback::Failed: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_stop_video_streaming_command, MAV_RESULT_FAILED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
    }
}

CameraServer::SetModeHandle
CameraServerImpl::subscribe_set_mode(const CameraServer::SetModeCallback& callback)
{
    return _set_mode_callbacks.subscribe(callback);
}

void CameraServerImpl::unsubscribe_set_mode(CameraServer::SetModeHandle handle)
{
    _set_mode_callbacks.unsubscribe(handle);
}

CameraServer::Result
CameraServerImpl::respond_set_mode(CameraServer::CameraFeedback set_mode_feedback)
{
    switch (set_mode_feedback) {
        default:
            // Fallthrough
        case CameraServer::CameraFeedback::Unknown:
            return CameraServer::Result::Error;
        case CameraServer::CameraFeedback::Ok: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_set_mode_command, MAV_RESULT_ACCEPTED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
        case CameraServer::CameraFeedback::Busy: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_set_mode_command, MAV_RESULT_TEMPORARILY_REJECTED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
        case CameraServer::CameraFeedback::Failed: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_set_mode_command, MAV_RESULT_FAILED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
    }
}

CameraServer::StorageInformationHandle CameraServerImpl::subscribe_storage_information(
    const CameraServer::StorageInformationCallback& callback)
{
    return _storage_information_callbacks.subscribe(callback);
}

void CameraServerImpl::unsubscribe_storage_information(
    CameraServer::StorageInformationHandle handle)
{
    _storage_information_callbacks.unsubscribe(handle);
}

CameraServer::Result CameraServerImpl::respond_storage_information(
    CameraServer::CameraFeedback storage_information_feedback,
    CameraServer::StorageInformation storage_information)
{
    switch (storage_information_feedback) {
        default:
            // Fallthrough
        case CameraServer::CameraFeedback::Unknown:
            return CameraServer::Result::Error;
        case CameraServer::CameraFeedback::Ok: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_storage_information_command, MAV_RESULT_ACCEPTED);
            _server_component_impl->send_command_ack(command_ack);
            // break and send storage information
            break;
        }
        case CameraServer::CameraFeedback::Busy: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_storage_information_command, MAV_RESULT_TEMPORARILY_REJECTED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
        case CameraServer::CameraFeedback::Failed: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_storage_information_command, MAV_RESULT_FAILED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
    }

    const uint8_t storage_count = 1;

    const float total_capacity = storage_information.total_storage_mib;
    const float used_capacity = storage_information.used_storage_mib;
    const float available_capacity = storage_information.available_storage_mib;
    const float read_speed = storage_information.read_speed_mib_s;
    const float write_speed = storage_information.write_speed_mib_s;

    auto status = STORAGE_STATUS::STORAGE_STATUS_NOT_SUPPORTED;
    switch (storage_information.storage_status) {
        case CameraServer::StorageInformation::StorageStatus::NotAvailable:
            status = STORAGE_STATUS::STORAGE_STATUS_NOT_SUPPORTED;
            break;
        case CameraServer::StorageInformation::StorageStatus::Unformatted:
            status = STORAGE_STATUS::STORAGE_STATUS_UNFORMATTED;
            break;
        case CameraServer::StorageInformation::StorageStatus::Formatted:
            status = STORAGE_STATUS::STORAGE_STATUS_READY;
            break;
        case CameraServer::StorageInformation::StorageStatus::NotSupported:
            status = STORAGE_STATUS::STORAGE_STATUS_NOT_SUPPORTED;
            break;
    }

    auto type = STORAGE_TYPE::STORAGE_TYPE_UNKNOWN;
    switch (storage_information.storage_type) {
        case CameraServer::StorageInformation::StorageType::UsbStick:
            type = STORAGE_TYPE::STORAGE_TYPE_USB_STICK;
            break;
        case CameraServer::StorageInformation::StorageType::Sd:
            type = STORAGE_TYPE::STORAGE_TYPE_SD;
            break;
        case CameraServer::StorageInformation::StorageType::Microsd:
            type = STORAGE_TYPE::STORAGE_TYPE_MICROSD;
            break;
        case CameraServer::StorageInformation::StorageType::Hd:
            type = STORAGE_TYPE::STORAGE_TYPE_HD;
            break;
        case CameraServer::StorageInformation::StorageType::Other:
            type = STORAGE_TYPE::STORAGE_TYPE_OTHER;
            break;
        default:
            break;
    }

    std::string name("");
    // This needs to be long enough, otherwise the memcpy in mavlink overflows.
    name.resize(32);
    const uint8_t storage_usage = 0;

    _server_component_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message{};
        mavlink_msg_storage_information_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            static_cast<uint32_t>(_server_component_impl->get_time().elapsed_s() * 1e3),
            _last_storage_id,
            storage_count,
            status,
            total_capacity,
            used_capacity,
            available_capacity,
            read_speed,
            write_speed,
            type,
            name.data(),
            storage_usage);
        return message;
    });

    return CameraServer::Result::Success;
}

CameraServer::CaptureStatusHandle
CameraServerImpl::subscribe_capture_status(const CameraServer::CaptureStatusCallback& callback)
{
    return _capture_status_callbacks.subscribe(callback);
}

void CameraServerImpl::unsubscribe_capture_status(CameraServer::CaptureStatusHandle handle)
{
    _capture_status_callbacks.unsubscribe(handle);
}

CameraServer::Result CameraServerImpl::respond_capture_status(
    CameraServer::CameraFeedback capture_status_feedback,
    CameraServer::CaptureStatus capture_status)
{
    switch (capture_status_feedback) {
        default:
            // Fallthrough
        case CameraServer::CameraFeedback::Unknown:
            return CameraServer::Result::Error;
        case CameraServer::CameraFeedback::Ok: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_capture_status_command, MAV_RESULT_ACCEPTED);
            _server_component_impl->send_command_ack(command_ack);
            // break and send capture status
            break;
        }
        case CameraServer::CameraFeedback::Busy: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_capture_status_command, MAV_RESULT_TEMPORARILY_REJECTED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
        case CameraServer::CameraFeedback::Failed: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_capture_status_command, MAV_RESULT_FAILED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
    }

    _capture_status = capture_status;

    send_capture_status();

    return CameraServer::Result::Success;
}

CameraServer::FormatStorageHandle
CameraServerImpl::subscribe_format_storage(const CameraServer::FormatStorageCallback& callback)
{
    return _format_storage_callbacks.subscribe(callback);
}
void CameraServerImpl::unsubscribe_format_storage(CameraServer::FormatStorageHandle handle)
{
    _format_storage_callbacks.unsubscribe(handle);
}

CameraServer::Result
CameraServerImpl::respond_format_storage(CameraServer::CameraFeedback format_storage_feedback)
{
    switch (format_storage_feedback) {
        default:
            // Fallthrough
        case CameraServer::CameraFeedback::Unknown:
            return CameraServer::Result::Error;
        case CameraServer::CameraFeedback::Ok: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_format_storage_command, MAV_RESULT_ACCEPTED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
        case CameraServer::CameraFeedback::Busy: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_format_storage_command, MAV_RESULT_TEMPORARILY_REJECTED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
        case CameraServer::CameraFeedback::Failed: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_format_storage_command, MAV_RESULT_FAILED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
    }
}

CameraServer::ResetSettingsHandle
CameraServerImpl::subscribe_reset_settings(const CameraServer::ResetSettingsCallback& callback)
{
    return _reset_settings_callbacks.subscribe(callback);
}

void CameraServerImpl::unsubscribe_reset_settings(CameraServer::ResetSettingsHandle handle)
{
    _reset_settings_callbacks.unsubscribe(handle);
}

CameraServer::Result
CameraServerImpl::respond_reset_settings(CameraServer::CameraFeedback reset_settings_feedback)
{
    switch (reset_settings_feedback) {
        default:
            // Fallthrough
        case CameraServer::CameraFeedback::Unknown:
            return CameraServer::Result::Error;
        case CameraServer::CameraFeedback::Ok: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_reset_settings_command, MAV_RESULT_ACCEPTED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
        case CameraServer::CameraFeedback::Busy: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_reset_settings_command, MAV_RESULT_TEMPORARILY_REJECTED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
        case CameraServer::CameraFeedback::Failed: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_reset_settings_command, MAV_RESULT_FAILED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
    }
}

CameraServer::TrackingPointCommandHandle CameraServerImpl::subscribe_tracking_point_command(
    const CameraServer::TrackingPointCommandCallback& callback)
{
    return _tracking_point_callbacks.subscribe(callback);
}

void CameraServerImpl::unsubscribe_tracking_point_command(
    CameraServer::TrackingPointCommandHandle handle)
{
    _tracking_point_callbacks.unsubscribe(handle);
}

CameraServer::Result CameraServerImpl::respond_tracking_point_command(
    CameraServer::CameraFeedback tracking_point_feedback)
{
    switch (tracking_point_feedback) {
        default:
            // Fallthrough
        case CameraServer::CameraFeedback::Unknown:
            return CameraServer::Result::Error;
        case CameraServer::CameraFeedback::Ok: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_track_point_command, MAV_RESULT_ACCEPTED);
            _server_component_impl->send_command_ack(command_ack);
            break;
        }
        case CameraServer::CameraFeedback::Busy: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_track_point_command, MAV_RESULT_TEMPORARILY_REJECTED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
        case CameraServer::CameraFeedback::Failed: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_track_point_command, MAV_RESULT_FAILED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
    }
    return CameraServer::Result::Success;
}

CameraServer::TrackingRectangleCommandHandle CameraServerImpl::subscribe_tracking_rectangle_command(
    const CameraServer::TrackingRectangleCommandCallback& callback)
{
    return _tracking_rectangle_callbacks.subscribe(callback);
}

void CameraServerImpl::unsubscribe_tracking_rectangle_command(
    CameraServer::TrackingRectangleCommandHandle handle)
{
    _tracking_rectangle_callbacks.unsubscribe(handle);
}

CameraServer::Result CameraServerImpl::respond_tracking_rectangle_command(
    CameraServer::CameraFeedback tracking_rectangle_feedback)
{
    switch (tracking_rectangle_feedback) {
        default:
            // Fallthrough
        case CameraServer::CameraFeedback::Unknown:
            return CameraServer::Result::Error;
        case CameraServer::CameraFeedback::Ok: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_track_rectangle_command, MAV_RESULT_ACCEPTED);
            _server_component_impl->send_command_ack(command_ack);
            break;
        }
        case CameraServer::CameraFeedback::Busy: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_track_rectangle_command, MAV_RESULT_TEMPORARILY_REJECTED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
        case CameraServer::CameraFeedback::Failed: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_track_rectangle_command, MAV_RESULT_FAILED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
    }
    return CameraServer::Result::Success;
}

CameraServer::TrackingOffCommandHandle CameraServerImpl::subscribe_tracking_off_command(
    const CameraServer::TrackingOffCommandCallback& callback)
{
    return _tracking_off_callbacks.subscribe(callback);
}

void CameraServerImpl::unsubscribe_tracking_off_command(
    CameraServer::TrackingOffCommandHandle handle)
{
    _tracking_off_callbacks.unsubscribe(handle);
}

CameraServer::Result
CameraServerImpl::respond_tracking_off_command(CameraServer::CameraFeedback tracking_off_feedback)
{
    switch (tracking_off_feedback) {
        default:
            // Fallthrough
        case CameraServer::CameraFeedback::Unknown:
            return CameraServer::Result::Error;
        case CameraServer::CameraFeedback::Ok: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_tracking_off_command, MAV_RESULT_ACCEPTED);
            _server_component_impl->send_command_ack(command_ack);
            break;
        }
        case CameraServer::CameraFeedback::Busy: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_tracking_off_command, MAV_RESULT_TEMPORARILY_REJECTED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
        case CameraServer::CameraFeedback::Failed: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_tracking_off_command, MAV_RESULT_FAILED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
    }
    return CameraServer::Result::Success;
}

void CameraServerImpl::start_image_capture_interval(float interval_s, int32_t count, int32_t index)
{
    // If count == 0, it means capture "forever" until a stop command is received.
    auto remaining = std::make_shared<int32_t>(count == 0 ? INT32_MAX : count);

    _image_capture_timer_cookie = _server_component_impl->add_call_every(
        [this, remaining, index]() {
            LogDebug() << "capture image timer triggered";

            if (!_take_photo_callbacks.empty()) {
                _take_photo_callbacks(index);
                (*remaining)--;
            }

            if (*remaining == 0) {
                stop_image_capture_interval();
            }
        },
        interval_s);

    _is_image_capture_interval_set = true;
    _image_capture_timer_interval_s = interval_s;
}

void CameraServerImpl::stop_image_capture_interval()
{
    _server_component_impl->remove_call_every(_image_capture_timer_cookie);

    _is_image_capture_interval_set = false;
    _image_capture_timer_interval_s = 0;
}

std::optional<mavlink_command_ack_t> CameraServerImpl::process_camera_information_request(
    const MavlinkCommandReceiver::CommandLong& command)
{
    LogWarn() << "Camera info request";
    auto capabilities = static_cast<bool>(command.params.param1);

    if (!capabilities) {
        LogDebug() << "early info return";
        return _server_component_impl->make_command_ack_message(
            command, MAV_RESULT::MAV_RESULT_ACCEPTED);
    }

    if (!_is_information_set) {
        return _server_component_impl->make_command_ack_message(
            command, MAV_RESULT::MAV_RESULT_TEMPORARILY_REJECTED);
    }

    // ack needs to be sent before camera information message
    auto command_ack =
        _server_component_impl->make_command_ack_message(command, MAV_RESULT::MAV_RESULT_ACCEPTED);
    _server_component_impl->send_command_ack(command_ack);
    LogDebug() << "sent info ack";

    // It is safe to ignore the return value of parse_version_string() here
    // since the string was already validated in set_information().
    uint32_t firmware_version;
    parse_version_string(_information.firmware_version, firmware_version);

    // capability flags are determined by subscriptions
    uint32_t capability_flags{};

    if (!_take_photo_callbacks.empty()) {
        capability_flags |= CAMERA_CAP_FLAGS::CAMERA_CAP_FLAGS_CAPTURE_IMAGE;
    }

    if (!_start_video_callbacks.empty()) {
        capability_flags |= CAMERA_CAP_FLAGS::CAMERA_CAP_FLAGS_CAPTURE_VIDEO;
    }

    if (!_set_mode_callbacks.empty()) {
        capability_flags |= CAMERA_CAP_FLAGS::CAMERA_CAP_FLAGS_HAS_MODES;
    }

    if (_is_video_streaming_set) {
        capability_flags |= CAMERA_CAP_FLAGS::CAMERA_CAP_FLAGS_HAS_VIDEO_STREAM;
    }

    if (!_tracking_point_callbacks.empty()) {
        capability_flags |= CAMERA_CAP_FLAGS::CAMERA_CAP_FLAGS_HAS_TRACKING_POINT;
    }

    if (!_tracking_rectangle_callbacks.empty()) {
        capability_flags |= CAMERA_CAP_FLAGS::CAMERA_CAP_FLAGS_HAS_TRACKING_RECTANGLE;
    }

    _information.vendor_name.resize(sizeof(mavlink_camera_information_t::vendor_name));
    _information.model_name.resize(sizeof(mavlink_camera_information_t::model_name));
    _information.definition_file_uri.resize(
        sizeof(mavlink_camera_information_t::cam_definition_uri));

    _server_component_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message{};
        mavlink_msg_camera_information_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            static_cast<uint32_t>(_server_component_impl->get_time().elapsed_s() * 1e3),
            reinterpret_cast<const uint8_t*>(_information.vendor_name.c_str()),
            reinterpret_cast<const uint8_t*>(_information.model_name.c_str()),
            firmware_version,
            _information.focal_length_mm,
            _information.horizontal_sensor_size_mm,
            _information.vertical_sensor_size_mm,
            _information.horizontal_resolution_px,
            _information.vertical_resolution_px,
            _information.lens_id,
            capability_flags,
            _information.definition_file_version,
            _information.definition_file_uri.c_str(),
            0);
        return message;
    });
    LogDebug() << "sent info msg";

    // ack was already sent
    return std::nullopt;
}

std::optional<mavlink_command_ack_t> CameraServerImpl::process_camera_settings_request(
    const MavlinkCommandReceiver::CommandLong& command)
{
    auto settings = static_cast<bool>(command.params.param1);

    if (!settings) {
        LogDebug() << "early settings return";
        return _server_component_impl->make_command_ack_message(
            command, MAV_RESULT::MAV_RESULT_ACCEPTED);
    }

    // ack needs to be sent before camera information message
    auto command_ack =
        _server_component_impl->make_command_ack_message(command, MAV_RESULT::MAV_RESULT_ACCEPTED);
    _server_component_impl->send_command_ack(command_ack);
    LogDebug() << "sent settings ack";

    // unsupported
    const auto mode_id = CAMERA_MODE::CAMERA_MODE_IMAGE;
    const float zoom_level = 0;
    const float focus_level = 0;

    _server_component_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message{};
        mavlink_msg_camera_settings_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            static_cast<uint32_t>(_server_component_impl->get_time().elapsed_s() * 1e3),
            mode_id,
            zoom_level,
            focus_level);
        return message;
    });
    LogDebug() << "sent settings msg";

    // ack was already sent
    return std::nullopt;
}

std::optional<mavlink_command_ack_t> CameraServerImpl::process_storage_information_request(
    const MavlinkCommandReceiver::CommandLong& command)
{
    auto storage_id = static_cast<uint8_t>(command.params.param1);
    auto information = static_cast<bool>(command.params.param2);

    if (!information) {
        LogDebug() << "early storage return";
        return _server_component_impl->make_command_ack_message(
            command, MAV_RESULT::MAV_RESULT_ACCEPTED);
    }

    if (_storage_information_callbacks.empty()) {
        LogDebug()
            << "Get storage information requested with no set storage information subscriber";
        return _server_component_impl->make_command_ack_message(
            command, MAV_RESULT::MAV_RESULT_UNSUPPORTED);
    }

    // ack needs to be sent before storage information message
    auto command_ack =
        _server_component_impl->make_command_ack_message(command, MAV_RESULT::MAV_RESULT_ACCEPTED);
    _server_component_impl->send_command_ack(command_ack);

    // TODO may need support multi storage id
    _last_storage_id = storage_id;

    _last_storage_information_command = command;
    _storage_information_callbacks(storage_id);

    // unsupported
    const uint8_t storage_count = 0;
    const auto status = STORAGE_STATUS::STORAGE_STATUS_NOT_SUPPORTED;
    const float total_capacity = 0;
    const float used_capacity = 0;
    const float available_capacity = 0;
    const float read_speed = 0;
    const float write_speed = 0;
    const uint8_t type = STORAGE_TYPE::STORAGE_TYPE_UNKNOWN;
    std::string name("");
    // This needs to be long enough, otherwise the memcpy in mavlink overflows.
    name.resize(32);
    const uint8_t storage_usage = 0;

    _server_component_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message{};
        mavlink_msg_storage_information_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            static_cast<uint32_t>(_server_component_impl->get_time().elapsed_s() * 1e3),
            storage_id,
            storage_count,
            status,
            total_capacity,
            used_capacity,
            available_capacity,
            read_speed,
            write_speed,
            type,
            name.data(),
            storage_usage);

        return message;
    });

    LogDebug() << "sent storage msg";

    // ack was already sent
    return std::nullopt;
}

std::optional<mavlink_command_ack_t>
CameraServerImpl::process_storage_format(const MavlinkCommandReceiver::CommandLong& command)
{
    auto storage_id = static_cast<uint8_t>(command.params.param1);
    auto format = static_cast<bool>(command.params.param2);
    auto reset_image_log = static_cast<bool>(command.params.param3);

    UNUSED(format);
    UNUSED(reset_image_log);
    if (_format_storage_callbacks.empty()) {
        LogDebug() << "process storage format requested with no storage format subscriber";
        return _server_component_impl->make_command_ack_message(
            command, MAV_RESULT::MAV_RESULT_UNSUPPORTED);
    }

    _last_format_storage_command = command;
    _format_storage_callbacks(storage_id);

    return std::nullopt;
}

std::optional<mavlink_command_ack_t> CameraServerImpl::process_camera_capture_status_request(
    const MavlinkCommandReceiver::CommandLong& command)
{
    auto capture_status = static_cast<bool>(command.params.param1);

    if (!capture_status) {
        return _server_component_impl->make_command_ack_message(
            command, MAV_RESULT::MAV_RESULT_ACCEPTED);
    }

    if (_capture_status_callbacks.empty()) {
        LogDebug() << "process camera capture status requested with no capture status subscriber";
        return _server_component_impl->make_command_ack_message(
            command, MAV_RESULT::MAV_RESULT_UNSUPPORTED);
    }

    // ack needs to be sent before camera capture status message
    auto command_ack =
        _server_component_impl->make_command_ack_message(command, MAV_RESULT::MAV_RESULT_ACCEPTED);
    _server_component_impl->send_command_ack(command_ack);

    _last_capture_status_command = command;

    // may not need param for now ,just use zero
    _capture_status_callbacks(0);

    send_capture_status();

    // ack was already sent
    return std::nullopt;
}

void CameraServerImpl::send_capture_status()
{
    uint8_t image_status{};
    if (_capture_status.image_status ==
            CameraServer::CaptureStatus::ImageStatus::CaptureInProgress ||
        _capture_status.image_status ==
            CameraServer::CaptureStatus::ImageStatus::IntervalInProgress) {
        image_status |= StatusFlags::IN_PROGRESS;
    }

    if (_capture_status.image_status == CameraServer::CaptureStatus::ImageStatus::IntervalIdle ||
        _capture_status.image_status ==
            CameraServer::CaptureStatus::ImageStatus::IntervalInProgress ||
        _is_image_capture_interval_set) {
        image_status |= StatusFlags::INTERVAL_SET;
    }

    uint8_t video_status = 0;
    if (_capture_status.video_status == CameraServer::CaptureStatus::VideoStatus::Idle) {
        video_status = 0;
    } else if (
        _capture_status.video_status ==
        CameraServer::CaptureStatus::VideoStatus::CaptureInProgress) {
        video_status = 1;
    }

    const uint32_t recording_time_ms =
        static_cast<uint32_t>(static_cast<double>(_capture_status.recording_time_s) * 1e3);
    const float available_capacity = _capture_status.available_capacity_mib;

    _server_component_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message{};
        mavlink_msg_camera_capture_status_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            static_cast<uint32_t>(_server_component_impl->get_time().elapsed_s() * 1e3),
            image_status,
            video_status,
            _image_capture_timer_interval_s,
            recording_time_ms,
            available_capacity,
            _image_capture_count);
        return message;
    });
}

std::optional<mavlink_command_ack_t>
CameraServerImpl::process_reset_camera_settings(const MavlinkCommandReceiver::CommandLong& command)
{
    auto reset = static_cast<bool>(command.params.param1);

    UNUSED(reset);

    if (_reset_settings_callbacks.empty()) {
        LogDebug() << "reset camera settings requested with no camera settings subscriber";
        return _server_component_impl->make_command_ack_message(
            command, MAV_RESULT::MAV_RESULT_UNSUPPORTED);
    }

    _last_reset_settings_command = command;
    _reset_settings_callbacks(0);

    return std::nullopt;
}

std::optional<mavlink_command_ack_t>
CameraServerImpl::process_set_camera_mode(const MavlinkCommandReceiver::CommandLong& command)
{
    auto camera_mode = static_cast<CAMERA_MODE>(command.params.param2);

    if (_set_mode_callbacks.empty()) {
        LogDebug() << "Set mode requested with no set mode subscriber";
        return _server_component_impl->make_command_ack_message(
            command, MAV_RESULT::MAV_RESULT_UNSUPPORTED);
    }

    // convert camera mode enum type
    CameraServer::Mode convert_camera_mode = CameraServer::Mode::Unknown;
    if (camera_mode == CAMERA_MODE_IMAGE) {
        convert_camera_mode = CameraServer::Mode::Photo;
    } else if (camera_mode == CAMERA_MODE_VIDEO) {
        convert_camera_mode = CameraServer::Mode::Video;
    }

    if (convert_camera_mode == CameraServer::Mode::Unknown) {
        return _server_component_impl->make_command_ack_message(
            command, MAV_RESULT::MAV_RESULT_DENIED);
    }

    _last_set_mode_command = command;
    _set_mode_callbacks(convert_camera_mode);

    return std::nullopt;
}

std::optional<mavlink_command_ack_t>
CameraServerImpl::process_set_camera_zoom(const MavlinkCommandReceiver::CommandLong& command)
{
    auto zoom_type = static_cast<CAMERA_ZOOM_TYPE>(command.params.param1);
    auto zoom_value = command.params.param2;

    if (_zoom_in_start_callbacks.empty() && _zoom_out_start_callbacks.empty() &&
        _zoom_stop_callbacks.empty() && _zoom_range_callbacks.empty()) {
        LogWarn() << "No camera zoom is supported";
        return _server_component_impl->make_command_ack_message(
            command, MAV_RESULT::MAV_RESULT_UNSUPPORTED);
    }

    auto unsupported = [&]() {
        LogWarn() << "unsupported set camera zoom type (" << (int)zoom_type << ") request";
    };

    switch (zoom_type) {
        case ZOOM_TYPE_CONTINUOUS:
            if (zoom_value == -1.f) {
                if (_zoom_out_start_callbacks.empty()) {
                    unsupported();
                    return _server_component_impl->make_command_ack_message(
                        command, MAV_RESULT::MAV_RESULT_DENIED);
                } else {
                    _last_zoom_out_start_command = command;
                    int dummy = 0;
                    _zoom_out_start_callbacks(dummy);
                }
            } else if (zoom_value == 1.f) {
                if (_zoom_in_start_callbacks.empty()) {
                    unsupported();
                    return _server_component_impl->make_command_ack_message(
                        command, MAV_RESULT::MAV_RESULT_DENIED);
                } else {
                    _last_zoom_in_start_command = command;
                    int dummy = 0;
                    _zoom_in_start_callbacks(dummy);
                }
            } else if (zoom_value == 0.f) {
                if (_zoom_stop_callbacks.empty()) {
                    unsupported();
                    return _server_component_impl->make_command_ack_message(
                        command, MAV_RESULT::MAV_RESULT_DENIED);
                } else {
                    _last_zoom_stop_command = command;
                    int dummy = 0;
                    _zoom_stop_callbacks(dummy);
                }
            } else {
                LogWarn() << "Invalid zoom value";
                return _server_component_impl->make_command_ack_message(
                    command, MAV_RESULT::MAV_RESULT_DENIED);
            }
            break;
        case ZOOM_TYPE_RANGE:
            if (_zoom_range_callbacks.empty()) {
                unsupported();
                return _server_component_impl->make_command_ack_message(
                    command, MAV_RESULT::MAV_RESULT_DENIED);

            } else {
                _last_zoom_range_command = command;
                _zoom_range_callbacks(zoom_value);
            }
            break;
        case ZOOM_TYPE_STEP:
        // Fallthrough
        case ZOOM_TYPE_FOCAL_LENGTH:
        // Fallthrough
        case ZOOM_TYPE_HORIZONTAL_FOV:
        // Fallthrough
        default:
            unsupported();
            return _server_component_impl->make_command_ack_message(
                command, MAV_RESULT::MAV_RESULT_DENIED);
            break;
    }

    // For any success so far, we don't ack yet, but later when the respond function is called.
    return std::nullopt;
}

std::optional<mavlink_command_ack_t>
CameraServerImpl::process_set_camera_focus(const MavlinkCommandReceiver::CommandLong& command)
{
    auto focus_type = static_cast<SET_FOCUS_TYPE>(command.params.param1);
    auto focus_value = command.params.param2;

    UNUSED(focus_type);
    UNUSED(focus_value);

    LogDebug() << "unsupported set camera focus request";

    return _server_component_impl->make_command_ack_message(
        command, MAV_RESULT::MAV_RESULT_UNSUPPORTED);
}

std::optional<mavlink_command_ack_t>
CameraServerImpl::process_set_storage_usage(const MavlinkCommandReceiver::CommandLong& command)
{
    auto storage_id = static_cast<uint8_t>(command.params.param1);
    auto usage = static_cast<STORAGE_USAGE_FLAG>(command.params.param2);

    UNUSED(storage_id);
    UNUSED(usage);

    LogDebug() << "unsupported set storage usage request";

    return _server_component_impl->make_command_ack_message(
        command, MAV_RESULT::MAV_RESULT_UNSUPPORTED);
}

std::optional<mavlink_command_ack_t>
CameraServerImpl::process_image_start_capture(const MavlinkCommandReceiver::CommandLong& command)
{
    auto interval_s = command.params.param2;
    auto total_images = static_cast<int32_t>(command.params.param3);
    auto seq_number = static_cast<int32_t>(command.params.param4);

    LogDebug() << "received image start capture request - interval: " << +interval_s
               << " total: " << +total_images << " index: " << +seq_number;

    // TODO: validate parameters and return MAV_RESULT_DENIED not valid

    stop_image_capture_interval();

    if (_take_photo_callbacks.empty()) {
        LogDebug() << "image capture requested with no take photo subscriber";
        return _server_component_impl->make_command_ack_message(
            command, MAV_RESULT::MAV_RESULT_UNSUPPORTED);
    }

    // single image capture
    if (total_images == 1) {
        // MAV_RESULT_ACCEPTED must be sent before CAMERA_IMAGE_CAPTURED
        auto command_ack = _server_component_impl->make_command_ack_message(
            command, MAV_RESULT::MAV_RESULT_IN_PROGRESS);
        _server_component_impl->send_command_ack(command_ack);

        _last_take_photo_command = command;

        _take_photo_callbacks(seq_number);

        return std::nullopt;
    }

    start_image_capture_interval(interval_s, total_images, seq_number);

    return _server_component_impl->make_command_ack_message(
        command, MAV_RESULT::MAV_RESULT_ACCEPTED);
}

std::optional<mavlink_command_ack_t>
CameraServerImpl::process_image_stop_capture(const MavlinkCommandReceiver::CommandLong& command)
{
    LogDebug() << "received image stop capture request";

    // REVISIT: should we return something other that MAV_RESULT_ACCEPTED if
    // there is not currently a capture interval active?
    stop_image_capture_interval();

    return _server_component_impl->make_command_ack_message(
        command, MAV_RESULT::MAV_RESULT_ACCEPTED);
}

std::optional<mavlink_command_ack_t> CameraServerImpl::process_camera_image_capture_request(
    const MavlinkCommandReceiver::CommandLong& command)
{
    auto seq_number = static_cast<uint32_t>(command.params.param1);

    UNUSED(seq_number);

    LogDebug() << "unsupported image capture request";

    return _server_component_impl->make_command_ack_message(
        command, MAV_RESULT::MAV_RESULT_UNSUPPORTED);
}

std::optional<mavlink_command_ack_t>
CameraServerImpl::process_video_start_capture(const MavlinkCommandReceiver::CommandLong& command)
{
    auto stream_id = static_cast<uint8_t>(command.params.param1);
    auto status_frequency = command.params.param2;

    UNUSED(status_frequency);

    if (_start_video_callbacks.empty()) {
        LogDebug() << "video start capture requested with no video start capture subscriber";
        return _server_component_impl->make_command_ack_message(
            command, MAV_RESULT::MAV_RESULT_UNSUPPORTED);
    }

    _last_start_video_command = command;
    _start_video_callbacks(stream_id);

    return std::nullopt;
}

std::optional<mavlink_command_ack_t>
CameraServerImpl::process_video_stop_capture(const MavlinkCommandReceiver::CommandLong& command)
{
    auto stream_id = static_cast<uint8_t>(command.params.param1);

    if (_stop_video_callbacks.empty()) {
        LogDebug() << "video stop capture requested with no video stop capture subscriber";
        return _server_component_impl->make_command_ack_message(
            command, MAV_RESULT::MAV_RESULT_UNSUPPORTED);
    }

    _last_stop_video_command = command;
    _stop_video_callbacks(stream_id);

    return std::nullopt;
}

std::optional<mavlink_command_ack_t>
CameraServerImpl::process_video_start_streaming(const MavlinkCommandReceiver::CommandLong& command)
{
    auto stream_id = static_cast<uint8_t>(command.params.param1);

    if (_start_video_streaming_callbacks.empty()) {
        LogDebug() << "video start streaming requested with no video start streaming subscriber";
        return _server_component_impl->make_command_ack_message(
            command, MAV_RESULT::MAV_RESULT_UNSUPPORTED);
    }

    _last_start_video_streaming_command = command;
    _start_video_streaming_callbacks(stream_id);

    return std::nullopt;
}

std::optional<mavlink_command_ack_t>
CameraServerImpl::process_video_stop_streaming(const MavlinkCommandReceiver::CommandLong& command)
{
    auto stream_id = static_cast<uint8_t>(command.params.param1);

    if (_stop_video_streaming_callbacks.empty()) {
        LogDebug() << "video stop streaming requested with no video stop streaming subscriber";
        return _server_component_impl->make_command_ack_message(
            command, MAV_RESULT::MAV_RESULT_UNSUPPORTED);
    }

    _last_stop_video_streaming_command = command;
    _stop_video_streaming_callbacks(stream_id);

    return std::nullopt;
}

std::optional<mavlink_command_ack_t> CameraServerImpl::process_video_stream_information_request(
    const MavlinkCommandReceiver::CommandLong& command)
{
    auto stream_id = static_cast<uint8_t>(command.params.param1);

    UNUSED(stream_id);

    if (_is_video_streaming_set) {
        auto command_ack = _server_component_impl->make_command_ack_message(
            command, MAV_RESULT::MAV_RESULT_ACCEPTED);
        _server_component_impl->send_command_ack(command_ack);
        LogDebug() << "sent video streaming ack";

        const char name[32] = "";

        _video_streaming.rtsp_uri.resize(sizeof(mavlink_video_stream_information_t::uri));

        mavlink_message_t msg{};
        mavlink_msg_video_stream_information_pack(
            _server_component_impl->get_own_system_id(),
            _server_component_impl->get_own_component_id(),
            &msg,
            0, // Stream id
            0, // Count
            VIDEO_STREAM_TYPE_RTSP,
            VIDEO_STREAM_STATUS_FLAGS_RUNNING,
            0, // famerate
            0, // resolution horizontal
            0, // resolution vertical
            0, // bitrate
            0, // rotation
            0, // horizontal field of view
            name,
            _video_streaming.rtsp_uri.c_str());

        _server_component_impl->send_message(msg);

        // Ack already sent.
        return std::nullopt;

    } else {
        return _server_component_impl->make_command_ack_message(
            command, MAV_RESULT::MAV_RESULT_UNSUPPORTED);
    }
}

std::optional<mavlink_command_ack_t> CameraServerImpl::process_video_stream_status_request(
    const MavlinkCommandReceiver::CommandLong& command)
{
    auto stream_id = static_cast<uint8_t>(command.params.param1);

    UNUSED(stream_id);

    if (!_is_video_streaming_set) {
        return _server_component_impl->make_command_ack_message(
            command, MAV_RESULT::MAV_RESULT_UNSUPPORTED);
    }

    auto command_ack =
        _server_component_impl->make_command_ack_message(command, MAV_RESULT::MAV_RESULT_ACCEPTED);
    _server_component_impl->send_command_ack(command_ack);
    LogDebug() << "sent video streaming ack";

    mavlink_message_t msg{};
    mavlink_msg_video_stream_status_pack(
        _server_component_impl->get_own_system_id(),
        _server_component_impl->get_own_component_id(),
        &msg,
        0, // Stream id
        VIDEO_STREAM_STATUS_FLAGS_RUNNING,
        0, // framerate
        0, // resolution horizontal
        0, // resolution vertical
        0, // bitrate
        0, // rotation
        0 // horizontal field of view
    );
    _server_component_impl->send_message(msg);

    // ack was already sent
    return std::nullopt;
}

CameraServer::ZoomInStartHandle
CameraServerImpl::subscribe_zoom_in_start(const CameraServer::ZoomInStartCallback& callback)
{
    return _zoom_in_start_callbacks.subscribe(callback);
}

void CameraServerImpl::unsubscribe_zoom_in_start(CameraServer::ZoomInStartHandle handle)
{
    _zoom_in_start_callbacks.unsubscribe(handle);
}

CameraServer::Result
CameraServerImpl::respond_zoom_in_start(CameraServer::CameraFeedback zoom_in_start_feedback)
{
    switch (zoom_in_start_feedback) {
        default:
            // Fallthrough
        case CameraServer::CameraFeedback::Unknown:
            return CameraServer::Result::Error;
        case CameraServer::CameraFeedback::Ok: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_zoom_in_start_command, MAV_RESULT_ACCEPTED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
        case CameraServer::CameraFeedback::Busy: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_zoom_in_start_command, MAV_RESULT_TEMPORARILY_REJECTED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
        case CameraServer::CameraFeedback::Failed: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_zoom_in_start_command, MAV_RESULT_FAILED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
    }
}

CameraServer::ZoomOutStartHandle
CameraServerImpl::subscribe_zoom_out_start(const CameraServer::ZoomOutStartCallback& callback)
{
    return _zoom_out_start_callbacks.subscribe(callback);
}

void CameraServerImpl::unsubscribe_zoom_out_start(CameraServer::ZoomOutStartHandle handle)
{
    _zoom_out_start_callbacks.unsubscribe(handle);
}

CameraServer::Result
CameraServerImpl::respond_zoom_out_start(CameraServer::CameraFeedback zoom_out_start_feedback)
{
    switch (zoom_out_start_feedback) {
        default:
            // Fallthrough
        case CameraServer::CameraFeedback::Unknown:
            return CameraServer::Result::Error;
        case CameraServer::CameraFeedback::Ok: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_zoom_out_start_command, MAV_RESULT_ACCEPTED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
        case CameraServer::CameraFeedback::Busy: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_zoom_out_start_command, MAV_RESULT_TEMPORARILY_REJECTED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
        case CameraServer::CameraFeedback::Failed: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_zoom_out_start_command, MAV_RESULT_FAILED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
    }
}

CameraServer::ZoomStopHandle
CameraServerImpl::subscribe_zoom_stop(const CameraServer::ZoomStopCallback& callback)
{
    return _zoom_stop_callbacks.subscribe(callback);
}

void CameraServerImpl::unsubscribe_zoom_stop(CameraServer::ZoomStopHandle handle)
{
    _zoom_stop_callbacks.unsubscribe(handle);
}

CameraServer::Result
CameraServerImpl::respond_zoom_stop(CameraServer::CameraFeedback zoom_stop_feedback)
{
    switch (zoom_stop_feedback) {
        default:
            // Fallthrough
        case CameraServer::CameraFeedback::Unknown:
            return CameraServer::Result::Error;
        case CameraServer::CameraFeedback::Ok: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_zoom_stop_command, MAV_RESULT_ACCEPTED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
        case CameraServer::CameraFeedback::Busy: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_zoom_stop_command, MAV_RESULT_TEMPORARILY_REJECTED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
        case CameraServer::CameraFeedback::Failed: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_zoom_stop_command, MAV_RESULT_FAILED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
    }
}

CameraServer::ZoomRangeHandle
CameraServerImpl::subscribe_zoom_range(const CameraServer::ZoomRangeCallback& callback)
{
    return _zoom_range_callbacks.subscribe(callback);
}

void CameraServerImpl::unsubscribe_zoom_range(CameraServer::ZoomRangeHandle handle)
{
    _zoom_range_callbacks.unsubscribe(handle);
}

CameraServer::Result
CameraServerImpl::respond_zoom_range(CameraServer::CameraFeedback zoom_range_feedback)
{
    switch (zoom_range_feedback) {
        case CameraServer::CameraFeedback::Ok: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_zoom_range_command, MAV_RESULT_ACCEPTED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
        case CameraServer::CameraFeedback::Busy: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_zoom_range_command, MAV_RESULT_TEMPORARILY_REJECTED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
        case CameraServer::CameraFeedback::Failed: {
            auto command_ack = _server_component_impl->make_command_ack_message(
                _last_zoom_range_command, MAV_RESULT_FAILED);
            _server_component_impl->send_command_ack(command_ack);
            return CameraServer::Result::Success;
        }
        case CameraServer::CameraFeedback::Unknown:
            // Fallthrough
        default:
            return CameraServer::Result::Error;
    }
}

std::optional<mavlink_command_ack_t>
CameraServerImpl::process_track_point_command(const MavlinkCommandReceiver::CommandLong& command)
{
    if (!is_command_sender_ok(command)) {
        LogWarn() << "Incoming track point command is for target sysid "
                  << int(command.target_system_id) << " instead of "
                  << int(_server_component_impl->get_own_system_id());
        return std::nullopt;
    }

    if (_tracking_point_callbacks.empty()) {
        LogDebug() << "Track point requested with no user callback provided";
        return _server_component_impl->make_command_ack_message(
            command, MAV_RESULT::MAV_RESULT_UNSUPPORTED);
    }

    CameraServer::TrackPoint track_point{
        command.params.param1, command.params.param2, command.params.param3};

    _last_track_point_command = command;
    _tracking_point_callbacks(track_point);
    // We don't send an ack but leave that to the user.
    return std::nullopt;
}

std::optional<mavlink_command_ack_t> CameraServerImpl::process_track_rectangle_command(
    const MavlinkCommandReceiver::CommandLong& command)
{
    if (!is_command_sender_ok(command)) {
        LogWarn() << "Incoming track rectangle command is for target sysid "
                  << int(command.target_system_id) << " instead of "
                  << int(_server_component_impl->get_own_system_id());
        return std::nullopt;
    }

    if (_tracking_rectangle_callbacks.empty()) {
        LogDebug() << "Track rectangle requested with no user callback provided";
        return _server_component_impl->make_command_ack_message(
            command, MAV_RESULT::MAV_RESULT_UNSUPPORTED);
    }

    CameraServer::TrackRectangle track_rectangle{
        command.params.param1, command.params.param2, command.params.param3, command.params.param4};

    _last_track_rectangle_command = command;
    _tracking_rectangle_callbacks(track_rectangle);
    // We don't send an ack but leave that to the user.
    return std::nullopt;
}

std::optional<mavlink_command_ack_t>
CameraServerImpl::process_track_off_command(const MavlinkCommandReceiver::CommandLong& command)
{
    if (!is_command_sender_ok(command)) {
        LogWarn() << "Incoming track off command is for target sysid "
                  << int(command.target_system_id) << " instead of "
                  << int(_server_component_impl->get_own_system_id());
        return std::nullopt;
    }

    if (_tracking_off_callbacks.empty()) {
        LogDebug() << "Tracking off requested with no user callback provided";
        return _server_component_impl->make_command_ack_message(
            command, MAV_RESULT::MAV_RESULT_UNSUPPORTED);
    }

    _last_tracking_off_command = command;
    _tracking_off_callbacks(0);
    // We don't send an ack but leave that to the user.
    return std::nullopt;
}

std::optional<mavlink_command_ack_t>
CameraServerImpl::process_set_message_interval(const MavlinkCommandReceiver::CommandLong& command)
{
    if (!is_command_sender_ok(command)) {
        LogWarn() << "Incoming track off command is for target sysid "
                  << int(command.target_system_id) << " instead of "
                  << int(_server_component_impl->get_own_system_id());
        return std::nullopt;
    }

    auto message_id = static_cast<uint32_t>(command.params.param1);
    auto interval_us = static_cast<int32_t>(command.params.param2);
    UNUSED(message_id);

    // Interval value of -1 means to disable sending messages
    if (interval_us < 0) {
        stop_sending_tracking_status();
    } else {
        start_sending_tracking_status(interval_us);
    }

    // Always send the "Accepted" result
    return _server_component_impl->make_command_ack_message(
        command, MAV_RESULT::MAV_RESULT_ACCEPTED);
}

void CameraServerImpl::send_tracking_status_with_interval(uint32_t interval_us)
{
    while (true) {
        std::this_thread::sleep_for(std::chrono::microseconds{interval_us});
        {
            std::scoped_lock lg{_tracking_status_mutex};
            if (!_sending_tracking_status) {
                return;
            }
        }
        _server_component_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
            mavlink_message_t message;
            std::lock_guard<std::mutex> lg{_tracking_status_mutex};

            // The message is filled based on current tracking mode
            switch (_tracking_mode) {
                default:
                    // Fallthrough
                case TrackingMode::NONE:

                    mavlink_msg_camera_tracking_image_status_pack_chan(
                        mavlink_address.system_id,
                        mavlink_address.component_id,
                        channel,
                        &message,
                        CAMERA_TRACKING_STATUS_FLAGS_IDLE,
                        CAMERA_TRACKING_MODE_NONE,
                        CAMERA_TRACKING_TARGET_DATA_NONE,
                        0.0f,
                        0.0f,
                        0.0f,
                        0.0f,
                        0.0f,
                        0.0f,
                        0.0f);
                    break;
                case TrackingMode::POINT:

                    mavlink_msg_camera_tracking_image_status_pack_chan(
                        mavlink_address.system_id,
                        mavlink_address.component_id,
                        channel,
                        &message,
                        CAMERA_TRACKING_STATUS_FLAGS_ACTIVE,
                        CAMERA_TRACKING_MODE_POINT,
                        CAMERA_TRACKING_TARGET_DATA_IN_STATUS,
                        _tracked_point.point_x,
                        _tracked_point.point_y,
                        _tracked_point.radius,
                        0.0f,
                        0.0f,
                        0.0f,
                        0.0f);
                    break;

                case TrackingMode::RECTANGLE:

                    mavlink_msg_camera_tracking_image_status_pack_chan(
                        mavlink_address.system_id,
                        mavlink_address.component_id,
                        channel,
                        &message,
                        CAMERA_TRACKING_STATUS_FLAGS_ACTIVE,
                        CAMERA_TRACKING_MODE_RECTANGLE,
                        CAMERA_TRACKING_TARGET_DATA_IN_STATUS,
                        0.0f,
                        0.0f,
                        0.0f,
                        _tracked_rectangle.top_left_corner_x,
                        _tracked_rectangle.top_left_corner_y,
                        _tracked_rectangle.bottom_right_corner_x,
                        _tracked_rectangle.bottom_right_corner_y);
                    break;
            }
            return message;
        });
    }
}

void CameraServerImpl::start_sending_tracking_status(uint32_t interval_ms)
{
    // Stop sending status with the old interval
    stop_sending_tracking_status();
    _sending_tracking_status = true;
    _tracking_status_sending_thread =
        std::thread{&CameraServerImpl::send_tracking_status_with_interval, this, interval_ms};
}

void CameraServerImpl::stop_sending_tracking_status()
{
    // Firstly, ask the other thread to stop sending the status
    {
        std::scoped_lock lg{_tracking_status_mutex};
        _sending_tracking_status = false;
    }
    // If the thread was active, wait for it to finish
    if (_tracking_status_sending_thread.joinable()) {
        _tracking_status_sending_thread.join();
    }
}

} // namespace mavsdk
