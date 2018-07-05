#include <gmock/gmock.h>

#include "camera/camera.h"

namespace dronecode_sdk {
namespace testing {

class MockCamera {
public:
    MOCK_CONST_METHOD0(take_photo, Camera::Result());
    MOCK_CONST_METHOD1(start_photo_interval, Camera::Result(float));
    MOCK_CONST_METHOD0(stop_photo_interval, Camera::Result());
    MOCK_CONST_METHOD0(start_video, Camera::Result());
    MOCK_CONST_METHOD0(stop_video, Camera::Result());
    MOCK_CONST_METHOD0(start_video_streaming, Camera::Result());
    MOCK_CONST_METHOD0(stop_video_streaming, Camera::Result());
    MOCK_CONST_METHOD1(set_mode, Camera::Result(Camera::Mode));
    MOCK_CONST_METHOD1(subscribe_mode, void(Camera::subscribe_mode_callback_t));
    MOCK_CONST_METHOD1(set_video_stream_settings, void(Camera::VideoStreamSettings));
    MOCK_CONST_METHOD1(subscribe_video_stream_info,
                       void(Camera::subscribe_video_stream_info_callback_t));
    MOCK_CONST_METHOD1(subscribe_capture_info, void(Camera::capture_info_callback_t));
};

} // namespace testing
} // namespace dronecode_sdk
