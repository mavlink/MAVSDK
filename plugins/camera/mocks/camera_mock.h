#include <gmock/gmock.h>

#include "camera/camera.h"

namespace dronecore {
namespace testing {

class MockCamera
{
public:
    MOCK_CONST_METHOD0(take_photo, Camera::Result());
    MOCK_CONST_METHOD1(start_photo_interval, Camera::Result(float));
    MOCK_CONST_METHOD0(stop_photo_interval, Camera::Result());
    MOCK_CONST_METHOD0(start_video, Camera::Result());
    MOCK_CONST_METHOD0(stop_video, Camera::Result());
    MOCK_CONST_METHOD0(start_video_streaming, Camera::Result());
};

} // namespace testing
} // namespace dronecore
