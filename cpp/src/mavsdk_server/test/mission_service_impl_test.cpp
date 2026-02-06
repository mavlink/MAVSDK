#include <future>
#include <gmock/gmock.h>
#include <grpc++/grpc++.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <memory>
#include <utility>
#include <vector>

#include "mission/mission_service_impl.h"
#include "mission/mocks/mission_mock.h"
#include "mocks/lazy_plugin_mock.h"

namespace {

namespace rpc = mavsdk::rpc::mission;

using testing::_;
using testing::DoDefault;
using testing::NiceMock;
using testing::Return;

using MockMission = NiceMock<mavsdk::testing::MockMission>;
using MockLazyPlugin =
    testing::NiceMock<mavsdk::mavsdk_server::testing::MockLazyPlugin<MockMission>>;
using MissionServiceImpl = mavsdk::mavsdk_server::MissionServiceImpl<MockMission, MockLazyPlugin>;
using MissionService = mavsdk::rpc::mission::MissionService;
using InputPair = std::pair<std::string, mavsdk::Mission::Result>;

using UploadMissionRequest = mavsdk::rpc::mission::UploadMissionRequest;
using UploadMissionResponse = mavsdk::rpc::mission::UploadMissionResponse;

using CameraAction = mavsdk::Mission::MissionItem::CameraAction;
using RPCCameraAction = mavsdk::rpc::mission::MissionItem::CameraAction;

using DownloadMissionResponse = mavsdk::rpc::mission::DownloadMissionResponse;

using StartMissionRequest = mavsdk::rpc::mission::StartMissionRequest;
using StartMissionResponse = mavsdk::rpc::mission::StartMissionResponse;

using PauseMissionRequest = mavsdk::rpc::mission::PauseMissionRequest;
using PauseMissionResponse = mavsdk::rpc::mission::PauseMissionResponse;

static constexpr auto ARBITRARY_INDEX = 42;

mavsdk::Mission::MissionPlan generateListOfOneItem()
{
    mavsdk::Mission::MissionPlan mission_plan;

    mavsdk::Mission::MissionItem mission_item{};
    mission_item.latitude_deg = 41.848695;
    mission_item.longitude_deg = 75.132751;
    mission_item.relative_altitude_m = 50.4f;
    mission_item.speed_m_s = 8.3f;
    mission_item.is_fly_through = false;
    mission_item.gimbal_pitch_deg = 45.2f;
    mission_item.gimbal_yaw_deg = 90.3f;
    mission_item.camera_action = CameraAction::None;
    mission_item.camera_photo_interval_s = 5;
    mission_item.loiter_time_s = 3.2f;

    mission_plan.mission_items.push_back(mission_item);
    return mission_plan;
}

mavsdk::Mission::MissionPlan generateListOfMultipleItems()
{
    mavsdk::Mission::MissionPlan mission_plan;

    mavsdk::Mission::MissionItem mission_item0{};
    mission_item0.latitude_deg = 41.848695;
    mission_item0.longitude_deg = 75.132751;
    mission_item0.relative_altitude_m = 50.4f;
    mission_item0.speed_m_s = 8.3f;
    mission_item0.is_fly_through = false;
    mission_item0.gimbal_pitch_deg = 45.2f;
    mission_item0.gimbal_yaw_deg = 90.3f;
    mission_item0.camera_action = CameraAction::None;
    mission_item0.loiter_time_s = 1.1f;

    mavsdk::Mission::MissionItem mission_item1{};
    mission_item1.latitude_deg = 46.522626;
    mission_item1.longitude_deg = 6.635356;
    mission_item1.relative_altitude_m = 76.2f;
    mission_item1.speed_m_s = 6.0f;
    mission_item1.is_fly_through = true;
    mission_item1.gimbal_pitch_deg = 41.2f;
    mission_item1.gimbal_yaw_deg = 70.3f;
    mission_item1.camera_action = CameraAction::TakePhoto;

    mavsdk::Mission::MissionItem mission_item2{};
    mission_item2.latitude_deg = -50.995944711358824;
    mission_item2.longitude_deg = -72.99892046835936;
    mission_item2.relative_altitude_m = 24.0f;
    mission_item2.speed_m_s = 4.2f;
    mission_item2.is_fly_through = false;
    mission_item2.gimbal_pitch_deg = 55.0f;
    mission_item2.gimbal_yaw_deg = 68.8f;
    mission_item2.camera_action = CameraAction::StartPhotoInterval;

    mavsdk::Mission::MissionItem mission_item3{};
    mission_item3.latitude_deg = 46.522652;
    mission_item3.longitude_deg = 6.621356;
    mission_item3.relative_altitude_m = 71.2f;
    mission_item3.speed_m_s = 7.1f;
    mission_item3.is_fly_through = false;
    mission_item3.gimbal_pitch_deg = 11.2f;
    mission_item3.gimbal_yaw_deg = 20.3f;
    mission_item3.camera_action = CameraAction::StopPhotoInterval;
    mission_item3.loiter_time_s = 4.4f;

    mavsdk::Mission::MissionItem mission_item4{};
    mission_item4.latitude_deg = 48.142652;
    mission_item4.longitude_deg = 3.626236;
    mission_item4.relative_altitude_m = 56.9f;
    mission_item4.speed_m_s = 5.4f;
    mission_item4.is_fly_through = false;
    mission_item4.gimbal_pitch_deg = 14.6f;
    mission_item4.gimbal_yaw_deg = 31.5f;
    mission_item4.camera_action = CameraAction::StartVideo;

    mavsdk::Mission::MissionItem mission_item5{};
    mission_item5.latitude_deg = 11.142334;
    mission_item5.longitude_deg = 4.622234;
    mission_item5.relative_altitude_m = 65.3f;
    mission_item5.speed_m_s = 5.7f;
    mission_item5.is_fly_through = true;
    mission_item5.gimbal_pitch_deg = 17.2f;
    mission_item5.gimbal_yaw_deg = 90.0f;
    mission_item5.camera_action = CameraAction::StopVideo;

    mission_plan.mission_items.push_back(mission_item0);
    mission_plan.mission_items.push_back(mission_item1);
    mission_plan.mission_items.push_back(mission_item2);
    mission_plan.mission_items.push_back(mission_item3);
    mission_plan.mission_items.push_back(mission_item4);
    mission_plan.mission_items.push_back(mission_item5);

    return mission_plan;
}

class MissionServiceImplTestBase : public ::testing::TestWithParam<InputPair> {
protected:
    MissionServiceImplTestBase() : _mission_service(_lazy_plugin)
    {
        ON_CALL(_lazy_plugin, maybe_plugin()).WillByDefault(Return(&_mission));

        _callback_saved_future = _callback_saved_promise.get_future();
    }

    MockLazyPlugin _lazy_plugin{};
    MockMission _mission{};
    MissionServiceImpl _mission_service;

    /* The mission returns its result through a callback, which is saved in _result_callback. */
    mavsdk::Mission::ResultCallback _result_callback{};

    /* The tests need to make sure that _result_callback has been set before calling it, hence the
     * promise. */
    std::promise<void> _callback_saved_promise{};

    /* The tests need to make sure that _result_callback has been set before calling it, hence the
     * future. */
    std::future<void> _callback_saved_future{};
};

class MissionServiceImplUploadTest : public MissionServiceImplTestBase {
protected:
    /* Generate an UploadMissionRequest from a list of mission items. */
    std::shared_ptr<UploadMissionRequest>
    generateUploadRequest(const mavsdk::Mission::MissionPlan& mission_plan) const;

    /**
     * Upload a list of items through gRPC, catch the list that is actually sent by
     * the mavsdk_server, and verify that it has been sent correctly over gRPC.
     */
    void checkItemsAreUploadedCorrectly(mavsdk::Mission::MissionPlan& mission_plan);
};

TEST_F(MissionServiceImplUploadTest, doesNotFailWhenArgsAreNull)
{
    _mission_service.UploadMission(nullptr, nullptr, nullptr);
}

ACTION_P(SaveUploadParams, mission_plan)
{
    *mission_plan = arg0;
    return mavsdk::Mission::Result::Success;
}

TEST_F(MissionServiceImplUploadTest, uploadResultIsTranslatedCorrectly)
{
    auto response = std::make_shared<UploadMissionResponse>();
    mavsdk::Mission::MissionPlan mission_plan;
    auto request = generateUploadRequest(mission_plan);

    EXPECT_CALL(_mission, upload_mission(_)).Times(1);
    _mission_service.UploadMission(nullptr, request.get(), response.get());
}

std::shared_ptr<UploadMissionRequest> MissionServiceImplUploadTest::generateUploadRequest(
    const mavsdk::Mission::MissionPlan& mission_plan) const
{
    auto request = std::make_shared<UploadMissionRequest>();

    auto rpc_mission_plan = request->mutable_mission_plan();

    for (const auto& mission_item : mission_plan.mission_items) {
        auto* rpc_mission_item = rpc_mission_plan->add_mission_items();
        rpc_mission_item->CopyFrom(
            *MissionServiceImpl::translateToRpcMissionItem(mission_item).release());
    }

    return request;
}

TEST_F(MissionServiceImplUploadTest, uploadsOneItemMission)
{
    auto mission_plan = generateListOfOneItem();
    checkItemsAreUploadedCorrectly(mission_plan);
}

void MissionServiceImplUploadTest::checkItemsAreUploadedCorrectly(
    mavsdk::Mission::MissionPlan& mission_plan)
{
    auto request = generateUploadRequest(mission_plan);
    auto response = std::make_shared<UploadMissionResponse>();

    mavsdk::Mission::MissionPlan downloaded_mission_plan;

    EXPECT_CALL(_mission, upload_mission(_)).WillOnce(SaveUploadParams(&downloaded_mission_plan));

    _mission_service.UploadMission(nullptr, request.get(), response.get());

    ASSERT_EQ(mission_plan.mission_items.size(), downloaded_mission_plan.mission_items.size());

    for (size_t i = 0; i < mission_plan.mission_items.size(); i++) {
        EXPECT_EQ(mission_plan.mission_items.at(i), downloaded_mission_plan.mission_items.at(i));
    }
}

TEST_F(MissionServiceImplUploadTest, uploadMultipleItemsMission)
{
    auto mission_items = generateListOfMultipleItems();
    checkItemsAreUploadedCorrectly(mission_items);
}

class MissionServiceImplDownloadTest : public MissionServiceImplTestBase {
protected:
    void checkItemsAreDownloadedCorrectly(mavsdk::Mission::MissionPlan& mission_plan);
};

TEST_F(MissionServiceImplDownloadTest, doesNotFailWhenArgsAreNull)
{
    _mission_service.DownloadMission(nullptr, nullptr, nullptr);
}

TEST_F(MissionServiceImplDownloadTest, downloadResultIsTranslatedCorrectly)
{
    auto response = std::make_shared<DownloadMissionResponse>();
    std::pair<mavsdk::Mission::Result, mavsdk::Mission::MissionPlan> result;
    result.first = mavsdk::Mission::Result::Success;
    mavsdk::Mission::MissionPlan arbitrary_mission;
    result.second = arbitrary_mission;

    EXPECT_CALL(_mission, download_mission()).WillOnce(Return(result));
    _mission_service.DownloadMission(nullptr, nullptr, response.get());
}

TEST_F(MissionServiceImplDownloadTest, downloadsOneItemMission)
{
    auto mission_plan = generateListOfOneItem();
    checkItemsAreDownloadedCorrectly(mission_plan);
}

void MissionServiceImplDownloadTest::checkItemsAreDownloadedCorrectly(
    mavsdk::Mission::MissionPlan& mission_plan)
{
    auto response = std::make_shared<DownloadMissionResponse>();

    std::pair<mavsdk::Mission::Result, mavsdk::Mission::MissionPlan> result;
    result.first = mavsdk::Mission::Result::Success;
    result.second = mission_plan;

    EXPECT_CALL(_mission, download_mission()).WillOnce(Return(result));
    _mission_service.DownloadMission(nullptr, nullptr, response.get());

    ASSERT_EQ(mission_plan.mission_items.size(), response->mission_plan().mission_items().size());

    for (size_t i = 0; i < mission_plan.mission_items.size(); i++) {
        EXPECT_EQ(
            mission_plan.mission_items.at(i),
            MissionServiceImpl::translateFromRpcMissionItem(
                response->mission_plan().mission_items().Get(i)));
    }
}

TEST_F(MissionServiceImplDownloadTest, downloadsMultipleItemsMission)
{
    auto mission_items = generateListOfMultipleItems();
    checkItemsAreDownloadedCorrectly(mission_items);
}

class MissionServiceImplStartTest : public MissionServiceImplTestBase {};

TEST_F(MissionServiceImplStartTest, doesNotFailWhenArgsAreNull)
{
    _mission_service.StartMission(nullptr, nullptr, nullptr);
}

TEST_F(MissionServiceImplStartTest, startResultIsTranslatedCorrectly)
{
    auto response = std::make_shared<StartMissionResponse>();
    std::vector<mavsdk::Mission::MissionItem> mission_items;
    EXPECT_CALL(_mission, start_mission()).Times(1);
    _mission_service.StartMission(nullptr, nullptr, response.get());
}

class MissionServiceImplIsFinishedTest : public MissionServiceImplTestBase {
protected:
    void checkReturnsCorrectFinishedStatus(
        const std::pair<mavsdk::Mission::Result, bool> expected_finished_status);
};

TEST_F(MissionServiceImplIsFinishedTest, isMissionFinishedCallsGetter)
{
    EXPECT_CALL(_mission, is_mission_finished()).Times(1);
    mavsdk::rpc::mission::IsMissionFinishedResponse response;

    _mission_service.IsMissionFinished(nullptr, nullptr, &response);
}

TEST_F(MissionServiceImplIsFinishedTest, isMissionFinishedgetsCorrectValue)
{
    checkReturnsCorrectFinishedStatus(std::make_pair<>(mavsdk::Mission::Result::Success, false));
    checkReturnsCorrectFinishedStatus(std::make_pair<>(mavsdk::Mission::Result::Success, true));
}

void MissionServiceImplIsFinishedTest::checkReturnsCorrectFinishedStatus(
    const std::pair<mavsdk::Mission::Result, bool> expected_finished_status)
{
    ON_CALL(_mission, is_mission_finished()).WillByDefault(Return(expected_finished_status));
    mavsdk::rpc::mission::IsMissionFinishedResponse response;

    _mission_service.IsMissionFinished(nullptr, nullptr, &response);

    EXPECT_EQ(expected_finished_status.second, response.is_finished());
}

TEST_F(MissionServiceImplIsFinishedTest, isMissionFinishedDoesNotCrashWithNullResponse)
{
    _mission_service.IsMissionFinished(nullptr, nullptr, nullptr);
}

class MissionServiceImplPauseTest : public MissionServiceImplTestBase {};

TEST_F(MissionServiceImplPauseTest, doesNotFailWhenArgsAreNull)
{
    _mission_service.PauseMission(nullptr, nullptr, nullptr);
}

TEST_F(MissionServiceImplPauseTest, pauseResultIsTranslatedCorrectly)
{
    auto response = std::make_shared<PauseMissionResponse>();
    EXPECT_CALL(_mission, pause_mission()).Times(1);
    _mission_service.PauseMission(nullptr, nullptr, response.get());
}

class MissionServiceImplSetCurrentTest : public MissionServiceImplTestBase {};

ACTION_P2(SaveSetItemCallback, callback, callback_saved_promise)
{
    *callback = arg1;
    callback_saved_promise->set_value();
}

TEST_F(MissionServiceImplSetCurrentTest, setCurrentItemDoesNotCrashWithNullRequest)
{
    _mission_service.SetCurrentMissionItem(nullptr, nullptr, nullptr);
}

TEST_F(MissionServiceImplSetCurrentTest, setCurrentItemSetsRightValue)
{
    const int expected_item_index = ARBITRARY_INDEX;
    EXPECT_CALL(_mission, set_current_mission_item(expected_item_index)).Times(1);
    mavsdk::rpc::mission::SetCurrentMissionItemRequest request;
    request.set_index(expected_item_index);

    _mission_service.SetCurrentMissionItem(nullptr, &request, nullptr);
}

TEST_F(MissionServiceImplSetCurrentTest, setCurrentItemResultIsTranslatedCorrectly)
{
    mavsdk::rpc::mission::SetCurrentMissionItemResponse response;
    mavsdk::rpc::mission::SetCurrentMissionItemRequest request;
    request.set_index(ARBITRARY_INDEX);
    EXPECT_CALL(_mission, set_current_mission_item(_)).Times(1);

    auto set_current_item_handle = std::async(std::launch::async, [this, &request, &response]() {
        _mission_service.SetCurrentMissionItem(nullptr, &request, &response);
    });
}

class MissionServiceImplGetRTLAfterMissionTest : public MissionServiceImplTestBase {
protected:
    void checkGetRTLAfterMissionReturns(std::pair<mavsdk::Mission::Result, bool> expected_value);
};

TEST_F(MissionServiceImplGetRTLAfterMissionTest, getRTLAfterMissionDoesNotCrashWithNullResponse)
{
    _mission_service.GetReturnToLaunchAfterMission(nullptr, nullptr, nullptr);
}

TEST_F(MissionServiceImplGetRTLAfterMissionTest, getRTLAfterMissionReturnsCorrectValue)
{
    checkGetRTLAfterMissionReturns(std::make_pair<>(mavsdk::Mission::Result::Success, true));
    checkGetRTLAfterMissionReturns(std::make_pair<>(mavsdk::Mission::Result::Success, false));
}

void MissionServiceImplGetRTLAfterMissionTest::checkGetRTLAfterMissionReturns(
    std::pair<mavsdk::Mission::Result, bool> expected_value)
{
    mavsdk::rpc::mission::GetReturnToLaunchAfterMissionResponse response;
    ON_CALL(_mission, get_return_to_launch_after_mission()).WillByDefault(Return(expected_value));

    _mission_service.GetReturnToLaunchAfterMission(nullptr, nullptr, &response);
    EXPECT_EQ(expected_value.second, response.enable());
}

class MissionServiceImplSetRTLAfterMissionTest : public MissionServiceImplTestBase {
protected:
    void checkSetRTLAfterMissionSets(const bool expected_value);
};

TEST_F(MissionServiceImplSetRTLAfterMissionTest, setRTLAfterMissionDoesNotCrashWithNullRequest)
{
    _mission_service.SetReturnToLaunchAfterMission(nullptr, nullptr, nullptr);
}

TEST_F(MissionServiceImplSetRTLAfterMissionTest, setRTLAfterMissionSetsRightValue)
{
    checkSetRTLAfterMissionSets(false);
    checkSetRTLAfterMissionSets(true);
}

void MissionServiceImplSetRTLAfterMissionTest::checkSetRTLAfterMissionSets(
    const bool expected_value)
{
    EXPECT_CALL(_mission, set_return_to_launch_after_mission(expected_value));
    mavsdk::rpc::mission::SetReturnToLaunchAfterMissionRequest request;
    request.set_enable(expected_value);

    _mission_service.SetReturnToLaunchAfterMission(nullptr, &request, nullptr);
}

} // namespace
