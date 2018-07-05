#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "dronecode_sdk.h"

using namespace dronecode_sdk;

int main(int argc, char *argv[])
{
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
