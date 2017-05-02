#include "mavlink_channels.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace dronelink;


int main(int argc, char *argv[])
{
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
