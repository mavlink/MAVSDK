#pragma once

#include "dronecore.h"
#include "plugins/camera/camera.h"

dronecore::Camera::Mode get_mode(std::shared_ptr<dronecore::Camera> camera);
void set_mode(std::shared_ptr<dronecore::Camera> camera, dronecore::Camera::Mode mode);

dronecore::Camera::Result set_setting(std::shared_ptr<dronecore::Camera> camera,
                                      const std::string &setting,
                                      const std::string &option);

dronecore::Camera::Result get_setting(std::shared_ptr<dronecore::Camera> camera,
                                      const std::string &setting,
                                      std::string &option);
