#pragma once

#include "dronecore.h"
#include "plugins/camera/camera.h"

dronecore::Camera::Mode get_mode(std::shared_ptr<dronecore::Camera> camera);
void set_mode(std::shared_ptr<dronecore::Camera> camera, dronecore::Camera::Mode mode);
