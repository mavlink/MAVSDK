#pragma once

#include <chrono>
#include <future>
#include <memory>

#include <gtest/gtest.h>

#include "mavsdk.h"
#include "log.h"

namespace mavsdk {

std::future<std::shared_ptr<System>> wait_for_first_system_detected(Mavsdk& mavsdk);

} // namespace mavsdk