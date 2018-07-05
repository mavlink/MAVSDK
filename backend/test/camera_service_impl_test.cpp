#include <gmock/gmock.h>

#include <future>
#include <grpc++/grpc++.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <memory>

#include "camera/camera_service_impl.h"
#include "camera/mocks/camera_mock.h"

namespace {

using testing::_;
using testing::NiceMock;
using testing::Return;

using MockCamera = NiceMock<dronecode_sdk::testing::MockCamera>;
using CameraServiceImpl = dronecode_sdk::backend::CameraServiceImpl<MockCamera>;
using CameraService = dronecode_sdk::rpc::camera::CameraService;

using CameraResult = dronecode_sdk::rpc::camera::CameraResult;
using InputPair = std::pair<std::string, dronecode_sdk::Camera::Result>;

static constexpr auto ARBITRARY_FLOAT = 24.2f;
static constexpr auto ARBITRARY_RPC_CAMERA_MODE = dronecore::rpc::camera::CameraMode::PHOTO;
static constexpr auto ARBITRARY_CAMERA_MODE = dronecore::Camera::Mode::VIDEO;
static constexpr auto ARBITRARY_FRAME_RATE = 24.0f;
static constexpr auto ARBITRARY_RESOLUTION = 1280;
static constexpr auto ARBITRARY_BIT_RATE = 1492;
static constexpr auto ARBITRARY_ROTATION = 24;
static constexpr auto ARBITRARY_URI = "rtsp://blah:1337";
static constexpr auto ARBITRARY_VIDEO_STREAM_STATUS =
    dronecore::rpc::camera::VideoStreamInfo_VideoStreamStatus_IN_PROGRESS;
static constexpr auto ARBITRARY_INT = 123456;
static constexpr auto ARBITRARY_BOOL = true;
static constexpr auto ARBITRARY_CAMERA_STORAGE_STATUS =
    dronecore::Camera::Status::StorageStatus::FORMATTED;

std::vector<InputPair> generateInputPairs();

class CameraServiceImplTest : public ::testing::TestWithParam<InputPair> {
protected:
    CameraServiceImplTest() : _camera_service(_camera)
    {
        _callback_saved_future = _callback_saved_promise.get_future();
    }

    virtual void SetUp()
    {
        grpc::ServerBuilder builder;
        builder.RegisterService(&_camera_service);
        _server = builder.BuildAndStart();

        grpc::ChannelArguments channel_args;
        auto channel = _server->InProcessChannel(channel_args);
        _stub = CameraService::NewStub(channel);
    }

    void setCameraMode(const dronecore::rpc::camera::CameraMode mode);

    std::future<void> subscribeModeAsync(std::vector<dronecore::Camera::Mode> &camera_modes,
                                         std::shared_ptr<grpc::ClientContext> context =
                                             std::make_shared<grpc::ClientContext>()) const;
    void checkSendsModes(const std::vector<dronecore::Camera::Mode> &modes) const;
    std::unique_ptr<dronecore::rpc::camera::VideoStreamSettings>
    createArbitraryRPCVideoStreamSettings() const;

    std::future<void>
    subscribeVideoStreamInfoAsync(std::vector<dronecore::Camera::VideoStreamInfo> &video_info,
                                  std::shared_ptr<grpc::ClientContext> context) const;
    std::unique_ptr<dronecore::rpc::camera::VideoStreamInfo>
    createArbitraryRPCVideoStreamInfo() const;
    void checkSendsVideoStreamInfo(
        const std::vector<dronecore::Camera::VideoStreamInfo> &video_info_events) const;
    dronecore::Camera::VideoStreamInfo
    createVideoStreamInfo(const dronecore::Camera::VideoStreamSettings settings,
                          const dronecore::Camera::VideoStreamInfo::Status status) const;

    std::future<void>
    subscribeCaptureInfoAsync(std::vector<dronecore::Camera::CaptureInfo> &capture_info_events,
                              std::shared_ptr<grpc::ClientContext> context) const;
    std::unique_ptr<dronecore::rpc::camera::CaptureInfo> createArbitraryRPCCaptureInfo() const;
    std::unique_ptr<dronecore::rpc::camera::Position> createRPCPosition(const double lat,
                                                                        const double lng,
                                                                        const float abs_alt,
                                                                        const float rel_alt) const;
    std::unique_ptr<dronecore::rpc::camera::Quaternion>
    createRPCQuaternion(const float w, const float x, const float y, const float z) const;
    void checkSendsCaptureInfo(
        const std::vector<dronecore::Camera::CaptureInfo> &capture_info_events) const;
    dronecore::Camera::CaptureInfo createArbitraryCaptureInfo() const;

    dronecore::Camera::Status
    createCameraStatus(const bool is_video_on,
                       const bool is_photo_interval_on,
                       const dronecore::Camera::Status::StorageStatus storage_status,
                       const float used_storage_mib,
                       const float available_storage_mib,
                       const float total_storage_mib) const;
    std::future<void>
    subscribeCameraStatusAsync(std::vector<dronecore::Camera::Status> &camera_status_events,
                               std::shared_ptr<grpc::ClientContext> context) const;
    void checkSendsCameraStatus(
        const std::vector<dronecore::Camera::Status> &camera_status_events) const;

    MockCamera _camera;
    CameraServiceImpl _camera_service;
    std::unique_ptr<grpc::Server> _server;
    std::unique_ptr<CameraService::Stub> _stub;

    std::promise<void> _callback_saved_promise;
    std::future<void> _callback_saved_future;
};

ACTION_P2(SaveResult, callback, callback_saved_promise)
{
    *callback = arg0;
    callback_saved_promise->set_value();
}

TEST_P(CameraServiceImplTest, takePhotoResultIsTranslatedCorrectly)
{
    ON_CALL(_camera, take_photo()).WillByDefault(Return(GetParam().second));
    dronecode_sdk::rpc::camera::TakePhotoResponse response;

    _camera_service.TakePhoto(nullptr, nullptr, &response);

    EXPECT_EQ(GetParam().first, CameraResult::Result_Name(response.camera_result().result()));
}

TEST_F(CameraServiceImplTest, takesPhotoEvenWhenArgsAreNull)
{
    EXPECT_CALL(_camera, take_photo()).Times(1);

    _camera_service.TakePhoto(nullptr, nullptr, nullptr);
}

TEST_P(CameraServiceImplTest, startPhotoIntervalResultIsTranslatedCorrectly)
{
    ON_CALL(_camera, start_photo_interval(_)).WillByDefault(Return(GetParam().second));
    dronecode_sdk::rpc::camera::StartPhotoIntervalRequest request;
    request.set_interval_s(ARBITRARY_FLOAT);
    dronecode_sdk::rpc::camera::StartPhotoIntervalResponse response;

    _camera_service.StartPhotoInterval(nullptr, &request, &response);

    EXPECT_EQ(GetParam().first, CameraResult::Result_Name(response.camera_result().result()));
}

TEST_F(CameraServiceImplTest, startsPhotoIntervalEvenWhenContextAndResponseAreNull)
{
    EXPECT_CALL(_camera, start_photo_interval(_)).Times(1);
    dronecode_sdk::rpc::camera::StartPhotoIntervalRequest request;
    request.set_interval_s(ARBITRARY_FLOAT);

    _camera_service.StartPhotoInterval(nullptr, &request, nullptr);
}

TEST_F(CameraServiceImplTest, startsPhotoIntervalWithRightParameter)
{
    auto expected_interval = ARBITRARY_FLOAT;
    EXPECT_CALL(_camera, start_photo_interval(expected_interval)).Times(1);
    dronecode_sdk::rpc::camera::StartPhotoIntervalRequest request;
    request.set_interval_s(expected_interval);

    _camera_service.StartPhotoInterval(nullptr, &request, nullptr);
}

TEST_F(CameraServiceImplTest, startPhotoIntervalReturnsWrongArgumentErrorIfRequestIsNull)
{
    dronecode_sdk::rpc::camera::StartPhotoIntervalResponse response;

    _camera_service.StartPhotoInterval(nullptr, nullptr, &response);

    EXPECT_EQ("WRONG_ARGUMENT", CameraResult::Result_Name(response.camera_result().result()));
}

TEST_F(CameraServiceImplTest, startPhotoIntervalDoesNotCrashWhenArgsAreNull)
{
    _camera_service.StartPhotoInterval(nullptr, nullptr, nullptr);
}

TEST_P(CameraServiceImplTest, stopPhotoIntervalResultIsTranslatedCorrectly)
{
    ON_CALL(_camera, stop_photo_interval()).WillByDefault(Return(GetParam().second));
    dronecode_sdk::rpc::camera::StopPhotoIntervalResponse response;

    _camera_service.StopPhotoInterval(nullptr, nullptr, &response);

    EXPECT_EQ(GetParam().first, CameraResult::Result_Name(response.camera_result().result()));
}

TEST_F(CameraServiceImplTest, stopsPhotoIntervalEvenWhenArgsAreNull)
{
    EXPECT_CALL(_camera, stop_photo_interval()).Times(1);

    _camera_service.StopPhotoInterval(nullptr, nullptr, nullptr);
}

TEST_P(CameraServiceImplTest, startVideoResultIsTranslatedCorrectly)
{
    ON_CALL(_camera, start_video()).WillByDefault(Return(GetParam().second));
    dronecode_sdk::rpc::camera::StartVideoResponse response;

    _camera_service.StartVideo(nullptr, nullptr, &response);

    EXPECT_EQ(GetParam().first, CameraResult::Result_Name(response.camera_result().result()));
}

TEST_F(CameraServiceImplTest, startsVideoEvenWhenArgsAreNull)
{
    EXPECT_CALL(_camera, start_video()).Times(1);

    _camera_service.StartVideo(nullptr, nullptr, nullptr);
}

TEST_P(CameraServiceImplTest, stopVideoResultIsTranslatedCorrectly)
{
    ON_CALL(_camera, stop_video()).WillByDefault(Return(GetParam().second));
    dronecode_sdk::rpc::camera::StopVideoResponse response;

    _camera_service.StopVideo(nullptr, nullptr, &response);

    EXPECT_EQ(GetParam().first, CameraResult::Result_Name(response.camera_result().result()));
}

TEST_F(CameraServiceImplTest, stopVideoEvenWhenArgsAreNull)
{
    EXPECT_CALL(_camera, stop_video()).Times(1);

    _camera_service.StopVideo(nullptr, nullptr, nullptr);
}

TEST_P(CameraServiceImplTest, startVideoStreamingResultIsTranslatedCorrectly)
{
    ON_CALL(_camera, start_video_streaming()).WillByDefault(Return(GetParam().second));
    dronecode_sdk::rpc::camera::StartVideoStreamingResponse response;

    _camera_service.StartVideoStreaming(nullptr, nullptr, &response);

    EXPECT_EQ(GetParam().first, CameraResult::Result_Name(response.camera_result().result()));
}

TEST_F(CameraServiceImplTest, startsVideoStreamingEvenWhenArgsAreNull)
{
    EXPECT_CALL(_camera, start_video_streaming()).Times(1);

    _camera_service.StartVideoStreaming(nullptr, nullptr, nullptr);
}

TEST_P(CameraServiceImplTest, stopVideoStreamingResultIsTranslatedCorrectly)
{
    ON_CALL(_camera, stop_video_streaming()).WillByDefault(Return(GetParam().second));
    dronecode_sdk::rpc::camera::StopVideoStreamingResponse response;

    _camera_service.StopVideoStreaming(nullptr, nullptr, &response);

    EXPECT_EQ(GetParam().first, CameraResult::Result_Name(response.camera_result().result()));
}

TEST_F(CameraServiceImplTest, stopsVideoStreamingEvenWhenArgsAreNull)
{
    EXPECT_CALL(_camera, stop_video_streaming()).Times(1);

    _camera_service.StopVideoStreaming(nullptr, nullptr, nullptr);
}

TEST_F(CameraServiceImplTest, setModeDoesNotFailWithAllNullParams)
{
    _camera_service.SetMode(nullptr, nullptr, nullptr);
}

TEST_F(CameraServiceImplTest, setModeDoesNotFailWithNullResponse)
{
    dronecore::rpc::camera::SetModeRequest request;
    request.set_camera_mode(ARBITRARY_RPC_CAMERA_MODE);

    _camera_service.SetMode(nullptr, &request, nullptr);
}

TEST_P(CameraServiceImplTest, setModeResultIsTranslatedCorrectly)
{
    ON_CALL(_camera, set_mode(_)).WillByDefault(Return(GetParam().second));
    dronecore::rpc::camera::SetModeRequest request;
    request.set_camera_mode(ARBITRARY_RPC_CAMERA_MODE);
    dronecore::rpc::camera::SetModeResponse response;

    _camera_service.SetMode(nullptr, &request, &response);

    EXPECT_EQ(GetParam().first, CameraResult::Result_Name(response.camera_result().result()));
}

TEST_F(CameraServiceImplTest, setModeSetsPhotoMode)
{
    const auto expected_mode = dronecore::Camera::Mode::PHOTO;
    EXPECT_CALL(_camera, set_mode(expected_mode)).Times(1);

    setCameraMode(dronecore::rpc::camera::CameraMode::PHOTO);
}

void CameraServiceImplTest::setCameraMode(const dronecore::rpc::camera::CameraMode mode)
{
    dronecore::rpc::camera::SetModeRequest request;
    request.set_camera_mode(mode);
    dronecore::rpc::camera::SetModeResponse response;

    _camera_service.SetMode(nullptr, &request, &response);
}

TEST_F(CameraServiceImplTest, setModeSetsVideoMode)
{
    const auto expected_mode = dronecore::Camera::Mode::VIDEO;
    EXPECT_CALL(_camera, set_mode(expected_mode)).Times(1);

    setCameraMode(dronecore::rpc::camera::CameraMode::VIDEO);
}

TEST_F(CameraServiceImplTest, registersToCameraMode)
{
    dronecore::Camera::subscribe_mode_callback_t mode_callback;
    EXPECT_CALL(_camera, subscribe_mode(_))
        .WillOnce(SaveResult(&mode_callback, &_callback_saved_promise));
    std::vector<dronecore::Camera::Mode> mode_events;
    auto context = std::make_shared<grpc::ClientContext>();

    auto mode_events_future = subscribeModeAsync(mode_events, context);
    _callback_saved_future.wait();
    context->TryCancel();
    mode_callback(ARBITRARY_CAMERA_MODE);
    mode_events_future.wait();
}

std::future<void>
CameraServiceImplTest::subscribeModeAsync(std::vector<dronecore::Camera::Mode> &camera_modes,
                                          std::shared_ptr<grpc::ClientContext> context) const
{
    return std::async(std::launch::async, [&]() {
        dronecore::rpc::camera::SubscribeModeRequest request;
        auto response_reader = _stub->SubscribeMode(context.get(), request);

        dronecore::rpc::camera::ModeResponse response;
        while (response_reader->Read(&response)) {
            camera_modes.push_back(
                CameraServiceImpl::translateRPCCameraMode(response.camera_mode()));
        }

        response_reader->Finish();
    });
}

TEST_F(CameraServiceImplTest, doesNotSendModeIfCallbackNotCalled)
{
    std::vector<dronecore::Camera::Mode> mode_events;
    auto context = std::make_shared<grpc::ClientContext>();
    auto mode_events_future = subscribeModeAsync(mode_events, context);

    context->TryCancel();
    mode_events_future.wait();

    EXPECT_EQ(0, mode_events.size());
}

TEST_F(CameraServiceImplTest, sendsOneMode)
{
    std::vector<dronecore::Camera::Mode> modes;
    modes.push_back(ARBITRARY_CAMERA_MODE);

    checkSendsModes(modes);
}

void CameraServiceImplTest::checkSendsModes(const std::vector<dronecore::Camera::Mode> &modes) const
{
    std::promise<void> subscription_promise;
    auto subscription_future = subscription_promise.get_future();
    dronecore::Camera::subscribe_mode_callback_t mode_callback;
    auto context = std::make_shared<grpc::ClientContext>();
    EXPECT_CALL(_camera, subscribe_mode(_))
        .WillOnce(SaveResult(&mode_callback, &subscription_promise));

    std::vector<dronecore::Camera::Mode> received_modes;
    auto mode_events_future = subscribeModeAsync(received_modes, context);
    subscription_future.wait();
    for (const auto mode : modes) {
        mode_callback(mode);
    }
    context->TryCancel();
    mode_callback(ARBITRARY_CAMERA_MODE);
    mode_events_future.wait();

    ASSERT_EQ(modes.size(), received_modes.size());
    for (size_t i = 0; i < modes.size(); i++) {
        EXPECT_EQ(modes.at(i), received_modes.at(i));
    }
}

TEST_F(CameraServiceImplTest, sendsMultipleModes)
{
    std::vector<dronecore::Camera::Mode> modes;
    modes.push_back(ARBITRARY_CAMERA_MODE);
    modes.push_back(ARBITRARY_CAMERA_MODE);
    modes.push_back(ARBITRARY_CAMERA_MODE);
    modes.push_back(dronecore::Camera::Mode::PHOTO);
    modes.push_back(dronecore::Camera::Mode::VIDEO);
    modes.push_back(dronecore::Camera::Mode::UNKNOWN);

    checkSendsModes(modes);
}

TEST_F(CameraServiceImplTest, setVideoStreamSettingsDoesNotFailWithAllNullParams)
{
    _camera_service.SetVideoStreamSettings(nullptr, nullptr, nullptr);
}

TEST_F(CameraServiceImplTest, setVideoStreamSettingsDoesNotFailWithNullResponse)
{
    dronecore::rpc::camera::SetVideoStreamSettingsRequest request;

    auto rpc_settings = createArbitraryRPCVideoStreamSettings();
    request.set_allocated_video_stream_settings(rpc_settings.release());

    _camera_service.SetVideoStreamSettings(nullptr, &request, nullptr);
}

std::unique_ptr<dronecore::rpc::camera::VideoStreamSettings>
CameraServiceImplTest::createArbitraryRPCVideoStreamSettings() const
{
    auto rpc_settings = std::unique_ptr<dronecore::rpc::camera::VideoStreamSettings>(
        new dronecore::rpc::camera::VideoStreamSettings());
    rpc_settings->set_frame_rate_hz(ARBITRARY_FRAME_RATE);
    rpc_settings->set_horizontal_resolution_pix(ARBITRARY_RESOLUTION);
    rpc_settings->set_vertical_resolution_pix(ARBITRARY_RESOLUTION);
    rpc_settings->set_bit_rate_b_s(ARBITRARY_BIT_RATE);
    rpc_settings->set_rotation_deg(ARBITRARY_ROTATION);
    rpc_settings->set_uri(ARBITRARY_URI);

    return rpc_settings;
}

TEST_F(CameraServiceImplTest, setsVideoStreamSettingsCorrectly)
{
    auto rpc_video_stream_settings = createArbitraryRPCVideoStreamSettings();
    const auto expected_video_stream_settings =
        CameraServiceImpl::translateRPCVideoStreamSettings(*rpc_video_stream_settings);
    EXPECT_CALL(_camera, set_video_stream_settings(expected_video_stream_settings)).Times(1);
    dronecore::rpc::camera::SetVideoStreamSettingsRequest request;
    request.set_allocated_video_stream_settings(rpc_video_stream_settings.release());

    _camera_service.SetVideoStreamSettings(nullptr, &request, nullptr);
}

TEST_F(CameraServiceImplTest, registersToVideoStreamInfo)
{
    auto rpc_video_stream_info = createArbitraryRPCVideoStreamInfo();
    const auto expected_video_stream_info =
        CameraServiceImpl::translateRPCVideoStreamInfo(*rpc_video_stream_info);
    dronecore::Camera::subscribe_video_stream_info_callback_t video_info_callback;
    EXPECT_CALL(_camera, subscribe_video_stream_info(_))
        .WillOnce(SaveResult(&video_info_callback, &_callback_saved_promise));
    std::vector<dronecore::Camera::VideoStreamInfo> video_info_events;
    auto context = std::make_shared<grpc::ClientContext>();

    auto mode_events_future = subscribeVideoStreamInfoAsync(video_info_events, context);
    _callback_saved_future.wait();
    context->TryCancel();
    video_info_callback(expected_video_stream_info);
    mode_events_future.wait();
}

std::future<void> CameraServiceImplTest::subscribeVideoStreamInfoAsync(
    std::vector<dronecore::Camera::VideoStreamInfo> &video_info_events,
    std::shared_ptr<grpc::ClientContext> context) const
{
    return std::async(std::launch::async, [&]() {
        dronecore::rpc::camera::SubscribeVideoStreamInfoRequest request;
        auto response_reader = _stub->SubscribeVideoStreamInfo(context.get(), request);

        dronecore::rpc::camera::VideoStreamInfoResponse response;
        while (response_reader->Read(&response)) {
            video_info_events.push_back(
                CameraServiceImpl::translateRPCVideoStreamInfo(response.video_stream_info()));
        }

        response_reader->Finish();
    });
}

std::unique_ptr<dronecore::rpc::camera::VideoStreamInfo>
CameraServiceImplTest::createArbitraryRPCVideoStreamInfo() const
{
    auto rpc_info = std::unique_ptr<dronecore::rpc::camera::VideoStreamInfo>(
        new dronecore::rpc::camera::VideoStreamInfo());
    rpc_info->set_video_stream_status(ARBITRARY_VIDEO_STREAM_STATUS);
    rpc_info->set_allocated_video_stream_settings(
        createArbitraryRPCVideoStreamSettings().release());

    return rpc_info;
}

TEST_F(CameraServiceImplTest, doesNotSendVideoStreamInfoIfCallbackNotCalled)
{
    std::vector<dronecore::Camera::VideoStreamInfo> video_info_events;
    auto context = std::make_shared<grpc::ClientContext>();
    auto video_info_events_future = subscribeVideoStreamInfoAsync(video_info_events, context);

    context->TryCancel();
    video_info_events_future.wait();

    EXPECT_EQ(0, video_info_events.size());
}

TEST_F(CameraServiceImplTest, sendsOneVideoStreamInfo)
{
    std::vector<dronecore::Camera::VideoStreamInfo> video_info_events;
    auto video_info_event = createArbitraryRPCVideoStreamInfo();
    video_info_events.push_back(CameraServiceImpl::translateRPCVideoStreamInfo(*video_info_event));

    checkSendsVideoStreamInfo(video_info_events);
}

void CameraServiceImplTest::checkSendsVideoStreamInfo(
    const std::vector<dronecore::Camera::VideoStreamInfo> &video_info_events) const
{
    std::promise<void> subscription_promise;
    auto subscription_future = subscription_promise.get_future();
    dronecore::Camera::subscribe_video_stream_info_callback_t video_info_callback;
    auto context = std::make_shared<grpc::ClientContext>();
    EXPECT_CALL(_camera, subscribe_video_stream_info(_))
        .WillOnce(SaveResult(&video_info_callback, &subscription_promise));

    std::vector<dronecore::Camera::VideoStreamInfo> received_video_info_events;
    auto video_info_events_future =
        subscribeVideoStreamInfoAsync(received_video_info_events, context);
    subscription_future.wait();
    for (const auto video_info_event : video_info_events) {
        video_info_callback(video_info_event);
    }
    context->TryCancel();
    auto arbitrary_video_info_event = createArbitraryRPCVideoStreamInfo();
    video_info_callback(
        CameraServiceImpl::translateRPCVideoStreamInfo(*arbitrary_video_info_event));
    video_info_events_future.wait();

    ASSERT_EQ(video_info_events.size(), received_video_info_events.size());
    for (size_t i = 0; i < video_info_events.size(); i++) {
        EXPECT_EQ(video_info_events.at(i), received_video_info_events.at(i));
    }
}

TEST_F(CameraServiceImplTest, sendsMultipleVideoStreamInfos)
{
    std::vector<dronecore::Camera::VideoStreamInfo> video_info_events;

    dronecore::Camera::VideoStreamSettings settings1;
    settings1.set_highest();
    video_info_events.push_back(
        createVideoStreamInfo(settings1, dronecore::Camera::VideoStreamInfo::Status::NOT_RUNNING));

    dronecore::Camera::VideoStreamSettings settings2;
    video_info_events.push_back(
        createVideoStreamInfo(settings2, dronecore::Camera::VideoStreamInfo::Status::IN_PROGRESS));

    checkSendsVideoStreamInfo(video_info_events);
}

dronecore::Camera::VideoStreamInfo CameraServiceImplTest::createVideoStreamInfo(
    const dronecore::Camera::VideoStreamSettings settings,
    const dronecore::Camera::VideoStreamInfo::Status status) const
{
    dronecore::Camera::VideoStreamInfo video_stream_info;
    video_stream_info.settings = settings;
    video_stream_info.status = status;

    return video_stream_info;
}

TEST_F(CameraServiceImplTest, registersToCaptureInfo)
{
    auto rpc_capture_info = createArbitraryRPCCaptureInfo();
    const auto expected_capture_info =
        CameraServiceImpl::translateRPCCaptureInfo(*rpc_capture_info);
    dronecore::Camera::capture_info_callback_t capture_info_callback;
    EXPECT_CALL(_camera, subscribe_capture_info(_))
        .WillOnce(SaveResult(&capture_info_callback, &_callback_saved_promise));
    std::vector<dronecore::Camera::CaptureInfo> capture_info_events;
    auto context = std::make_shared<grpc::ClientContext>();

    auto mode_events_future = subscribeCaptureInfoAsync(capture_info_events, context);
    _callback_saved_future.wait();
    context->TryCancel();
    capture_info_callback(expected_capture_info);
    mode_events_future.wait();
}

std::future<void> CameraServiceImplTest::subscribeCaptureInfoAsync(
    std::vector<dronecore::Camera::CaptureInfo> &capture_info_events,
    std::shared_ptr<grpc::ClientContext> context) const
{
    return std::async(std::launch::async, [&]() {
        dronecore::rpc::camera::SubscribeCaptureInfoRequest request;
        auto response_reader = _stub->SubscribeCaptureInfo(context.get(), request);

        dronecore::rpc::camera::CaptureInfoResponse response;
        while (response_reader->Read(&response)) {
            capture_info_events.push_back(
                CameraServiceImpl::translateRPCCaptureInfo(response.capture_info()));
        }

        response_reader->Finish();
    });
}

std::unique_ptr<dronecore::rpc::camera::CaptureInfo>
CameraServiceImplTest::createArbitraryRPCCaptureInfo() const
{
    auto rpc_info = std::unique_ptr<dronecore::rpc::camera::CaptureInfo>(
        new dronecore::rpc::camera::CaptureInfo());
    rpc_info->set_allocated_position(
        createRPCPosition(41.848695, 75.132751, 3002.1f, 50.3f).release());
    rpc_info->set_allocated_quaternion(createRPCQuaternion(0.1f, 0.2f, 0.3f, 0.4f).release());
    rpc_info->set_time_utc_us(ARBITRARY_INT);
    rpc_info->set_is_success(ARBITRARY_BOOL);
    rpc_info->set_index(ARBITRARY_INT);
    rpc_info->set_file_url(ARBITRARY_URI);

    return rpc_info;
}

std::unique_ptr<dronecore::rpc::camera::Position> CameraServiceImplTest::createRPCPosition(
    const double lat, const double lng, const float abs_alt, const float rel_alt) const
{
    auto expected_position =
        std::unique_ptr<dronecore::rpc::camera::Position>(new dronecore::rpc::camera::Position());

    expected_position->set_latitude_deg(lat);
    expected_position->set_longitude_deg(lng);
    expected_position->set_absolute_altitude_m(abs_alt);
    expected_position->set_relative_altitude_m(rel_alt);

    return expected_position;
}

std::unique_ptr<dronecore::rpc::camera::Quaternion> CameraServiceImplTest::createRPCQuaternion(
    const float w, const float x, const float y, const float z) const
{
    auto quaternion = std::unique_ptr<dronecore::rpc::camera::Quaternion>(
        new dronecore::rpc::camera::Quaternion());

    quaternion->set_w(w);
    quaternion->set_x(x);
    quaternion->set_y(y);
    quaternion->set_z(z);

    return quaternion;
}

TEST_F(CameraServiceImplTest, doesNotSendCaptureInfoIfCallbackNotCalled)
{
    std::vector<dronecore::Camera::CaptureInfo> camera_info_events;
    auto context = std::make_shared<grpc::ClientContext>();
    auto camera_info_events_future = subscribeCaptureInfoAsync(camera_info_events, context);

    context->TryCancel();
    camera_info_events_future.wait();

    EXPECT_EQ(0, camera_info_events.size());
}

TEST_F(CameraServiceImplTest, sendsOneCaptureInfo)
{
    std::vector<dronecore::Camera::CaptureInfo> capture_info_events;
    auto capture_info_event = createArbitraryRPCCaptureInfo();
    capture_info_events.push_back(CameraServiceImpl::translateRPCCaptureInfo(*capture_info_event));

    checkSendsCaptureInfo(capture_info_events);
}

void CameraServiceImplTest::checkSendsCaptureInfo(
    const std::vector<dronecore::Camera::CaptureInfo> &capture_info_events) const
{
    std::promise<void> subscription_promise;
    auto subscription_future = subscription_promise.get_future();
    dronecore::Camera::capture_info_callback_t capture_info_callback;
    auto context = std::make_shared<grpc::ClientContext>();
    EXPECT_CALL(_camera, subscribe_capture_info(_))
        .WillOnce(SaveResult(&capture_info_callback, &subscription_promise));

    std::vector<dronecore::Camera::CaptureInfo> received_capture_info_events;
    auto capture_info_events_future =
        subscribeCaptureInfoAsync(received_capture_info_events, context);
    subscription_future.wait();
    for (const auto capture_info_event : capture_info_events) {
        capture_info_callback(capture_info_event);
    }
    context->TryCancel();
    auto arbitrary_capture_info_event = createArbitraryRPCCaptureInfo();
    capture_info_callback(
        CameraServiceImpl::translateRPCCaptureInfo(*arbitrary_capture_info_event));
    capture_info_events_future.wait();

    ASSERT_EQ(capture_info_events.size(), received_capture_info_events.size());
    for (size_t i = 0; i < capture_info_events.size(); i++) {
        EXPECT_EQ(capture_info_events.at(i), received_capture_info_events.at(i));
    }
}

TEST_F(CameraServiceImplTest, sendsMultipleCaptureInfos)
{
    std::vector<dronecore::Camera::CaptureInfo> capture_info_events;

    capture_info_events.push_back(createArbitraryCaptureInfo());
    capture_info_events.push_back(createArbitraryCaptureInfo());
    capture_info_events.push_back(createArbitraryCaptureInfo());
    capture_info_events.push_back(createArbitraryCaptureInfo());

    checkSendsCaptureInfo(capture_info_events);
}

dronecore::Camera::CaptureInfo CameraServiceImplTest::createArbitraryCaptureInfo() const
{
    dronecore::Camera::CaptureInfo::Position position;
    position.latitude_deg = 12.12223;
    position.longitude_deg = 24.342;
    position.absolute_altitude_m = 2334.2f;
    position.relative_altitude_m = 54.12f;

    dronecore::Camera::CaptureInfo::Quaternion quaternion;
    quaternion.w = 12.3f;
    quaternion.x = 0.3f;
    quaternion.y = 1.1f;
    quaternion.z = -4.2f;

    dronecore::Camera::CaptureInfo capture_info;
    capture_info.position = position;
    capture_info.quaternion = quaternion;
    capture_info.time_utc_us = ARBITRARY_INT;
    capture_info.success = ARBITRARY_BOOL;
    capture_info.index = ARBITRARY_INT;
    capture_info.file_url = ARBITRARY_URI;

    return capture_info;
}

TEST_F(CameraServiceImplTest, registersToCameraStatus)
{
    const auto expected_camera_status = createCameraStatus(
        false, true, dronecore::Camera::Status::StorageStatus::FORMATTED, 3.4f, 12.6f, 16.0f);
    dronecore::Camera::subscribe_status_callback_t status_callback;
    EXPECT_CALL(_camera, subscribe_status(_))
        .WillOnce(SaveResult(&status_callback, &_callback_saved_promise));
    std::vector<dronecore::Camera::Status> camera_status_events;
    auto context = std::make_shared<grpc::ClientContext>();

    auto mode_events_future = subscribeCameraStatusAsync(camera_status_events, context);
    _callback_saved_future.wait();
    context->TryCancel();
    status_callback(expected_camera_status);
    mode_events_future.wait();
}

dronecore::Camera::Status CameraServiceImplTest::createCameraStatus(
    const bool is_video_on,
    const bool is_photo_interval_on,
    const dronecore::Camera::Status::StorageStatus storage_status,
    const float used_storage_mib,
    const float available_storage_mib,
    const float total_storage_mib) const
{
    dronecore::Camera::Status status;
    status.video_on = is_video_on;
    status.photo_interval_on = is_photo_interval_on;
    status.storage_status = storage_status;
    status.used_storage_mib = used_storage_mib;
    status.available_storage_mib = available_storage_mib;
    status.total_storage_mib = total_storage_mib;

    return status;
}

std::future<void> CameraServiceImplTest::subscribeCameraStatusAsync(
    std::vector<dronecore::Camera::Status> &camera_status_events,
    std::shared_ptr<grpc::ClientContext> context) const
{
    return std::async(std::launch::async, [&]() {
        dronecore::rpc::camera::SubscribeCameraStatusRequest request;
        auto response_reader = _stub->SubscribeCameraStatus(context.get(), request);

        dronecore::rpc::camera::CameraStatusResponse response;
        while (response_reader->Read(&response)) {
            camera_status_events.push_back(
                CameraServiceImpl::translateRPCCameraStatus(response.camera_status()));
        }

        response_reader->Finish();
    });
}

TEST_F(CameraServiceImplTest, doesNotSendCameraStatusIfCallbackNotCalled)
{
    std::vector<dronecore::Camera::Status> camera_status_events;
    auto context = std::make_shared<grpc::ClientContext>();
    auto camera_status_events_future = subscribeCameraStatusAsync(camera_status_events, context);

    context->TryCancel();
    camera_status_events_future.wait();

    EXPECT_EQ(0, camera_status_events.size());
}

TEST_F(CameraServiceImplTest, sendsOneCameraStatus)
{
    std::vector<dronecore::Camera::Status> camera_status_events;
    auto camera_status_event = createCameraStatus(
        false, true, dronecore::Camera::Status::StorageStatus::FORMATTED, 3.4f, 12.6f, 16.0f);
    camera_status_events.push_back(camera_status_event);

    checkSendsCameraStatus(camera_status_events);
}

void CameraServiceImplTest::checkSendsCameraStatus(
    const std::vector<dronecore::Camera::Status> &camera_status_events) const
{
    std::promise<void> subscription_promise;
    auto subscription_future = subscription_promise.get_future();
    dronecore::Camera::subscribe_status_callback_t camera_status_callback;
    auto context = std::make_shared<grpc::ClientContext>();
    EXPECT_CALL(_camera, subscribe_status(_))
        .WillOnce(SaveResult(&camera_status_callback, &subscription_promise));

    std::vector<dronecore::Camera::Status> received_camera_status_events;
    auto camera_status_events_future =
        subscribeCameraStatusAsync(received_camera_status_events, context);
    subscription_future.wait();
    for (const auto camera_status_event : camera_status_events) {
        camera_status_callback(camera_status_event);
    }
    context->TryCancel();
    auto arbitrary_camera_status_event = createCameraStatus(
        false, true, dronecore::Camera::Status::StorageStatus::FORMATTED, 3.4f, 12.6f, 16.0f);
    camera_status_callback(arbitrary_camera_status_event);
    camera_status_events_future.wait();

    ASSERT_EQ(camera_status_events.size(), received_camera_status_events.size());
    for (size_t i = 0; i < camera_status_events.size(); i++) {
        EXPECT_EQ(camera_status_events.at(i), received_camera_status_events.at(i));
    }
}

TEST_F(CameraServiceImplTest, sendsMultipleCameraStatus)
{
    std::vector<dronecore::Camera::Status> camera_status_events;

    camera_status_events.push_back(createCameraStatus(
        true, true, dronecore::Camera::Status::StorageStatus::UNFORMATTED, 1.2f, 3.4f, 2.2f));
    camera_status_events.push_back(createCameraStatus(
        true, false, dronecore::Camera::Status::StorageStatus::FORMATTED, 11.2f, 58.4f, 8.65f));
    camera_status_events.push_back(createCameraStatus(
        false, false, dronecore::Camera::Status::StorageStatus::NOT_AVAILABLE, 1.5f, 8.1f, 6.3f));

    checkSendsCameraStatus(camera_status_events);
}

INSTANTIATE_TEST_CASE_P(CameraResultCorrespondences,
                        CameraServiceImplTest,
                        ::testing::ValuesIn(generateInputPairs()));

std::vector<InputPair> generateInputPairs()
{
    std::vector<InputPair> input_pairs;
    input_pairs.push_back(std::make_pair("SUCCESS", dronecode_sdk::Camera::Result::SUCCESS));
    input_pairs.push_back(
        std::make_pair("IN_PROGRESS", dronecode_sdk::Camera::Result::IN_PROGRESS));
    input_pairs.push_back(std::make_pair("BUSY", dronecode_sdk::Camera::Result::BUSY));
    input_pairs.push_back(std::make_pair("DENIED", dronecode_sdk::Camera::Result::DENIED));
    input_pairs.push_back(std::make_pair("ERROR", dronecode_sdk::Camera::Result::ERROR));
    input_pairs.push_back(std::make_pair("TIMEOUT", dronecode_sdk::Camera::Result::TIMEOUT));
    input_pairs.push_back(
        std::make_pair("WRONG_ARGUMENT", dronecode_sdk::Camera::Result::WRONG_ARGUMENT));
    input_pairs.push_back(std::make_pair("UNKNOWN", dronecode_sdk::Camera::Result::UNKNOWN));

    return input_pairs;
}

} // namespace
