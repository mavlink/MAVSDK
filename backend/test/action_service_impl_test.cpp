#include <gmock/gmock.h>
#include <string>
#include <utility>
#include <vector>

#include "action/action_service_impl.h"
#include "action/mocks/action_mock.h"

namespace {

using testing::_;
using testing::NiceMock;
using testing::Return;

using MockAction = NiceMock<dronecode_sdk::testing::MockAction>;
using ActionServiceImpl = dronecode_sdk::backend::ActionServiceImpl<MockAction>;

using ActionResult = dronecode_sdk::rpc::action::ActionResult;
using InputPair = std::pair<std::string, dronecode_sdk::ActionResult>;

static constexpr float ARBITRARY_ALTITUDE = 42.42f;
static constexpr float ARBITRARY_SPEED = 8.24f;

std::vector<InputPair> generateInputPairs();
std::string armAndGetTranslatedResult(dronecode_sdk::ActionResult arm_result);
std::string disarmAndGetTranslatedResult(dronecode_sdk::ActionResult disarm_result);
std::string takeoffAndGetTranslatedResult(dronecode_sdk::ActionResult takeoff_result);
std::string landAndGetTranslatedResult(dronecode_sdk::ActionResult land_result);
std::string killAndGetTranslatedResult(dronecode_sdk::ActionResult kill_result);
std::string returnToLaunchAndGetTranslatedResult(dronecode_sdk::ActionResult rtl_result);
std::string
transitionToFWAndGetTranslatedResult(const dronecode_sdk::ActionResult transition_to_fw_result);
std::string
transitionToMCAndGetTranslatedResult(const dronecode_sdk::ActionResult transition_to_fw_result);

class ActionServiceImplTest : public ::testing::TestWithParam<InputPair> {};

TEST_P(ActionServiceImplTest, armResultIsTranslatedCorrectly)
{
    const auto rpc_result = armAndGetTranslatedResult(GetParam().second);
    EXPECT_EQ(rpc_result, GetParam().first);
}

std::string armAndGetTranslatedResult(const dronecode_sdk::ActionResult arm_result)
{
    MockAction action;
    ON_CALL(action, arm()).WillByDefault(Return(arm_result));
    ActionServiceImpl actionService(action);
    dronecode_sdk::rpc::action::ArmResponse response;

    actionService.Arm(nullptr, nullptr, &response);

    return ActionResult::Result_Name(response.action_result().result());
}

TEST_F(ActionServiceImplTest, armsEvenWhenArgsAreNull)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    EXPECT_CALL(action, arm()).Times(1);

    actionService.Arm(nullptr, nullptr, nullptr);
}

TEST_P(ActionServiceImplTest, disarmResultIsTranslatedCorrectly)
{
    const auto rpc_result = disarmAndGetTranslatedResult(GetParam().second);
    EXPECT_EQ(rpc_result, GetParam().first);
}

std::string disarmAndGetTranslatedResult(dronecode_sdk::ActionResult disarm_result)
{
    MockAction action;
    ON_CALL(action, disarm()).WillByDefault(Return(disarm_result));
    ActionServiceImpl actionService(action);
    dronecode_sdk::rpc::action::DisarmResponse response;

    actionService.Disarm(nullptr, nullptr, &response);

    return ActionResult::Result_Name(response.action_result().result());
}

TEST_F(ActionServiceImplTest, disarmsEvenWhenArgsAreNull)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    EXPECT_CALL(action, disarm()).Times(1);

    actionService.Disarm(nullptr, nullptr, nullptr);
}

TEST_P(ActionServiceImplTest, takeoffResultIsTranslatedCorrectly)
{
    const auto rpc_result = takeoffAndGetTranslatedResult(GetParam().second);
    EXPECT_EQ(rpc_result, GetParam().first);
}

std::string takeoffAndGetTranslatedResult(const dronecode_sdk::ActionResult takeoff_result)
{
    MockAction action;
    ON_CALL(action, takeoff()).WillByDefault(Return(takeoff_result));
    ActionServiceImpl actionService(action);
    dronecode_sdk::rpc::action::TakeoffResponse response;

    actionService.Takeoff(nullptr, nullptr, &response);

    return ActionResult::Result_Name(response.action_result().result());
}

TEST_F(ActionServiceImplTest, takeoffEvenWhenArgsAreNull)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    EXPECT_CALL(action, takeoff()).Times(1);

    actionService.Takeoff(nullptr, nullptr, nullptr);
}

TEST_P(ActionServiceImplTest, landResultIsTranslatedCorrectly)
{
    const auto rpc_result = landAndGetTranslatedResult(GetParam().second);
    EXPECT_EQ(rpc_result, GetParam().first);
}

std::string landAndGetTranslatedResult(const dronecode_sdk::ActionResult land_result)
{
    MockAction action;
    ON_CALL(action, land()).WillByDefault(Return(land_result));
    ActionServiceImpl actionService(action);
    dronecode_sdk::rpc::action::LandResponse response;

    actionService.Land(nullptr, nullptr, &response);

    return ActionResult::Result_Name(response.action_result().result());
}

TEST_F(ActionServiceImplTest, landsEvenWhenArgsAreNull)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    EXPECT_CALL(action, land()).Times(1);

    actionService.Land(nullptr, nullptr, nullptr);
}

TEST_P(ActionServiceImplTest, killResultIsTranslatedCorrectly)
{
    const auto rpc_result = killAndGetTranslatedResult(GetParam().second);
    EXPECT_EQ(rpc_result, GetParam().first);
}

std::string killAndGetTranslatedResult(const dronecode_sdk::ActionResult kill_result)
{
    MockAction action;
    ON_CALL(action, kill()).WillByDefault(Return(kill_result));
    ActionServiceImpl actionService(action);
    dronecode_sdk::rpc::action::KillResponse response;

    actionService.Kill(nullptr, nullptr, &response);

    return ActionResult::Result_Name(response.action_result().result());
}

TEST_F(ActionServiceImplTest, killsEvenWhenArgsAreNull)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    EXPECT_CALL(action, kill()).Times(1);

    actionService.Kill(nullptr, nullptr, nullptr);
}

TEST_P(ActionServiceImplTest, rtlResultIsTranslatedCorrectly)
{
    const auto rpc_result = returnToLaunchAndGetTranslatedResult(GetParam().second);
    EXPECT_EQ(rpc_result, GetParam().first);
}

std::string returnToLaunchAndGetTranslatedResult(const dronecode_sdk::ActionResult rtl_result)
{
    MockAction action;
    ON_CALL(action, return_to_launch()).WillByDefault(Return(rtl_result));
    ActionServiceImpl actionService(action);
    dronecode_sdk::rpc::action::ReturnToLaunchResponse response;

    actionService.ReturnToLaunch(nullptr, nullptr, &response);

    return ActionResult::Result_Name(response.action_result().result());
}

TEST_F(ActionServiceImplTest, rtlsEvenWhenArgsAreNull)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    EXPECT_CALL(action, return_to_launch()).Times(1);

    actionService.ReturnToLaunch(nullptr, nullptr, nullptr);
}

TEST_P(ActionServiceImplTest, transition2fwResultIsTranslatedCorrectly)
{
    const auto rpc_result = transitionToFWAndGetTranslatedResult(GetParam().second);
    EXPECT_EQ(rpc_result, GetParam().first);
}

std::string
transitionToFWAndGetTranslatedResult(const dronecode_sdk::ActionResult transition_to_fw_result)
{
    MockAction action;
    ON_CALL(action, transition_to_fixedwing()).WillByDefault(Return(transition_to_fw_result));
    ActionServiceImpl actionService(action);
    dronecode_sdk::rpc::action::TransitionToFixedWingResponse response;

    actionService.TransitionToFixedWing(nullptr, nullptr, &response);

    return ActionResult::Result_Name(response.action_result().result());
}

TEST_F(ActionServiceImplTest, transitions2fwEvenWhenArgsAreNull)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    EXPECT_CALL(action, transition_to_fixedwing()).Times(1);

    actionService.TransitionToFixedWing(nullptr, nullptr, nullptr);
}

TEST_P(ActionServiceImplTest, transition2mcResultIsTranslatedCorrectly)
{
    const auto rpc_result = transitionToMCAndGetTranslatedResult(GetParam().second);
    EXPECT_EQ(rpc_result, GetParam().first);
}

std::string
transitionToMCAndGetTranslatedResult(const dronecode_sdk::ActionResult transition_to_mc_result)
{
    MockAction action;
    ON_CALL(action, transition_to_multicopter()).WillByDefault(Return(transition_to_mc_result));
    ActionServiceImpl actionService(action);
    dronecode_sdk::rpc::action::TransitionToMulticopterResponse response;

    actionService.TransitionToMulticopter(nullptr, nullptr, &response);

    return ActionResult::Result_Name(response.action_result().result());
}

TEST_F(ActionServiceImplTest, transitions2mcEvenWhenArgsAreNull)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    EXPECT_CALL(action, transition_to_multicopter()).Times(1);

    actionService.TransitionToMulticopter(nullptr, nullptr, nullptr);
}

TEST_F(ActionServiceImplTest, getTakeoffAltitudeCallsGetter)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    EXPECT_CALL(action, get_takeoff_altitude()).Times(1);
    dronecode_sdk::rpc::action::GetTakeoffAltitudeResponse response;

    actionService.GetTakeoffAltitude(nullptr, nullptr, &response);
}

TEST_P(ActionServiceImplTest, getsCorrectTakeoffAltitude)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    const auto expected_pair = std::make_pair<>(GetParam().second, ARBITRARY_ALTITUDE);
    ON_CALL(action, get_takeoff_altitude()).WillByDefault(Return(expected_pair));
    dronecode_sdk::rpc::action::GetTakeoffAltitudeResponse response;

    actionService.GetTakeoffAltitude(nullptr, nullptr, &response);

    EXPECT_EQ(GetParam().first, ActionResult::Result_Name(response.action_result().result()));
    EXPECT_EQ(expected_pair.second, response.altitude());
}

TEST_F(ActionServiceImplTest, getTakeoffAltitudeDoesNotCrashWithNullResponse)
{
    MockAction action;
    ActionServiceImpl actionService(action);

    actionService.GetTakeoffAltitude(nullptr, nullptr, nullptr);
}

TEST_F(ActionServiceImplTest, setTakeoffAltitudeDoesNotCrashWithNullRequest)
{
    MockAction action;
    ActionServiceImpl actionService(action);

    actionService.SetTakeoffAltitude(nullptr, nullptr, nullptr);
}

TEST_F(ActionServiceImplTest, setTakeoffAltitudeCallsSetter)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    EXPECT_CALL(action, set_takeoff_altitude(_)).Times(1);
    dronecode_sdk::rpc::action::SetTakeoffAltitudeRequest request;

    actionService.SetTakeoffAltitude(nullptr, &request, nullptr);
}

TEST_P(ActionServiceImplTest, setTakeoffAltitudeSetsRightValue)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    float expected_altitude = ARBITRARY_ALTITUDE;
    EXPECT_CALL(action, set_takeoff_altitude(expected_altitude)).Times(1);
    dronecode_sdk::rpc::action::SetTakeoffAltitudeRequest request;
    request.set_altitude(expected_altitude);

    actionService.SetTakeoffAltitude(nullptr, &request, nullptr);
}

TEST_F(ActionServiceImplTest, getMaxSpeedDoesNotCrashWithNullResponse)
{
    MockAction action;
    ActionServiceImpl actionService(action);

    actionService.GetMaximumSpeed(nullptr, nullptr, nullptr);
}

TEST_F(ActionServiceImplTest, getMaxSpeedCallsGetter)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    EXPECT_CALL(action, get_max_speed()).Times(1);
    dronecode_sdk::rpc::action::GetMaximumSpeedResponse response;

    actionService.GetMaximumSpeed(nullptr, nullptr, &response);
}

TEST_P(ActionServiceImplTest, getMaxSpeedGetsRightValue)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    const auto expected_pair = std::make_pair<>(GetParam().second, ARBITRARY_SPEED);
    ON_CALL(action, get_max_speed()).WillByDefault(Return(expected_pair));
    dronecode_sdk::rpc::action::GetMaximumSpeedResponse response;

    actionService.GetMaximumSpeed(nullptr, nullptr, &response);

    EXPECT_EQ(GetParam().first, ActionResult::Result_Name(response.action_result().result()));
    EXPECT_EQ(expected_pair.second, response.speed());
}

TEST_F(ActionServiceImplTest, setMaxSpeedDoesNotCrashWithNullRequest)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    dronecode_sdk::rpc::action::SetMaximumSpeedResponse response;

    actionService.SetMaximumSpeed(nullptr, nullptr, &response);
}

TEST_F(ActionServiceImplTest, setMaxSpeedDoesNotCrashWithNullResponse)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    dronecode_sdk::rpc::action::SetMaximumSpeedRequest request;
    request.set_speed(ARBITRARY_SPEED);

    actionService.SetMaximumSpeed(nullptr, &request, nullptr);
}

TEST_F(ActionServiceImplTest, setMaxSpeedCallsSetter)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    EXPECT_CALL(action, set_max_speed(_)).Times(1);
    dronecode_sdk::rpc::action::SetMaximumSpeedRequest request;

    actionService.SetMaximumSpeed(nullptr, &request, nullptr);
}

TEST_F(ActionServiceImplTest, setMaxSpeedSetsRightValue)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    const auto expected_speed = ARBITRARY_SPEED;
    EXPECT_CALL(action, set_max_speed(expected_speed)).Times(1);
    dronecode_sdk::rpc::action::SetMaximumSpeedRequest request;
    request.set_speed(expected_speed);

    actionService.SetMaximumSpeed(nullptr, &request, nullptr);
}

INSTANTIATE_TEST_CASE_P(ActionResultCorrespondences,
                        ActionServiceImplTest,
                        ::testing::ValuesIn(generateInputPairs()));

std::vector<InputPair> generateInputPairs()
{
    std::vector<InputPair> input_pairs;
    input_pairs.push_back(std::make_pair("SUCCESS", dronecode_sdk::ActionResult::SUCCESS));
    input_pairs.push_back(std::make_pair("NO_SYSTEM", dronecode_sdk::ActionResult::NO_SYSTEM));
    input_pairs.push_back(
        std::make_pair("CONNECTION_ERROR", dronecode_sdk::ActionResult::CONNECTION_ERROR));
    input_pairs.push_back(std::make_pair("BUSY", dronecode_sdk::ActionResult::BUSY));
    input_pairs.push_back(
        std::make_pair("COMMAND_DENIED", dronecode_sdk::ActionResult::COMMAND_DENIED));
    input_pairs.push_back(
        std::make_pair("COMMAND_DENIED_LANDED_STATE_UNKNOWN",
                       dronecode_sdk::ActionResult::COMMAND_DENIED_LANDED_STATE_UNKNOWN));
    input_pairs.push_back(std::make_pair("COMMAND_DENIED_NOT_LANDED",
                                         dronecode_sdk::ActionResult::COMMAND_DENIED_NOT_LANDED));
    input_pairs.push_back(std::make_pair("TIMEOUT", dronecode_sdk::ActionResult::TIMEOUT));
    input_pairs.push_back(
        std::make_pair("VTOL_TRANSITION_SUPPORT_UNKNOWN",
                       dronecode_sdk::ActionResult::VTOL_TRANSITION_SUPPORT_UNKNOWN));
    input_pairs.push_back(std::make_pair("NO_VTOL_TRANSITION_SUPPORT",
                                         dronecode_sdk::ActionResult::NO_VTOL_TRANSITION_SUPPORT));
    input_pairs.push_back(std::make_pair("UNKNOWN", dronecode_sdk::ActionResult::UNKNOWN));
    input_pairs.push_back(
        std::make_pair("PARAMETER_ERROR", dronecode_sdk::ActionResult::PARAMETER_ERROR));

    return input_pairs;
}

} // namespace
