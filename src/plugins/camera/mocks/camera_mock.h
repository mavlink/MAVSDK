#include <gmock/gmock.h>

#include "plugins/camera/camera.h"

namespace mavsdk {
namespace testing {

class MockCamera {
public:
    MOCK_CONST_METHOD0(take_photo, Camera::Result()){};
    MOCK_CONST_METHOD1(start_photo_interval, Camera::Result(const float)){};
    MOCK_CONST_METHOD0(stop_photo_interval, Camera::Result()){};
    MOCK_CONST_METHOD0(start_video, Camera::Result()){};
    MOCK_CONST_METHOD0(stop_video, Camera::Result()){};
    MOCK_CONST_METHOD0(start_video_streaming, Camera::Result()){};
    MOCK_CONST_METHOD0(stop_video_streaming, Camera::Result()){};
    MOCK_CONST_METHOD1(set_mode, Camera::Result(Camera::Mode)){};
    MOCK_CONST_METHOD1(set_setting, Camera::Result(Camera::Setting)){};
    MOCK_CONST_METHOD1(get_setting, std::pair<Camera::Result, Camera::Setting>(Camera::Setting)){};
    MOCK_CONST_METHOD1(mode_async, void(Camera::mode_callback_t)){};
    MOCK_CONST_METHOD1(set_video_stream_settings, void(Camera::VideoStreamSettings)){};
    MOCK_CONST_METHOD1(video_stream_info_async, void(Camera::video_stream_info_callback_t)){};
    MOCK_CONST_METHOD1(capture_info_async, void(Camera::capture_info_callback_t)){};
    MOCK_CONST_METHOD1(status_async, void(Camera::status_callback_t)){};
    MOCK_CONST_METHOD1(information_async, void(Camera::information_callback_t)){};
    MOCK_CONST_METHOD1(current_settings_async, void(Camera::current_settings_callback_t)){};
    MOCK_CONST_METHOD1(
        possible_setting_options_async, void(Camera::possible_setting_options_callback_t)){};
    MOCK_CONST_METHOD3(
        set_option_async,
        void(Camera::result_callback_t, const std::string&, const Camera::Option)){};
    MOCK_CONST_METHOD0(format_storage, Camera::Result()){};
};

} // namespace testing
} // namespace mavsdk
