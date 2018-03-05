#include <gmock/gmock.h>
#include <string>

// TODO remove this include
#include "action/action.h"
#include "action/action_service_impl.h"
#include "action/mocks/action_mock.h"

using testing::NiceMock;
using testing::Return;

using MockAction = NiceMock<dronecore::testing::MockAction>;
using ActionServiceImpl = dronecore::backend::ActionServiceImpl<MockAction>;

using ActionResult = dronecore::rpc::action::ActionResult;

std::string armAndGetTranslatedResult(dronecore::Action::Result arm_result);
std::string takeoffAndGetTranslatedResult(dronecore::Action::Result takeoff_result);
std::string landAndGetTranslatedResult(dronecore::Action::Result land_result);

TEST(ActionServiceImpl, armSuccessIsCorrectlyTranslated)
{
    const auto rpc_result = armAndGetTranslatedResult(dronecore::Action::Result::SUCCESS);
    EXPECT_EQ(rpc_result, "SUCCESS");
}

std::string armAndGetTranslatedResult(const dronecore::Action::Result arm_result)
{
    MockAction action;
    ON_CALL(action, arm())
    .WillByDefault(Return(arm_result));
    ActionServiceImpl actionService(action);
    dronecore::rpc::action::ArmResponse response;

    actionService.Arm(nullptr, nullptr, &response);

    return ActionResult::Result_Name(response.action_result().result());
}

TEST(ActionServiceImpl, armNoDeviceIsCorrectlyTranslated)
{
    const auto rpc_result = armAndGetTranslatedResult(dronecore::Action::Result::NO_DEVICE);
    EXPECT_EQ(rpc_result, "NO_DEVICE");
}

TEST(ActionServiceImpl, armConnectionErrorIsCorrectlyTranslated)
{
    const auto rpc_result = armAndGetTranslatedResult(
                                dronecore::Action::Result::CONNECTION_ERROR);
    EXPECT_EQ(rpc_result, "CONNECTION_ERROR");
}

TEST(ActionServiceImpl, armBusyIsCorrectlyTranslated)
{
    const auto rpc_result = armAndGetTranslatedResult(dronecore::Action::Result::BUSY);
    EXPECT_EQ(rpc_result, "BUSY");
}

TEST(ActionServiceImpl, armCommandDeniedIsCorrectlyTranslated)
{
    const auto rpc_result = armAndGetTranslatedResult(
                                dronecore::Action::Result::COMMAND_DENIED);
    EXPECT_EQ(rpc_result, "COMMAND_DENIED");
}

TEST(ActionServiceImpl, armCommandDeniedLandedStateUnknownIsCorrectlyTranslated)
{
    const auto rpc_result = armAndGetTranslatedResult(
                                dronecore::Action::Result::COMMAND_DENIED_LANDED_STATE_UNKNOWN);
    EXPECT_EQ(rpc_result, "COMMAND_DENIED_LANDED_STATE_UNKNOWN");
}

TEST(ActionServiceImpl, armCommandDeniedNotLandedIsCorrectlyTranslated)
{
    const auto rpc_result = armAndGetTranslatedResult(
                                dronecore::Action::Result::COMMAND_DENIED_NOT_LANDED);
    EXPECT_EQ(rpc_result, "COMMAND_DENIED_NOT_LANDED");
}

TEST(ActionServiceImpl, armTimeoutIsCorrectlyTranslated)
{
    const auto rpc_result = armAndGetTranslatedResult(dronecore::Action::Result::TIMEOUT);
    EXPECT_EQ(rpc_result, "TIMEOUT");
}

TEST(ActionServiceImpl, armVTOLTransitionSupportUnknownIsCorrectlyTranslated)
{
    const auto rpc_result = armAndGetTranslatedResult(
                                dronecore::Action::Result::VTOL_TRANSITION_SUPPORT_UNKNOWN);
    EXPECT_EQ(rpc_result, "VTOL_TRANSITION_SUPPORT_UNKNOWN");
}

TEST(ActionServiceImpl, armNoVTOLTransitionSupportIsCorrectlyTranslated)
{
    const auto rpc_result = armAndGetTranslatedResult(
                                dronecore::Action::Result::NO_VTOL_TRANSITION_SUPPORT);
    EXPECT_EQ(rpc_result, "NO_VTOL_TRANSITION_SUPPORT");
}

TEST(ActionServiceImpl, armUnknownIsCorrectlyTranslated)
{
    const auto rpc_result = armAndGetTranslatedResult(dronecore::Action::Result::UNKNOWN);
    EXPECT_EQ(rpc_result, "UNKNOWN");
}

TEST(ActionServiceImpl, takeoffSuccessIsCorrectlyTranslated)
{
    const auto rpc_result = takeoffAndGetTranslatedResult(dronecore::Action::Result::SUCCESS);
    EXPECT_EQ(rpc_result, "SUCCESS");
}

std::string takeoffAndGetTranslatedResult(const dronecore::Action::Result takeoff_result)
{
    MockAction action;
    ON_CALL(action, takeoff())
    .WillByDefault(Return(takeoff_result));
    ActionServiceImpl actionService(action);
    dronecore::rpc::action::TakeoffResponse response;

    actionService.Takeoff(nullptr, nullptr, &response);

    return ActionResult::Result_Name(response.action_result().result());
}

TEST(ActionServiceImpl, takeoffNoDeviceIsCorrectlyTranslated)
{
    const auto rpc_result = takeoffAndGetTranslatedResult(dronecore::Action::Result::NO_DEVICE);
    EXPECT_EQ(rpc_result, "NO_DEVICE");
}

TEST(ActionServiceImpl, takeoffConnectionErrorIsCorrectlyTranslated)
{
    const auto rpc_result = takeoffAndGetTranslatedResult(
                                dronecore::Action::Result::CONNECTION_ERROR);
    EXPECT_EQ(rpc_result, "CONNECTION_ERROR");
}

TEST(ActionServiceImpl, takeoffBusyIsCorrectlyTranslated)
{
    const auto rpc_result = takeoffAndGetTranslatedResult(dronecore::Action::Result::BUSY);
    EXPECT_EQ(rpc_result, "BUSY");
}

TEST(ActionServiceImpl, takeoffCommandDeniedIsCorrectlyTranslated)
{
    const auto rpc_result = takeoffAndGetTranslatedResult(
                                dronecore::Action::Result::COMMAND_DENIED);
    EXPECT_EQ(rpc_result, "COMMAND_DENIED");
}

TEST(ActionServiceImpl, takeoffCommandDeniedLandedStateUnknownIsCorrectlyTranslated)
{
    const auto rpc_result = takeoffAndGetTranslatedResult(
                                dronecore::Action::Result::COMMAND_DENIED_LANDED_STATE_UNKNOWN);
    EXPECT_EQ(rpc_result, "COMMAND_DENIED_LANDED_STATE_UNKNOWN");
}

TEST(ActionServiceImpl, takeoffCommandDeniedNotLandedIsCorrectlyTranslated)
{
    const auto rpc_result = takeoffAndGetTranslatedResult(
                                dronecore::Action::Result::COMMAND_DENIED_NOT_LANDED);
    EXPECT_EQ(rpc_result, "COMMAND_DENIED_NOT_LANDED");
}

TEST(ActionServiceImpl, takeoffTimeoutIsCorrectlyTranslated)
{
    const auto rpc_result = takeoffAndGetTranslatedResult(dronecore::Action::Result::TIMEOUT);
    EXPECT_EQ(rpc_result, "TIMEOUT");
}

TEST(ActionServiceImpl, takeoffVTOLTransitionSupportUnknownIsCorrectlyTranslated)
{
    const auto rpc_result = takeoffAndGetTranslatedResult(
                                dronecore::Action::Result::VTOL_TRANSITION_SUPPORT_UNKNOWN);
    EXPECT_EQ(rpc_result, "VTOL_TRANSITION_SUPPORT_UNKNOWN");
}

TEST(ActionServiceImpl, takeoffNoVTOLTransitionSupportIsCorrectlyTranslated)
{
    const auto rpc_result = takeoffAndGetTranslatedResult(
                                dronecore::Action::Result::NO_VTOL_TRANSITION_SUPPORT);
    EXPECT_EQ(rpc_result, "NO_VTOL_TRANSITION_SUPPORT");
}

TEST(ActionServiceImpl, takeoffUnknownIsCorrectlyTranslated)
{
    const auto rpc_result = takeoffAndGetTranslatedResult(dronecore::Action::Result::UNKNOWN);
    EXPECT_EQ(rpc_result, "UNKNOWN");
}

TEST(ActionServiceImpl, landSuccessIsCorrectlyTranslated)
{
    const auto rpc_result = landAndGetTranslatedResult(dronecore::Action::Result::SUCCESS);
    EXPECT_EQ(rpc_result, "SUCCESS");
}

std::string landAndGetTranslatedResult(const dronecore::Action::Result land_result)
{
    MockAction action;
    ON_CALL(action, land())
    .WillByDefault(Return(land_result));
    ActionServiceImpl actionService(action);
    dronecore::rpc::action::LandResponse response;

    actionService.Land(nullptr, nullptr, &response);

    return ActionResult::Result_Name(response.action_result().result());
}

TEST(ActionServiceImpl, landNoDeviceIsCorrectlyTranslated)
{
    const auto rpc_result = landAndGetTranslatedResult(dronecore::Action::Result::NO_DEVICE);
    EXPECT_EQ(rpc_result, "NO_DEVICE");
}

TEST(ActionServiceImpl, landConnectionErrorIsCorrectlyTranslated)
{
    const auto rpc_result = landAndGetTranslatedResult(
                                dronecore::Action::Result::CONNECTION_ERROR);
    EXPECT_EQ(rpc_result, "CONNECTION_ERROR");
}

TEST(ActionServiceImpl, landBusyIsCorrectlyTranslated)
{
    const auto rpc_result = landAndGetTranslatedResult(dronecore::Action::Result::BUSY);
    EXPECT_EQ(rpc_result, "BUSY");
}

TEST(ActionServiceImpl, landCommandDeniedIsCorrectlyTranslated)
{
    const auto rpc_result = landAndGetTranslatedResult(
                                dronecore::Action::Result::COMMAND_DENIED);
    EXPECT_EQ(rpc_result, "COMMAND_DENIED");
}

TEST(ActionServiceImpl, landCommandDeniedLandedStateUnknownIsCorrectlyTranslated)
{
    const auto rpc_result = landAndGetTranslatedResult(
                                dronecore::Action::Result::COMMAND_DENIED_LANDED_STATE_UNKNOWN);
    EXPECT_EQ(rpc_result, "COMMAND_DENIED_LANDED_STATE_UNKNOWN");
}

TEST(ActionServiceImpl, landCommandDeniedNotLandedIsCorrectlyTranslated)
{
    const auto rpc_result = landAndGetTranslatedResult(
                                dronecore::Action::Result::COMMAND_DENIED_NOT_LANDED);
    EXPECT_EQ(rpc_result, "COMMAND_DENIED_NOT_LANDED");
}

TEST(ActionServiceImpl, landTimeoutIsCorrectlyTranslated)
{
    const auto rpc_result = landAndGetTranslatedResult(dronecore::Action::Result::TIMEOUT);
    EXPECT_EQ(rpc_result, "TIMEOUT");
}

TEST(ActionServiceImpl, landVTOLTransitionSupportUnknownIsCorrectlyTranslated)
{
    const auto rpc_result = landAndGetTranslatedResult(
                                dronecore::Action::Result::VTOL_TRANSITION_SUPPORT_UNKNOWN);
    EXPECT_EQ(rpc_result, "VTOL_TRANSITION_SUPPORT_UNKNOWN");
}

TEST(ActionServiceImpl, landNoVTOLTransitionSupportIsCorrectlyTranslated)
{
    const auto rpc_result = landAndGetTranslatedResult(
                                dronecore::Action::Result::NO_VTOL_TRANSITION_SUPPORT);
    EXPECT_EQ(rpc_result, "NO_VTOL_TRANSITION_SUPPORT");
}

TEST(ActionServiceImpl, landUnknownIsCorrectlyTranslated)
{
    const auto rpc_result = landAndGetTranslatedResult(dronecore::Action::Result::UNKNOWN);
    EXPECT_EQ(rpc_result, "UNKNOWN");
}
