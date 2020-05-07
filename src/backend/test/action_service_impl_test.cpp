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

using MockAction = NiceMock<mavsdk::testing::MockAction>;
using ActionServiceImpl = mavsdk::backend::ActionServiceImpl<MockAction>;

using ActionResult = mavsdk::rpc::action::ActionResult;

static constexpr float ARBITRARY_ALTITUDE = 42.42f;
static constexpr float ARBITRARY_SPEED = 8.24f;

std::vector<mavsdk::Action::Result> generateResults();
mavsdk::Action::Result armAndGetTranslatedResult(mavsdk::Action::Result arm_result);
mavsdk::Action::Result disarmAndGetTranslatedResult(mavsdk::Action::Result disarm_result);
mavsdk::Action::Result takeoffAndGetTranslatedResult(mavsdk::Action::Result takeoff_result);
mavsdk::Action::Result landAndGetTranslatedResult(mavsdk::Action::Result land_result);
mavsdk::Action::Result killAndGetTranslatedResult(mavsdk::Action::Result kill_result);
mavsdk::Action::Result returnToLaunchAndGetTranslatedResult(mavsdk::Action::Result rtl_result);
mavsdk::Action::Result
transitionToFWAndGetTranslatedResult(const mavsdk::Action::Result transition_to_fw_result);
mavsdk::Action::Result
transitionToMCAndGetTranslatedResult(const mavsdk::Action::Result transition_to_fw_result);
mavsdk::Action::Result
getReturnToLaunchAltitudeAndGetTranslatedResult(const mavsdk::Action::Result action_result);
mavsdk::Action::Result
setReturnToLaunchAltitudeAndGetTranslatedResult(const mavsdk::Action::Result action_result);
mavsdk::Action::Result
getTakeoffAltitudeAndGetTranslatedResult(const mavsdk::Action::Result action_result);
mavsdk::Action::Result
setTakeoffAltitudeAndGetTranslatedResult(const mavsdk::Action::Result action_result);
mavsdk::Action::Result
translateFromRpcResult(const mavsdk::rpc::action::ActionResult::Result& result);

class ActionServiceImplTest : public ::testing::TestWithParam<mavsdk::Action::Result> {};

TEST_P(ActionServiceImplTest, armResultIsTranslatedCorrectly)
{
    const auto result = armAndGetTranslatedResult(GetParam());
    EXPECT_EQ(result, GetParam());
}

mavsdk::Action::Result armAndGetTranslatedResult(const mavsdk::Action::Result arm_result)
{
    MockAction action;
    ON_CALL(action, arm()).WillByDefault(Return(arm_result));
    ActionServiceImpl actionService(action);
    mavsdk::rpc::action::ArmResponse response;

    actionService.Arm(nullptr, nullptr, &response);

    return translateFromRpcResult(response.action_result().result());
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
    const auto rpc_result = disarmAndGetTranslatedResult(GetParam());
    EXPECT_EQ(rpc_result, GetParam());
}

mavsdk::Action::Result disarmAndGetTranslatedResult(mavsdk::Action::Result disarm_result)
{
    MockAction action;
    ON_CALL(action, disarm()).WillByDefault(Return(disarm_result));
    ActionServiceImpl actionService(action);
    mavsdk::rpc::action::DisarmResponse response;

    actionService.Disarm(nullptr, nullptr, &response);

    return translateFromRpcResult(response.action_result().result());
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
    const auto rpc_result = takeoffAndGetTranslatedResult(GetParam());
    EXPECT_EQ(rpc_result, GetParam());
}

mavsdk::Action::Result takeoffAndGetTranslatedResult(const mavsdk::Action::Result takeoff_result)
{
    MockAction action;
    ON_CALL(action, takeoff()).WillByDefault(Return(takeoff_result));
    ActionServiceImpl actionService(action);
    mavsdk::rpc::action::TakeoffResponse response;

    actionService.Takeoff(nullptr, nullptr, &response);

    return translateFromRpcResult(response.action_result().result());
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
    const auto rpc_result = landAndGetTranslatedResult(GetParam());
    EXPECT_EQ(rpc_result, GetParam());
}

mavsdk::Action::Result landAndGetTranslatedResult(const mavsdk::Action::Result land_result)
{
    MockAction action;
    ON_CALL(action, land()).WillByDefault(Return(land_result));
    ActionServiceImpl actionService(action);
    mavsdk::rpc::action::LandResponse response;

    actionService.Land(nullptr, nullptr, &response);

    return translateFromRpcResult(response.action_result().result());
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
    const auto rpc_result = killAndGetTranslatedResult(GetParam());
    EXPECT_EQ(rpc_result, GetParam());
}

mavsdk::Action::Result killAndGetTranslatedResult(const mavsdk::Action::Result kill_result)
{
    MockAction action;
    ON_CALL(action, kill()).WillByDefault(Return(kill_result));
    ActionServiceImpl actionService(action);
    mavsdk::rpc::action::KillResponse response;

    actionService.Kill(nullptr, nullptr, &response);

    return translateFromRpcResult(response.action_result().result());
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
    const auto rpc_result = returnToLaunchAndGetTranslatedResult(GetParam());
    EXPECT_EQ(rpc_result, GetParam());
}

mavsdk::Action::Result returnToLaunchAndGetTranslatedResult(const mavsdk::Action::Result rtl_result)
{
    MockAction action;
    ON_CALL(action, return_to_launch()).WillByDefault(Return(rtl_result));
    ActionServiceImpl actionService(action);
    mavsdk::rpc::action::ReturnToLaunchResponse response;

    actionService.ReturnToLaunch(nullptr, nullptr, &response);

    return translateFromRpcResult(response.action_result().result());
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
    const auto rpc_result = transitionToFWAndGetTranslatedResult(GetParam());
    EXPECT_EQ(rpc_result, GetParam());
}

mavsdk::Action::Result
transitionToFWAndGetTranslatedResult(const mavsdk::Action::Result transition_to_fw_result)
{
    MockAction action;
    ON_CALL(action, transition_to_fixedwing()).WillByDefault(Return(transition_to_fw_result));
    ActionServiceImpl actionService(action);
    mavsdk::rpc::action::TransitionToFixedwingResponse response;

    actionService.TransitionToFixedwing(nullptr, nullptr, &response);

    return translateFromRpcResult(response.action_result().result());
}

TEST_F(ActionServiceImplTest, transitions2fwEvenWhenArgsAreNull)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    EXPECT_CALL(action, transition_to_fixedwing()).Times(1);

    actionService.TransitionToFixedwing(nullptr, nullptr, nullptr);
}

TEST_P(ActionServiceImplTest, transition2mcResultIsTranslatedCorrectly)
{
    const auto rpc_result = transitionToMCAndGetTranslatedResult(GetParam());
    EXPECT_EQ(rpc_result, GetParam());
}

mavsdk::Action::Result
transitionToMCAndGetTranslatedResult(const mavsdk::Action::Result transition_to_mc_result)
{
    MockAction action;
    ON_CALL(action, transition_to_multicopter()).WillByDefault(Return(transition_to_mc_result));
    ActionServiceImpl actionService(action);
    mavsdk::rpc::action::TransitionToMulticopterResponse response;

    actionService.TransitionToMulticopter(nullptr, nullptr, &response);

    return translateFromRpcResult(response.action_result().result());
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
    mavsdk::rpc::action::GetTakeoffAltitudeResponse response;

    actionService.GetTakeoffAltitude(nullptr, nullptr, &response);
}

TEST_P(ActionServiceImplTest, getsCorrectTakeoffAltitude)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    const auto expected_pair = std::make_pair<>(GetParam(), ARBITRARY_ALTITUDE);
    ON_CALL(action, get_takeoff_altitude()).WillByDefault(Return(expected_pair));
    mavsdk::rpc::action::GetTakeoffAltitudeResponse response;

    actionService.GetTakeoffAltitude(nullptr, nullptr, &response);

    EXPECT_EQ(GetParam(), translateFromRpcResult(response.action_result().result()));
    EXPECT_EQ(expected_pair.second, response.altitude());
}

TEST_F(ActionServiceImplTest, getTakeoffAltitudeDoesNotCrashWithNullResponse)
{
    MockAction action;
    ActionServiceImpl actionService(action);

    actionService.GetTakeoffAltitude(nullptr, nullptr, nullptr);
}

TEST_P(ActionServiceImplTest, getTakeoffAltitudeResultIsTranslatedCorrectly)
{
    const auto rpc_result = getTakeoffAltitudeAndGetTranslatedResult(GetParam());
    EXPECT_EQ(rpc_result, GetParam());
}

mavsdk::Action::Result
getTakeoffAltitudeAndGetTranslatedResult(const mavsdk::Action::Result action_result)
{
    MockAction action;
    const auto return_pair = std::make_pair<>(action_result, ARBITRARY_ALTITUDE);
    ON_CALL(action, get_takeoff_altitude()).WillByDefault(Return(return_pair));
    ActionServiceImpl actionService(action);
    mavsdk::rpc::action::GetTakeoffAltitudeResponse response;

    actionService.GetTakeoffAltitude(nullptr, nullptr, &response);

    return translateFromRpcResult(response.action_result().result());
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
    mavsdk::rpc::action::SetTakeoffAltitudeRequest request;

    actionService.SetTakeoffAltitude(nullptr, &request, nullptr);
}

TEST_P(ActionServiceImplTest, setTakeoffAltitudeSetsRightValue)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    float expected_altitude = ARBITRARY_ALTITUDE;
    EXPECT_CALL(action, set_takeoff_altitude(expected_altitude)).Times(1);
    mavsdk::rpc::action::SetTakeoffAltitudeRequest request;
    request.set_altitude(expected_altitude);

    actionService.SetTakeoffAltitude(nullptr, &request, nullptr);
}

TEST_P(ActionServiceImplTest, setTakeoffAltitudeResultIsTranslatedCorrectly)
{
    const auto rpc_result = setTakeoffAltitudeAndGetTranslatedResult(GetParam());
    EXPECT_EQ(rpc_result, GetParam());
}

mavsdk::Action::Result
setTakeoffAltitudeAndGetTranslatedResult(const mavsdk::Action::Result action_result)
{
    MockAction action;
    ON_CALL(action, set_takeoff_altitude(_)).WillByDefault(Return(action_result));
    ActionServiceImpl actionService(action);
    mavsdk::rpc::action::SetTakeoffAltitudeRequest request;
    request.set_altitude(ARBITRARY_ALTITUDE);
    mavsdk::rpc::action::SetTakeoffAltitudeResponse response;

    actionService.SetTakeoffAltitude(nullptr, &request, &response);

    return translateFromRpcResult(response.action_result().result());
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
    EXPECT_CALL(action, get_maximum_speed()).Times(1);
    mavsdk::rpc::action::GetMaximumSpeedResponse response;

    actionService.GetMaximumSpeed(nullptr, nullptr, &response);
}

TEST_P(ActionServiceImplTest, getMaxSpeedGetsRightValue)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    const auto expected_pair = std::make_pair<>(GetParam(), ARBITRARY_SPEED);
    ON_CALL(action, get_maximum_speed()).WillByDefault(Return(expected_pair));
    mavsdk::rpc::action::GetMaximumSpeedResponse response;

    actionService.GetMaximumSpeed(nullptr, nullptr, &response);

    EXPECT_EQ(GetParam(), translateFromRpcResult(response.action_result().result()));
    EXPECT_EQ(expected_pair.second, response.speed());
}

TEST_F(ActionServiceImplTest, setMaxSpeedDoesNotCrashWithNullRequest)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    mavsdk::rpc::action::SetMaximumSpeedResponse response;

    actionService.SetMaximumSpeed(nullptr, nullptr, &response);
}

TEST_F(ActionServiceImplTest, setMaxSpeedDoesNotCrashWithNullResponse)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    mavsdk::rpc::action::SetMaximumSpeedRequest request;
    request.set_speed(ARBITRARY_SPEED);

    actionService.SetMaximumSpeed(nullptr, &request, nullptr);
}

TEST_F(ActionServiceImplTest, setMaxSpeedCallsSetter)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    EXPECT_CALL(action, set_maximum_speed(_)).Times(1);
    mavsdk::rpc::action::SetMaximumSpeedRequest request;

    actionService.SetMaximumSpeed(nullptr, &request, nullptr);
}

TEST_F(ActionServiceImplTest, setMaxSpeedSetsRightValue)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    const auto expected_speed = ARBITRARY_SPEED;
    EXPECT_CALL(action, set_maximum_speed(expected_speed)).Times(1);
    mavsdk::rpc::action::SetMaximumSpeedRequest request;
    request.set_speed(expected_speed);

    actionService.SetMaximumSpeed(nullptr, &request, nullptr);
}

TEST_P(ActionServiceImplTest, getReturnToLaunchAltitudeResultIsTranslatedCorrectly)
{
    const auto rpc_result = getReturnToLaunchAltitudeAndGetTranslatedResult(GetParam());
    EXPECT_EQ(rpc_result, GetParam());
}

mavsdk::Action::Result
getReturnToLaunchAltitudeAndGetTranslatedResult(const mavsdk::Action::Result action_result)
{
    MockAction action;
    const auto return_pair = std::make_pair<>(action_result, ARBITRARY_ALTITUDE);
    ON_CALL(action, get_return_to_launch_altitude()).WillByDefault(Return(return_pair));
    ActionServiceImpl actionService(action);
    mavsdk::rpc::action::GetReturnToLaunchAltitudeResponse response;

    actionService.GetReturnToLaunchAltitude(nullptr, nullptr, &response);

    return translateFromRpcResult(response.action_result().result());
}

TEST_F(ActionServiceImplTest, getReturnToLaunchAltitudeCallsGetter)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    EXPECT_CALL(action, get_return_to_launch_altitude()).Times(1);
    mavsdk::rpc::action::GetReturnToLaunchAltitudeResponse response;

    actionService.GetReturnToLaunchAltitude(nullptr, nullptr, &response);
}

TEST_P(ActionServiceImplTest, getsCorrectReturnToLaunchAltitude)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    const auto expected_pair = std::make_pair<>(GetParam(), ARBITRARY_ALTITUDE);
    ON_CALL(action, get_return_to_launch_altitude()).WillByDefault(Return(expected_pair));
    mavsdk::rpc::action::GetReturnToLaunchAltitudeResponse response;

    actionService.GetReturnToLaunchAltitude(nullptr, nullptr, &response);

    EXPECT_EQ(GetParam(), translateFromRpcResult(response.action_result().result()));
    EXPECT_EQ(expected_pair.second, response.relative_altitude_m());
}

TEST_F(ActionServiceImplTest, getReturnToLaunchAltitudeDoesNotCrashWithNullResponse)
{
    MockAction action;
    ActionServiceImpl actionService(action);

    actionService.GetReturnToLaunchAltitude(nullptr, nullptr, nullptr);
}

TEST_P(ActionServiceImplTest, setReturnToLaunchAltitudeResultIsTranslatedCorrectly)
{
    const auto rpc_result = setReturnToLaunchAltitudeAndGetTranslatedResult(GetParam());
    EXPECT_EQ(rpc_result, GetParam());
}

mavsdk::Action::Result
setReturnToLaunchAltitudeAndGetTranslatedResult(const mavsdk::Action::Result action_result)
{
    MockAction action;
    ON_CALL(action, set_return_to_launch_altitude(_)).WillByDefault(Return(action_result));
    ActionServiceImpl actionService(action);
    mavsdk::rpc::action::SetReturnToLaunchAltitudeRequest request;
    request.set_relative_altitude_m(ARBITRARY_ALTITUDE);
    mavsdk::rpc::action::SetReturnToLaunchAltitudeResponse response;

    actionService.SetReturnToLaunchAltitude(nullptr, &request, &response);

    return translateFromRpcResult(response.action_result().result());
}

TEST_F(ActionServiceImplTest, setReturnToLaunchAltitudeDoesNotCrashWithNullParams)
{
    MockAction action;
    ActionServiceImpl actionService(action);

    actionService.SetReturnToLaunchAltitude(nullptr, nullptr, nullptr);
}

TEST_F(ActionServiceImplTest, setReturnToLaunchAltitudeCallsSetter)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    EXPECT_CALL(action, set_return_to_launch_altitude(_)).Times(1);
    mavsdk::rpc::action::SetReturnToLaunchAltitudeRequest request;

    actionService.SetReturnToLaunchAltitude(nullptr, &request, nullptr);
}

TEST_P(ActionServiceImplTest, setReturnToLaunchAltitudeSetsRightValue)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    float expected_altitude = ARBITRARY_ALTITUDE;
    EXPECT_CALL(action, set_return_to_launch_altitude(expected_altitude)).Times(1);
    mavsdk::rpc::action::SetReturnToLaunchAltitudeRequest request;
    request.set_relative_altitude_m(expected_altitude);

    actionService.SetReturnToLaunchAltitude(nullptr, &request, nullptr);
}

mavsdk::Action::Result
translateFromRpcResult(const mavsdk::rpc::action::ActionResult::Result& result)
{
    switch (result) {
        case mavsdk::rpc::action::ActionResult_Result_RESULT_UNKNOWN:
            return mavsdk::Action::Result::Unknown;
        case mavsdk::rpc::action::ActionResult_Result_RESULT_SUCCESS:
            return mavsdk::Action::Result::Success;
        case mavsdk::rpc::action::ActionResult_Result_RESULT_NO_SYSTEM:
            return mavsdk::Action::Result::NoSystem;
        case mavsdk::rpc::action::ActionResult_Result_RESULT_CONNECTION_ERROR:
            return mavsdk::Action::Result::ConnectionError;
        case mavsdk::rpc::action::ActionResult_Result_RESULT_BUSY:
            return mavsdk::Action::Result::Busy;
        case mavsdk::rpc::action::ActionResult_Result_RESULT_COMMAND_DENIED:
            return mavsdk::Action::Result::CommandDenied;
        case mavsdk::rpc::action::ActionResult_Result_RESULT_COMMAND_DENIED_LANDED_STATE_UNKNOWN:
            return mavsdk::Action::Result::CommandDeniedLandedStateUnknown;
        case mavsdk::rpc::action::ActionResult_Result_RESULT_COMMAND_DENIED_NOT_LANDED:
            return mavsdk::Action::Result::CommandDeniedNotLanded;
        case mavsdk::rpc::action::ActionResult_Result_RESULT_TIMEOUT:
            return mavsdk::Action::Result::Timeout;
        case mavsdk::rpc::action::ActionResult_Result_RESULT_VTOL_TRANSITION_SUPPORT_UNKNOWN:
            return mavsdk::Action::Result::VtolTransitionSupportUnknown;
        case mavsdk::rpc::action::ActionResult_Result_RESULT_NO_VTOL_TRANSITION_SUPPORT:
            return mavsdk::Action::Result::NoVtolTransitionSupport;
        case mavsdk::rpc::action::ActionResult_Result_RESULT_PARAMETER_ERROR:
            return mavsdk::Action::Result::ParameterError;
        default:
            return mavsdk::Action::Result::Unknown;
    }
}

INSTANTIATE_TEST_SUITE_P(
    ActionResultCorrespondences, ActionServiceImplTest, ::testing::ValuesIn(generateResults()));

std::vector<mavsdk::Action::Result> generateResults()
{
    std::vector<mavsdk::Action::Result> results;
    results.push_back(mavsdk::Action::Result::Success);
    results.push_back(mavsdk::Action::Result::NoSystem);
    results.push_back(mavsdk::Action::Result::ConnectionError);
    results.push_back(mavsdk::Action::Result::Busy);
    results.push_back(mavsdk::Action::Result::CommandDenied);
    results.push_back(mavsdk::Action::Result::CommandDeniedLandedStateUnknown);
    results.push_back(mavsdk::Action::Result::CommandDeniedNotLanded);
    results.push_back(mavsdk::Action::Result::Timeout);
    results.push_back(mavsdk::Action::Result::VtolTransitionSupportUnknown);
    results.push_back(mavsdk::Action::Result::NoVtolTransitionSupport);
    results.push_back(mavsdk::Action::Result::Unknown);
    results.push_back(mavsdk::Action::Result::ParameterError);

    return results;
}

} // namespace
