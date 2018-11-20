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
using testing::DoDefault;
using testing::NiceMock;
using testing::Return;

using MockCamera = NiceMock<dronecode_sdk::testing::MockCamera>;
using CameraServiceImpl = dronecode_sdk::backend::CameraServiceImpl<MockCamera>;
using CameraService = dronecode_sdk::rpc::camera::CameraService;

using CameraResult = dronecode_sdk::rpc::camera::CameraResult;
using InputPair = std::pair<std::string, dronecode_sdk::Camera::Result>;

static constexpr auto ARBITRARY_FLOAT = 24.2f;
static constexpr auto ARBITRARY_RPC_CAMERA_MODE = dronecode_sdk::rpc::camera::CameraMode::PHOTO;
static constexpr auto ARBITRARY_CAMERA_MODE = dronecode_sdk::Camera::Mode::VIDEO;
static constexpr auto ARBITRARY_FRAME_RATE = 24.0f;
static constexpr auto ARBITRARY_RESOLUTION = 1280;
static constexpr auto ARBITRARY_BIT_RATE = 1492;
static constexpr auto ARBITRARY_ROTATION = 24;
static constexpr auto ARBITRARY_URI = "rtsp://blah:1337";
static constexpr auto ARBITRARY_VIDEO_STREAM_STATUS =
    dronecode_sdk::rpc::camera::VideoStreamInfo_VideoStreamStatus_IN_PROGRESS;
static constexpr auto ARBITRARY_INT = 123456;
static constexpr auto ARBITRARY_BOOL = true;
static constexpr auto ARBITRARY_CAMERA_STORAGE_STATUS =
    dronecode_sdk::Camera::Status::StorageStatus::FORMATTED;
static constexpr auto ARBITRARY_SETTING_ID = "23b442";
static constexpr auto ARBITRARY_SETTING_DESCRIPTION = "Twenty-three b four hundred-forty-two";
static constexpr auto ARBITRARY_OPTION_ID = "small";
static constexpr auto ARBITRARY_OPTION_DESCRIPTION = "Bigger";
static constexpr auto ARBITRARY_CAMERA_RESULT = dronecode_sdk::Camera::Result::SUCCESS;

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

    void setCameraMode(const dronecode_sdk::rpc::camera::CameraMode mode);

    std::future<void> subscribeModeAsync(std::vector<dronecode_sdk::Camera::Mode> &camera_modes,
                                         std::shared_ptr<grpc::ClientContext> context =
                                             std::make_shared<grpc::ClientContext>()) const;
    void checkSendsModes(const std::vector<dronecode_sdk::Camera::Mode> &modes) const;
    std::unique_ptr<dronecode_sdk::rpc::camera::VideoStreamSettings>
    createArbitraryRPCVideoStreamSettings() const;

    std::future<void>
    subscribeVideoStreamInfoAsync(std::vector<dronecode_sdk::Camera::VideoStreamInfo> &video_info,
                                  std::shared_ptr<grpc::ClientContext> context) const;
    std::unique_ptr<dronecode_sdk::rpc::camera::VideoStreamInfo>
    createArbitraryRPCVideoStreamInfo() const;
    void checkSendsVideoStreamInfo(
        const std::vector<dronecode_sdk::Camera::VideoStreamInfo> &video_info_events) const;
    dronecode_sdk::Camera::VideoStreamInfo
    createVideoStreamInfo(const dronecode_sdk::Camera::VideoStreamSettings settings,
                          const dronecode_sdk::Camera::VideoStreamInfo::Status status) const;

    std::future<void>
    subscribeCaptureInfoAsync(std::vector<dronecode_sdk::Camera::CaptureInfo> &capture_info_events,
                              std::shared_ptr<grpc::ClientContext> context) const;
    std::unique_ptr<dronecode_sdk::rpc::camera::CaptureInfo> createArbitraryRPCCaptureInfo() const;
    std::unique_ptr<dronecode_sdk::rpc::camera::Position> createRPCPosition(
        const double lat, const double lng, const float abs_alt, const float rel_alt) const;
    std::unique_ptr<dronecode_sdk::rpc::camera::Quaternion>
    createRPCAttitudeQuaternion(const float w, const float x, const float y, const float z) const;
    std::unique_ptr<dronecode_sdk::rpc::camera::EulerAngle> createRPCAttitudeEulerAngle(
        const float yaw_deg, const float pitch_deg, const float roll_deg) const;
    void checkSendsCaptureInfo(
        const std::vector<dronecode_sdk::Camera::CaptureInfo> &capture_info_events) const;
    dronecode_sdk::Camera::CaptureInfo createArbitraryCaptureInfo() const;

    dronecode_sdk::Camera::Status
    createCameraStatus(const bool is_video_on,
                       const bool is_photo_interval_on,
                       const dronecode_sdk::Camera::Status::StorageStatus storage_status,
                       const float used_storage_mib,
                       const float available_storage_mib,
                       const float total_storage_mib,
                       const float recording_time_s,
                       const std::string media_folder_name) const;
    std::future<void>
    subscribeCameraStatusAsync(std::vector<dronecode_sdk::Camera::Status> &camera_status_events,
                               std::shared_ptr<grpc::ClientContext> context) const;
    void checkSendsCameraStatus(
        const std::vector<dronecode_sdk::Camera::Status> &camera_status_events) const;

    dronecode_sdk::Camera::Option createOption(const std::string option_id,
                                               const std::string option_description) const;
    dronecode_sdk::Camera::Setting createSetting(const std::string setting_id,
                                                 const std::string setting_description,
                                                 const dronecode_sdk::Camera::Option &option) const;
    std::future<void> subscribeCurrentSettingsAsync(
        std::vector<std::vector<dronecode_sdk::Camera::Setting>> &current_settings_events,
        std::shared_ptr<grpc::ClientContext> context) const;
    void checkSendsCurrentSettings(const std::vector<std::vector<dronecode_sdk::Camera::Setting>>
                                       &current_settings_events) const;

    dronecode_sdk::Camera::SettingOptions
    createSettingOptions(const std::string setting_id,
                         const std::string setting_description,
                         const std::vector<dronecode_sdk::Camera::Option> &options) const;
    std::future<void> subscribePossibleSettingOptionsAsync(
        std::vector<std::vector<dronecode_sdk::Camera::SettingOptions>> &possible_settings_events,
        std::shared_ptr<grpc::ClientContext> context) const;
    void checkSendsPossibleSettingOptions(
        const std::vector<std::vector<dronecode_sdk::Camera::SettingOptions>>
            &possible_setting_options_events) const;

    std::unique_ptr<dronecode_sdk::rpc::camera::Setting>
    createRPCSetting(const std::string &setting_id,
                     const std::string &setting_description,
                     const std::string &option_id,
                     const std::string &option_description);
    std::future<void> setSettingAndSaveParams(
        const dronecode_sdk::rpc::camera::SetSettingRequest &request,
        std::shared_ptr<dronecode_sdk::rpc::camera::SetSettingResponse> response,
        dronecode_sdk::Camera::result_callback_t &result_callback);

    MockCamera _camera{};
    CameraServiceImpl _camera_service;
    std::unique_ptr<grpc::Server> _server{};
    std::unique_ptr<CameraService::Stub> _stub{};

    std::promise<void> _callback_saved_promise{};
    std::future<void> _callback_saved_future{};
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
    dronecode_sdk::rpc::camera::SetModeRequest request;
    request.set_camera_mode(ARBITRARY_RPC_CAMERA_MODE);

    _camera_service.SetMode(nullptr, &request, nullptr);
}

TEST_P(CameraServiceImplTest, setModeResultIsTranslatedCorrectly)
{
    ON_CALL(_camera, set_mode(_)).WillByDefault(Return(GetParam().second));
    dronecode_sdk::rpc::camera::SetModeRequest request;
    request.set_camera_mode(ARBITRARY_RPC_CAMERA_MODE);
    dronecode_sdk::rpc::camera::SetModeResponse response;

    _camera_service.SetMode(nullptr, &request, &response);

    EXPECT_EQ(GetParam().first, CameraResult::Result_Name(response.camera_result().result()));
}

TEST_F(CameraServiceImplTest, setModeSetsPhotoMode)
{
    const auto expected_mode = dronecode_sdk::Camera::Mode::PHOTO;
    EXPECT_CALL(_camera, set_mode(expected_mode)).Times(1);

    setCameraMode(dronecode_sdk::rpc::camera::CameraMode::PHOTO);
}

void CameraServiceImplTest::setCameraMode(const dronecode_sdk::rpc::camera::CameraMode mode)
{
    dronecode_sdk::rpc::camera::SetModeRequest request;
    request.set_camera_mode(mode);
    dronecode_sdk::rpc::camera::SetModeResponse response;

    _camera_service.SetMode(nullptr, &request, &response);
}

TEST_F(CameraServiceImplTest, setModeSetsVideoMode)
{
    const auto expected_mode = dronecode_sdk::Camera::Mode::VIDEO;
    EXPECT_CALL(_camera, set_mode(expected_mode)).Times(1);

    setCameraMode(dronecode_sdk::rpc::camera::CameraMode::VIDEO);
}

TEST_F(CameraServiceImplTest, registersToCameraMode)
{
    dronecode_sdk::Camera::subscribe_mode_callback_t mode_callback;
    EXPECT_CALL(_camera, subscribe_mode(_))
        .Times(2)
        .WillOnce(SaveResult(&mode_callback, &_callback_saved_promise))
        .WillOnce(DoDefault());
    std::vector<dronecode_sdk::Camera::Mode> mode_events;
    auto context = std::make_shared<grpc::ClientContext>();

    auto mode_events_future = subscribeModeAsync(mode_events, context);
    _callback_saved_future.wait();
    context->TryCancel();
    mode_callback(ARBITRARY_CAMERA_MODE);
    mode_events_future.wait();
}

std::future<void>
CameraServiceImplTest::subscribeModeAsync(std::vector<dronecode_sdk::Camera::Mode> &camera_modes,
                                          std::shared_ptr<grpc::ClientContext> context) const
{
    return std::async(std::launch::async, [&]() {
        dronecode_sdk::rpc::camera::SubscribeModeRequest request;
        auto response_reader = _stub->SubscribeMode(context.get(), request);

        dronecode_sdk::rpc::camera::ModeResponse response;
        while (response_reader->Read(&response)) {
            camera_modes.push_back(
                CameraServiceImpl::translateRPCCameraMode(response.camera_mode()));
        }

        response_reader->Finish();
    });
}

TEST_F(CameraServiceImplTest, doesNotSendModeIfCallbackNotCalled)
{
    std::vector<dronecode_sdk::Camera::Mode> mode_events;
    auto context = std::make_shared<grpc::ClientContext>();
    auto mode_events_future = subscribeModeAsync(mode_events, context);

    context->TryCancel();
    mode_events_future.wait();

    EXPECT_EQ(0, mode_events.size());
}

TEST_F(CameraServiceImplTest, sendsOneMode)
{
    std::vector<dronecode_sdk::Camera::Mode> modes;
    modes.push_back(ARBITRARY_CAMERA_MODE);

    checkSendsModes(modes);
}

void CameraServiceImplTest::checkSendsModes(
    const std::vector<dronecode_sdk::Camera::Mode> &modes) const
{
    std::promise<void> subscription_promise;
    auto subscription_future = subscription_promise.get_future();
    dronecode_sdk::Camera::subscribe_mode_callback_t mode_callback;
    auto context = std::make_shared<grpc::ClientContext>();
    EXPECT_CALL(_camera, subscribe_mode(_))
        .Times(2)
        .WillOnce(SaveResult(&mode_callback, &subscription_promise))
        .WillOnce(DoDefault());

    std::vector<dronecode_sdk::Camera::Mode> received_modes;
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
    std::vector<dronecode_sdk::Camera::Mode> modes;
    modes.push_back(ARBITRARY_CAMERA_MODE);
    modes.push_back(ARBITRARY_CAMERA_MODE);
    modes.push_back(ARBITRARY_CAMERA_MODE);
    modes.push_back(dronecode_sdk::Camera::Mode::PHOTO);
    modes.push_back(dronecode_sdk::Camera::Mode::VIDEO);
    modes.push_back(dronecode_sdk::Camera::Mode::UNKNOWN);

    checkSendsModes(modes);
}

TEST_F(CameraServiceImplTest, setVideoStreamSettingsDoesNotFailWithAllNullParams)
{
    _camera_service.SetVideoStreamSettings(nullptr, nullptr, nullptr);
}

TEST_F(CameraServiceImplTest, setVideoStreamSettingsDoesNotFailWithNullResponse)
{
    dronecode_sdk::rpc::camera::SetVideoStreamSettingsRequest request;

    auto rpc_settings = createArbitraryRPCVideoStreamSettings();
    request.set_allocated_video_stream_settings(rpc_settings.release());

    _camera_service.SetVideoStreamSettings(nullptr, &request, nullptr);
}

std::unique_ptr<dronecode_sdk::rpc::camera::VideoStreamSettings>
CameraServiceImplTest::createArbitraryRPCVideoStreamSettings() const
{
    auto rpc_settings = std::unique_ptr<dronecode_sdk::rpc::camera::VideoStreamSettings>(
        new dronecode_sdk::rpc::camera::VideoStreamSettings());
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
    dronecode_sdk::rpc::camera::SetVideoStreamSettingsRequest request;
    request.set_allocated_video_stream_settings(rpc_video_stream_settings.release());

    _camera_service.SetVideoStreamSettings(nullptr, &request, nullptr);
}

TEST_F(CameraServiceImplTest, registersToVideoStreamInfo)
{
    auto rpc_video_stream_info = createArbitraryRPCVideoStreamInfo();
    const auto expected_video_stream_info =
        CameraServiceImpl::translateRPCVideoStreamInfo(*rpc_video_stream_info);
    dronecode_sdk::Camera::subscribe_video_stream_info_callback_t video_info_callback;
    EXPECT_CALL(_camera, subscribe_video_stream_info(_))
        .Times(2)
        .WillOnce(SaveResult(&video_info_callback, &_callback_saved_promise))
        .WillOnce(DoDefault());
    std::vector<dronecode_sdk::Camera::VideoStreamInfo> video_info_events;
    auto context = std::make_shared<grpc::ClientContext>();

    auto mode_events_future = subscribeVideoStreamInfoAsync(video_info_events, context);
    _callback_saved_future.wait();
    context->TryCancel();
    video_info_callback(expected_video_stream_info);
    mode_events_future.wait();
}

std::future<void> CameraServiceImplTest::subscribeVideoStreamInfoAsync(
    std::vector<dronecode_sdk::Camera::VideoStreamInfo> &video_info_events,
    std::shared_ptr<grpc::ClientContext> context) const
{
    return std::async(std::launch::async, [&]() {
        dronecode_sdk::rpc::camera::SubscribeVideoStreamInfoRequest request;
        auto response_reader = _stub->SubscribeVideoStreamInfo(context.get(), request);

        dronecode_sdk::rpc::camera::VideoStreamInfoResponse response;
        while (response_reader->Read(&response)) {
            video_info_events.push_back(
                CameraServiceImpl::translateRPCVideoStreamInfo(response.video_stream_info()));
        }

        response_reader->Finish();
    });
}

std::unique_ptr<dronecode_sdk::rpc::camera::VideoStreamInfo>
CameraServiceImplTest::createArbitraryRPCVideoStreamInfo() const
{
    auto rpc_info = std::unique_ptr<dronecode_sdk::rpc::camera::VideoStreamInfo>(
        new dronecode_sdk::rpc::camera::VideoStreamInfo());
    rpc_info->set_video_stream_status(ARBITRARY_VIDEO_STREAM_STATUS);
    rpc_info->set_allocated_video_stream_settings(
        createArbitraryRPCVideoStreamSettings().release());

    return rpc_info;
}

TEST_F(CameraServiceImplTest, doesNotSendVideoStreamInfoIfCallbackNotCalled)
{
    std::vector<dronecode_sdk::Camera::VideoStreamInfo> video_info_events;
    auto context = std::make_shared<grpc::ClientContext>();
    auto video_info_events_future = subscribeVideoStreamInfoAsync(video_info_events, context);

    context->TryCancel();
    video_info_events_future.wait();

    EXPECT_EQ(0, video_info_events.size());
}

TEST_F(CameraServiceImplTest, sendsOneVideoStreamInfo)
{
    std::vector<dronecode_sdk::Camera::VideoStreamInfo> video_info_events;
    auto video_info_event = createArbitraryRPCVideoStreamInfo();
    video_info_events.push_back(CameraServiceImpl::translateRPCVideoStreamInfo(*video_info_event));

    checkSendsVideoStreamInfo(video_info_events);
}

void CameraServiceImplTest::checkSendsVideoStreamInfo(
    const std::vector<dronecode_sdk::Camera::VideoStreamInfo> &video_info_events) const
{
    std::promise<void> subscription_promise;
    auto subscription_future = subscription_promise.get_future();
    dronecode_sdk::Camera::subscribe_video_stream_info_callback_t video_info_callback;
    auto context = std::make_shared<grpc::ClientContext>();
    EXPECT_CALL(_camera, subscribe_video_stream_info(_))
        .Times(2)
        .WillOnce(SaveResult(&video_info_callback, &subscription_promise))
        .WillOnce(DoDefault());

    std::vector<dronecode_sdk::Camera::VideoStreamInfo> received_video_info_events;
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
    std::vector<dronecode_sdk::Camera::VideoStreamInfo> video_info_events;

    dronecode_sdk::Camera::VideoStreamSettings settings1;
    settings1.set_highest();
    video_info_events.push_back(createVideoStreamInfo(
        settings1, dronecode_sdk::Camera::VideoStreamInfo::Status::NOT_RUNNING));

    dronecode_sdk::Camera::VideoStreamSettings settings2;
    video_info_events.push_back(createVideoStreamInfo(
        settings2, dronecode_sdk::Camera::VideoStreamInfo::Status::IN_PROGRESS));

    checkSendsVideoStreamInfo(video_info_events);
}

dronecode_sdk::Camera::VideoStreamInfo CameraServiceImplTest::createVideoStreamInfo(
    const dronecode_sdk::Camera::VideoStreamSettings settings,
    const dronecode_sdk::Camera::VideoStreamInfo::Status status) const
{
    dronecode_sdk::Camera::VideoStreamInfo video_stream_info{settings, status};
    return video_stream_info;
}

TEST_F(CameraServiceImplTest, registersToCaptureInfo)
{
    auto rpc_capture_info = createArbitraryRPCCaptureInfo();
    const auto expected_capture_info =
        CameraServiceImpl::translateRPCCaptureInfo(*rpc_capture_info);
    dronecode_sdk::Camera::capture_info_callback_t capture_info_callback;
    EXPECT_CALL(_camera, subscribe_capture_info(_))
        .Times(2)
        .WillOnce(SaveResult(&capture_info_callback, &_callback_saved_promise))
        .WillOnce(DoDefault());
    std::vector<dronecode_sdk::Camera::CaptureInfo> capture_info_events;
    auto context = std::make_shared<grpc::ClientContext>();

    auto mode_events_future = subscribeCaptureInfoAsync(capture_info_events, context);
    _callback_saved_future.wait();
    context->TryCancel();
    capture_info_callback(expected_capture_info);
    mode_events_future.wait();
}

std::future<void> CameraServiceImplTest::subscribeCaptureInfoAsync(
    std::vector<dronecode_sdk::Camera::CaptureInfo> &capture_info_events,
    std::shared_ptr<grpc::ClientContext> context) const
{
    return std::async(std::launch::async, [&]() {
        dronecode_sdk::rpc::camera::SubscribeCaptureInfoRequest request;
        auto response_reader = _stub->SubscribeCaptureInfo(context.get(), request);

        dronecode_sdk::rpc::camera::CaptureInfoResponse response;
        while (response_reader->Read(&response)) {
            capture_info_events.push_back(
                CameraServiceImpl::translateRPCCaptureInfo(response.capture_info()));
        }

        response_reader->Finish();
    });
}

std::unique_ptr<dronecode_sdk::rpc::camera::CaptureInfo>
CameraServiceImplTest::createArbitraryRPCCaptureInfo() const
{
    auto rpc_info = std::unique_ptr<dronecode_sdk::rpc::camera::CaptureInfo>(
        new dronecode_sdk::rpc::camera::CaptureInfo());
    rpc_info->set_allocated_position(
        createRPCPosition(41.848695, 75.132751, 3002.1f, 50.3f).release());
    rpc_info->set_allocated_attitude_quaternion(
        createRPCAttitudeQuaternion(0.1f, 0.2f, 0.3f, 0.4f).release());
    rpc_info->set_allocated_attitude_euler_angle(
        createRPCAttitudeEulerAngle(12.2f, 90.0f, 11.1f).release());
    rpc_info->set_time_utc_us(ARBITRARY_INT);
    rpc_info->set_is_success(ARBITRARY_BOOL);
    rpc_info->set_index(ARBITRARY_INT);
    rpc_info->set_file_url(ARBITRARY_URI);

    return rpc_info;
}

std::unique_ptr<dronecode_sdk::rpc::camera::Position> CameraServiceImplTest::createRPCPosition(
    const double lat, const double lng, const float abs_alt, const float rel_alt) const
{
    auto expected_position = std::unique_ptr<dronecode_sdk::rpc::camera::Position>(
        new dronecode_sdk::rpc::camera::Position());

    expected_position->set_latitude_deg(lat);
    expected_position->set_longitude_deg(lng);
    expected_position->set_absolute_altitude_m(abs_alt);
    expected_position->set_relative_altitude_m(rel_alt);

    return expected_position;
}

std::unique_ptr<dronecode_sdk::rpc::camera::Quaternion>
CameraServiceImplTest::createRPCAttitudeQuaternion(const float w,
                                                   const float x,
                                                   const float y,
                                                   const float z) const
{
    auto quaternion = std::unique_ptr<dronecode_sdk::rpc::camera::Quaternion>(
        new dronecode_sdk::rpc::camera::Quaternion());

    quaternion->set_w(w);
    quaternion->set_x(x);
    quaternion->set_y(y);
    quaternion->set_z(z);

    return quaternion;
}

std::unique_ptr<dronecode_sdk::rpc::camera::EulerAngle>
CameraServiceImplTest::createRPCAttitudeEulerAngle(const float yaw_deg,
                                                   const float pitch_deg,
                                                   const float roll_deg) const
{
    auto euler_angle = std::unique_ptr<dronecode_sdk::rpc::camera::EulerAngle>(
        new dronecode_sdk::rpc::camera::EulerAngle());

    euler_angle->set_yaw_deg(yaw_deg);
    euler_angle->set_pitch_deg(pitch_deg);
    euler_angle->set_roll_deg(roll_deg);

    return euler_angle;
}

TEST_F(CameraServiceImplTest, doesNotSendCaptureInfoIfCallbackNotCalled)
{
    std::vector<dronecode_sdk::Camera::CaptureInfo> camera_info_events;
    auto context = std::make_shared<grpc::ClientContext>();
    auto camera_info_events_future = subscribeCaptureInfoAsync(camera_info_events, context);

    context->TryCancel();
    camera_info_events_future.wait();

    EXPECT_EQ(0, camera_info_events.size());
}

TEST_F(CameraServiceImplTest, sendsOneCaptureInfo)
{
    std::vector<dronecode_sdk::Camera::CaptureInfo> capture_info_events;
    auto capture_info_event = createArbitraryRPCCaptureInfo();
    capture_info_events.push_back(CameraServiceImpl::translateRPCCaptureInfo(*capture_info_event));

    checkSendsCaptureInfo(capture_info_events);
}

void CameraServiceImplTest::checkSendsCaptureInfo(
    const std::vector<dronecode_sdk::Camera::CaptureInfo> &capture_info_events) const
{
    std::promise<void> subscription_promise;
    auto subscription_future = subscription_promise.get_future();
    dronecode_sdk::Camera::capture_info_callback_t capture_info_callback;
    auto context = std::make_shared<grpc::ClientContext>();
    EXPECT_CALL(_camera, subscribe_capture_info(_))
        .Times(2)
        .WillOnce(SaveResult(&capture_info_callback, &subscription_promise))
        .WillOnce(DoDefault());

    std::vector<dronecode_sdk::Camera::CaptureInfo> received_capture_info_events;
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
    std::vector<dronecode_sdk::Camera::CaptureInfo> capture_info_events;

    capture_info_events.push_back(createArbitraryCaptureInfo());
    capture_info_events.push_back(createArbitraryCaptureInfo());
    capture_info_events.push_back(createArbitraryCaptureInfo());
    capture_info_events.push_back(createArbitraryCaptureInfo());

    checkSendsCaptureInfo(capture_info_events);
}

dronecode_sdk::Camera::CaptureInfo CameraServiceImplTest::createArbitraryCaptureInfo() const
{
    dronecode_sdk::Camera::CaptureInfo::Position position{};
    position.latitude_deg = 12.12223;
    position.longitude_deg = 24.342;
    position.absolute_altitude_m = 2334.2f;
    position.relative_altitude_m = 54.12f;

    dronecode_sdk::Camera::CaptureInfo::Quaternion quaternion;
    quaternion.w = 12.3f;
    quaternion.x = 0.3f;
    quaternion.y = 1.1f;
    quaternion.z = -4.2f;

    dronecode_sdk::Camera::CaptureInfo::EulerAngle euler_angle;
    euler_angle.yaw_deg = 12.6f;
    euler_angle.pitch_deg = 102.3f;
    euler_angle.roll_deg = -24.0f;

    dronecode_sdk::Camera::CaptureInfo capture_info{};
    capture_info.position = position;
    capture_info.attitude_quaternion = quaternion;
    capture_info.attitude_euler_angle = euler_angle;
    capture_info.time_utc_us = ARBITRARY_INT;
    capture_info.success = ARBITRARY_BOOL;
    capture_info.index = ARBITRARY_INT;
    capture_info.file_url = ARBITRARY_URI;

    return capture_info;
}

TEST_F(CameraServiceImplTest, registersToCameraStatus)
{
    const auto expected_camera_status = createCameraStatus(
        false, true, ARBITRARY_CAMERA_STORAGE_STATUS, 3.4f, 12.6f, 16.0f, 0.4f, "100E90HD");
    dronecode_sdk::Camera::subscribe_status_callback_t status_callback;
    EXPECT_CALL(_camera, subscribe_status(_))
        .Times(2)
        .WillOnce(SaveResult(&status_callback, &_callback_saved_promise))
        .WillOnce(DoDefault());
    std::vector<dronecode_sdk::Camera::Status> camera_status_events;
    auto context = std::make_shared<grpc::ClientContext>();

    auto mode_events_future = subscribeCameraStatusAsync(camera_status_events, context);
    _callback_saved_future.wait();
    context->TryCancel();
    status_callback(expected_camera_status);
    mode_events_future.wait();
}

dronecode_sdk::Camera::Status CameraServiceImplTest::createCameraStatus(
    const bool is_video_on,
    const bool is_photo_interval_on,
    const dronecode_sdk::Camera::Status::StorageStatus storage_status,
    const float used_storage_mib,
    const float available_storage_mib,
    const float total_storage_mib,
    const float recording_time_s,
    const std::string media_folder_name) const
{
    dronecode_sdk::Camera::Status status{};
    status.video_on = is_video_on;
    status.photo_interval_on = is_photo_interval_on;
    status.storage_status = storage_status;
    status.used_storage_mib = used_storage_mib;
    status.available_storage_mib = available_storage_mib;
    status.total_storage_mib = total_storage_mib;
    status.recording_time_s = recording_time_s;
    status.media_folder_name = media_folder_name;

    return status;
}

std::future<void> CameraServiceImplTest::subscribeCameraStatusAsync(
    std::vector<dronecode_sdk::Camera::Status> &camera_status_events,
    std::shared_ptr<grpc::ClientContext> context) const
{
    return std::async(std::launch::async, [&]() {
        dronecode_sdk::rpc::camera::SubscribeCameraStatusRequest request;
        auto response_reader = _stub->SubscribeCameraStatus(context.get(), request);

        dronecode_sdk::rpc::camera::CameraStatusResponse response;
        while (response_reader->Read(&response)) {
            camera_status_events.push_back(
                CameraServiceImpl::translateRPCCameraStatus(response.camera_status()));
        }

        response_reader->Finish();
    });
}

TEST_F(CameraServiceImplTest, doesNotSendCameraStatusIfCallbackNotCalled)
{
    std::vector<dronecode_sdk::Camera::Status> camera_status_events;
    auto context = std::make_shared<grpc::ClientContext>();
    auto camera_status_events_future = subscribeCameraStatusAsync(camera_status_events, context);

    context->TryCancel();
    camera_status_events_future.wait();

    EXPECT_EQ(0, camera_status_events.size());
}

TEST_F(CameraServiceImplTest, sendsOneCameraStatus)
{
    std::vector<dronecode_sdk::Camera::Status> camera_status_events;
    auto camera_status_event = createCameraStatus(
        false, true, ARBITRARY_CAMERA_STORAGE_STATUS, 3.4f, 12.6f, 16.0f, 22.4f, "105E90HD");
    camera_status_events.push_back(camera_status_event);

    checkSendsCameraStatus(camera_status_events);
}

void CameraServiceImplTest::checkSendsCameraStatus(
    const std::vector<dronecode_sdk::Camera::Status> &camera_status_events) const
{
    std::promise<void> subscription_promise;
    auto subscription_future = subscription_promise.get_future();
    dronecode_sdk::Camera::subscribe_status_callback_t camera_status_callback;
    auto context = std::make_shared<grpc::ClientContext>();
    EXPECT_CALL(_camera, subscribe_status(_))
        .Times(2)
        .WillOnce(SaveResult(&camera_status_callback, &subscription_promise))
        .WillOnce(DoDefault());

    std::vector<dronecode_sdk::Camera::Status> received_camera_status_events;
    auto camera_status_events_future =
        subscribeCameraStatusAsync(received_camera_status_events, context);
    subscription_future.wait();
    for (const auto camera_status_event : camera_status_events) {
        camera_status_callback(camera_status_event);
    }
    context->TryCancel();
    auto arbitrary_camera_status_event = createCameraStatus(
        false, true, ARBITRARY_CAMERA_STORAGE_STATUS, 3.4f, 12.6f, 16.0f, 0.0f, "111E90HD");
    camera_status_callback(arbitrary_camera_status_event);
    camera_status_events_future.wait();

    ASSERT_EQ(camera_status_events.size(), received_camera_status_events.size());
    for (size_t i = 0; i < camera_status_events.size(); i++) {
        EXPECT_EQ(camera_status_events.at(i), received_camera_status_events.at(i));
    }
}

TEST_F(CameraServiceImplTest, sendsMultipleCameraStatus)
{
    std::vector<dronecode_sdk::Camera::Status> camera_status_events;

    camera_status_events.push_back(
        createCameraStatus(true,
                           true,
                           dronecode_sdk::Camera::Status::StorageStatus::UNFORMATTED,
                           1.2f,
                           3.4f,
                           2.2f,
                           2.3f,
                           "104E90HD"));
    camera_status_events.push_back(
        createCameraStatus(true,
                           false,
                           dronecode_sdk::Camera::Status::StorageStatus::FORMATTED,
                           11.2f,
                           58.4f,
                           8.65f,
                           2.2f,
                           "360E90HD"));
    camera_status_events.push_back(
        createCameraStatus(false,
                           false,
                           dronecode_sdk::Camera::Status::StorageStatus::NOT_AVAILABLE,
                           1.5f,
                           8.1f,
                           6.3f,
                           1.4f,
                           "GOPRO621"));

    checkSendsCameraStatus(camera_status_events);
}

TEST_F(CameraServiceImplTest, registersToCurrentSettings)
{
    std::vector<dronecode_sdk::Camera::Setting> current_settings;
    current_settings.push_back(
        createSetting(ARBITRARY_SETTING_ID,
                      ARBITRARY_SETTING_DESCRIPTION,
                      createOption(ARBITRARY_OPTION_ID, ARBITRARY_OPTION_DESCRIPTION)));
    dronecode_sdk::Camera::subscribe_current_settings_callback_t current_settings_callback;
    EXPECT_CALL(_camera, subscribe_current_settings(_))
        .Times(2)
        .WillOnce(SaveResult(&current_settings_callback, &_callback_saved_promise))
        .WillOnce(DoDefault());
    std::vector<std::vector<dronecode_sdk::Camera::Setting>> current_settings_events;
    auto context = std::make_shared<grpc::ClientContext>();

    auto mode_events_future = subscribeCurrentSettingsAsync(current_settings_events, context);
    _callback_saved_future.wait();
    context->TryCancel();
    current_settings_callback(current_settings);
    mode_events_future.wait();
}

dronecode_sdk::Camera::Setting
CameraServiceImplTest::createSetting(const std::string setting_id,
                                     const std::string setting_description,
                                     const dronecode_sdk::Camera::Option &option) const
{
    dronecode_sdk::Camera::Setting setting{};
    setting.setting_id = setting_id;
    setting.setting_description = setting_description;
    setting.option = option;

    return setting;
}

dronecode_sdk::Camera::Option
CameraServiceImplTest::createOption(const std::string option_id,
                                    const std::string option_description) const
{
    dronecode_sdk::Camera::Option option;
    option.option_id = option_id;
    option.option_description = option_description;

    return option;
}

std::future<void> CameraServiceImplTest::subscribeCurrentSettingsAsync(
    std::vector<std::vector<dronecode_sdk::Camera::Setting>> &current_settings_events,
    std::shared_ptr<grpc::ClientContext> context) const
{
    return std::async(std::launch::async, [&]() {
        dronecode_sdk::rpc::camera::SubscribeCurrentSettingsRequest request;
        auto response_reader = _stub->SubscribeCurrentSettings(context.get(), request);

        dronecode_sdk::rpc::camera::CurrentSettingsResponse response;
        while (response_reader->Read(&response)) {
            std::vector<dronecode_sdk::Camera::Setting> response_settings;

            for (auto current_setting : response.current_settings()) {
                response_settings.push_back(
                    CameraServiceImpl::translateRPCSetting(current_setting));
            }

            current_settings_events.push_back(response_settings);
        }

        response_reader->Finish();
    });
}

TEST_F(CameraServiceImplTest, doesNotSendCurrentSettingsIfCallbackNotCalled)
{
    std::vector<std::vector<dronecode_sdk::Camera::Setting>> current_settings_events;
    auto context = std::make_shared<grpc::ClientContext>();
    auto current_settings_events_future =
        subscribeCurrentSettingsAsync(current_settings_events, context);

    context->TryCancel();
    current_settings_events_future.wait();

    EXPECT_EQ(0, current_settings_events.size());
}

TEST_F(CameraServiceImplTest, sendsOneCurrentSettings)
{
    std::vector<std::vector<dronecode_sdk::Camera::Setting>> current_settings_events;

    std::vector<dronecode_sdk::Camera::Setting> current_settings;
    current_settings.push_back(
        createSetting(ARBITRARY_SETTING_ID,
                      ARBITRARY_SETTING_DESCRIPTION,
                      createOption(ARBITRARY_OPTION_ID, ARBITRARY_OPTION_DESCRIPTION)));
    current_settings_events.push_back(current_settings);

    checkSendsCurrentSettings(current_settings_events);
}

void CameraServiceImplTest::checkSendsCurrentSettings(
    const std::vector<std::vector<dronecode_sdk::Camera::Setting>> &current_settings_events) const
{
    std::promise<void> subscription_promise;
    auto subscription_future = subscription_promise.get_future();
    dronecode_sdk::Camera::subscribe_current_settings_callback_t current_settings_callback;
    auto context = std::make_shared<grpc::ClientContext>();
    EXPECT_CALL(_camera, subscribe_current_settings(_))
        .Times(2)
        .WillOnce(SaveResult(&current_settings_callback, &subscription_promise))
        .WillOnce(DoDefault());

    std::vector<std::vector<dronecode_sdk::Camera::Setting>> received_current_settings_events;
    auto current_settings_events_future =
        subscribeCurrentSettingsAsync(received_current_settings_events, context);
    subscription_future.wait();
    for (const auto current_settings_event : current_settings_events) {
        current_settings_callback(current_settings_event);
    }
    context->TryCancel();
    std::vector<dronecode_sdk::Camera::Setting> arbitrary_current_settings_event;
    arbitrary_current_settings_event.push_back(
        createSetting(ARBITRARY_SETTING_ID,
                      ARBITRARY_SETTING_DESCRIPTION,
                      createOption(ARBITRARY_OPTION_ID, ARBITRARY_OPTION_DESCRIPTION)));
    current_settings_callback(arbitrary_current_settings_event);
    current_settings_events_future.wait();

    ASSERT_EQ(current_settings_events.size(), received_current_settings_events.size());
    for (size_t i = 0; i < current_settings_events.size(); i++) {
        auto current_settings = current_settings_events.at(i);
        auto received_current_settings = received_current_settings_events.at(i);

        for (size_t j = 0; j < current_settings.size(); j++) {
            EXPECT_EQ(current_settings.at(j), received_current_settings.at(j));
        }
    }
}

TEST_F(CameraServiceImplTest, sendsMultipleCurrentSettings)
{
    std::vector<std::vector<dronecode_sdk::Camera::Setting>> current_settings_events;

    std::vector<dronecode_sdk::Camera::Setting> current_settings1;
    current_settings1.push_back(
        createSetting("arbitrary_setting_id1",
                      "Arbitrary setting id one",
                      createOption("arbitrary_option_id1", "Arbitrary Option ID one")));
    current_settings1.push_back(
        createSetting("arbitrary_setting_id2",
                      "Arbitrary setting id two",
                      createOption("arbitrary_option_id2", "Arbitrary Option ID two")));
    current_settings1.push_back(
        createSetting("arbitrary_setting_id3",
                      "Arbitrary setting id three",
                      createOption("arbitrary_option_id3", "Arbitrary Option ID three")));
    current_settings_events.push_back(current_settings1);

    std::vector<dronecode_sdk::Camera::Setting> current_settings2;
    current_settings2.push_back(
        createSetting("arbitrary_setting_id4",
                      "Arbitrary setting id four",
                      createOption("arbitrary_option_id4", "Arbitrary Option ID four")));
    current_settings2.push_back(
        createSetting("arbitrary_setting_id5",
                      "Arbitrary setting id five",
                      createOption("arbitrary_option_id5", "Arbitrary Option ID five")));
    current_settings2.push_back(
        createSetting("arbitrary_setting_id6",
                      "Arbitrary setting id six",
                      createOption("arbitrary_option_id6", "Arbitrary Option ID six")));
    current_settings_events.push_back(current_settings2);

    checkSendsCurrentSettings(current_settings_events);
}

TEST_F(CameraServiceImplTest, registersToPossibleSettings)
{
    std::vector<dronecode_sdk::Camera::SettingOptions> possible_settings;
    std::vector<dronecode_sdk::Camera::Option> options;
    options.push_back(createOption(ARBITRARY_OPTION_ID, ARBITRARY_OPTION_DESCRIPTION));
    possible_settings.push_back(
        createSettingOptions(ARBITRARY_SETTING_ID, ARBITRARY_SETTING_DESCRIPTION, options));
    dronecode_sdk::Camera::subscribe_possible_setting_options_callback_t possible_settings_callback;
    EXPECT_CALL(_camera, subscribe_possible_setting_options(_))
        .Times(2)
        .WillOnce(SaveResult(&possible_settings_callback, &_callback_saved_promise))
        .WillOnce(DoDefault());
    std::vector<std::vector<dronecode_sdk::Camera::SettingOptions>> possible_settings_events;
    auto context = std::make_shared<grpc::ClientContext>();

    auto mode_events_future =
        subscribePossibleSettingOptionsAsync(possible_settings_events, context);
    _callback_saved_future.wait();
    context->TryCancel();
    possible_settings_callback(possible_settings);
    mode_events_future.wait();
}

dronecode_sdk::Camera::SettingOptions CameraServiceImplTest::createSettingOptions(
    const std::string setting_id,
    const std::string setting_description,
    const std::vector<dronecode_sdk::Camera::Option> &options) const
{
    dronecode_sdk::Camera::SettingOptions setting_options;
    setting_options.setting_id = setting_id;
    setting_options.setting_description = setting_description;
    setting_options.options = options;

    return setting_options;
}

std::future<void> CameraServiceImplTest::subscribePossibleSettingOptionsAsync(
    std::vector<std::vector<dronecode_sdk::Camera::SettingOptions>> &possible_settings_events,
    std::shared_ptr<grpc::ClientContext> context) const
{
    return std::async(std::launch::async, [&]() {
        dronecode_sdk::rpc::camera::SubscribePossibleSettingOptionsRequest request;
        auto response_reader = _stub->SubscribePossibleSettingOptions(context.get(), request);

        dronecode_sdk::rpc::camera::PossibleSettingOptionsResponse response;
        while (response_reader->Read(&response)) {
            std::vector<dronecode_sdk::Camera::SettingOptions> response_setting_options;

            for (auto setting_options : response.setting_options()) {
                response_setting_options.push_back(
                    CameraServiceImpl::translateRPCSettingOptions(setting_options));
            }

            possible_settings_events.push_back(response_setting_options);
        }

        response_reader->Finish();
    });
}

TEST_F(CameraServiceImplTest, doesNotSendPossibleSettingOptionsIfCallbackNotCalled)
{
    std::vector<std::vector<dronecode_sdk::Camera::SettingOptions>> possible_setting_options_events;
    auto context = std::make_shared<grpc::ClientContext>();
    auto possible_setting_options_events_future =
        subscribePossibleSettingOptionsAsync(possible_setting_options_events, context);

    context->TryCancel();
    possible_setting_options_events_future.wait();

    EXPECT_EQ(0, possible_setting_options_events.size());
}

TEST_F(CameraServiceImplTest, sendsOnePossibleSettingOptions)
{
    std::vector<std::vector<dronecode_sdk::Camera::SettingOptions>> possible_setting_options_events;

    std::vector<dronecode_sdk::Camera::SettingOptions> possible_setting_options;
    std::vector<dronecode_sdk::Camera::Option> options;
    options.push_back(createOption(ARBITRARY_OPTION_ID, ARBITRARY_OPTION_DESCRIPTION));
    possible_setting_options.push_back(
        createSettingOptions(ARBITRARY_SETTING_ID, ARBITRARY_SETTING_DESCRIPTION, options));
    possible_setting_options_events.push_back(possible_setting_options);

    checkSendsPossibleSettingOptions(possible_setting_options_events);
}

void CameraServiceImplTest::checkSendsPossibleSettingOptions(
    const std::vector<std::vector<dronecode_sdk::Camera::SettingOptions>>
        &possible_setting_options_events) const
{
    std::promise<void> subscription_promise;
    auto subscription_future = subscription_promise.get_future();
    dronecode_sdk::Camera::subscribe_possible_setting_options_callback_t
        possible_setting_options_callback;
    auto context = std::make_shared<grpc::ClientContext>();
    EXPECT_CALL(_camera, subscribe_possible_setting_options(_))
        .Times(2)
        .WillOnce(SaveResult(&possible_setting_options_callback, &subscription_promise))
        .WillOnce(DoDefault());

    std::vector<std::vector<dronecode_sdk::Camera::SettingOptions>>
        received_possible_setting_options_events;
    auto possible_setting_options_events_future =
        subscribePossibleSettingOptionsAsync(received_possible_setting_options_events, context);
    subscription_future.wait();
    for (const auto possible_setting_options_event : possible_setting_options_events) {
        possible_setting_options_callback(possible_setting_options_event);
    }
    context->TryCancel();

    std::vector<dronecode_sdk::Camera::SettingOptions> arbitrary_possible_setting_options;
    std::vector<dronecode_sdk::Camera::Option> options;
    options.push_back(createOption(ARBITRARY_OPTION_ID, ARBITRARY_OPTION_DESCRIPTION));
    arbitrary_possible_setting_options.push_back(
        createSettingOptions(ARBITRARY_SETTING_ID, ARBITRARY_SETTING_DESCRIPTION, options));
    possible_setting_options_callback(arbitrary_possible_setting_options);
    possible_setting_options_events_future.wait();

    ASSERT_EQ(possible_setting_options_events.size(),
              received_possible_setting_options_events.size());
    for (size_t i = 0; i < possible_setting_options_events.size(); i++) {
        auto possible_setting_options = possible_setting_options_events.at(i);
        auto received_possible_setting_options = received_possible_setting_options_events.at(i);

        for (size_t j = 0; j < possible_setting_options.size(); j++) {
            EXPECT_EQ(possible_setting_options.at(j), received_possible_setting_options.at(j));
        }
    }
}

TEST_F(CameraServiceImplTest, sendsMultiplePossibleSettingOptionss)
{
    std::vector<std::vector<dronecode_sdk::Camera::SettingOptions>> possible_setting_options_events;

    std::vector<dronecode_sdk::Camera::SettingOptions> possible_setting_options;

    std::vector<dronecode_sdk::Camera::Option> options1;
    options1.push_back(createOption("option1_1", "Option One One"));
    options1.push_back(createOption("option1_2", "Option One Two"));
    options1.push_back(createOption("option1_3", "Option One Three"));
    possible_setting_options.push_back(createSettingOptions("setting1", "Setting one", options1));
    std::vector<dronecode_sdk::Camera::Option> options2;
    options2.push_back(createOption("option1", "Option One"));
    options2.push_back(createOption("option2", "Option One"));
    possible_setting_options.push_back(createSettingOptions("setting2", "Setting two", options2));

    std::vector<dronecode_sdk::Camera::Option> options3;
    options3.push_back(createOption("option1", "Option One"));
    options3.push_back(createOption("option2", "Option Two"));
    options3.push_back(createOption("option3", "Option Three"));
    options3.push_back(createOption("option4", "Option Four"));
    options3.push_back(createOption("option5", "Option Five"));
    possible_setting_options.push_back(createSettingOptions("setting3", "Setting Three", options3));

    possible_setting_options_events.push_back(possible_setting_options);

    checkSendsPossibleSettingOptions(possible_setting_options_events);
}

TEST_P(CameraServiceImplTest, setSettingResultIsTranslatedCorrectly)
{
    dronecode_sdk::Camera::result_callback_t result_callback;
    dronecode_sdk::rpc::camera::SetSettingRequest request;
    request.set_allocated_setting(createRPCSetting("arbitrary_setting_id",
                                                   "arbitrary_setting_description",
                                                   "arbitrary_option_id",
                                                   "arbitrary_option_description")
                                      .release());
    auto response = std::make_shared<dronecode_sdk::rpc::camera::SetSettingResponse>();

    auto handle = setSettingAndSaveParams(request, response, result_callback);
    result_callback(GetParam().second);

    EXPECT_EQ(GetParam().first, CameraResult::Result_Name(response->camera_result().result()));
}

std::future<void> CameraServiceImplTest::setSettingAndSaveParams(
    const dronecode_sdk::rpc::camera::SetSettingRequest &request,
    std::shared_ptr<dronecode_sdk::rpc::camera::SetSettingResponse> response,
    dronecode_sdk::Camera::result_callback_t &result_callback)
{
    EXPECT_CALL(_camera, set_option_async(_, _, _))
        .WillOnce(SaveResult(&result_callback, &_callback_saved_promise));

    auto handle = std::async(std::launch::async, [this, &request, response]() {
        _camera_service.SetSetting(nullptr, &request, response.get());
    });

    _callback_saved_future.wait();
    return handle;
}

std::unique_ptr<dronecode_sdk::rpc::camera::Setting>
CameraServiceImplTest::createRPCSetting(const std::string &setting_id,
                                        const std::string &setting_description,
                                        const std::string &option_id,
                                        const std::string &option_description)
{
    auto setting = std::unique_ptr<dronecode_sdk::rpc::camera::Setting>(
        new dronecode_sdk::rpc::camera::Setting);
    setting->set_setting_id(setting_id);
    setting->set_setting_description(setting_description);

    auto option =
        std::unique_ptr<dronecode_sdk::rpc::camera::Option>(new dronecode_sdk::rpc::camera::Option);
    option->set_option_id(option_id);
    option->set_option_description(option_description);
    setting->set_allocated_option(option.release());

    return setting;
}

TEST_F(CameraServiceImplTest, setSettingInProgressResultIsIgnored)
{
    dronecode_sdk::Camera::result_callback_t result_callback;
    dronecode_sdk::rpc::camera::SetSettingRequest request;
    request.set_allocated_setting(createRPCSetting("arbitrary_setting_id",
                                                   "arbitrary_setting_description",
                                                   "arbitrary_option_id",
                                                   "arbitrary_option_description")
                                      .release());
    auto response = std::make_shared<dronecode_sdk::rpc::camera::SetSettingResponse>();

    auto handle = setSettingAndSaveParams(request, response, result_callback);
    result_callback(dronecode_sdk::Camera::Result::IN_PROGRESS);
    result_callback(dronecode_sdk::Camera::Result::SUCCESS);

    EXPECT_EQ("SUCCESS", CameraResult::Result_Name(response->camera_result().result()));
}

TEST_F(CameraServiceImplTest, setsSettingEvenWhenContextAndResponseAreNull)
{
    dronecode_sdk::Camera::result_callback_t result_callback;
    dronecode_sdk::rpc::camera::SetSettingRequest request;
    request.set_allocated_setting(createRPCSetting(ARBITRARY_SETTING_ID,
                                                   ARBITRARY_SETTING_DESCRIPTION,
                                                   ARBITRARY_OPTION_ID,
                                                   ARBITRARY_OPTION_DESCRIPTION)
                                      .release());
    auto response = std::make_shared<dronecode_sdk::rpc::camera::SetSettingResponse>();

    auto handle = setSettingAndSaveParams(request, nullptr, result_callback);
    result_callback(ARBITRARY_CAMERA_RESULT);
}

TEST_F(CameraServiceImplTest, setsSettingWithRightParameter)
{
    dronecode_sdk::Camera::result_callback_t result_callback;
    dronecode_sdk::rpc::camera::SetSettingRequest request;
    request.set_allocated_setting(createRPCSetting(ARBITRARY_SETTING_ID,
                                                   ARBITRARY_SETTING_DESCRIPTION,
                                                   ARBITRARY_OPTION_ID,
                                                   ARBITRARY_OPTION_DESCRIPTION)
                                      .release());
    dronecode_sdk::Camera::Option option;
    option.option_id = ARBITRARY_OPTION_ID;

    EXPECT_CALL(_camera, set_option_async(_, ARBITRARY_SETTING_ID, option))
        .WillOnce(SaveResult(&result_callback, &_callback_saved_promise));

    auto handle = std::async(std::launch::async, [this, &request]() {
        _camera_service.SetSetting(nullptr, &request, nullptr);
    });

    _callback_saved_future.wait();
    result_callback(ARBITRARY_CAMERA_RESULT);
}

TEST_F(CameraServiceImplTest, setSettingReturnsWrongArgumentErrorIfRequestIsNull)
{
    dronecode_sdk::rpc::camera::SetSettingResponse response;

    _camera_service.SetSetting(nullptr, nullptr, &response);

    EXPECT_EQ("WRONG_ARGUMENT", CameraResult::Result_Name(response.camera_result().result()));
}

TEST_F(CameraServiceImplTest, setSettingDoesNotCrashWhenArgsAreNull)
{
    _camera_service.SetSetting(nullptr, nullptr, nullptr);
}

INSTANTIATE_TEST_CASE_P(CameraResultCorrespondences,
                        CameraServiceImplTest,
                        ::testing::ValuesIn(generateInputPairs()));

std::vector<InputPair> generateInputPairs()
{
    std::vector<InputPair> input_pairs;
    input_pairs.push_back(std::make_pair("SUCCESS", dronecode_sdk::Camera::Result::SUCCESS));
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
