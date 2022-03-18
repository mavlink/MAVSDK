#include <gmock/gmock.h>

#include "plugins/camera/camera.h"

namespace mavsdk {
namespace testing {

class MockCamera {
public:
    MOCK_CONST_METHOD0(prepare, Camera::Result()){};
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
    MOCK_CONST_METHOD1(subscribe_mode, void(Camera::ModeCallback)){};
    MOCK_CONST_METHOD1(set_video_stream_settings, void(Camera::VideoStreamSettings)){};
    MOCK_CONST_METHOD1(subscribe_video_stream_info, void(Camera::VideoStreamInfoCallback)){};
    MOCK_CONST_METHOD1(subscribe_capture_info, void(Camera::CaptureInfoCallback)){};
    MOCK_CONST_METHOD1(subscribe_status, void(Camera::StatusCallback)){};
    MOCK_CONST_METHOD1(subscribe_information, void(Camera::InformationCallback)){};
    MOCK_CONST_METHOD1(subscribe_current_settings, void(Camera::CurrentSettingsCallback)){};
    MOCK_CONST_METHOD1(
        subscribe_possible_setting_options, void(Camera::PossibleSettingOptionsCallback)){};
    MOCK_CONST_METHOD3(
        set_option_async, void(Camera::ResultCallback, const std::string&, const Camera::Option)){};
    MOCK_CONST_METHOD0(format_storage, Camera::Result()){};
    MOCK_CONST_METHOD1(
        list_photos,
        std::pair<Camera::Result, std::vector<Camera::CaptureInfo>>(Camera::PhotosRange)){};
    MOCK_CONST_METHOD1(select_camera, Camera::Result(int32_t)){};
};

} // namespace testing
} // namespace mavsdk
