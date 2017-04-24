#pragma once

#include <iostream>
#include <gtest/gtest.h>
#include "global_include.h"

class SitlTest : public testing::Test
{
protected:
    virtual void SetUp()
    {
        system("./start_px4_sitl.sh");
        // We need to wait a bit until it's up and running.
        sleep(3);
    }
    virtual void TearDown()
    {
        // Don't rush this either.
        sleep(1);
        system("./stop_px4_sitl.sh");
    }
};
