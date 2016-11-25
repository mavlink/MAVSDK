#pragma once

#include <iostream>
#include <gtest/gtest.h>

namespace sitl {

static void start()
{
    system("./start_px4_sitl.sh");
    // We need to wait a bit until it's up and running.
    sleep(2);
}

static void stop()
{
    // Don't rush this either.
    sleep(2);
    system("./stop_px4_sitl.sh");
}

} // namespace sitl
