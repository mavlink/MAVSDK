#pragma once

#include "mavsdk.h"
#include "plugins/camera/camera.h"

mavsdk::Camera::Result set_setting(
    std::shared_ptr<mavsdk::Camera> camera,
    const std::string& setting_id,
    const std::string& option_id);

mavsdk::Camera::Result get_setting(
    std::shared_ptr<mavsdk::Camera> camera, const std::string& setting_id, std::string& option_d);
