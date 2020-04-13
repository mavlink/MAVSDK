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

namespace {

namespace rpc = mavsdk::rpc::mission;

using testing::_;
using testing::DoDefault;
using testing::NiceMock;
using testing::Return;

using MockMission = NiceMock<mavsdk::testing::MockMission>;
using MissionServiceImpl = mavsdk::backend::MissionServiceImpl<MockMission>;
using MissionService = mavsdk::rpc::mission::MissionService;
using InputPair = std::pair<std::string, mavsdk::Mission::Result>;

using UploadMissionRequest = mavsdk::rpc::mission::UploadMissionRequest;
using UploadMissionResponse = mavsdk::rpc::mission::UploadMissionResponse;

using CameraAction = mavsdk::Mission::CameraAction;
using RPCCameraAction = mavsdk::rpc::mission::MissionItem::CameraAction;

using DownloadMissionResponse = mavsdk::rpc::mission::DownloadMissionResponse;

using StartMissionRequest = mavsdk::rpc::mission::StartMissionRequest;
using StartMissionResponse = mavsdk::rpc::mission::StartMissionResponse;

using PauseMissionRequest = mavsdk::rpc::mission::PauseMissionRequest;
using PauseMissionResponse = mavsdk::rpc::mission::PauseMissionResponse;

static constexpr auto ARBITRARY_RESULT = mavsdk::Mission::Result::Unknown;
static constexpr auto ARBITRARY_INDEX = 42;
static constexpr auto ARBITRARY_SMALL_INT = 12;

std::vector<InputPair> generateInputPairs();

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
    MissionServiceImplTestBase() : _mission_service(_mission)
    {
        _callback_saved_future = _callback_saved_promise.get_future();
    }

    /* The mocked mission module. */
    MockMission _mission{};

    /* The mission service that is actually being tested here. */
    MissionServiceImpl _mission_service;

    /* The mission returns its result through a callback, which is saved in _result_callback. */
    mavsdk::Mission::result_callback_t _result_callback{};

    /* The tests need to make sure that _result_callback has been set before calling it, hence the
     * promise. */
    std::promise<void> _callback_saved_promise{};

    /* The tests need to make sure that _result_callback has been set before calling it, hence the
     * future. */
    std::future<void> _callback_saved_future{};
};

class MissionServiceImplUploadTest : public MissionServiceImplTestBase {
protected:
    /**
     * Uploads the mission and saves the result callback together with the actual list of items
     * that are sent to mavsdk. The result callback is saved in _result_callback, and the
     * mission items are saved in _uploaded_mission.
     */
    std::future<void> uploadMissionAndSaveParams(
        std::shared_ptr<UploadMissionRequest> request,
        std::shared_ptr<UploadMissionResponse> response);

    /* Generate an UploadMissionRequest from a list of mission items. */
    std::shared_ptr<UploadMissionRequest>
    generateUploadRequest(const mavsdk::Mission::MissionPlan& mission_plan) const;

    /**
     * Upload a list of items through gRPC, catch the list that is actually sent by
     * the backend, and verify that it has been sent correctly over gRPC.
     */
    void checkItemsAreUploadedCorrectly(mavsdk::Mission::MissionPlan& mission_plan);

    /* Captures the actual mission sent to mavsdk by the backend. */
    mavsdk::Mission::MissionPlan _uploaded_mission{};
};

INSTANTIATE_TEST_SUITE_P(
    MissionResultCorrespondences,
    MissionServiceImplUploadTest,
    ::testing::ValuesIn(generateInputPairs()));

ACTION_P3(SaveUploadParams, mission, callback, callback_saved_promise)
{
    *mission = arg0;
    *callback = arg1;
    callback_saved_promise->set_value();
}

TEST_F(MissionServiceImplUploadTest, doesNotFailWhenArgsAreNull)
{
    auto upload_handle = uploadMissionAndSaveParams(nullptr, nullptr);
    _result_callback(ARBITRARY_RESULT);
}

std::future<void> MissionServiceImplUploadTest::uploadMissionAndSaveParams(
    std::shared_ptr<UploadMissionRequest> request, std::shared_ptr<UploadMissionResponse> response)
{
    EXPECT_CALL(_mission, upload_mission_async(_, _))
        .WillOnce(
            SaveUploadParams(&_uploaded_mission, &_result_callback, &_callback_saved_promise));

    auto upload_handle = std::async(std::launch::async, [this, request, response]() {
        _mission_service.UploadMission(nullptr, request.get(), response.get());
    });

    _callback_saved_future.wait();
    return upload_handle;
}

TEST_P(MissionServiceImplUploadTest, uploadResultIsTranslatedCorrectly)
{
    auto response = std::make_shared<UploadMissionResponse>();
    mavsdk::Mission::MissionPlan mission_plan;
    auto request = generateUploadRequest(mission_plan);
    auto upload_handle = uploadMissionAndSaveParams(request, response);

    _result_callback(GetParam().second);
    upload_handle.wait();

    EXPECT_EQ(
        GetParam().first, rpc::MissionResult::Result_Name(response->mission_result().result()));
}

std::shared_ptr<UploadMissionRequest> MissionServiceImplUploadTest::generateUploadRequest(
    const mavsdk::Mission::MissionPlan& mission_plan) const
{
    auto request = std::make_shared<UploadMissionRequest>();

    auto rpc_mission_plan = request->mission_plan();
    for (const auto& mission_item : mission_plan.mission_items) {
        auto* rpc_mission_item = rpc_mission_plan.add_mission_items();
        rpc_mission_item->CopyFrom(
            *MissionServiceImpl::translateToRpcMissionItem(mission_item).release());
    }

    return request;
}

TEST_F(MissionServiceImplUploadTest, uploadsEmptyMissionWhenNullRequest)
{
    auto upload_handle = uploadMissionAndSaveParams(nullptr, nullptr);

    _result_callback(ARBITRARY_RESULT);
    upload_handle.wait();

    EXPECT_EQ(0, _uploaded_mission.mission_items.size());
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

    auto upload_handle = uploadMissionAndSaveParams(request, nullptr);
    _result_callback(ARBITRARY_RESULT);
    upload_handle.wait();

    ASSERT_EQ(mission_plan.mission_items.size(), _uploaded_mission.mission_items.size());

    for (size_t i = 0; i < mission_plan.mission_items.size(); i++) {
        EXPECT_EQ(mission_plan.mission_items.at(i), _uploaded_mission.mission_items.at(i));
    }
}

TEST_F(MissionServiceImplUploadTest, uploadMultipleItemsMission)
{
    auto mission_items = generateListOfMultipleItems();
    checkItemsAreUploadedCorrectly(mission_items);
}

class MissionServiceImplDownloadTest : public MissionServiceImplTestBase {
protected:
    std::future<void>
    downloadMissionAndSaveParams(std::shared_ptr<DownloadMissionResponse> response);
    void checkItemsAreDownloadedCorrectly(mavsdk::Mission::MissionPlan& mission_plan);

    mavsdk::Mission::download_mission_callback_t _download_callback{};
};

INSTANTIATE_TEST_SUITE_P(
    MissionResultCorrespondences,
    MissionServiceImplDownloadTest,
    ::testing::ValuesIn(generateInputPairs()));

ACTION_P2(SaveResult, callback, callback_saved_promise)
{
    *callback = arg0;
    callback_saved_promise->set_value();
}

TEST_F(MissionServiceImplDownloadTest, doesNotFailWhenArgsAreNull)
{
    auto download_handle = downloadMissionAndSaveParams(nullptr);
    mavsdk::Mission::MissionPlan arbitrary_mission;

    _download_callback(ARBITRARY_RESULT, arbitrary_mission);
}

std::future<void> MissionServiceImplDownloadTest::downloadMissionAndSaveParams(
    std::shared_ptr<DownloadMissionResponse> response)
{
    EXPECT_CALL(_mission, download_mission_async(_))
        .WillOnce(SaveResult(&_download_callback, &_callback_saved_promise));

    auto download_handle = std::async(std::launch::async, [this, response]() {
        _mission_service.DownloadMission(nullptr, nullptr, response.get());
    });

    _callback_saved_future.wait();
    return download_handle;
}

TEST_P(MissionServiceImplDownloadTest, downloadResultIsTranslatedCorrectly)
{
    auto response = std::make_shared<DownloadMissionResponse>();
    std::vector<mavsdk::Mission::MissionItem> mission_items;
    auto download_handle = downloadMissionAndSaveParams(response);
    mavsdk::Mission::MissionPlan arbitrary_mission;

    _download_callback(GetParam().second, arbitrary_mission);
    download_handle.wait();

    EXPECT_EQ(
        GetParam().first, rpc::MissionResult::Result_Name(response->mission_result().result()));
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
    auto download_handle = downloadMissionAndSaveParams(response);
    _download_callback(ARBITRARY_RESULT, mission_plan);
    download_handle.wait();

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

class MissionServiceImplStartTest : public MissionServiceImplTestBase {
protected:
    std::future<void> startMissionAndSaveParams(std::shared_ptr<StartMissionResponse> response);
};

INSTANTIATE_TEST_SUITE_P(
    MissionResultCorrespondences,
    MissionServiceImplStartTest,
    ::testing::ValuesIn(generateInputPairs()));

TEST_F(MissionServiceImplStartTest, doesNotFailWhenArgsAreNull)
{
    auto start_handle = startMissionAndSaveParams(nullptr);
    _result_callback(ARBITRARY_RESULT);
}

std::future<void> MissionServiceImplStartTest::startMissionAndSaveParams(
    std::shared_ptr<StartMissionResponse> response)
{
    EXPECT_CALL(_mission, start_mission_async(_))
        .WillOnce(SaveResult(&_result_callback, &_callback_saved_promise));

    auto start_handle = std::async(std::launch::async, [this, response]() {
        _mission_service.StartMission(nullptr, nullptr, response.get());
    });

    _callback_saved_future.wait();
    return start_handle;
}

TEST_P(MissionServiceImplStartTest, startResultIsTranslatedCorrectly)
{
    auto response = std::make_shared<StartMissionResponse>();
    std::vector<mavsdk::Mission::MissionItem> mission_items;
    auto start_handle = startMissionAndSaveParams(response);

    _result_callback(GetParam().second);
    start_handle.wait();

    EXPECT_EQ(
        GetParam().first, rpc::MissionResult::Result_Name(response->mission_result().result()));
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

class MissionServiceImplPauseTest : public MissionServiceImplTestBase {
protected:
    std::future<void> pauseMissionAndSaveParams(std::shared_ptr<PauseMissionResponse> response);
};

INSTANTIATE_TEST_SUITE_P(
    MissionResultCorrespondences,
    MissionServiceImplPauseTest,
    ::testing::ValuesIn(generateInputPairs()));

TEST_F(MissionServiceImplPauseTest, doesNotFailWhenArgsAreNull)
{
    auto start_handle = pauseMissionAndSaveParams(nullptr);
    _result_callback(ARBITRARY_RESULT);
}

std::future<void> MissionServiceImplPauseTest::pauseMissionAndSaveParams(
    std::shared_ptr<PauseMissionResponse> response)
{
    EXPECT_CALL(_mission, pause_mission_async(_))
        .WillOnce(SaveResult(&_result_callback, &_callback_saved_promise));

    auto start_handle = std::async(std::launch::async, [this, response]() {
        _mission_service.PauseMission(nullptr, nullptr, response.get());
    });

    _callback_saved_future.wait();
    return start_handle;
}

TEST_P(MissionServiceImplPauseTest, pauseResultIsTranslatedCorrectly)
{
    auto response = std::make_shared<PauseMissionResponse>();
    auto pause_handle = pauseMissionAndSaveParams(response);

    _result_callback(GetParam().second);
    pause_handle.wait();

    EXPECT_EQ(
        GetParam().first, rpc::MissionResult::Result_Name(response->mission_result().result()));
}

class MissionServiceImplSetCurrentTest : public MissionServiceImplTestBase {};

INSTANTIATE_TEST_SUITE_P(
    MissionResultCorrespondences,
    MissionServiceImplSetCurrentTest,
    ::testing::ValuesIn(generateInputPairs()));

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
    EXPECT_CALL(_mission, set_current_mission_item_async(expected_item_index, _))
        .WillOnce(SaveSetItemCallback(&_result_callback, &_callback_saved_promise));
    mavsdk::rpc::mission::SetCurrentMissionItemRequest request;
    request.set_index(expected_item_index);

    auto set_current_item_handle = std::async(std::launch::async, [this, &request]() {
        _mission_service.SetCurrentMissionItem(nullptr, &request, nullptr);
    });

    _callback_saved_future.wait();
    _result_callback(mavsdk::Mission::Result::Success);
    set_current_item_handle.wait();
}

TEST_P(MissionServiceImplSetCurrentTest, setCurrentItemResultIsTranslatedCorrectly)
{
    mavsdk::rpc::mission::SetCurrentMissionItemResponse response;
    mavsdk::rpc::mission::SetCurrentMissionItemRequest request;
    request.set_index(ARBITRARY_INDEX);
    EXPECT_CALL(_mission, set_current_mission_item_async(_, _))
        .WillOnce(SaveSetItemCallback(&_result_callback, &_callback_saved_promise));

    auto set_current_item_handle = std::async(std::launch::async, [this, &request, &response]() {
        _mission_service.SetCurrentMissionItem(nullptr, &request, &response);
    });

    _callback_saved_future.wait();

    _result_callback(GetParam().second);
    set_current_item_handle.wait();

    EXPECT_EQ(
        GetParam().first, rpc::MissionResult::Result_Name(response.mission_result().result()));
}

class MissionServiceImplProgressTest : public MissionServiceImplTestBase {
protected:
    virtual void SetUp()
    {
        grpc::ServerBuilder builder;
        builder.RegisterService(&_mission_service);
        _server = builder.BuildAndStart();

        grpc::ChannelArguments channel_args;
        auto channel = _server->InProcessChannel(channel_args);
        _stub = MissionService::NewStub(channel);
    }

    std::future<void> subscribeMissionProgressAsync(
        std::vector<std::pair<int, int>>& progress_events,
        std::shared_ptr<grpc::ClientContext> context) const;

    std::unique_ptr<grpc::Server> _server{};
    std::unique_ptr<MissionService::Stub> _stub{};
};

TEST_F(MissionServiceImplProgressTest, registersToMissionProgress)
{
    mavsdk::Mission::mission_progress_callback_t progress_callback;
    auto context = std::make_shared<grpc::ClientContext>();
    EXPECT_CALL(_mission, mission_progress_async(_))
        .Times(2)
        .WillOnce(SaveResult(&progress_callback, &_callback_saved_promise))
        .WillOnce(DoDefault());
    std::vector<std::pair<int, int>> progress_events;

    auto progress_events_future = subscribeMissionProgressAsync(progress_events, context);
    _callback_saved_future.wait();

    mavsdk::Mission::MissionProgress progress1;
    progress1.current = 0;
    progress1.total = 1;
    progress_callback(progress1);
    context->TryCancel();

    mavsdk::Mission::MissionProgress progress2;
    progress2.current = 0;
    progress2.total = 0;
    progress_callback(progress2); // TryCancel() requires one more event to trigger...
    progress_events_future.wait();
}

std::future<void> MissionServiceImplProgressTest::subscribeMissionProgressAsync(
    std::vector<std::pair<int, int>>& progress_events,
    std::shared_ptr<grpc::ClientContext> context) const
{
    return std::async(std::launch::async, [&]() {
        mavsdk::rpc::mission::SubscribeMissionProgressRequest request;
        auto response_reader = _stub->SubscribeMissionProgress(context.get(), request);

        mavsdk::rpc::mission::MissionProgressResponse response;
        while (response_reader->Read(&response)) {
            auto progress_event = std::make_pair(
                response.mission_progress().current(), response.mission_progress().total());

            progress_events.push_back(progress_event);
        }

        response_reader->Finish();
    });
}

TEST_F(MissionServiceImplProgressTest, SendsMultipleMissionProgressEvents)
{
    mavsdk::Mission::mission_progress_callback_t progress_callback;
    EXPECT_CALL(_mission, mission_progress_async(_))
        .Times(2)
        .WillOnce(SaveResult(&progress_callback, &_callback_saved_promise))
        .WillOnce(DoDefault());

    auto expected_mission_count = ARBITRARY_SMALL_INT;
    std::vector<std::pair<int, int>> expected_progress_events;
    for (int i = 0; i < expected_mission_count; i++) {
        expected_progress_events.push_back(std::make_pair(i, expected_mission_count));
    }
    std::vector<std::pair<int, int>> received_progress_events;

    auto context = std::make_shared<grpc::ClientContext>();
    auto progress_events_future = subscribeMissionProgressAsync(received_progress_events, context);
    _callback_saved_future.wait();

    for (const auto progress_event : expected_progress_events) {
        mavsdk::Mission::MissionProgress progress;
        progress.current = progress_event.first;
        progress.total = progress_event.second;
        progress_callback(progress);
    }
    context->TryCancel();
    mavsdk::Mission::MissionProgress progress;
    progress.current = 0;
    progress.total = 0;
    progress_callback(progress); // TryCancel() requires one more event to trigger...
    progress_events_future.wait();

    ASSERT_EQ(expected_mission_count, received_progress_events.size());
    EXPECT_EQ(expected_progress_events, received_progress_events);
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

std::vector<InputPair> generateInputPairs()
{
    std::vector<InputPair> input_pairs;
    input_pairs.push_back(std::make_pair("UNKNOWN", mavsdk::Mission::Result::Unknown));
    input_pairs.push_back(std::make_pair("SUCCESS", mavsdk::Mission::Result::Success));
    input_pairs.push_back(std::make_pair("ERROR", mavsdk::Mission::Result::Error));
    input_pairs.push_back(
        std::make_pair("TOO_MANY_MISSION_ITEMS", mavsdk::Mission::Result::TooManyMissionItems));
    input_pairs.push_back(std::make_pair("BUSY", mavsdk::Mission::Result::Busy));
    input_pairs.push_back(std::make_pair("TIMEOUT", mavsdk::Mission::Result::Timeout));
    input_pairs.push_back(
        std::make_pair("INVALID_ARGUMENT", mavsdk::Mission::Result::InvalidArgument));
    input_pairs.push_back(std::make_pair("UNSUPPORTED", mavsdk::Mission::Result::Unsupported));
    input_pairs.push_back(
        std::make_pair("NO_MISSION_AVAILABLE", mavsdk::Mission::Result::NoMissionAvailable));
    input_pairs.push_back(
        std::make_pair("FAILED_TO_OPEN_QGC_PLAN", mavsdk::Mission::Result::FailedToOpenQgcPlan));
    input_pairs.push_back(
        std::make_pair("FAILED_TO_PARSE_QGC_PLAN", mavsdk::Mission::Result::FailedToParseQgcPlan));
    input_pairs.push_back(
        std::make_pair("UNSUPPORTED_MISSION_CMD", mavsdk::Mission::Result::UnsupportedMissionCmd));

    return input_pairs;
}

} // namespace
