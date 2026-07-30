#include "mavlink_parameter_helper.hpp"
#include <gtest/gtest.h>
#include <cstring>

using namespace mavsdk;

TEST(MavlinkParameterHelper, ExtractSafeParamIdNullTerminated)
{
    char buf[PARAM_ID_LEN] = {};
    std::memcpy(buf, "MPC_XY_VEL", 10);
    EXPECT_EQ(extract_safe_param_id(buf), "MPC_XY_VEL");
}

TEST(MavlinkParameterHelper, ExtractSafeParamIdFull16NoNull)
{
    // Exactly 16 non-null bytes — classic MAVLink non-terminated case.
    char buf[PARAM_ID_LEN];
    std::memset(buf, 'A', PARAM_ID_LEN);
    EXPECT_EQ(extract_safe_param_id(buf), std::string(PARAM_ID_LEN, 'A'));
}

TEST(MavlinkParameterHelper, ExtractSafeParamIdNullptr)
{
    EXPECT_EQ(extract_safe_param_id(nullptr), "");
}

TEST(MavlinkParameterHelper, ParamIdToMessageBufferShort)
{
    auto arr = param_id_to_message_buffer("ABC");
    EXPECT_EQ(arr[0], 'A');
    EXPECT_EQ(arr[1], 'B');
    EXPECT_EQ(arr[2], 'C');
    EXPECT_EQ(arr[3], '\0');
    for (size_t i = 3; i < PARAM_ID_LEN; ++i) {
        EXPECT_EQ(arr[i], '\0');
    }
}

TEST(MavlinkParameterHelper, ParamIdToMessageBufferClampsLongInput)
{
    const std::string long_id(PARAM_ID_LEN + 8, 'Z');
    auto arr = param_id_to_message_buffer(long_id);
    for (size_t i = 0; i < PARAM_ID_LEN; ++i) {
        EXPECT_EQ(arr[i], 'Z');
    }
}

TEST(MavlinkParameterHelper, RoundTripShortId)
{
    auto arr = param_id_to_message_buffer("SYS_AUTOSTART");
    EXPECT_EQ(extract_safe_param_id(arr.data()), "SYS_AUTOSTART");
}
