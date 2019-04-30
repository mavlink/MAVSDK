#include <gmock/gmock.h>
#include <string>
#include <vector>

#include "offboard/offboard_service_impl.h"
#include "offboard/mocks/offboard_mock.h"

namespace {

using testing::_;
using testing::NiceMock;
using testing::Return;

using MockOffboard = NiceMock<dronecode_sdk::testing::MockOffboard>;
using OffboardServiceImpl = dronecode_sdk::backend::OffboardServiceImpl<MockOffboard>;
using OffboardResult = dronecode_sdk::rpc::offboard::OffboardResult;
using InputPair = std::pair<std::string, dronecode_sdk::Offboard::Result>;

static constexpr float ARBITRARY_ROLL = 2.5f;
static constexpr float ARBITRARY_PITCH = 4.0f;
static constexpr float ARBITRARY_YAW = 3.7f;
static constexpr float ARBITRARY_THRUST = 0.5f;

std::vector<InputPair> generateInputPairs();
std::string startAndGetTranslatedResult(dronecode_sdk::Offboard::Result start_result);
std::string stopAndGetTranslatedResult(dronecode_sdk::Offboard::Result stop_result);

class OffboardServiceImplTest : public ::testing::TestWithParam<InputPair> {
protected:
    void checkReturnsCorrectIsActiveStatus(const bool expected_is_active_status);

    std::unique_ptr<dronecode_sdk::rpc::offboard::AttitudeRate>
    createArbitraryRPCAttitudeRate() const;
};

TEST_P(OffboardServiceImplTest, startResultIsTranslatedCorrectly)
{
    const auto rpc_result = startAndGetTranslatedResult(GetParam().second);
    EXPECT_EQ(rpc_result, GetParam().first);
}

std::string startAndGetTranslatedResult(const dronecode_sdk::Offboard::Result start_result)
{
    MockOffboard offboard;
    ON_CALL(offboard, start()).WillByDefault(Return(start_result));
    OffboardServiceImpl offboardService(offboard);
    dronecode_sdk::rpc::offboard::StartResponse response;

    offboardService.Start(nullptr, nullptr, &response);

    return OffboardResult::Result_Name(response.offboard_result().result());
}

TEST_F(OffboardServiceImplTest, startsEvenWhenArgsAreNull)
{
    MockOffboard offboard;
    OffboardServiceImpl offboardService(offboard);
    EXPECT_CALL(offboard, start()).Times(1);

    offboardService.Start(nullptr, nullptr, nullptr);
}

TEST_P(OffboardServiceImplTest, stopResultIsTranslatedCorrectly)
{
    const auto rpc_result = stopAndGetTranslatedResult(GetParam().second);
    EXPECT_EQ(rpc_result, GetParam().first);
}

std::string stopAndGetTranslatedResult(const dronecode_sdk::Offboard::Result stop_result)
{
    MockOffboard offboard;
    ON_CALL(offboard, stop()).WillByDefault(Return(stop_result));
    OffboardServiceImpl offboardService(offboard);
    dronecode_sdk::rpc::offboard::StopResponse response;

    offboardService.Stop(nullptr, nullptr, &response);

    return OffboardResult::Result_Name(response.offboard_result().result());
}

TEST_F(OffboardServiceImplTest, stopsEvenWhenArgsAreNull)
{
    MockOffboard offboard;
    OffboardServiceImpl offboardService(offboard);
    EXPECT_CALL(offboard, stop()).Times(1);

    offboardService.Stop(nullptr, nullptr, nullptr);
}

TEST_F(OffboardServiceImplTest, isActiveCallsGetter)
{
    MockOffboard offboard;
    OffboardServiceImpl offboardService(offboard);
    EXPECT_CALL(offboard, is_active()).Times(1);
    dronecode_sdk::rpc::offboard::IsActiveResponse response;

    offboard.IsActive(nullptr, nullptr, &response);
}

TEST_F(OffboardServiceImplTest, isActiveGetsCorrectValue)
{
    checkReturnsCorrectIsActiveStatus(false);
    checkReturnsCorrectIsActiveStatus(true);
}

void OffboardServiceImplTest::checkReturnsCorrectIsActiveStatus(
    const bool expected_is_active_status)
{
    MockOffboard offboard;
    OffboardServiceImpl offboardService(offboard);
    ON_CALL(offboard, is_active()).WillByDefault(Return(expected_is_active_status));
    dronecode_sdk::rpc::offboard::IsActiveResponse response;

    offboardService.IsActive(nullptr, nullptr, &response);

    EXPECT_EQ(expected_is_active_status, response.is_active());
}

TEST_F(OffboardServiceImplTest, isActiveDoesNotCrashWithNullResponse)
{
    MockOffboard offboard;
    OffboardServiceImpl offboardService(offboard);

    offboardService.IsActive(nullptr, nullptr, nullptr);
}

TEST_F(OffboardServiceImplTest, setAttitudeRateDoesNotFailWithAllNullParams)
{
    MockOffboard offboard;
    OffboardServiceImpl offboardService(offboard);

    offboardService.SetAttitudeRate(nullptr, nullptr, nullptr);
}

TEST_F(OffboardServiceImplTest, setAttitudeRateDoesNotFailWithNullResponse)
{
    MockOffboard offboard;
    OffboardServiceImpl offboardService(offboard);
    dronecode_sdk::rpc::offboard::SetAttitudeRateRequest request;

    auto rpc_attitude_rate = createArbitraryRPCAttitudeRate();
    request.set_allocated_attitude_rate(rpc_attitude_rate.release());

    offboardService.SetAttitudeRate(nullptr, &request, nullptr);
}

std::unique_ptr<dronecode_sdk::rpc::offboard::AttitudeRate>
OffboardServiceImplTest::createArbitraryRPCAttitudeRate() const
{
    auto rpc_attitude_rate = std::unique_ptr<dronecode_sdk::rpc::offboard::AttitudeRate>(
        new dronecode_sdk::rpc::offboard::AttitudeRate());
    rpc_attitude_rate->set_roll_deg_s(ARBITRARY_ROLL);
    rpc_attitude_rate->set_pitch_deg_s(ARBITRARY_PITCH);
    rpc_attitude_rate->set_yaw_deg_s(ARBITRARY_YAW);
    rpc_attitude_rate->set_thrust_value(ARBITRARY_THRUST);

    return rpc_attitude_rate;
}

TEST_F(OffboardServiceImplTest, setsAttitudeRateCorrectly)
{
    MockOffboard offboard;
    OffboardServiceImpl offboardService(offboard);
    dronecode_sdk::rpc::offboard::SetAttitudeRateRequest request;

    auto rpc_attitude_rate = createArbitraryRPCAttitudeRate();
    const auto expected_attitude_rate =
        OffboardServiceImpl::translateRPCAttitudeRate(*rpc_attitude_rate);
    EXPECT_CALL(offboard, set_attitude_rate(expected_attitude_rate)).Times(1);

    request.set_allocated_attitude_rate(rpc_attitude_rate.release());

    offboardService.SetAttitudeRate(nullptr, &request, nullptr);
}

std::vector<InputPair> generateInputPairs()
{
    std::vector<InputPair> input_pairs;
    input_pairs.push_back(std::make_pair("SUCCESS", dronecode_sdk::Offboard::Result::SUCCESS));
    input_pairs.push_back(std::make_pair("NO_SYSTEM", dronecode_sdk::Offboard::Result::NO_SYSTEM));
    input_pairs.push_back(
        std::make_pair("CONNECTION_ERROR", dronecode_sdk::Offboard::Result::CONNECTION_ERROR));
    input_pairs.push_back(std::make_pair("BUSY", dronecode_sdk::Offboard::Result::BUSY));
    input_pairs.push_back(
        std::make_pair("COMMAND_DENIED", dronecode_sdk::Offboard::Result::COMMAND_DENIED));
    input_pairs.push_back(std::make_pair("TIMEOUT", dronecode_sdk::Offboard::Result::TIMEOUT));
    input_pairs.push_back(
        std::make_pair("NO_SETPOINT_SET", dronecode_sdk::Offboard::Result::NO_SETPOINT_SET));
    input_pairs.push_back(std::make_pair("UNKNOWN", dronecode_sdk::Offboard::Result::UNKNOWN));

    return input_pairs;
}

} // namespace
