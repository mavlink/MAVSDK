#include <gmock/gmock.h>
#include <string>
#include <utility>
#include <vector>

#include "action/action_service_impl.h"
#include "action/mocks/action_mock.h"
#include "mocks/lazy_plugin_mock.h"

namespace {

using testing::_;
using testing::Return;

using MockAction = testing::NiceMock<mavsdk::testing::MockAction>;
using MockLazyPlugin =
    testing::NiceMock<mavsdk::mavsdk_server::testing::MockLazyPlugin<MockAction>>;
using ActionServiceImpl = mavsdk::mavsdk_server::ActionServiceImpl<MockAction, MockLazyPlugin>;

using ActionResult = mavsdk::rpc::action::ActionResult;

static constexpr float ARBITRARY_ALTITUDE = 42.42f;

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
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ON_CALL(action, arm()).WillByDefault(Return(arm_result));
    ActionServiceImpl actionService(lazy_plugin);
    mavsdk::rpc::action::ArmResponse response;

    actionService.Arm(nullptr, nullptr, &response);

    return translateFromRpcResult(response.action_result().result());
}

TEST_F(ActionServiceImplTest, armsEvenWhenArgsAreNull)
{
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ActionServiceImpl actionService(lazy_plugin);
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
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ON_CALL(action, disarm()).WillByDefault(Return(disarm_result));
    ActionServiceImpl actionService(lazy_plugin);
    mavsdk::rpc::action::DisarmResponse response;

    actionService.Disarm(nullptr, nullptr, &response);

    return translateFromRpcResult(response.action_result().result());
}

TEST_F(ActionServiceImplTest, disarmsEvenWhenArgsAreNull)
{
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ActionServiceImpl actionService(lazy_plugin);
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
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ON_CALL(action, takeoff()).WillByDefault(Return(takeoff_result));
    ActionServiceImpl actionService(lazy_plugin);
    mavsdk::rpc::action::TakeoffResponse response;

    actionService.Takeoff(nullptr, nullptr, &response);

    return translateFromRpcResult(response.action_result().result());
}

TEST_F(ActionServiceImplTest, takeoffEvenWhenArgsAreNull)
{
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ActionServiceImpl actionService(lazy_plugin);
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
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ON_CALL(action, land()).WillByDefault(Return(land_result));
    ActionServiceImpl actionService(lazy_plugin);
    mavsdk::rpc::action::LandResponse response;

    actionService.Land(nullptr, nullptr, &response);

    return translateFromRpcResult(response.action_result().result());
}

TEST_F(ActionServiceImplTest, landsEvenWhenArgsAreNull)
{
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ActionServiceImpl actionService(lazy_plugin);
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
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ON_CALL(action, kill()).WillByDefault(Return(kill_result));
    ActionServiceImpl actionService(lazy_plugin);
    mavsdk::rpc::action::KillResponse response;

    actionService.Kill(nullptr, nullptr, &response);

    return translateFromRpcResult(response.action_result().result());
}

TEST_F(ActionServiceImplTest, killsEvenWhenArgsAreNull)
{
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ActionServiceImpl actionService(lazy_plugin);
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
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ON_CALL(action, return_to_launch()).WillByDefault(Return(rtl_result));
    ActionServiceImpl actionService(lazy_plugin);
    mavsdk::rpc::action::ReturnToLaunchResponse response;

    actionService.ReturnToLaunch(nullptr, nullptr, &response);

    return translateFromRpcResult(response.action_result().result());
}

TEST_F(ActionServiceImplTest, rtlsEvenWhenArgsAreNull)
{
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ActionServiceImpl actionService(lazy_plugin);
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
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ON_CALL(action, transition_to_fixedwing()).WillByDefault(Return(transition_to_fw_result));
    ActionServiceImpl actionService(lazy_plugin);
    mavsdk::rpc::action::TransitionToFixedwingResponse response;

    actionService.TransitionToFixedwing(nullptr, nullptr, &response);

    return translateFromRpcResult(response.action_result().result());
}

TEST_F(ActionServiceImplTest, transitions2fwEvenWhenArgsAreNull)
{
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ActionServiceImpl actionService(lazy_plugin);
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
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ON_CALL(action, transition_to_multicopter()).WillByDefault(Return(transition_to_mc_result));
    ActionServiceImpl actionService(lazy_plugin);
    mavsdk::rpc::action::TransitionToMulticopterResponse response;

    actionService.TransitionToMulticopter(nullptr, nullptr, &response);

    return translateFromRpcResult(response.action_result().result());
}

TEST_F(ActionServiceImplTest, transitions2mcEvenWhenArgsAreNull)
{
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ActionServiceImpl actionService(lazy_plugin);
    EXPECT_CALL(action, transition_to_multicopter()).Times(1);

    actionService.TransitionToMulticopter(nullptr, nullptr, nullptr);
}

TEST_F(ActionServiceImplTest, getTakeoffAltitudeCallsGetter)
{
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ActionServiceImpl actionService(lazy_plugin);
    EXPECT_CALL(action, get_takeoff_altitude()).Times(1);
    mavsdk::rpc::action::GetTakeoffAltitudeResponse response;

    actionService.GetTakeoffAltitude(nullptr, nullptr, &response);
}

TEST_P(ActionServiceImplTest, getsCorrectTakeoffAltitude)
{
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ActionServiceImpl actionService(lazy_plugin);
    const auto expected_pair = std::make_pair<>(GetParam(), ARBITRARY_ALTITUDE);
    ON_CALL(action, get_takeoff_altitude()).WillByDefault(Return(expected_pair));
    mavsdk::rpc::action::GetTakeoffAltitudeResponse response;

    actionService.GetTakeoffAltitude(nullptr, nullptr, &response);

    EXPECT_EQ(GetParam(), translateFromRpcResult(response.action_result().result()));
    EXPECT_EQ(expected_pair.second, response.altitude());
}

TEST_F(ActionServiceImplTest, getTakeoffAltitudeDoesNotCrashWithNullResponse)
{
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ActionServiceImpl actionService(lazy_plugin);

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
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    const auto return_pair = std::make_pair<>(action_result, ARBITRARY_ALTITUDE);
    ON_CALL(action, get_takeoff_altitude()).WillByDefault(Return(return_pair));
    ActionServiceImpl actionService(lazy_plugin);
    mavsdk::rpc::action::GetTakeoffAltitudeResponse response;

    actionService.GetTakeoffAltitude(nullptr, nullptr, &response);

    return translateFromRpcResult(response.action_result().result());
}

TEST_F(ActionServiceImplTest, setTakeoffAltitudeDoesNotCrashWithNullRequest)
{
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ActionServiceImpl actionService(lazy_plugin);

    actionService.SetTakeoffAltitude(nullptr, nullptr, nullptr);
}

TEST_F(ActionServiceImplTest, setTakeoffAltitudeCallsSetter)
{
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ActionServiceImpl actionService(lazy_plugin);
    EXPECT_CALL(action, set_takeoff_altitude(_)).Times(1);
    mavsdk::rpc::action::SetTakeoffAltitudeRequest request;

    actionService.SetTakeoffAltitude(nullptr, &request, nullptr);
}

TEST_P(ActionServiceImplTest, setTakeoffAltitudeSetsRightValue)
{
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ActionServiceImpl actionService(lazy_plugin);
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
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ON_CALL(action, set_takeoff_altitude(_)).WillByDefault(Return(action_result));
    ActionServiceImpl actionService(lazy_plugin);
    mavsdk::rpc::action::SetTakeoffAltitudeRequest request;
    request.set_altitude(ARBITRARY_ALTITUDE);
    mavsdk::rpc::action::SetTakeoffAltitudeResponse response;

    actionService.SetTakeoffAltitude(nullptr, &request, &response);

    return translateFromRpcResult(response.action_result().result());
}

TEST_P(ActionServiceImplTest, getReturnToLaunchAltitudeResultIsTranslatedCorrectly)
{
    const auto rpc_result = getReturnToLaunchAltitudeAndGetTranslatedResult(GetParam());
    EXPECT_EQ(rpc_result, GetParam());
}

mavsdk::Action::Result
getReturnToLaunchAltitudeAndGetTranslatedResult(const mavsdk::Action::Result action_result)
{
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    const auto return_pair = std::make_pair<>(action_result, ARBITRARY_ALTITUDE);
    ON_CALL(action, get_return_to_launch_altitude()).WillByDefault(Return(return_pair));
    ActionServiceImpl actionService(lazy_plugin);
    mavsdk::rpc::action::GetReturnToLaunchAltitudeResponse response;

    actionService.GetReturnToLaunchAltitude(nullptr, nullptr, &response);

    return translateFromRpcResult(response.action_result().result());
}

TEST_F(ActionServiceImplTest, getReturnToLaunchAltitudeCallsGetter)
{
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ActionServiceImpl actionService(lazy_plugin);
    EXPECT_CALL(action, get_return_to_launch_altitude()).Times(1);
    mavsdk::rpc::action::GetReturnToLaunchAltitudeResponse response;

    actionService.GetReturnToLaunchAltitude(nullptr, nullptr, &response);
}

TEST_P(ActionServiceImplTest, getsCorrectReturnToLaunchAltitude)
{
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ActionServiceImpl actionService(lazy_plugin);
    const auto expected_pair = std::make_pair<>(GetParam(), ARBITRARY_ALTITUDE);
    ON_CALL(action, get_return_to_launch_altitude()).WillByDefault(Return(expected_pair));
    mavsdk::rpc::action::GetReturnToLaunchAltitudeResponse response;

    actionService.GetReturnToLaunchAltitude(nullptr, nullptr, &response);

    EXPECT_EQ(GetParam(), translateFromRpcResult(response.action_result().result()));
    EXPECT_EQ(expected_pair.second, response.relative_altitude_m());
}

TEST_F(ActionServiceImplTest, getReturnToLaunchAltitudeDoesNotCrashWithNullResponse)
{
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ActionServiceImpl actionService(lazy_plugin);

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
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ON_CALL(action, set_return_to_launch_altitude(_)).WillByDefault(Return(action_result));
    ActionServiceImpl actionService(lazy_plugin);
    mavsdk::rpc::action::SetReturnToLaunchAltitudeRequest request;
    request.set_relative_altitude_m(ARBITRARY_ALTITUDE);
    mavsdk::rpc::action::SetReturnToLaunchAltitudeResponse response;

    actionService.SetReturnToLaunchAltitude(nullptr, &request, &response);

    return translateFromRpcResult(response.action_result().result());
}

TEST_F(ActionServiceImplTest, setReturnToLaunchAltitudeDoesNotCrashWithNullParams)
{
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ActionServiceImpl actionService(lazy_plugin);

    actionService.SetReturnToLaunchAltitude(nullptr, nullptr, nullptr);
}

TEST_F(ActionServiceImplTest, setReturnToLaunchAltitudeCallsSetter)
{
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ActionServiceImpl actionService(lazy_plugin);
    EXPECT_CALL(action, set_return_to_launch_altitude(_)).Times(1);
    mavsdk::rpc::action::SetReturnToLaunchAltitudeRequest request;

    actionService.SetReturnToLaunchAltitude(nullptr, &request, nullptr);
}

TEST_P(ActionServiceImplTest, setReturnToLaunchAltitudeSetsRightValue)
{
    MockLazyPlugin lazy_plugin;
    MockAction action;
    ON_CALL(lazy_plugin, maybe_plugin()).WillByDefault(Return(&action));
    ActionServiceImpl actionService(lazy_plugin);
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
