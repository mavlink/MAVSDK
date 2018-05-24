#include <gmock/gmock.h>

#include "camera/camera_service_impl.h"
#include "camera/mocks/camera_mock.h"

namespace {

using testing::_;
using testing::NiceMock;
using testing::Return;

using MockCamera = NiceMock<dronecore::testing::MockCamera>;
using CameraServiceImpl = dronecore::backend::CameraServiceImpl<MockCamera>;

using CameraResult = dronecore::rpc::camera::CameraResult;
using InputPair = std::pair<std::string, dronecore::Camera::Result>;

static constexpr auto ARBITRARY_FLOAT = 24.2f;

std::vector<InputPair> generateInputPairs();

class CameraServiceImplTest : public ::testing::TestWithParam<InputPair>
{
protected:
    CameraServiceImplTest()
        : _camera_service(_camera) {}

    MockCamera _camera;
    CameraServiceImpl _camera_service;
};

TEST_P(CameraServiceImplTest, takePhotoResultIsTranslatedCorrectly)
{
    ON_CALL(_camera, take_photo())
    .WillByDefault(Return(GetParam().second));
    dronecore::rpc::camera::TakePhotoResponse response;

    _camera_service.TakePhoto(nullptr, nullptr, &response);

    EXPECT_EQ(GetParam().first, CameraResult::Result_Name(response.camera_result().result()));
}

TEST_F(CameraServiceImplTest, takesPhotoEvenWhenArgsAreNull)
{
    EXPECT_CALL(_camera, take_photo())
    .Times(1);

    _camera_service.TakePhoto(nullptr, nullptr, nullptr);
}

TEST_P(CameraServiceImplTest, startPhotoIntervalResultIsTranslatedCorrectly)
{
    ON_CALL(_camera, start_photo_interval(_))
    .WillByDefault(Return(GetParam().second));
    dronecore::rpc::camera::StartPhotoIntervalRequest request;
    request.set_interval_s(ARBITRARY_FLOAT);
    dronecore::rpc::camera::StartPhotoIntervalResponse response;

    _camera_service.StartPhotoInterval(nullptr, &request, &response);

    EXPECT_EQ(GetParam().first, CameraResult::Result_Name(response.camera_result().result()));
}

TEST_F(CameraServiceImplTest, startsPhotoIntervalEvenWhenContextAndResponseAreNull)
{
    EXPECT_CALL(_camera, start_photo_interval(_))
    .Times(1);
    dronecore::rpc::camera::StartPhotoIntervalRequest request;
    request.set_interval_s(ARBITRARY_FLOAT);

    _camera_service.StartPhotoInterval(nullptr, &request, nullptr);
}

TEST_F(CameraServiceImplTest, startsPhotoIntervalWithRightParameter)
{
    auto expected_interval = ARBITRARY_FLOAT;
    EXPECT_CALL(_camera, start_photo_interval(expected_interval))
    .Times(1);
    dronecore::rpc::camera::StartPhotoIntervalRequest request;
    request.set_interval_s(expected_interval);

    _camera_service.StartPhotoInterval(nullptr, &request, nullptr);
}

TEST_F(CameraServiceImplTest, startPhotoIntervalReturnsWrongArgumentErrorIfRequestIsNull)
{
    dronecore::rpc::camera::StartPhotoIntervalResponse response;

    _camera_service.StartPhotoInterval(nullptr, nullptr, &response);

    EXPECT_EQ("WRONG_ARGUMENT", CameraResult::Result_Name(response.camera_result().result()));
}

INSTANTIATE_TEST_CASE_P(CameraResultCorrespondences,
                        CameraServiceImplTest,
                        ::testing::ValuesIn(generateInputPairs()));

std::vector<InputPair> generateInputPairs()
{
    std::vector<InputPair> input_pairs;
    input_pairs.push_back(std::make_pair("SUCCESS", dronecore::Camera::Result::SUCCESS));
    input_pairs.push_back(std::make_pair("IN_PROGRESS",
                                         dronecore::Camera::Result::IN_PROGRESS));
    input_pairs.push_back(std::make_pair("BUSY", dronecore::Camera::Result::BUSY));
    input_pairs.push_back(std::make_pair("DENIED", dronecore::Camera::Result::DENIED));
    input_pairs.push_back(std::make_pair("ERROR",
                                         dronecore::Camera::Result::ERROR));
    input_pairs.push_back(std::make_pair("TIMEOUT", dronecore::Camera::Result::TIMEOUT));
    input_pairs.push_back(std::make_pair("WRONG_ARGUMENT",
                                         dronecore::Camera::Result::WRONG_ARGUMENT));
    input_pairs.push_back(std::make_pair("UNKNOWN", dronecore::Camera::Result::UNKNOWN));

    return input_pairs;
}

} // namespace
