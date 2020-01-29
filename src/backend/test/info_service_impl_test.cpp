#include <gmock/gmock.h>
#include <string>
#include <utility>
#include <vector>

#include "info/info_service_impl.h"
#include "info/mocks/info_mock.h"

namespace {

using testing::_;
using testing::NiceMock;
using testing::Return;

using MockInfo = NiceMock<mavsdk::testing::MockInfo>;
using InfoServiceImpl = mavsdk::backend::InfoServiceImpl<MockInfo>;

using InfoResult = mavsdk::rpc::info::InfoResult;
using InputPair = std::pair<std::string, mavsdk::Info::Result>;

static constexpr int ARBITRARY_SW_VERSION_MAJOR = 1;
static constexpr int ARBITRARY_SW_VERSION_MINOR = 2;
static constexpr int ARBITRARY_SW_VERSION_PATCH = 3;

static constexpr int ARBITRARY_SW_VERSION_VENDOR_MAJOR = 1;
static constexpr int ARBITRARY_SW_VERSION_VENDOR_MINOR = 2;
static constexpr int ARBITRARY_SW_VERSION_VENDOR_PATCH = 3;

static constexpr int ARBITRARY_SW_VERSION_OS_MAJOR = 4;
static constexpr int ARBITRARY_SW_VERSION_OS_MINOR = 5;
static constexpr int ARBITRARY_SW_VERSION_OS_PATCH = 6;

std::vector<InputPair> generateInputPairs();

class InfoServiceImplTest : public ::testing::TestWithParam<InputPair> {};

TEST_F(InfoServiceImplTest, getVersionCallsGetter)
{
    MockInfo info;
    InfoServiceImpl infoService(info);
    EXPECT_CALL(info, get_version()).Times(1);
    mavsdk::rpc::info::GetVersionResponse response;

    infoService.GetVersion(nullptr, nullptr, &response);
}

TEST_P(InfoServiceImplTest, getsCorrectVersion)
{
    MockInfo info;
    InfoServiceImpl infoService(info);

    mavsdk::Info::Version arbitrary_version;

    arbitrary_version.flight_sw_major = ARBITRARY_SW_VERSION_MAJOR;
    arbitrary_version.flight_sw_minor = ARBITRARY_SW_VERSION_MINOR;
    arbitrary_version.flight_sw_patch = ARBITRARY_SW_VERSION_PATCH;
    arbitrary_version.flight_sw_vendor_major = ARBITRARY_SW_VERSION_VENDOR_MAJOR;
    arbitrary_version.flight_sw_vendor_minor = ARBITRARY_SW_VERSION_VENDOR_MINOR;
    arbitrary_version.flight_sw_vendor_patch = ARBITRARY_SW_VERSION_VENDOR_PATCH;
    arbitrary_version.os_sw_major = ARBITRARY_SW_VERSION_OS_MAJOR;
    arbitrary_version.os_sw_minor = ARBITRARY_SW_VERSION_OS_MINOR;
    arbitrary_version.os_sw_patch = ARBITRARY_SW_VERSION_OS_PATCH;

    const auto expected_pair = std::make_pair<>(GetParam().second, arbitrary_version);
    ON_CALL(info, get_version()).WillByDefault(Return(expected_pair));
    mavsdk::rpc::info::GetVersionResponse response;

    infoService.GetVersion(nullptr, nullptr, &response);

    EXPECT_EQ(GetParam().first, InfoResult::Result_Name(response.info_result().result()));
    EXPECT_EQ(expected_pair.second.flight_sw_major, response.version().flight_sw_major());
    EXPECT_EQ(expected_pair.second.flight_sw_minor, response.version().flight_sw_minor());
    EXPECT_EQ(expected_pair.second.flight_sw_patch, response.version().flight_sw_patch());
    EXPECT_EQ(
        expected_pair.second.flight_sw_vendor_major, response.version().flight_sw_vendor_major());
    EXPECT_EQ(
        expected_pair.second.flight_sw_vendor_minor, response.version().flight_sw_vendor_minor());
    EXPECT_EQ(
        expected_pair.second.flight_sw_vendor_patch, response.version().flight_sw_vendor_patch());
    EXPECT_EQ(expected_pair.second.os_sw_major, response.version().os_sw_major());
    EXPECT_EQ(expected_pair.second.os_sw_minor, response.version().os_sw_minor());
    EXPECT_EQ(expected_pair.second.os_sw_patch, response.version().os_sw_patch());
}

TEST_F(InfoServiceImplTest, getVersionDoesNotCrashWithNullResponse)
{
    MockInfo info;
    InfoServiceImpl infoService(info);

    infoService.GetVersion(nullptr, nullptr, nullptr);
}

INSTANTIATE_TEST_SUITE_P(
    InfoResultCorrespondences, InfoServiceImplTest, ::testing::ValuesIn(generateInputPairs()));

std::vector<InputPair> generateInputPairs()
{
    std::vector<InputPair> input_pairs;
    input_pairs.push_back(std::make_pair("SUCCESS", mavsdk::Info::Result::SUCCESS));
    input_pairs.push_back(std::make_pair(
        "INFORMATION_NOT_RECEIVED_YET", mavsdk::Info::Result::INFORMATION_NOT_RECEIVED_YET));
    input_pairs.push_back(std::make_pair("UNKNOWN", mavsdk::Info::Result::UNKNOWN));

    return input_pairs;
}

} // namespace
