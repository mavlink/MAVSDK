#pragma once

#include <iostream>
#include <gtest/gtest.h>

namespace sitl {

static void start()
{
    system("./start_px4_sitl.sh");
}

static void stop()
{
    system("./stop_px4_sitl.sh");
}

} // namespace sitl
