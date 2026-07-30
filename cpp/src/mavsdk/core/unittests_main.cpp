#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "mavsdk.hpp"

using namespace mavsdk;

int main(int argc, char* argv[])
{
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
