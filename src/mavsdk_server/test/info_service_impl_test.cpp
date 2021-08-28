#include <gmock/gmock.h>
#include <string>
#include <utility>
#include <vector>

#include "info/info_service_impl.h"
#include "info/mocks/info_mock.h"
#include "mocks/lazy_plugin_mock.h"

namespace {

using testing::_;
using testing::NiceMock;
using testing::Return;

using MockInfo = NiceMock<mavsdk::testing::MockInfo>;
using MockLazyPlugin = testing::NiceMock<mavsdk::mavsdk_server::testing::MockLazyPlugin<MockInfo>>;
using InfoServiceImpl = mavsdk::mavsdk_server::InfoServiceImpl<MockInfo, MockLazyPlugin>;

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

std::vector<mavsdk::Info::Result> generateResults();

class InfoServiceImplTest : public ::testing::TestWithParam<mavsdk::Info::Result> {};

mavsdk::Info::Result translateFromRpcResult(const mavsdk::rpc::info::InfoResult::Result result)
{
    switch (result) {
        default:
        // FALLTHROUGH
        case mavsdk::rpc::info::InfoResult_Result_RESULT_UNKNOWN:
            return mavsdk::Info::Result::Unknown;
        case mavsdk::rpc::info::InfoResult_Result_RESULT_SUCCESS:
            return mavsdk::Info::Result::Success;
        case mavsdk::rpc::info::InfoResult_Result_RESULT_INFORMATION_NOT_RECEIVED_YET:
            return mavsdk::Info::Result::InformationNotReceivedYet;
    }
}

TEST_F(InfoServiceImplTest, getVersionCallsGetter)
{
    MockLazyPlugin lazy_plugin;
    MockInfo info;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&info));
    InfoServiceImpl infoService(lazy_plugin);
    EXPECT_CALL(info, get_version()).Times(1);
    mavsdk::rpc::info::GetVersionResponse response;

    infoService.GetVersion(nullptr, nullptr, &response);
}

TEST_P(InfoServiceImplTest, getsCorrectVersion)
{
    MockLazyPlugin lazy_plugin;
    MockInfo info;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&info));
    InfoServiceImpl infoService(lazy_plugin);

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

    const auto expected_pair = std::make_pair<>(GetParam(), arbitrary_version);
    ON_CALL(info, get_version()).WillByDefault(Return(expected_pair));
    mavsdk::rpc::info::GetVersionResponse response;

    infoService.GetVersion(nullptr, nullptr, &response);

    EXPECT_EQ(GetParam(), translateFromRpcResult(response.info_result().result()));
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
    MockLazyPlugin lazy_plugin;
    MockInfo info;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&info));
    InfoServiceImpl infoService(lazy_plugin);

    infoService.GetVersion(nullptr, nullptr, nullptr);
}

INSTANTIATE_TEST_SUITE_P(
    InfoResultCorrespondences, InfoServiceImplTest, ::testing::ValuesIn(generateResults()));

std::vector<mavsdk::Info::Result> generateResults()
{
    std::vector<mavsdk::Info::Result> results;
    results.push_back(mavsdk::Info::Result::Success);
    results.push_back(mavsdk::Info::Result::InformationNotReceivedYet);
    results.push_back(mavsdk::Info::Result::Unknown);

    return results;
}

} // namespace
