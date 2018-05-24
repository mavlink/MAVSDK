#include "camera/camera.h"
#include "camera/camera.grpc.pb.h"

namespace dronecore {
namespace backend {

template <typename Camera = Camera>
class CameraServiceImpl final : public rpc::camera::CameraService::Service
{
public:
    CameraServiceImpl(Camera &camera)
        : _camera(camera) {}

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

    template <typename ResponseType>
    void fillResponseWithResult(ResponseType *response, dronecore::Camera::Result camera_result) const
    {
        auto rpc_result = static_cast<rpc::camera::CameraResult::Result>(camera_result);

        auto *rpc_camera_result = new rpc::camera::CameraResult();
        rpc_camera_result->set_result(rpc_result);
        rpc_camera_result->set_result_str(dronecore::Camera::result_str(camera_result));

        response->set_allocated_camera_result(rpc_camera_result);
    }

    grpc::Status StartPhotoInterval(grpc::ServerContext * /* context */,
                                    const rpc::camera::StartPhotoIntervalRequest *request,
                                    rpc::camera::StartPhotoIntervalResponse *response) override
    {
        if (request == nullptr) {
            if (response != nullptr) {
                fillResponseWithResult(response, dronecore::Camera::Result::WRONG_ARGUMENT);
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

private:
    Camera &_camera;
};

} // namespace backend
} // namespace dronecore
