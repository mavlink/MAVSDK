#include <future>

#include "camera/camera.h"
#include "camera/camera.grpc.pb.h"

namespace dronecode_sdk {
namespace backend {

template<typename Camera = Camera>
class CameraServiceImpl final : public rpc::camera::CameraService::Service {
public:
    CameraServiceImpl(Camera &camera) : _camera(camera) {}

    grpc::Status TakePhoto(grpc::ServerContext * /* context */,
                           const rpc::camera::TakePhotoRequest * /* request */,
                           rpc::camera::TakePhotoResponse *response) override
    {
        auto camera_result = _camera.take_photo();

        if (response != nullptr) {
            fillResponseWithResult(response, camera_result);
        }

        return grpc::Status::OK;
    }

    template<typename ResponseType>
    void fillResponseWithResult(ResponseType *response,
                                dronecode_sdk::Camera::Result camera_result) const
    {
        auto rpc_result = static_cast<rpc::camera::CameraResult::Result>(camera_result);

        auto *rpc_camera_result = new rpc::camera::CameraResult();
        rpc_camera_result->set_result(rpc_result);
        rpc_camera_result->set_result_str(dronecode_sdk::Camera::result_str(camera_result));

        response->set_allocated_camera_result(rpc_camera_result);
    }

    grpc::Status StartPhotoInterval(grpc::ServerContext * /* context */,
                                    const rpc::camera::StartPhotoIntervalRequest *request,
                                    rpc::camera::StartPhotoIntervalResponse *response) override
    {
        if (request == nullptr) {
            if (response != nullptr) {
                fillResponseWithResult(response, dronecode_sdk::Camera::Result::WRONG_ARGUMENT);
            }

            return grpc::Status::OK;
        }

        auto camera_result = _camera.start_photo_interval(request->interval_s());

        if (response != nullptr) {
            fillResponseWithResult(response, camera_result);
        }

        return grpc::Status::OK;
    }

    grpc::Status StopPhotoInterval(grpc::ServerContext * /* context */,
                                   const rpc::camera::StopPhotoIntervalRequest * /* request */,
                                   rpc::camera::StopPhotoIntervalResponse *response) override
    {
        auto camera_result = _camera.stop_photo_interval();

        if (response != nullptr) {
            fillResponseWithResult(response, camera_result);
        }

        return grpc::Status::OK;
    }

    grpc::Status StartVideo(grpc::ServerContext * /* context */,
                            const rpc::camera::StartVideoRequest * /* request */,
                            rpc::camera::StartVideoResponse *response) override
    {
        auto camera_result = _camera.start_video();

        if (response != nullptr) {
            fillResponseWithResult(response, camera_result);
        }

        return grpc::Status::OK;
    }

    grpc::Status StopVideo(grpc::ServerContext * /* context */,
                           const rpc::camera::StopVideoRequest * /* request */,
                           rpc::camera::StopVideoResponse *response) override
    {
        auto camera_result = _camera.stop_video();

        if (response != nullptr) {
            fillResponseWithResult(response, camera_result);
        }

        return grpc::Status::OK;
    }

    grpc::Status StartVideoStreaming(grpc::ServerContext * /* context */,
                                     const rpc::camera::StartVideoStreamingRequest * /* request */,
                                     rpc::camera::StartVideoStreamingResponse *response) override
    {
        auto camera_result = _camera.start_video_streaming();

        if (response != nullptr) {
            fillResponseWithResult(response, camera_result);
        }

        return grpc::Status::OK;
    }

    grpc::Status StopVideoStreaming(grpc::ServerContext * /* context */,
                                    const rpc::camera::StopVideoStreamingRequest * /* request */,
                                    rpc::camera::StopVideoStreamingResponse *response) override
    {
        auto camera_result = _camera.stop_video_streaming();

        if (response != nullptr) {
            fillResponseWithResult(response, camera_result);
        }

        return grpc::Status::OK;
    }

    grpc::Status SetMode(grpc::ServerContext * /* context */,
                         const rpc::camera::SetModeRequest *request,
                         rpc::camera::SetModeResponse *response) override
    {
        if (request != nullptr) {
            auto camera_result = _camera.set_mode(translateRPCCameraMode(request->camera_mode()));

            if (response != nullptr) {
                fillResponseWithResult(response, camera_result);
            }
        }

        return grpc::Status::OK;
    }

    static dronecode_sdk::Camera::Mode translateRPCCameraMode(const rpc::camera::CameraMode mode)
    {
        switch (mode) {
            case rpc::camera::CameraMode::PHOTO:
                return dronecode_sdk::Camera::Mode::PHOTO;
            case rpc::camera::CameraMode::VIDEO:
                return dronecode_sdk::Camera::Mode::VIDEO;
            default:
                return dronecode_sdk::Camera::Mode::UNKNOWN;
        }
    }

    static rpc::camera::CameraMode translateCameraMode(const dronecode_sdk::Camera::Mode mode)
    {
        switch (mode) {
            case dronecode_sdk::Camera::Mode::PHOTO:
                return rpc::camera::CameraMode::PHOTO;
            case dronecode_sdk::Camera::Mode::VIDEO:
                return rpc::camera::CameraMode::VIDEO;
            default:
                return rpc::camera::CameraMode::UNKNOWN;
        }
    }

    grpc::Status SubscribeMode(grpc::ServerContext * /* context */,
                               const rpc::camera::SubscribeModeRequest * /* request */,
                               grpc::ServerWriter<rpc::camera::ModeResponse> *writer) override
    {
        std::promise<void> stream_closed_promise;
        auto stream_closed_future = stream_closed_promise.get_future();

        bool is_finished = false;

        _camera.subscribe_mode(
            [&writer, &stream_closed_promise, &is_finished](const dronecode_sdk::Camera::Mode mode) {
                rpc::camera::ModeResponse rpc_mode_response;
                rpc_mode_response.set_camera_mode(translateCameraMode(mode));

                if (!writer->Write(rpc_mode_response) && !is_finished) {
                    is_finished = true;
                    stream_closed_promise.set_value();
                }
            });

        stream_closed_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status
    SetVideoStreamSettings(grpc::ServerContext * /* context */,
                           const rpc::camera::SetVideoStreamSettingsRequest *request,
                           rpc::camera::SetVideoStreamSettingsResponse * /* response */) override
    {
        if (request != nullptr) {
            const auto video_stream_settings =
                translateRPCVideoStreamSettings(request->video_stream_settings());
            _camera.set_video_stream_settings(video_stream_settings);
        }

        return grpc::Status::OK;
    }

    static std::unique_ptr<rpc::camera::VideoStreamSettings>
    translateVideoStreamSettings(const dronecode_sdk::Camera::VideoStreamSettings video_stream_settings)
    {
        auto rpc_video_stream_settings = std::unique_ptr<rpc::camera::VideoStreamSettings>(
            new rpc::camera::VideoStreamSettings());
        rpc_video_stream_settings->set_frame_rate_hz(video_stream_settings.frame_rate_hz);
        rpc_video_stream_settings->set_horizontal_resolution_pix(
            video_stream_settings.horizontal_resolution_pix);
        rpc_video_stream_settings->set_vertical_resolution_pix(
            video_stream_settings.vertical_resolution_pix);
        rpc_video_stream_settings->set_bit_rate_b_s(video_stream_settings.bit_rate_b_s);
        rpc_video_stream_settings->set_rotation_deg(video_stream_settings.rotation_deg);
        rpc_video_stream_settings->set_uri(video_stream_settings.uri);

        return rpc_video_stream_settings;
    }

    static dronecode_sdk::Camera::VideoStreamSettings translateRPCVideoStreamSettings(
        const rpc::camera::VideoStreamSettings &rpc_video_stream_settings)
    {
        dronecode_sdk::Camera::VideoStreamSettings video_stream_settings;
        video_stream_settings.frame_rate_hz = rpc_video_stream_settings.frame_rate_hz();
        video_stream_settings.horizontal_resolution_pix =
            rpc_video_stream_settings.horizontal_resolution_pix();
        video_stream_settings.vertical_resolution_pix =
            rpc_video_stream_settings.vertical_resolution_pix();
        video_stream_settings.bit_rate_b_s = rpc_video_stream_settings.bit_rate_b_s();
        video_stream_settings.rotation_deg = rpc_video_stream_settings.rotation_deg();
        video_stream_settings.uri = rpc_video_stream_settings.uri();

        return video_stream_settings;
    }

    static std::unique_ptr<rpc::camera::VideoStreamInfo>
    translateVideoStreamInfo(const dronecode_sdk::Camera::VideoStreamInfo &video_stream_info)
    {
        auto rpc_video_stream_info =
            std::unique_ptr<rpc::camera::VideoStreamInfo>(new rpc::camera::VideoStreamInfo());

        auto rpc_video_stream_settings = translateVideoStreamSettings(video_stream_info.settings);
        rpc_video_stream_info->set_allocated_video_stream_settings(
            rpc_video_stream_settings.release());

        auto rpc_video_stream_status = translateVideoStreamStatus(video_stream_info.status);
        rpc_video_stream_info->set_video_stream_status(rpc_video_stream_status);

        return rpc_video_stream_info;
    }

    static rpc::camera::VideoStreamInfo_VideoStreamStatus
    translateVideoStreamStatus(const dronecode_sdk::Camera::VideoStreamInfo::Status status)
    {
        switch (status) {
            case dronecode_sdk::Camera::VideoStreamInfo::Status::IN_PROGRESS:
                return rpc::camera::VideoStreamInfo_VideoStreamStatus_IN_PROGRESS;
            case dronecode_sdk::Camera::VideoStreamInfo::Status::NOT_RUNNING:
            default:
                return rpc::camera::VideoStreamInfo_VideoStreamStatus_NOT_RUNNING;
        }
    }

    static dronecode_sdk::Camera::VideoStreamInfo::Status
    translateRPCVideoStreamStatus(const rpc::camera::VideoStreamInfo_VideoStreamStatus status)
    {
        switch (status) {
            case rpc::camera::VideoStreamInfo_VideoStreamStatus_IN_PROGRESS:
                return dronecode_sdk::Camera::VideoStreamInfo::Status::IN_PROGRESS;
            case rpc::camera::VideoStreamInfo_VideoStreamStatus_NOT_RUNNING:
            default:
                return dronecode_sdk::Camera::VideoStreamInfo::Status::NOT_RUNNING;
        }
    }

    static dronecode_sdk::Camera::VideoStreamInfo
    translateRPCVideoStreamInfo(const rpc::camera::VideoStreamInfo &rpc_video_stream_info)
    {
        dronecode_sdk::Camera::VideoStreamInfo video_stream_info;
        video_stream_info.settings =
            translateRPCVideoStreamSettings(rpc_video_stream_info.video_stream_settings());
        video_stream_info.status =
            translateRPCVideoStreamStatus(rpc_video_stream_info.video_stream_status());

        return video_stream_info;
    }

    grpc::Status SubscribeVideoStreamInfo(
        grpc::ServerContext * /* context */,
        const rpc::camera::SubscribeVideoStreamInfoRequest * /* request */,
        grpc::ServerWriter<rpc::camera::VideoStreamInfoResponse> *writer) override
    {
        std::promise<void> stream_closed_promise;
        auto stream_closed_future = stream_closed_promise.get_future();

        bool is_finished = false;

        _camera.subscribe_video_stream_info(
            [&writer, &stream_closed_promise, &is_finished](
                const dronecode_sdk::Camera::VideoStreamInfo video_info) {
                rpc::camera::VideoStreamInfoResponse rpc_video_stream_info_response;
                auto video_stream_info = translateVideoStreamInfo(video_info);
                rpc_video_stream_info_response.set_allocated_video_stream_info(
                    video_stream_info.release());

                if (!writer->Write(rpc_video_stream_info_response) && !is_finished) {
                    is_finished = true;
                    stream_closed_promise.set_value();
                }
            });

        stream_closed_future.wait();

        return grpc::Status::OK;
    }

    grpc::Status
    SubscribeCaptureInfo(grpc::ServerContext * /* context */,
                         const rpc::camera::SubscribeCaptureInfoRequest * /* request */,
                         grpc::ServerWriter<rpc::camera::CaptureInfoResponse> *writer) override
    {
        std::promise<void> stream_closed_promise;
        auto stream_closed_future = stream_closed_promise.get_future();

        bool is_finished = false;

        _camera.subscribe_capture_info([&writer, &stream_closed_promise, &is_finished](
                                           const dronecode_sdk::Camera::CaptureInfo capture_info) {
            rpc::camera::CaptureInfoResponse rpc_capture_info_response;
            auto rpc_capture_info = translateCaptureInfo(capture_info);
            rpc_capture_info_response.set_allocated_capture_info(rpc_capture_info.release());

            if (!writer->Write(rpc_capture_info_response) && !is_finished) {
                is_finished = true;
                stream_closed_promise.set_value();
            }
        });

        stream_closed_future.wait();

        return grpc::Status::OK;
    }

    static std::unique_ptr<rpc::camera::CaptureInfo>
    translateCaptureInfo(const dronecode_sdk::Camera::CaptureInfo &capture_info)
    {
        auto rpc_capture_info =
            std::unique_ptr<rpc::camera::CaptureInfo>(new rpc::camera::CaptureInfo());
        rpc_capture_info->set_allocated_position(
            translatePosition(capture_info.position).release());
        rpc_capture_info->set_allocated_quaternion(
            translateQuaternion(capture_info.quaternion).release());
        rpc_capture_info->set_time_utc_us(capture_info.time_utc_us);
        rpc_capture_info->set_is_success(capture_info.success);
        rpc_capture_info->set_index(capture_info.index);
        rpc_capture_info->set_file_url(capture_info.file_url);

        return rpc_capture_info;
    }

    static std::unique_ptr<rpc::camera::Position>
    translatePosition(const dronecode_sdk::Camera::CaptureInfo::Position &position)
    {
        auto rpc_position = std::unique_ptr<rpc::camera::Position>(new rpc::camera::Position());
        rpc_position->set_latitude_deg(position.latitude_deg);
        rpc_position->set_longitude_deg(position.longitude_deg);
        rpc_position->set_absolute_altitude_m(position.absolute_altitude_m);
        rpc_position->set_relative_altitude_m(position.relative_altitude_m);

        return rpc_position;
    }

    static std::unique_ptr<rpc::camera::Quaternion>
    translateQuaternion(const dronecode_sdk::Camera::CaptureInfo::Quaternion &quaternion)
    {
        auto rpc_quaternion =
            std::unique_ptr<rpc::camera::Quaternion>(new rpc::camera::Quaternion());
        rpc_quaternion->set_w(quaternion.w);
        rpc_quaternion->set_x(quaternion.x);
        rpc_quaternion->set_y(quaternion.y);
        rpc_quaternion->set_z(quaternion.z);

        return rpc_quaternion;
    }

    static dronecode_sdk::Camera::CaptureInfo
    translateRPCCaptureInfo(const rpc::camera::CaptureInfo &rpc_capture_info)
    {
        dronecode_sdk::Camera::CaptureInfo capture_info;
        capture_info.position = translateRPCPosition(rpc_capture_info.position());
        capture_info.quaternion = translateRPCQuaternion(rpc_capture_info.quaternion());
        capture_info.time_utc_us = rpc_capture_info.time_utc_us();
        capture_info.success = rpc_capture_info.is_success();
        capture_info.index = rpc_capture_info.index();
        capture_info.file_url = rpc_capture_info.file_url();

        return capture_info;
    }

    static dronecode_sdk::Camera::CaptureInfo::Position
    translateRPCPosition(const rpc::camera::Position &rpc_position)
    {
        dronecode_sdk::Camera::CaptureInfo::Position position;
        position.latitude_deg = rpc_position.latitude_deg();
        position.longitude_deg = rpc_position.longitude_deg();
        position.absolute_altitude_m = rpc_position.absolute_altitude_m();
        position.relative_altitude_m = rpc_position.relative_altitude_m();

        return position;
    }

    static dronecode_sdk::Camera::CaptureInfo::Quaternion
    translateRPCQuaternion(const rpc::camera::Quaternion &rpc_quaternion)
    {
        dronecode_sdk::Camera::CaptureInfo::Quaternion quaternion;
        quaternion.w = rpc_quaternion.w();
        quaternion.x = rpc_quaternion.x();
        quaternion.y = rpc_quaternion.y();
        quaternion.z = rpc_quaternion.z();

        return quaternion;
    }

    grpc::Status
    SubscribeCameraStatus(grpc::ServerContext * /* context */,
                          const rpc::camera::SubscribeCameraStatusRequest * /* request */,
                          grpc::ServerWriter<rpc::camera::CameraStatusResponse> *writer) override
    {
        std::promise<void> stream_closed_promise;
        auto stream_closed_future = stream_closed_promise.get_future();

        bool is_finished = false;

        _camera.subscribe_status([&writer, &stream_closed_promise, &is_finished](
                                     const dronecode_sdk::Camera::Status camera_status) {
            rpc::camera::CameraStatusResponse rpc_camera_status_response;
            auto rpc_camera_status = translateCameraStatus(camera_status);
            rpc_camera_status_response.set_allocated_camera_status(rpc_camera_status.release());

            if (!writer->Write(rpc_camera_status_response) && !is_finished) {
                is_finished = true;
                stream_closed_promise.set_value();
            }
        });

        stream_closed_future.wait();

        return grpc::Status::OK;
    }

    static std::unique_ptr<rpc::camera::CameraStatus>
    translateCameraStatus(const dronecode_sdk::Camera::Status &camera_status)
    {
        auto status = std::unique_ptr<rpc::camera::CameraStatus>(new rpc::camera::CameraStatus());
        status->set_video_on(camera_status.video_on);
        status->set_photo_interval_on(camera_status.photo_interval_on);
        status->set_storage_status(translateStorageStatus(camera_status.storage_status));
        status->set_used_storage_mib(camera_status.used_storage_mib);
        status->set_available_storage_mib(camera_status.available_storage_mib);
        status->set_total_storage_mib(camera_status.total_storage_mib);

        return status;
    }

    static rpc::camera::CameraStatus_StorageStatus
    translateStorageStatus(const dronecode_sdk::Camera::Status::StorageStatus storage_status)
    {
        switch (storage_status) {
            case dronecode_sdk::Camera::Status::StorageStatus::UNFORMATTED:
                return rpc::camera::CameraStatus_StorageStatus_UNFORMATTED;
            case dronecode_sdk::Camera::Status::StorageStatus::FORMATTED:
                return rpc::camera::CameraStatus_StorageStatus_FORMATTED;
            case dronecode_sdk::Camera::Status::StorageStatus::NOT_AVAILABLE:
            default:
                return rpc::camera::CameraStatus_StorageStatus_NOT_AVAILABLE;
        }
    }

    static dronecode_sdk::Camera::Status
    translateRPCCameraStatus(const rpc::camera::CameraStatus &rpc_camera_status)
    {
        dronecode_sdk::Camera::Status status;
        status.video_on = rpc_camera_status.video_on();
        status.photo_interval_on = rpc_camera_status.photo_interval_on();
        status.storage_status = translateRPCStorageStatus(rpc_camera_status.storage_status());
        status.used_storage_mib = rpc_camera_status.used_storage_mib();
        status.available_storage_mib = rpc_camera_status.available_storage_mib();
        status.total_storage_mib = rpc_camera_status.total_storage_mib();

        return status;
    }

    static dronecode_sdk::Camera::Status::StorageStatus
    translateRPCStorageStatus(const rpc::camera::CameraStatus_StorageStatus storage_status)
    {
        switch (storage_status) {
            case rpc::camera::CameraStatus_StorageStatus_UNFORMATTED:
                return dronecode_sdk::Camera::Status::StorageStatus::UNFORMATTED;
            case rpc::camera::CameraStatus_StorageStatus_FORMATTED:
                return dronecode_sdk::Camera::Status::StorageStatus::FORMATTED;
            case rpc::camera::CameraStatus_StorageStatus_NOT_AVAILABLE:
            default:
                return dronecode_sdk::Camera::Status::StorageStatus::NOT_AVAILABLE;
        }
    }

private:
    Camera &_camera;
};

} // namespace backend
} // namespace dronecode_sdk
