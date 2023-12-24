#include <gmock/gmock.h>

#include <future>
#include <grpc++/grpc++.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <memory>

#include "camera/camera_service_impl.h"
#include "camera/mocks/camera_mock.h"
#include "mocks/lazy_plugin_mock.h"
#include "callback_list.h"

namespace {

using testing::_;
using testing::DoDefault;
using testing::NiceMock;
using testing::Return;

using MockCamera = NiceMock<mavsdk::testing::MockCamera>;
using MockLazyPlugin =
    testing::NiceMock<mavsdk::mavsdk_server::testing::MockLazyPlugin<MockCamera>>;
using CameraServiceImpl = mavsdk::mavsdk_server::CameraServiceImpl<MockCamera, MockLazyPlugin>;
using CameraService = mavsdk::rpc::camera::CameraService;

using CameraResult = mavsdk::rpc::camera::CameraResult;
using InputPair = std::pair<std::string, mavsdk::Camera::Result>;

static constexpr auto ARBITRARY_FLOAT = 24.2f;
static constexpr auto ARBITRARY_RPC_CAMERA_MODE = mavsdk::rpc::camera::MODE_PHOTO;
static constexpr auto ARBITRARY_CAMERA_MODE = mavsdk::Camera::Mode::Video;
static constexpr auto ARBITRARY_FRAME_RATE = 24.0f;
static constexpr auto ARBITRARY_RESOLUTION = 1280;
static constexpr auto ARBITRARY_BIT_RATE = 1492;
static constexpr auto ARBITRARY_ROTATION = 24;
static constexpr auto ARBITRARY_URI = "rtsp://blah:1337";
static constexpr auto ARBITRARY_VIDEO_STREAM_STATUS =
    mavsdk::rpc::camera::VideoStreamInfo_VideoStreamStatus_VIDEO_STREAM_STATUS_IN_PROGRESS;
static constexpr auto ARBITRARY_INT = 123456;
static constexpr auto ARBITRARY_BOOL = true;
static constexpr auto ARBITRARY_CAMERA_STORAGE_STATUS =
    mavsdk::Camera::Status::StorageStatus::Formatted;
static constexpr auto ARBITRARY_SETTING_ID = "23b442";
static constexpr auto ARBITRARY_SETTING_DESCRIPTION = "Twenty-three b four hundred-forty-two";
static constexpr auto ARBITRARY_OPTION_ID = "small";
static constexpr auto ARBITRARY_OPTION_DESCRIPTION = "Bigger";
static constexpr auto ARBITRARY_CAMERA_RESULT = mavsdk::Camera::Result::Success;

std::vector<InputPair> generateInputPairs();

class CameraServiceImplTest : public ::testing::TestWithParam<InputPair> {
protected:
    CameraServiceImplTest() : _camera_service(_lazy_plugin)
    {
        ON_CALL(_lazy_plugin, maybe_plugin()).WillByDefault(Return(&_camera));

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

    virtual void TearDown() { _server->Shutdown(); }

    void setCameraMode(const mavsdk::rpc::camera::Mode mode);

    std::future<void> subscribeModeAsync(
        std::vector<mavsdk::Camera::Mode>& camera_modes,
        std::shared_ptr<grpc::ClientContext> context =
            std::make_shared<grpc::ClientContext>()) const;
    void checkSendsModes(const std::vector<mavsdk::Camera::Mode>& modes) const;
    std::unique_ptr<mavsdk::rpc::camera::VideoStreamSettings>
    createArbitraryRPCVideoStreamSettings() const;

    std::future<void> subscribeVideoStreamInfoAsync(
        std::vector<mavsdk::Camera::VideoStreamInfo>& video_info,
        std::shared_ptr<grpc::ClientContext> context) const;
    std::unique_ptr<mavsdk::rpc::camera::VideoStreamInfo> createArbitraryRPCVideoStreamInfo() const;
    void checkSendsVideoStreamInfo(
        const std::vector<mavsdk::Camera::VideoStreamInfo>& video_info_events) const;
    mavsdk::Camera::VideoStreamInfo createVideoStreamInfo(
        const mavsdk::Camera::VideoStreamSettings settings,
        const mavsdk::Camera::VideoStreamInfo::VideoStreamStatus status) const;

    std::future<void> subscribeCaptureInfoAsync(
        std::vector<mavsdk::Camera::CaptureInfo>& capture_info_events,
        std::shared_ptr<grpc::ClientContext> context) const;
    std::unique_ptr<mavsdk::rpc::camera::CaptureInfo> createArbitraryRPCCaptureInfo() const;
    std::unique_ptr<mavsdk::rpc::camera::Position> createRPCPosition(
        const double lat, const double lng, const float abs_alt, const float rel_alt) const;
    std::unique_ptr<mavsdk::rpc::camera::Quaternion>
    createRPCAttitudeQuaternion(const float w, const float x, const float y, const float z) const;
    std::unique_ptr<mavsdk::rpc::camera::EulerAngle> createRPCAttitudeEulerAngle(
        const float yaw_deg, const float pitch_deg, const float roll_deg) const;
    void checkSendsCaptureInfo(
        const std::vector<mavsdk::Camera::CaptureInfo>& capture_info_events) const;
    mavsdk::Camera::CaptureInfo createArbitraryCaptureInfo() const;

    mavsdk::Camera::Status createStatus(
        const bool is_video_on,
        const bool is_photo_interval_on,
        const mavsdk::Camera::Status::StorageStatus storage_status,
        const float used_storage_mib,
        const float available_storage_mib,
        const float total_storage_mib,
        const float recording_time_s,
        const std::string media_folder_name) const;
    std::future<void> subscribeStatusAsync(
        std::vector<mavsdk::Camera::Status>& camera_status_events,
        std::shared_ptr<grpc::ClientContext> context) const;
    void checkSendsStatus(const std::vector<mavsdk::Camera::Status>& camera_status_events) const;

    mavsdk::Camera::Option
    createOption(const std::string option_id, const std::string option_description) const;
    mavsdk::Camera::Setting createSetting(
        const std::string setting_id,
        const std::string setting_description,
        const mavsdk::Camera::Option& option) const;
    std::future<void> subscribeCurrentSettingsAsync(
        std::vector<std::vector<mavsdk::Camera::Setting>>& current_settings_events,
        std::shared_ptr<grpc::ClientContext> context) const;
    void checkSendsCurrentSettings(
        const std::vector<std::vector<mavsdk::Camera::Setting>>& current_settings_events) const;

    mavsdk::Camera::SettingOptions createSettingOptions(
        const std::string setting_id,
        const std::string setting_description,
        const std::vector<mavsdk::Camera::Option>& options) const;
    std::future<void> subscribePossibleSettingOptionsAsync(
        std::vector<std::vector<mavsdk::Camera::SettingOptions>>& possible_settings_events,
        std::shared_ptr<grpc::ClientContext> context) const;
    void
    checkSendsPossibleSettingOptions(const std::vector<std::vector<mavsdk::Camera::SettingOptions>>&
                                         possible_setting_options_events) const;

    std::unique_ptr<mavsdk::rpc::camera::Setting> createRPCSetting(
        const std::string& setting_id,
        const std::string& setting_description,
        const std::string& option_id,
        const std::string& option_description);
    void setSettingAndSaveParams(
        const mavsdk::rpc::camera::SetSettingRequest& request,
        std::shared_ptr<mavsdk::rpc::camera::SetSettingResponse> response,
        mavsdk::Camera::Result result);

    MockLazyPlugin _lazy_plugin{};
    MockCamera _camera{};
    CameraServiceImpl _camera_service;
    std::unique_ptr<grpc::Server> _server{};
    std::unique_ptr<CameraService::Stub> _stub{};

    std::promise<void> _callback_saved_promise{};
    std::future<void> _callback_saved_future{};
};

ACTION_P2(SaveResult, callbacks, callback_saved_promise)
{
    auto handle = callbacks->subscribe(arg0);
    callback_saved_promise->set_value();
    return handle;
}

TEST_P(CameraServiceImplTest, takePhotoResultIsTranslatedCorrectly)
{
    ON_CALL(_camera, take_photo()).WillByDefault(Return(GetParam().second));
    mavsdk::rpc::camera::TakePhotoResponse response;

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
    mavsdk::rpc::camera::StartPhotoIntervalRequest request;
    request.set_interval_s(ARBITRARY_FLOAT);
    mavsdk::rpc::camera::StartPhotoIntervalResponse response;

    _camera_service.StartPhotoInterval(nullptr, &request, &response);

    EXPECT_EQ(GetParam().first, CameraResult::Result_Name(response.camera_result().result()));
}

TEST_F(CameraServiceImplTest, startsPhotoIntervalEvenWhenContextAndResponseAreNull)
{
    EXPECT_CALL(_camera, start_photo_interval(_)).Times(1);
    mavsdk::rpc::camera::StartPhotoIntervalRequest request;
    request.set_interval_s(ARBITRARY_FLOAT);

    _camera_service.StartPhotoInterval(nullptr, &request, nullptr);
}

TEST_F(CameraServiceImplTest, startsPhotoIntervalWithRightParameter)
{
    auto expected_interval = ARBITRARY_FLOAT;
    EXPECT_CALL(_camera, start_photo_interval(expected_interval)).Times(1);
    mavsdk::rpc::camera::StartPhotoIntervalRequest request;
    request.set_interval_s(expected_interval);

    _camera_service.StartPhotoInterval(nullptr, &request, nullptr);
}

TEST_F(CameraServiceImplTest, startPhotoIntervalReturnsUnknownErrorIfRequestIsNull)
{
    mavsdk::rpc::camera::StartPhotoIntervalResponse response;
    _camera_service.StartPhotoInterval(nullptr, nullptr, &response);

    EXPECT_EQ("RESULT_UNKNOWN", CameraResult::Result_Name(response.camera_result().result()));
}

TEST_F(CameraServiceImplTest, startPhotoIntervalDoesNotCrashWhenArgsAreNull)
{
    _camera_service.StartPhotoInterval(nullptr, nullptr, nullptr);
}

TEST_P(CameraServiceImplTest, stopPhotoIntervalResultIsTranslatedCorrectly)
{
    ON_CALL(_camera, stop_photo_interval()).WillByDefault(Return(GetParam().second));
    mavsdk::rpc::camera::StopPhotoIntervalResponse response;

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
    mavsdk::rpc::camera::StartVideoResponse response;

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
    mavsdk::rpc::camera::StopVideoResponse response;

    _camera_service.StopVideo(nullptr, nullptr, &response);

    EXPECT_EQ(GetParam().first, CameraResult::Result_Name(response.camera_result().result()));
}

TEST_F(CameraServiceImplTest, setModeDoesNotFailWithAllNullParams)
{
    _camera_service.SetMode(nullptr, nullptr, nullptr);
}

TEST_F(CameraServiceImplTest, setModeDoesNotFailWithNullResponse)
{
    mavsdk::rpc::camera::SetModeRequest request;
    request.set_mode(ARBITRARY_RPC_CAMERA_MODE);

    _camera_service.SetMode(nullptr, &request, nullptr);
}

TEST_P(CameraServiceImplTest, setModeResultIsTranslatedCorrectly)
{
    ON_CALL(_camera, set_mode(_)).WillByDefault(Return(GetParam().second));
    mavsdk::rpc::camera::SetModeRequest request;
    request.set_mode(ARBITRARY_RPC_CAMERA_MODE);
    mavsdk::rpc::camera::SetModeResponse response;

    _camera_service.SetMode(nullptr, &request, &response);

    EXPECT_EQ(GetParam().first, CameraResult::Result_Name(response.camera_result().result()));
}

TEST_F(CameraServiceImplTest, setModeSetsPhotoMode)
{
    const auto expected_mode = mavsdk::Camera::Mode::Photo;
    EXPECT_CALL(_camera, set_mode(expected_mode)).Times(1);

    setCameraMode(mavsdk::rpc::camera::Mode::MODE_PHOTO);
}

void CameraServiceImplTest::setCameraMode(const mavsdk::rpc::camera::Mode mode)
{
    mavsdk::rpc::camera::SetModeRequest request;
    request.set_mode(mode);
    mavsdk::rpc::camera::SetModeResponse response;

    _camera_service.SetMode(nullptr, &request, &response);
}

TEST_F(CameraServiceImplTest, setModeSetsVideoMode)
{
    const auto expected_mode = mavsdk::Camera::Mode::Video;
    EXPECT_CALL(_camera, set_mode(expected_mode)).Times(1);

    setCameraMode(mavsdk::rpc::camera::Mode::MODE_VIDEO);
}

TEST_F(CameraServiceImplTest, registersToCameraMode)
{
    mavsdk::CallbackList<mavsdk::Camera::Mode> mode_callbacks;
    EXPECT_CALL(_camera, subscribe_mode(_))
        .WillOnce(SaveResult(&mode_callbacks, &_callback_saved_promise));
    std::vector<mavsdk::Camera::Mode> mode_events;
    auto context = std::make_shared<grpc::ClientContext>();

    auto mode_events_future = subscribeModeAsync(mode_events, context);
    _callback_saved_future.wait();
    context->TryCancel();
    mode_callbacks(ARBITRARY_CAMERA_MODE);
    mode_events_future.wait();
}

std::future<void> CameraServiceImplTest::subscribeModeAsync(
    std::vector<mavsdk::Camera::Mode>& camera_modes,
    std::shared_ptr<grpc::ClientContext> context) const
{
    return std::async(std::launch::async, [&camera_modes, context, this]() {
        mavsdk::rpc::camera::SubscribeModeRequest request;
        auto response_reader = _stub->SubscribeMode(context.get(), request);

        mavsdk::rpc::camera::ModeResponse response;
        while (response_reader->Read(&response)) {
            camera_modes.push_back(CameraServiceImpl::translateFromRpcMode(response.mode()));
        }

        response_reader->Finish();
    });
}

TEST_F(CameraServiceImplTest, sendsOneMode)
{
    std::vector<mavsdk::Camera::Mode> modes;
    modes.push_back(ARBITRARY_CAMERA_MODE);

    checkSendsModes(modes);
}

void CameraServiceImplTest::checkSendsModes(const std::vector<mavsdk::Camera::Mode>& modes) const
{
    std::promise<void> subscription_promise;
    auto subscription_future = subscription_promise.get_future();
    mavsdk::CallbackList<mavsdk::Camera::Mode> mode_callbacks;
    auto context = std::make_shared<grpc::ClientContext>();
    EXPECT_CALL(_camera, subscribe_mode(_))
        .WillOnce(SaveResult(&mode_callbacks, &subscription_promise));

    std::vector<mavsdk::Camera::Mode> received_modes;
    auto mode_events_future = subscribeModeAsync(received_modes, context);
    subscription_future.wait();
    for (const auto mode : modes) {
        mode_callbacks(mode);
    }
    context->TryCancel();
    mode_callbacks(ARBITRARY_CAMERA_MODE);
    mode_events_future.wait();

    if (modes.size() != received_modes.size()) {
        std::cout << "modes: " << '\n';
        for (size_t i = 0; i < modes.size(); i++) {
            std::cout << i << ": " << modes[i] << '\n';
        }

        std::cout << "received_modes: " << '\n';

        for (size_t i = 0; i < received_modes.size(); i++) {
            std::cout << i << ": " << received_modes[i] << '\n';
        }
    }

    ASSERT_EQ(modes.size(), received_modes.size());
    for (size_t i = 0; i < modes.size(); i++) {
        EXPECT_EQ(modes.at(i), received_modes.at(i));
    }
}

TEST_F(CameraServiceImplTest, sendsMultipleModes)
{
    std::vector<mavsdk::Camera::Mode> modes;
    modes.push_back(ARBITRARY_CAMERA_MODE);
    modes.push_back(ARBITRARY_CAMERA_MODE);
    modes.push_back(ARBITRARY_CAMERA_MODE);
    modes.push_back(mavsdk::Camera::Mode::Photo);
    modes.push_back(mavsdk::Camera::Mode::Video);
    modes.push_back(mavsdk::Camera::Mode::Unknown);

    checkSendsModes(modes);
}

std::unique_ptr<mavsdk::rpc::camera::VideoStreamSettings>
CameraServiceImplTest::createArbitraryRPCVideoStreamSettings() const
{
    auto rpc_settings = std::make_unique<mavsdk::rpc::camera::VideoStreamSettings>();
    rpc_settings->set_frame_rate_hz(ARBITRARY_FRAME_RATE);
    rpc_settings->set_horizontal_resolution_pix(ARBITRARY_RESOLUTION);
    rpc_settings->set_vertical_resolution_pix(ARBITRARY_RESOLUTION);
    rpc_settings->set_bit_rate_b_s(ARBITRARY_BIT_RATE);
    rpc_settings->set_rotation_deg(ARBITRARY_ROTATION);
    rpc_settings->set_uri(ARBITRARY_URI);

    return rpc_settings;
}

TEST_F(CameraServiceImplTest, registersToVideoStreamInfo)
{
    auto rpc_video_stream_info = createArbitraryRPCVideoStreamInfo();
    const auto expected_video_stream_info =
        CameraServiceImpl::translateFromRpcVideoStreamInfo(*rpc_video_stream_info);
    mavsdk::CallbackList<mavsdk::Camera::VideoStreamInfo> video_info_callbacks;
    EXPECT_CALL(_camera, subscribe_video_stream_info(_))
        .WillOnce(SaveResult(&video_info_callbacks, &_callback_saved_promise));
    std::vector<mavsdk::Camera::VideoStreamInfo> video_info_events;
    auto context = std::make_shared<grpc::ClientContext>();

    auto mode_events_future = subscribeVideoStreamInfoAsync(video_info_events, context);
    _callback_saved_future.wait();
    context->TryCancel();
    video_info_callbacks(expected_video_stream_info);
    mode_events_future.wait();
}

std::future<void> CameraServiceImplTest::subscribeVideoStreamInfoAsync(
    std::vector<mavsdk::Camera::VideoStreamInfo>& video_info_events,
    std::shared_ptr<grpc::ClientContext> context) const
{
    return std::async(std::launch::async, [this, context, &video_info_events]() {
        mavsdk::rpc::camera::SubscribeVideoStreamInfoRequest request;
        auto response_reader = _stub->SubscribeVideoStreamInfo(context.get(), request);

        mavsdk::rpc::camera::VideoStreamInfoResponse response;
        while (response_reader->Read(&response)) {
            video_info_events.push_back(
                CameraServiceImpl::translateFromRpcVideoStreamInfo(response.video_stream_info()));
        }

        response_reader->Finish();
    });
}

std::unique_ptr<mavsdk::rpc::camera::VideoStreamInfo>
CameraServiceImplTest::createArbitraryRPCVideoStreamInfo() const
{
    auto rpc_info = std::make_unique<mavsdk::rpc::camera::VideoStreamInfo>();
    rpc_info->set_status(ARBITRARY_VIDEO_STREAM_STATUS);
    rpc_info->set_allocated_settings(createArbitraryRPCVideoStreamSettings().release());

    return rpc_info;
}

TEST_F(CameraServiceImplTest, sendsOneVideoStreamInfo)
{
    std::vector<mavsdk::Camera::VideoStreamInfo> video_info_events;
    auto video_info_event = createArbitraryRPCVideoStreamInfo();
    video_info_events.push_back(
        CameraServiceImpl::translateFromRpcVideoStreamInfo(*video_info_event));

    checkSendsVideoStreamInfo(video_info_events);
}

void CameraServiceImplTest::checkSendsVideoStreamInfo(
    const std::vector<mavsdk::Camera::VideoStreamInfo>& video_info_events) const
{
    std::promise<void> subscription_promise;
    auto subscription_future = subscription_promise.get_future();
    mavsdk::CallbackList<mavsdk::Camera::VideoStreamInfo> video_info_callbacks;
    auto context = std::make_shared<grpc::ClientContext>();
    EXPECT_CALL(_camera, subscribe_video_stream_info(_))
        .WillOnce(SaveResult(&video_info_callbacks, &subscription_promise));

    std::vector<mavsdk::Camera::VideoStreamInfo> received_video_info_events;
    auto video_info_events_future =
        subscribeVideoStreamInfoAsync(received_video_info_events, context);
    subscription_future.wait();
    for (const auto& video_info_event : video_info_events) {
        video_info_callbacks(video_info_event);
    }
    context->TryCancel();
    auto arbitrary_video_info_event = createArbitraryRPCVideoStreamInfo();
    video_info_callbacks(
        CameraServiceImpl::translateFromRpcVideoStreamInfo(*arbitrary_video_info_event));
    video_info_events_future.wait();

    ASSERT_EQ(video_info_events.size(), received_video_info_events.size());
    for (size_t i = 0; i < video_info_events.size(); i++) {
        EXPECT_EQ(video_info_events.at(i), received_video_info_events.at(i));
    }
}

TEST_F(CameraServiceImplTest, registersToCaptureInfo)
{
    auto rpc_capture_info = createArbitraryRPCCaptureInfo();
    const auto expected_capture_info =
        CameraServiceImpl::translateFromRpcCaptureInfo(*rpc_capture_info);
    mavsdk::CallbackList<mavsdk::Camera::CaptureInfo> capture_info_callbacks;
    EXPECT_CALL(_camera, subscribe_capture_info(_))
        .WillOnce(SaveResult(&capture_info_callbacks, &_callback_saved_promise));
    std::vector<mavsdk::Camera::CaptureInfo> capture_info_events;
    auto context = std::make_shared<grpc::ClientContext>();

    auto mode_events_future = subscribeCaptureInfoAsync(capture_info_events, context);
    _callback_saved_future.wait();
    context->TryCancel();
    capture_info_callbacks(expected_capture_info);
    mode_events_future.wait();
}

std::future<void> CameraServiceImplTest::subscribeCaptureInfoAsync(
    std::vector<mavsdk::Camera::CaptureInfo>& capture_info_events,
    std::shared_ptr<grpc::ClientContext> context) const
{
    return std::async(std::launch::async, [this, context, &capture_info_events]() {
        mavsdk::rpc::camera::SubscribeCaptureInfoRequest request;
        auto response_reader = _stub->SubscribeCaptureInfo(context.get(), request);

        mavsdk::rpc::camera::CaptureInfoResponse response;
        while (response_reader->Read(&response)) {
            capture_info_events.push_back(
                CameraServiceImpl::translateFromRpcCaptureInfo(response.capture_info()));
        }

        response_reader->Finish();
    });
}

std::unique_ptr<mavsdk::rpc::camera::CaptureInfo>
CameraServiceImplTest::createArbitraryRPCCaptureInfo() const
{
    auto rpc_info = std::make_unique<mavsdk::rpc::camera::CaptureInfo>();
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

std::unique_ptr<mavsdk::rpc::camera::Position> CameraServiceImplTest::createRPCPosition(
    const double lat, const double lng, const float abs_alt, const float rel_alt) const
{
    auto expected_position = std::make_unique<mavsdk::rpc::camera::Position>();

    expected_position->set_latitude_deg(lat);
    expected_position->set_longitude_deg(lng);
    expected_position->set_absolute_altitude_m(abs_alt);
    expected_position->set_relative_altitude_m(rel_alt);

    return expected_position;
}

std::unique_ptr<mavsdk::rpc::camera::Quaternion> CameraServiceImplTest::createRPCAttitudeQuaternion(
    const float w, const float x, const float y, const float z) const
{
    auto quaternion = std::make_unique<mavsdk::rpc::camera::Quaternion>();

    quaternion->set_w(w);
    quaternion->set_x(x);
    quaternion->set_y(y);
    quaternion->set_z(z);

    return quaternion;
}

std::unique_ptr<mavsdk::rpc::camera::EulerAngle> CameraServiceImplTest::createRPCAttitudeEulerAngle(
    const float yaw_deg, const float pitch_deg, const float roll_deg) const
{
    auto euler_angle = std::make_unique<mavsdk::rpc::camera::EulerAngle>();

    euler_angle->set_yaw_deg(yaw_deg);
    euler_angle->set_pitch_deg(pitch_deg);
    euler_angle->set_roll_deg(roll_deg);

    return euler_angle;
}

TEST_F(CameraServiceImplTest, sendsOneCaptureInfo)
{
    std::vector<mavsdk::Camera::CaptureInfo> capture_info_events;
    auto capture_info_event = createArbitraryRPCCaptureInfo();
    capture_info_events.push_back(
        CameraServiceImpl::translateFromRpcCaptureInfo(*capture_info_event));

    checkSendsCaptureInfo(capture_info_events);
}

void CameraServiceImplTest::checkSendsCaptureInfo(
    const std::vector<mavsdk::Camera::CaptureInfo>& capture_info_events) const
{
    std::promise<void> subscription_promise;
    auto subscription_future = subscription_promise.get_future();
    mavsdk::CallbackList<mavsdk::Camera::CaptureInfo> capture_info_callbacks;
    auto context = std::make_shared<grpc::ClientContext>();
    EXPECT_CALL(_camera, subscribe_capture_info(_))
        .WillOnce(SaveResult(&capture_info_callbacks, &subscription_promise));

    std::vector<mavsdk::Camera::CaptureInfo> received_capture_info_events;
    auto capture_info_events_future =
        subscribeCaptureInfoAsync(received_capture_info_events, context);
    subscription_future.wait();
    for (const auto& capture_info_event : capture_info_events) {
        capture_info_callbacks(capture_info_event);
    }
    context->TryCancel();
    auto arbitrary_capture_info_event = createArbitraryRPCCaptureInfo();
    capture_info_callbacks(
        CameraServiceImpl::translateFromRpcCaptureInfo(*arbitrary_capture_info_event));
    capture_info_events_future.wait();

    ASSERT_EQ(capture_info_events.size(), received_capture_info_events.size());
    for (size_t i = 0; i < capture_info_events.size(); i++) {
        EXPECT_EQ(capture_info_events.at(i), received_capture_info_events.at(i));
    }
}

TEST_F(CameraServiceImplTest, sendsMultipleCaptureInfos)
{
    std::vector<mavsdk::Camera::CaptureInfo> capture_info_events;

    capture_info_events.push_back(createArbitraryCaptureInfo());
    capture_info_events.push_back(createArbitraryCaptureInfo());
    capture_info_events.push_back(createArbitraryCaptureInfo());
    capture_info_events.push_back(createArbitraryCaptureInfo());

    checkSendsCaptureInfo(capture_info_events);
}

mavsdk::Camera::CaptureInfo CameraServiceImplTest::createArbitraryCaptureInfo() const
{
    mavsdk::Camera::Position position{};
    position.latitude_deg = 12.12223;
    position.longitude_deg = 24.342;
    position.absolute_altitude_m = 2334.2f;
    position.relative_altitude_m = 54.12f;

    mavsdk::Camera::Quaternion quaternion;
    quaternion.w = 12.3f;
    quaternion.x = 0.3f;
    quaternion.y = 1.1f;
    quaternion.z = -4.2f;

    mavsdk::Camera::EulerAngle euler_angle;
    euler_angle.yaw_deg = 12.6f;
    euler_angle.pitch_deg = 102.3f;
    euler_angle.roll_deg = -24.0f;

    mavsdk::Camera::CaptureInfo capture_info{};
    capture_info.position = position;
    capture_info.attitude_quaternion = quaternion;
    capture_info.attitude_euler_angle = euler_angle;
    capture_info.time_utc_us = ARBITRARY_INT;
    capture_info.is_success = ARBITRARY_BOOL;
    capture_info.index = ARBITRARY_INT;
    capture_info.file_url = ARBITRARY_URI;

    return capture_info;
}

TEST_F(CameraServiceImplTest, registersToStatus)
{
    const auto expected_camera_status = createStatus(
        false, true, ARBITRARY_CAMERA_STORAGE_STATUS, 3.4f, 12.6f, 16.0f, 0.4f, "100E90HD");
    mavsdk::CallbackList<mavsdk::Camera::Status> status_callbacks;
    EXPECT_CALL(_camera, subscribe_status(_))
        .WillOnce(SaveResult(&status_callbacks, &_callback_saved_promise));
    std::vector<mavsdk::Camera::Status> camera_status_events;
    auto context = std::make_shared<grpc::ClientContext>();

    auto mode_events_future = subscribeStatusAsync(camera_status_events, context);
    _callback_saved_future.wait();
    context->TryCancel();
    status_callbacks(expected_camera_status);
    mode_events_future.wait();
}

mavsdk::Camera::Status CameraServiceImplTest::createStatus(
    const bool is_video_on,
    const bool is_photo_interval_on,
    const mavsdk::Camera::Status::StorageStatus storage_status,
    const float used_storage_mib,
    const float available_storage_mib,
    const float total_storage_mib,
    const float recording_time_s,
    const std::string media_folder_name) const
{
    mavsdk::Camera::Status status{};
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

std::future<void> CameraServiceImplTest::subscribeStatusAsync(
    std::vector<mavsdk::Camera::Status>& camera_status_events,
    std::shared_ptr<grpc::ClientContext> context) const
{
    return std::async(std::launch::async, [this, context, &camera_status_events]() {
        mavsdk::rpc::camera::SubscribeStatusRequest request;
        auto response_reader = _stub->SubscribeStatus(context.get(), request);

        mavsdk::rpc::camera::StatusResponse response;
        while (response_reader->Read(&response)) {
            camera_status_events.push_back(
                CameraServiceImpl::translateFromRpcStatus(response.camera_status()));
        }

        response_reader->Finish();
    });
}

TEST_F(CameraServiceImplTest, sendsOneStatus)
{
    std::vector<mavsdk::Camera::Status> camera_status_events;
    auto camera_status_event = createStatus(
        false, true, ARBITRARY_CAMERA_STORAGE_STATUS, 3.4f, 12.6f, 16.0f, 22.4f, "105E90HD");
    camera_status_events.push_back(camera_status_event);

    checkSendsStatus(camera_status_events);
}

void CameraServiceImplTest::checkSendsStatus(
    const std::vector<mavsdk::Camera::Status>& camera_status_events) const
{
    std::promise<void> subscription_promise;
    auto subscription_future = subscription_promise.get_future();
    mavsdk::CallbackList<mavsdk::Camera::Status> camera_status_callbacks;
    auto context = std::make_shared<grpc::ClientContext>();
    EXPECT_CALL(_camera, subscribe_status(_))
        .WillOnce(SaveResult(&camera_status_callbacks, &subscription_promise));

    std::vector<mavsdk::Camera::Status> received_camera_status_events;
    auto camera_status_events_future = subscribeStatusAsync(received_camera_status_events, context);
    subscription_future.wait();
    for (const auto& camera_status_event : camera_status_events) {
        camera_status_callbacks(camera_status_event);
    }
    context->TryCancel();
    auto arbitrary_camera_status_event = createStatus(
        false, true, ARBITRARY_CAMERA_STORAGE_STATUS, 3.4f, 12.6f, 16.0f, 0.0f, "111E90HD");
    camera_status_callbacks(arbitrary_camera_status_event);
    camera_status_events_future.wait();

    ASSERT_EQ(camera_status_events.size(), received_camera_status_events.size());
    for (size_t i = 0; i < camera_status_events.size(); i++) {
        EXPECT_EQ(camera_status_events.at(i), received_camera_status_events.at(i));
    }
}

TEST_F(CameraServiceImplTest, sendsMultipleStatus)
{
    std::vector<mavsdk::Camera::Status> camera_status_events;

    camera_status_events.push_back(createStatus(
        true,
        true,
        mavsdk::Camera::Status::StorageStatus::Unformatted,
        1.2f,
        3.4f,
        2.2f,
        2.3f,
        "104E90HD"));
    camera_status_events.push_back(createStatus(
        true,
        false,
        mavsdk::Camera::Status::StorageStatus::Formatted,
        11.2f,
        58.4f,
        8.65f,
        2.2f,
        "360E90HD"));
    camera_status_events.push_back(createStatus(
        false,
        false,
        mavsdk::Camera::Status::StorageStatus::NotAvailable,
        1.5f,
        8.1f,
        6.3f,
        1.4f,
        "GOPRO621"));

    checkSendsStatus(camera_status_events);
}

TEST_F(CameraServiceImplTest, registersToCurrentSettings)
{
    std::vector<mavsdk::Camera::Setting> current_settings;
    current_settings.push_back(createSetting(
        ARBITRARY_SETTING_ID,
        ARBITRARY_SETTING_DESCRIPTION,
        createOption(ARBITRARY_OPTION_ID, ARBITRARY_OPTION_DESCRIPTION)));
    mavsdk::CallbackList<std::vector<mavsdk::Camera::Setting>> current_settings_callbacks;
    EXPECT_CALL(_camera, subscribe_current_settings(_))
        .WillOnce(SaveResult(&current_settings_callbacks, &_callback_saved_promise));
    std::vector<std::vector<mavsdk::Camera::Setting>> current_settings_events;
    auto context = std::make_shared<grpc::ClientContext>();

    auto mode_events_future = subscribeCurrentSettingsAsync(current_settings_events, context);
    _callback_saved_future.wait();
    context->TryCancel();
    current_settings_callbacks(current_settings);
    mode_events_future.wait();
}

mavsdk::Camera::Setting CameraServiceImplTest::createSetting(
    const std::string setting_id,
    const std::string setting_description,
    const mavsdk::Camera::Option& option) const
{
    mavsdk::Camera::Setting setting{};
    setting.setting_id = setting_id;
    setting.setting_description = setting_description;
    setting.option = option;

    return setting;
}

mavsdk::Camera::Option CameraServiceImplTest::createOption(
    const std::string option_id, const std::string option_description) const
{
    mavsdk::Camera::Option option;
    option.option_id = option_id;
    option.option_description = option_description;

    return option;
}

std::future<void> CameraServiceImplTest::subscribeCurrentSettingsAsync(
    std::vector<std::vector<mavsdk::Camera::Setting>>& current_settings_events,
    std::shared_ptr<grpc::ClientContext> context) const
{
    return std::async(std::launch::async, [this, context, &current_settings_events]() {
        mavsdk::rpc::camera::SubscribeCurrentSettingsRequest request;
        auto response_reader = _stub->SubscribeCurrentSettings(context.get(), request);

        mavsdk::rpc::camera::CurrentSettingsResponse response;
        while (response_reader->Read(&response)) {
            std::vector<mavsdk::Camera::Setting> response_settings;

            for (auto current_setting : response.current_settings()) {
                response_settings.push_back(
                    CameraServiceImpl::translateFromRpcSetting(current_setting));
            }

            current_settings_events.push_back(response_settings);
        }

        response_reader->Finish();
    });
}

TEST_F(CameraServiceImplTest, sendsOneCurrentSettings)
{
    std::vector<std::vector<mavsdk::Camera::Setting>> current_settings_events;

    std::vector<mavsdk::Camera::Setting> current_settings;
    current_settings.push_back(createSetting(
        ARBITRARY_SETTING_ID,
        ARBITRARY_SETTING_DESCRIPTION,
        createOption(ARBITRARY_OPTION_ID, ARBITRARY_OPTION_DESCRIPTION)));
    current_settings_events.push_back(current_settings);

    checkSendsCurrentSettings(current_settings_events);
}

void CameraServiceImplTest::checkSendsCurrentSettings(
    const std::vector<std::vector<mavsdk::Camera::Setting>>& current_settings_events) const
{
    std::promise<void> subscription_promise;
    auto subscription_future = subscription_promise.get_future();
    mavsdk::CallbackList<std::vector<mavsdk::Camera::Setting>> current_settings_callbacks;
    auto context = std::make_shared<grpc::ClientContext>();
    EXPECT_CALL(_camera, subscribe_current_settings(_))
        .WillOnce(SaveResult(&current_settings_callbacks, &subscription_promise));

    std::vector<std::vector<mavsdk::Camera::Setting>> received_current_settings_events;
    auto current_settings_events_future =
        subscribeCurrentSettingsAsync(received_current_settings_events, context);
    subscription_future.wait();
    for (const auto& current_settings_event : current_settings_events) {
        current_settings_callbacks(current_settings_event);
    }
    context->TryCancel();
    std::vector<mavsdk::Camera::Setting> arbitrary_current_settings_event;
    arbitrary_current_settings_event.push_back(createSetting(
        ARBITRARY_SETTING_ID,
        ARBITRARY_SETTING_DESCRIPTION,
        createOption(ARBITRARY_OPTION_ID, ARBITRARY_OPTION_DESCRIPTION)));
    current_settings_callbacks(arbitrary_current_settings_event);
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
    std::vector<std::vector<mavsdk::Camera::Setting>> current_settings_events;

    std::vector<mavsdk::Camera::Setting> current_settings1;
    current_settings1.push_back(createSetting(
        "arbitrary_setting_id1",
        "Arbitrary setting id one",
        createOption("arbitrary_option_id1", "Arbitrary Option ID one")));
    current_settings1.push_back(createSetting(
        "arbitrary_setting_id2",
        "Arbitrary setting id two",
        createOption("arbitrary_option_id2", "Arbitrary Option ID two")));
    current_settings1.push_back(createSetting(
        "arbitrary_setting_id3",
        "Arbitrary setting id three",
        createOption("arbitrary_option_id3", "Arbitrary Option ID three")));
    current_settings_events.push_back(current_settings1);

    std::vector<mavsdk::Camera::Setting> current_settings2;
    current_settings2.push_back(createSetting(
        "arbitrary_setting_id4",
        "Arbitrary setting id four",
        createOption("arbitrary_option_id4", "Arbitrary Option ID four")));
    current_settings2.push_back(createSetting(
        "arbitrary_setting_id5",
        "Arbitrary setting id five",
        createOption("arbitrary_option_id5", "Arbitrary Option ID five")));
    current_settings2.push_back(createSetting(
        "arbitrary_setting_id6",
        "Arbitrary setting id six",
        createOption("arbitrary_option_id6", "Arbitrary Option ID six")));
    current_settings_events.push_back(current_settings2);

    checkSendsCurrentSettings(current_settings_events);
}

TEST_F(CameraServiceImplTest, registersToPossibleSettings)
{
    std::vector<mavsdk::Camera::SettingOptions> possible_settings;
    std::vector<mavsdk::Camera::Option> options;
    options.push_back(createOption(ARBITRARY_OPTION_ID, ARBITRARY_OPTION_DESCRIPTION));
    possible_settings.push_back(
        createSettingOptions(ARBITRARY_SETTING_ID, ARBITRARY_SETTING_DESCRIPTION, options));
    mavsdk::CallbackList<std::vector<mavsdk::Camera::SettingOptions>> possible_settings_callbacks;
    EXPECT_CALL(_camera, subscribe_possible_setting_options(_))
        .WillOnce(SaveResult(&possible_settings_callbacks, &_callback_saved_promise));
    std::vector<std::vector<mavsdk::Camera::SettingOptions>> possible_settings_events;
    auto context = std::make_shared<grpc::ClientContext>();

    auto mode_events_future =
        subscribePossibleSettingOptionsAsync(possible_settings_events, context);
    _callback_saved_future.wait();
    context->TryCancel();
    possible_settings_callbacks(possible_settings);
    mode_events_future.wait();
}

mavsdk::Camera::SettingOptions CameraServiceImplTest::createSettingOptions(
    const std::string setting_id,
    const std::string setting_description,
    const std::vector<mavsdk::Camera::Option>& options) const
{
    mavsdk::Camera::SettingOptions setting_options;
    setting_options.setting_id = setting_id;
    setting_options.setting_description = setting_description;
    setting_options.options = options;

    return setting_options;
}

std::future<void> CameraServiceImplTest::subscribePossibleSettingOptionsAsync(
    std::vector<std::vector<mavsdk::Camera::SettingOptions>>& possible_settings_events,
    std::shared_ptr<grpc::ClientContext> context) const
{
    return std::async(std::launch::async, [this, context, &possible_settings_events]() {
        mavsdk::rpc::camera::SubscribePossibleSettingOptionsRequest request;
        auto response_reader = _stub->SubscribePossibleSettingOptions(context.get(), request);

        mavsdk::rpc::camera::PossibleSettingOptionsResponse response{};
        while (response_reader->Read(&response)) {
            std::vector<mavsdk::Camera::SettingOptions> response_setting_options;

            for (auto setting_options : response.setting_options()) {
                response_setting_options.push_back(
                    CameraServiceImpl::translateFromRpcSettingOptions(setting_options));
            }

            possible_settings_events.push_back(response_setting_options);
        }

        response_reader->Finish();
    });
}

TEST_F(CameraServiceImplTest, sendsOnePossibleSettingOptions)
{
    std::vector<std::vector<mavsdk::Camera::SettingOptions>> possible_setting_options_events;

    std::vector<mavsdk::Camera::SettingOptions> possible_setting_options;
    std::vector<mavsdk::Camera::Option> options;
    options.push_back(createOption(ARBITRARY_OPTION_ID, ARBITRARY_OPTION_DESCRIPTION));
    possible_setting_options.push_back(
        createSettingOptions(ARBITRARY_SETTING_ID, ARBITRARY_SETTING_DESCRIPTION, options));
    possible_setting_options_events.push_back(possible_setting_options);

    checkSendsPossibleSettingOptions(possible_setting_options_events);
}

void CameraServiceImplTest::checkSendsPossibleSettingOptions(
    const std::vector<std::vector<mavsdk::Camera::SettingOptions>>& possible_setting_options_events)
    const
{
    std::promise<void> subscription_promise;
    auto subscription_future = subscription_promise.get_future();
    mavsdk::CallbackList<std::vector<mavsdk::Camera::SettingOptions>>
        possible_setting_options_callbacks;
    auto context = std::make_shared<grpc::ClientContext>();
    EXPECT_CALL(_camera, subscribe_possible_setting_options(_))
        .WillOnce(SaveResult(&possible_setting_options_callbacks, &subscription_promise));

    std::vector<std::vector<mavsdk::Camera::SettingOptions>>
        received_possible_setting_options_events;
    auto possible_setting_options_events_future =
        subscribePossibleSettingOptionsAsync(received_possible_setting_options_events, context);
    subscription_future.wait();
    for (const auto& possible_setting_options_event : possible_setting_options_events) {
        possible_setting_options_callbacks(possible_setting_options_event);
    }
    context->TryCancel();

    std::vector<mavsdk::Camera::SettingOptions> arbitrary_possible_setting_options;
    std::vector<mavsdk::Camera::Option> options;
    options.push_back(createOption(ARBITRARY_OPTION_ID, ARBITRARY_OPTION_DESCRIPTION));
    arbitrary_possible_setting_options.push_back(
        createSettingOptions(ARBITRARY_SETTING_ID, ARBITRARY_SETTING_DESCRIPTION, options));
    possible_setting_options_callbacks(arbitrary_possible_setting_options);
    possible_setting_options_events_future.wait();

    ASSERT_EQ(
        possible_setting_options_events.size(), received_possible_setting_options_events.size());
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
    std::vector<std::vector<mavsdk::Camera::SettingOptions>> possible_setting_options_events;

    std::vector<mavsdk::Camera::SettingOptions> possible_setting_options;

    std::vector<mavsdk::Camera::Option> options1;
    options1.push_back(createOption("option1_1", "Option One One"));
    options1.push_back(createOption("option1_2", "Option One Two"));
    options1.push_back(createOption("option1_3", "Option One Three"));
    possible_setting_options.push_back(createSettingOptions("setting1", "Setting one", options1));
    std::vector<mavsdk::Camera::Option> options2;
    options2.push_back(createOption("option1", "Option One"));
    options2.push_back(createOption("option2", "Option One"));
    possible_setting_options.push_back(createSettingOptions("setting2", "Setting two", options2));

    std::vector<mavsdk::Camera::Option> options3;
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
    mavsdk::rpc::camera::SetSettingRequest request;
    request.set_allocated_setting(createRPCSetting(
                                      "arbitrary_setting_id",
                                      "arbitrary_setting_description",
                                      "arbitrary_option_id",
                                      "arbitrary_option_description")
                                      .release());
    auto response = std::make_shared<mavsdk::rpc::camera::SetSettingResponse>();

    setSettingAndSaveParams(request, response, GetParam().second);

    EXPECT_EQ(GetParam().first, CameraResult::Result_Name(response->camera_result().result()));
}

void CameraServiceImplTest::setSettingAndSaveParams(
    const mavsdk::rpc::camera::SetSettingRequest& request,
    std::shared_ptr<mavsdk::rpc::camera::SetSettingResponse> response,
    mavsdk::Camera::Result result)
{
    EXPECT_CALL(_camera, set_setting(_)).WillOnce(Return(result));

    _camera_service.SetSetting(nullptr, &request, response.get());
}

std::unique_ptr<mavsdk::rpc::camera::Setting> CameraServiceImplTest::createRPCSetting(
    const std::string& setting_id,
    const std::string& setting_description,
    const std::string& option_id,
    const std::string& option_description)
{
    auto setting = std::make_unique<mavsdk::rpc::camera::Setting>();
    setting->set_setting_id(setting_id);
    setting->set_setting_description(setting_description);

    auto option = std::make_unique<mavsdk::rpc::camera::Option>();
    option->set_option_id(option_id);
    option->set_option_description(option_description);
    setting->set_allocated_option(option.release());

    return setting;
}

TEST_F(CameraServiceImplTest, setsSettingEvenWhenContextAndResponseAreNull)
{
    mavsdk::Camera::ResultCallback result_callback;
    mavsdk::rpc::camera::SetSettingRequest request;
    request.set_allocated_setting(createRPCSetting(
                                      ARBITRARY_SETTING_ID,
                                      ARBITRARY_SETTING_DESCRIPTION,
                                      ARBITRARY_OPTION_ID,
                                      ARBITRARY_OPTION_DESCRIPTION)
                                      .release());
    auto response = std::make_shared<mavsdk::rpc::camera::SetSettingResponse>();

    setSettingAndSaveParams(request, nullptr, ARBITRARY_CAMERA_RESULT);
}

TEST_F(CameraServiceImplTest, setsSettingWithRightParameter)
{
    mavsdk::Camera::ResultCallback result_callback;
    mavsdk::rpc::camera::SetSettingRequest request;
    request.set_allocated_setting(createRPCSetting(
                                      ARBITRARY_SETTING_ID,
                                      ARBITRARY_SETTING_DESCRIPTION,
                                      ARBITRARY_OPTION_ID,
                                      ARBITRARY_OPTION_DESCRIPTION)
                                      .release());

    mavsdk::Camera::Setting setting;
    setting.setting_id = ARBITRARY_SETTING_ID;
    setting.setting_description = ARBITRARY_SETTING_DESCRIPTION;
    setting.option.option_id = ARBITRARY_OPTION_ID;
    setting.option.option_description = ARBITRARY_OPTION_DESCRIPTION;

    EXPECT_CALL(_camera, set_setting(setting)).WillOnce(Return(ARBITRARY_CAMERA_RESULT));

    _camera_service.SetSetting(nullptr, &request, nullptr);
}

TEST_F(CameraServiceImplTest, setSettingReturnsWrongArgumentErrorIfRequestIsNull)
{
    mavsdk::rpc::camera::SetSettingResponse response;

    _camera_service.SetSetting(nullptr, nullptr, &response);

    EXPECT_EQ("RESULT_UNKNOWN", CameraResult::Result_Name(response.camera_result().result()));
}

TEST_F(CameraServiceImplTest, setSettingDoesNotCrashWhenArgsAreNull)
{
    _camera_service.SetSetting(nullptr, nullptr, nullptr);
}

INSTANTIATE_TEST_SUITE_P(
    CameraResultCorrespondences, CameraServiceImplTest, ::testing::ValuesIn(generateInputPairs()));

std::vector<InputPair> generateInputPairs()
{
    std::vector<InputPair> input_pairs;
    input_pairs.push_back(std::make_pair("RESULT_SUCCESS", mavsdk::Camera::Result::Success));
    input_pairs.push_back(std::make_pair("RESULT_BUSY", mavsdk::Camera::Result::Busy));
    input_pairs.push_back(std::make_pair("RESULT_DENIED", mavsdk::Camera::Result::Denied));
    input_pairs.push_back(std::make_pair("RESULT_ERROR", mavsdk::Camera::Result::Error));
    input_pairs.push_back(std::make_pair("RESULT_TIMEOUT", mavsdk::Camera::Result::Timeout));
    input_pairs.push_back(
        std::make_pair("RESULT_WRONG_ARGUMENT", mavsdk::Camera::Result::WrongArgument));
    input_pairs.push_back(std::make_pair("RESULT_UNKNOWN", mavsdk::Camera::Result::Unknown));

    return input_pairs;
}

} // namespace
