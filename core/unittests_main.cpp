#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "dronecore.h"

using namespace dronecore;

int main(int argc, char *argv[])
{
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
