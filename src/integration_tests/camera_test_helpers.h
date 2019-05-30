#pragma once

#include "dronecode_sdk.h"
#include "plugins/camera/camera.h"

dronecode_sdk::Camera::Mode get_mode(std::shared_ptr<dronecode_sdk::Camera> camera);
void set_mode_async(std::shared_ptr<dronecode_sdk::Camera> camera,
                    dronecode_sdk::Camera::Mode mode);

dronecode_sdk::Camera::Result set_setting(std::shared_ptr<dronecode_sdk::Camera> camera,
                                          const std::string &setting,
                                          const std::string &option);

dronecode_sdk::Camera::Result get_setting(std::shared_ptr<dronecode_sdk::Camera> camera,
                                          const std::string &setting,
                                          std::string &option);
