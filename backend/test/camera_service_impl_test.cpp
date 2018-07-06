#include <gmock/gmock.h>

#include "camera/camera_service_impl.h"
#include "camera/mocks/camera_mock.h"

namespace {

using testing::_;
using testing::NiceMock;
using testing::Return;

using MockCamera = NiceMock<dronecode_sdk::testing::MockCamera>;
using CameraServiceImpl = dronecode_sdk::backend::CameraServiceImpl<MockCamera>;

using CameraResult = dronecode_sdk::rpc::camera::CameraResult;
using InputPair = std::pair<std::string, dronecode_sdk::Camera::Result>;

static constexpr auto ARBITRARY_FLOAT = 24.2f;

std::vector<InputPair> generateInputPairs();

class CameraServiceImplTest : public ::testing::TestWithParam<InputPair> {
protected:
    CameraServiceImplTest() : _camera_service(_camera) {}

    MockCamera _camera;
    CameraServiceImpl _camera_service;
};

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
