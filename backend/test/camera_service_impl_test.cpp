#include <gmock/gmock.h>

#include "camera/camera_service_impl.h"
#include "camera/mocks/camera_mock.h"

namespace {

using testing::NiceMock;
using testing::Return;

using MockCamera = NiceMock<dronecore::testing::MockCamera>;
using CameraServiceImpl = dronecore::backend::CameraServiceImpl<MockCamera>;

using CameraResult = dronecore::rpc::camera::CameraResult;
using InputPair = std::pair<std::string, dronecore::Camera::Result>;

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
