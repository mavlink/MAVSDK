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
    dronecore::rpc::camera::VideoStreamSettings *createArbitraryRPCVideoStreamSettings() const;

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
    for (size_t i = 0; i < modes.size(); i++) {
        mode_callback(modes.at(i));
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
    request.set_allocated_video_stream_settings(rpc_settings);

    _camera_service.SetVideoStreamSettings(nullptr, &request, nullptr);
}

dronecore::rpc::camera::VideoStreamSettings *
CameraServiceImplTest::createArbitraryRPCVideoStreamSettings() const
{
    auto rpc_settings(new dronecore::rpc::camera::VideoStreamSettings());
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
    const auto rpc_video_stream_settings = createArbitraryRPCVideoStreamSettings();
    const auto expected_video_stream_settings =
        CameraServiceImpl::translateRPCVideoStreamSettings(*rpc_video_stream_settings);
    EXPECT_CALL(_camera, set_video_stream_settings(expected_video_stream_settings)).Times(1);
    dronecore::rpc::camera::SetVideoStreamSettingsRequest request;
    request.set_allocated_video_stream_settings(rpc_video_stream_settings);

    _camera_service.SetVideoStreamSettings(nullptr, &request, nullptr);
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
