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
    MOCK_CONST_METHOD1(start_video_streaming, Camera::Result(int32_t stream_id)){};
    MOCK_CONST_METHOD1(stop_video_streaming, Camera::Result(int32_t strema_id)){};
    MOCK_CONST_METHOD1(set_mode, Camera::Result(Camera::Mode)){};
    MOCK_CONST_METHOD1(set_setting, Camera::Result(Camera::Setting)){};
    MOCK_CONST_METHOD1(get_setting, std::pair<Camera::Result, Camera::Setting>(Camera::Setting)){};
    MOCK_CONST_METHOD1(subscribe_mode, Camera::ModeHandle(Camera::ModeCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_mode, void(Camera::ModeHandle)){};
    MOCK_CONST_METHOD1(set_video_stream_settings, void(Camera::VideoStreamSettings)){};
    MOCK_CONST_METHOD1(
        subscribe_video_stream_info,
        Camera::VideoStreamInfoHandle(Camera::VideoStreamInfoCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_video_stream_info, void(Camera::VideoStreamInfoHandle)){};
    MOCK_CONST_METHOD1(
        subscribe_capture_info, Camera::CaptureInfoHandle(Camera::CaptureInfoCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_capture_info, void(Camera::CaptureInfoHandle)){};
    MOCK_CONST_METHOD1(subscribe_status, Camera::StatusHandle(Camera::StatusCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_status, void(Camera::StatusHandle)){};
    MOCK_CONST_METHOD1(
        subscribe_information, Camera::InformationHandle(Camera::InformationCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_information, void(Camera::InformationHandle)){};
    MOCK_CONST_METHOD1(
        subscribe_current_settings,
        Camera::CurrentSettingsHandle(Camera::CurrentSettingsCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_current_settings, void(Camera::CurrentSettingsHandle)){};
    MOCK_CONST_METHOD1(
        subscribe_possible_setting_options,
        Camera::PossibleSettingOptionsHandle(Camera::PossibleSettingOptionsCallback)){};
    MOCK_CONST_METHOD1(
        unsubscribe_possible_setting_options, void(Camera::PossibleSettingOptionsHandle)){};
    MOCK_CONST_METHOD3(
        set_option_async, void(Camera::ResultCallback, const std::string&, const Camera::Option)){};
    MOCK_CONST_METHOD1(format_storage, Camera::Result(int32_t)){};
    MOCK_CONST_METHOD1(
        list_photos,
        std::pair<Camera::Result, std::vector<Camera::CaptureInfo>>(Camera::PhotosRange)){};
    MOCK_CONST_METHOD1(select_camera, Camera::Result(int32_t)){};
    MOCK_CONST_METHOD0(reset_settings, Camera::Result()){};

    MOCK_CONST_METHOD0(zoom_in_start, Camera::Result()){};
    MOCK_CONST_METHOD0(zoom_out_start, Camera::Result()){};
    MOCK_CONST_METHOD0(zoom_stop, Camera::Result()){};
    MOCK_CONST_METHOD1(zoom_range, Camera::Result(float)){};
    MOCK_CONST_METHOD3(track_point, Camera::Result(float, float, float)){};
    MOCK_CONST_METHOD4(track_rectangle, Camera::Result(float, float, float, float)){};
    MOCK_CONST_METHOD0(track_stop, Camera::Result()){};
    MOCK_CONST_METHOD0(focus_in_start, Camera::Result()){};
    MOCK_CONST_METHOD0(focus_out_start, Camera::Result()){};
    MOCK_CONST_METHOD0(focus_stop, Camera::Result()){};
    MOCK_CONST_METHOD1(focus_range, Camera::Result(float)){};
};

} // namespace testing
} // namespace mavsdk
