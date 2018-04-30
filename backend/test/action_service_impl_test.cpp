#include <gmock/gmock.h>
#include <string>
#include <utility>
#include <vector>

#include "action/action_service_impl.h"
#include "action/mocks/action_mock.h"

namespace {

using testing::NiceMock;
using testing::Return;

using MockAction = NiceMock<dronecore::testing::MockAction>;
using ActionServiceImpl = dronecore::backend::ActionServiceImpl<MockAction>;

using ActionResult = dronecore::rpc::action::ActionResult;
using InputPair = std::pair<std::string, dronecore::ActionResult>;

std::vector<InputPair> generateInputPairs();
std::string armAndGetTranslatedResult(dronecore::ActionResult arm_result);
std::string disarmAndGetTranslatedResult(dronecore::ActionResult disarm_result);
std::string takeoffAndGetTranslatedResult(dronecore::ActionResult takeoff_result);
std::string landAndGetTranslatedResult(dronecore::ActionResult land_result);
std::string killAndGetTranslatedResult(dronecore::ActionResult kill_result);
std::string returnToLaunchAndGetTranslatedResult(dronecore::ActionResult rtl_result);

class ActionServiceImplTest : public ::testing::TestWithParam<InputPair> {};

TEST_P(ActionServiceImplTest, armResultIsTranslatedCorrectly)
{
    const auto rpc_result = armAndGetTranslatedResult(GetParam().second);
    EXPECT_EQ(rpc_result, GetParam().first);
}

std::string armAndGetTranslatedResult(const dronecore::ActionResult arm_result)
{
    MockAction action;
    ON_CALL(action, arm())
    .WillByDefault(Return(arm_result));
    ActionServiceImpl actionService(action);
    dronecore::rpc::action::ArmResponse response;

    actionService.Arm(nullptr, nullptr, &response);

    return ActionResult::Result_Name(response.action_result().result());
}

TEST_F(ActionServiceImplTest, armsEvenWhenArgsAreNull)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    EXPECT_CALL(action, arm())
    .Times(1);

    actionService.Arm(nullptr, nullptr, nullptr);
}

TEST_P(ActionServiceImplTest, disarmResultIsTranslatedCorrectly)
{
    const auto rpc_result = disarmAndGetTranslatedResult(GetParam().second);
    EXPECT_EQ(rpc_result, GetParam().first);
}

std::string disarmAndGetTranslatedResult(dronecore::ActionResult disarm_result)
{
    MockAction action;
    ON_CALL(action, disarm())
    .WillByDefault(Return(disarm_result));
    ActionServiceImpl actionService(action);
    dronecore::rpc::action::DisarmResponse response;

    actionService.Disarm(nullptr, nullptr, &response);

    return ActionResult::Result_Name(response.action_result().result());
}

TEST_F(ActionServiceImplTest, disarmsEvenWhenArgsAreNull)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    EXPECT_CALL(action, disarm())
    .Times(1);

    actionService.Disarm(nullptr, nullptr, nullptr);
}

TEST_P(ActionServiceImplTest, takeoffResultIsTranslatedCorrectly)
{
    const auto rpc_result = takeoffAndGetTranslatedResult(GetParam().second);
    EXPECT_EQ(rpc_result, GetParam().first);
}

std::string takeoffAndGetTranslatedResult(const dronecore::ActionResult takeoff_result)
{
    MockAction action;
    ON_CALL(action, takeoff())
    .WillByDefault(Return(takeoff_result));
    ActionServiceImpl actionService(action);
    dronecore::rpc::action::TakeoffResponse response;

    actionService.Takeoff(nullptr, nullptr, &response);

    return ActionResult::Result_Name(response.action_result().result());
}

TEST_F(ActionServiceImplTest, takeoffEvenWhenArgsAreNull)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    EXPECT_CALL(action, takeoff())
    .Times(1);

    actionService.Takeoff(nullptr, nullptr, nullptr);
}

TEST_P(ActionServiceImplTest, landResultIsTranslatedCorrectly)
{
    const auto rpc_result = landAndGetTranslatedResult(GetParam().second);
    EXPECT_EQ(rpc_result, GetParam().first);
}

std::string landAndGetTranslatedResult(const dronecore::ActionResult land_result)
{
    MockAction action;
    ON_CALL(action, land())
    .WillByDefault(Return(land_result));
    ActionServiceImpl actionService(action);
    dronecore::rpc::action::LandResponse response;

    actionService.Land(nullptr, nullptr, &response);

    return ActionResult::Result_Name(response.action_result().result());
}

TEST_F(ActionServiceImplTest, landsEvenWhenArgsAreNull)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    EXPECT_CALL(action, land())
    .Times(1);

    actionService.Land(nullptr, nullptr, nullptr);
}

TEST_P(ActionServiceImplTest, killResultIsTranslatedCorrectly)
{
    const auto rpc_result = killAndGetTranslatedResult(GetParam().second);
    EXPECT_EQ(rpc_result, GetParam().first);
}

std::string killAndGetTranslatedResult(const dronecore::ActionResult kill_result)
{
    MockAction action;
    ON_CALL(action, kill())
    .WillByDefault(Return(kill_result));
    ActionServiceImpl actionService(action);
    dronecore::rpc::action::KillResponse response;

    actionService.Kill(nullptr, nullptr, &response);

    return ActionResult::Result_Name(response.action_result().result());
}

TEST_F(ActionServiceImplTest, killsEvenWhenArgsAreNull)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    EXPECT_CALL(action, kill())
    .Times(1);

    actionService.Kill(nullptr, nullptr, nullptr);
}

TEST_P(ActionServiceImplTest, rtlResultIsTranslatedCorrectly)
{
    const auto rpc_result = returnToLaunchAndGetTranslatedResult(GetParam().second);
    EXPECT_EQ(rpc_result, GetParam().first);
}

std::string returnToLaunchAndGetTranslatedResult(const dronecore::ActionResult rtl_result)
{
    MockAction action;
    ON_CALL(action, return_to_launch())
    .WillByDefault(Return(rtl_result));
    ActionServiceImpl actionService(action);
    dronecore::rpc::action::ReturnToLaunchResponse response;

    actionService.ReturnToLaunch(nullptr, nullptr, &response);

    return ActionResult::Result_Name(response.action_result().result());
}

TEST_F(ActionServiceImplTest, rtlsEvenWhenArgsAreNull)
{
    MockAction action;
    ActionServiceImpl actionService(action);
    EXPECT_CALL(action, return_to_launch())
    .Times(1);

    actionService.ReturnToLaunch(nullptr, nullptr, nullptr);
}

INSTANTIATE_TEST_CASE_P(ActionResultCorrespondences,
                        ActionServiceImplTest,
                        ::testing::ValuesIn(generateInputPairs()));

std::vector<InputPair> generateInputPairs()
{
    std::vector<InputPair> input_pairs;
    input_pairs.push_back(std::make_pair("SUCCESS", dronecore::ActionResult::SUCCESS));
    input_pairs.push_back(std::make_pair("NO_SYSTEM", dronecore::ActionResult::NO_SYSTEM));
    input_pairs.push_back(std::make_pair("CONNECTION_ERROR",
                                         dronecore::ActionResult::CONNECTION_ERROR));
    input_pairs.push_back(std::make_pair("BUSY", dronecore::ActionResult::BUSY));
    input_pairs.push_back(std::make_pair("COMMAND_DENIED", dronecore::ActionResult::COMMAND_DENIED));
    input_pairs.push_back(std::make_pair("COMMAND_DENIED_LANDED_STATE_UNKNOWN",
                                         dronecore::ActionResult::COMMAND_DENIED_LANDED_STATE_UNKNOWN));
    input_pairs.push_back(std::make_pair("COMMAND_DENIED_NOT_LANDED",
                                         dronecore::ActionResult::COMMAND_DENIED_NOT_LANDED));
    input_pairs.push_back(std::make_pair("TIMEOUT", dronecore::ActionResult::TIMEOUT));
    input_pairs.push_back(std::make_pair("VTOL_TRANSITION_SUPPORT_UNKNOWN",
                                         dronecore::ActionResult::VTOL_TRANSITION_SUPPORT_UNKNOWN));
    input_pairs.push_back(std::make_pair("NO_VTOL_TRANSITION_SUPPORT",
                                         dronecore::ActionResult::NO_VTOL_TRANSITION_SUPPORT));
    input_pairs.push_back(std::make_pair("UNKNOWN", dronecore::ActionResult::UNKNOWN));

    return input_pairs;
}

} // namespace
