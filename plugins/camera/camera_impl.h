#pragma once

#include "device.h"
#include "camera.h"
#include "plugin_impl_base.h"

namespace dronecore {

class CameraImpl : public PluginImplBase
{
public:
    CameraImpl(Device &device);
    ~CameraImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Camera::Result take_photo();

    Camera::Result start_photo_interval(float interval_s);
    Camera::Result stop_photo_interval();

    Camera::Result start_video();
    Camera::Result stop_video();

    void take_photo_async(const Camera::result_callback_t &callback);

    void start_photo_interval_async(float interval_s, const Camera::result_callback_t &callback);
    void stop_photo_interval_async(const Camera::result_callback_t &callback);

    void start_video_async(const Camera::result_callback_t &callback);
    void stop_video_async(const Camera::result_callback_t &callback);

    void set_mode_async(Camera::Mode mode, const Camera::mode_callback_t &callback);
    void get_mode_async(const Camera::mode_callback_t &callback);

    // Non-copyable
    CameraImpl(const CameraImpl &) = delete;
    const CameraImpl &operator=(const CameraImpl &) = delete;

private:
    void receive_set_mode_command_result(MavlinkCommands::Result command_result,
                                         const Camera::mode_callback_t &callback,
                                         Camera::Mode mode);

    static Camera::Result camera_result_from_command_result(
        MavlinkCommands::Result command_result);

    static bool interval_valid(float interval_s);

    int _capture_sequence = 0;

};


} // namespace dronecore
