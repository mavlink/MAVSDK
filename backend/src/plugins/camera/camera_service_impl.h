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

    static dronecore::Camera::Mode translateRPCCameraMode(const rpc::camera::CameraMode mode)
    {
        switch (mode) {
            case rpc::camera::CameraMode::PHOTO:
                return dronecore::Camera::Mode::PHOTO;
            case rpc::camera::CameraMode::VIDEO:
                return dronecore::Camera::Mode::VIDEO;
            default:
                return dronecore::Camera::Mode::UNKNOWN;
        }
    }

    static rpc::camera::CameraMode translateCameraMode(const dronecore::Camera::Mode mode)
    {
        switch (mode) {
            case dronecore::Camera::Mode::PHOTO:
                return rpc::camera::CameraMode::PHOTO;
            case dronecore::Camera::Mode::VIDEO:
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
            [&writer, &stream_closed_promise, &is_finished](const dronecore::Camera::Mode mode) {
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

private:
    Camera &_camera;
};

} // namespace backend
} // namespace dronecode_sdk
