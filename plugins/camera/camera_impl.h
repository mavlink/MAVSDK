#pragma once

#include <memory>
#include <map>
#include <mutex>

#include "system.h"
#include "mavlink_system.h"
#include "mavlink_include.h"
#include "camera.h"
#include "plugin_impl_base.h"
#include <json11.hpp>

namespace dronecore {

class CameraImpl : public PluginImplBase
{
public:
    CameraImpl(System &system, uint8_t camera_id);
    ~CameraImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    // Non-copyable
    CameraImpl(const CameraImpl &) = delete;
    const CameraImpl &operator=(const CameraImpl &) = delete;

private:
    void on_receive_camera_information(const mavlink_message_t &message);

    uint8_t _camera_id;
    Camera::CameraInformation _camera_info;
};

} // namespace dronecore
