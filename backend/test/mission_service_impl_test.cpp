#include <future>
#include <gmock/gmock.h>
#include <grpc++/grpc++.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <memory>
#include <utility>
#include <vector>

#include "mission/mission_service_impl.h"
#include "mission/mission_item.h"
#include "mission/mocks/mission_mock.h"

namespace {

namespace dc = dronecode_sdk;
namespace rpc = dronecode_sdk::rpc::mission;

using testing::_;
using testing::NiceMock;
using testing::Return;

using MockMission = NiceMock<dc::testing::MockMission>;
using MissionServiceImpl = dc::backend::MissionServiceImpl<MockMission>;
using MissionService = dc::rpc::mission::MissionService;
using InputPair = std::pair<std::string, dc::Mission::Result>;

using UploadMissionRequest = dc::rpc::mission::UploadMissionRequest;
using UploadMissionResponse = dc::rpc::mission::UploadMissionResponse;

using CameraAction = dc::MissionItem::CameraAction;
using RPCCameraAction = dc::rpc::mission::MissionItem::CameraAction;

using DownloadMissionResponse = dc::rpc::mission::DownloadMissionResponse;

using StartMissionRequest = dc::rpc::mission::StartMissionRequest;
using StartMissionResponse = dc::rpc::mission::StartMissionResponse;

using PauseMissionRequest = dc::rpc::mission::PauseMissionRequest;
using PauseMissionResponse = dc::rpc::mission::PauseMissionResponse;

static constexpr auto ARBITRARY_RESULT = dc::Mission::Result::UNKNOWN;
static constexpr auto ARBITRARY_INDEX = 42;
static constexpr auto ARBITRARY_SMALL_INT = 12;

std::vector<InputPair> generateInputPairs();

std::vector<std::shared_ptr<dc::MissionItem>> generateListOfOneItem()
{
    std::vector<std::shared_ptr<dc::MissionItem>> mission_items;

    auto mission_item = std::make_shared<dc::MissionItem>();
    mission_item->set_position(41.848695, 75.132751);
    mission_item->set_relative_altitude(50.4f);
    mission_item->set_speed(8.3f);
    mission_item->set_fly_through(false);
    mission_item->set_gimbal_pitch_and_yaw(45.2f, 90.3f);
    mission_item->set_camera_action(CameraAction::NONE);

    mission_items.push_back(mission_item);
    return mission_items;
}

std::vector<std::shared_ptr<dc::MissionItem>> generateListOfMultipleItems()
{
    std::vector<std::shared_ptr<dc::MissionItem>> mission_items;

    auto mission_item0 = std::make_shared<dc::MissionItem>();
    mission_item0->set_position(41.848695, 75.132751);
    mission_item0->set_relative_altitude(50.4f);
    mission_item0->set_speed(8.3f);
    mission_item0->set_fly_through(false);
    mission_item0->set_gimbal_pitch_and_yaw(45.2f, 90.3f);
    mission_item0->set_camera_action(CameraAction::NONE);

    auto mission_item1 = std::make_shared<dc::MissionItem>();
    mission_item1->set_position(46.522626, 6.635356);
    mission_item1->set_relative_altitude(76.2f);
    mission_item1->set_speed(6.0f);
    mission_item1->set_fly_through(true);
    mission_item1->set_gimbal_pitch_and_yaw(41.2f, 70.3f);
    mission_item1->set_camera_action(CameraAction::TAKE_PHOTO);

    auto mission_item2 = std::make_shared<dc::MissionItem>();
    mission_item2->set_position(-50.995944711358824, -72.99892046835936);
    mission_item2->set_relative_altitude(24.0f);
    mission_item2->set_speed(4.2f);
    mission_item2->set_fly_through(false);
    mission_item2->set_gimbal_pitch_and_yaw(55.0f, 68.8f);
    mission_item2->set_camera_action(CameraAction::START_PHOTO_INTERVAL);

    auto mission_item3 = std::make_shared<dc::MissionItem>();
    mission_item3->set_position(46.522652, 6.621356);
    mission_item3->set_relative_altitude(71.2f);
    mission_item3->set_speed(7.1f);
    mission_item3->set_fly_through(false);
    mission_item3->set_gimbal_pitch_and_yaw(11.2f, 20.3f);
    mission_item3->set_camera_action(CameraAction::STOP_PHOTO_INTERVAL);

    auto mission_item4 = std::make_shared<dc::MissionItem>();
    mission_item4->set_position(48.142652, 3.626236);
    mission_item4->set_relative_altitude(56.9f);
    mission_item4->set_speed(5.4f);
    mission_item4->set_fly_through(false);
    mission_item4->set_gimbal_pitch_and_yaw(14.6f, 31.5f);
    mission_item4->set_camera_action(CameraAction::START_VIDEO);

    auto mission_item5 = std::make_shared<dc::MissionItem>();
    mission_item5->set_position(11.142334, 4.622234);
    mission_item5->set_relative_altitude(65.3f);
    mission_item5->set_speed(5.7f);
    mission_item5->set_fly_through(true);
    mission_item5->set_gimbal_pitch_and_yaw(17.2f, 90.0f);
    mission_item5->set_camera_action(CameraAction::STOP_VIDEO);

    mission_items.push_back(mission_item0);
    mission_items.push_back(mission_item1);
    mission_items.push_back(mission_item2);
    mission_items.push_back(mission_item3);
    mission_items.push_back(mission_item4);
    mission_items.push_back(mission_item5);

    return mission_items;
}

class MissionServiceImplTestBase : public ::testing::TestWithParam<InputPair> {
protected:
    MissionServiceImplTestBase() : _mission_service(_mission)
    {
        _callback_saved_future = _callback_saved_promise.get_future();
    }

    /* The mocked mission module. */
    MockMission _mission;

    /* The mission service that is actually being tested here. */
    MissionServiceImpl _mission_service;

    /* The mission returns its result through a callback, which is saved in _result_callback. */
    dc::Mission::result_callback_t _result_callback;

    /* The tests need to make sure that _result_callback has been set before calling it, hence the
     * promise. */
    std::promise<void> _callback_saved_promise;

    /* The tests need to make sure that _result_callback has been set before calling it, hence the
     * future. */
    std::future<void> _callback_saved_future;
};

class MissionServiceImplUploadTest : public MissionServiceImplTestBase {
protected:
    /**
     * Uploads the mission and saves the result callback together with the actual list of items
     * that are sent to dronecode_sdk. The result callback is saved in _result_callback, and the
     * mission items are saved in _uploaded_mission.
     */
    std::future<void> uploadMissionAndSaveParams(std::shared_ptr<UploadMissionRequest> request,
                                                 std::shared_ptr<UploadMissionResponse> response);

    /* Generate an UploadMissionRequest from a list of mission items. */
    std::shared_ptr<UploadMissionRequest>
    generateUploadRequest(const std::vector<std::shared_ptr<dc::MissionItem>> &mission_items) const;

    /**
     * Upload a list of items through gRPC, catch the list that is actually sent by
     * the backend, and verify that it has been sent correctly over gRPC.
     */
    void
    checkItemsAreUploadedCorrectly(std::vector<std::shared_ptr<dc::MissionItem>> &mission_items);

    /* Captures the actual mission sent to dronecode_sdk by the backend. */
    std::vector<std::shared_ptr<dc::MissionItem>> _uploaded_mission;
};

INSTANTIATE_TEST_CASE_P(MissionResultCorrespondences,
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
    std::vector<std::shared_ptr<dc::MissionItem>> mission_items;
    auto request = generateUploadRequest(mission_items);
    auto upload_handle = uploadMissionAndSaveParams(request, response);

    _result_callback(GetParam().second);
    upload_handle.wait();

    EXPECT_EQ(GetParam().first,
              rpc::MissionResult::Result_Name(response->mission_result().result()));
}

std::shared_ptr<UploadMissionRequest> MissionServiceImplUploadTest::generateUploadRequest(
    const std::vector<std::shared_ptr<dc::MissionItem>> &mission_items) const
{
    auto request = std::make_shared<UploadMissionRequest>();
    auto rpc_mission = new dc::rpc::mission::Mission();

    for (const auto &mission_item : mission_items) {
        auto rpc_mission_item = rpc_mission->add_mission_item();
        MissionServiceImpl::translateMissionItem(mission_item, rpc_mission_item);
    }

    request->set_allocated_mission(rpc_mission);

    return request;
}

TEST_F(MissionServiceImplUploadTest, uploadsEmptyMissionWhenNullRequest)
{
    auto upload_handle = uploadMissionAndSaveParams(nullptr, nullptr);

    _result_callback(ARBITRARY_RESULT);
    upload_handle.wait();

    EXPECT_EQ(0, _uploaded_mission.size());
}

TEST_F(MissionServiceImplUploadTest, uploadsOneItemMission)
{
    auto mission_items = generateListOfOneItem();
    checkItemsAreUploadedCorrectly(mission_items);
}

void MissionServiceImplUploadTest::checkItemsAreUploadedCorrectly(
    std::vector<std::shared_ptr<dc::MissionItem>> &mission_items)
{
    auto request = generateUploadRequest(mission_items);

    auto upload_handle = uploadMissionAndSaveParams(request, nullptr);
    _result_callback(ARBITRARY_RESULT);
    upload_handle.wait();

    ASSERT_EQ(mission_items.size(), _uploaded_mission.size());

    for (size_t i = 0; i < mission_items.size(); i++) {
        EXPECT_EQ(*mission_items.at(i), *_uploaded_mission.at(i));
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
    void
    checkItemsAreDownloadedCorrectly(std::vector<std::shared_ptr<dc::MissionItem>> &mission_items);

    dc::Mission::mission_items_and_result_callback_t _download_callback;
};

INSTANTIATE_TEST_CASE_P(MissionResultCorrespondences,
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
    std::vector<std::shared_ptr<dronecode_sdk::MissionItem>> arbitrary_mission;

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
    std::vector<std::shared_ptr<dc::MissionItem>> mission_items;
    auto download_handle = downloadMissionAndSaveParams(response);
    std::vector<std::shared_ptr<dronecode_sdk::MissionItem>> arbitrary_mission;

    _download_callback(GetParam().second, arbitrary_mission);
    download_handle.wait();

    EXPECT_EQ(GetParam().first,
              rpc::MissionResult::Result_Name(response->mission_result().result()));
}

TEST_F(MissionServiceImplDownloadTest, downloadsOneItemMission)
{
    auto mission_items = generateListOfOneItem();
    checkItemsAreDownloadedCorrectly(mission_items);
}

void MissionServiceImplDownloadTest::checkItemsAreDownloadedCorrectly(
    std::vector<std::shared_ptr<dc::MissionItem>> &mission_items)
{
    auto response = std::make_shared<DownloadMissionResponse>();
    auto download_handle = downloadMissionAndSaveParams(response);
    _download_callback(ARBITRARY_RESULT, mission_items);
    download_handle.wait();

    ASSERT_EQ(mission_items.size(), response->mission().mission_item().size());

    for (size_t i = 0; i < mission_items.size(); i++) {
        EXPECT_EQ(*mission_items.at(i),
                  *MissionServiceImpl::translateRPCMissionItem(
                      response->mission().mission_item().Get(i)));
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

INSTANTIATE_TEST_CASE_P(MissionResultCorrespondences,
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
    std::vector<std::shared_ptr<dc::MissionItem>> mission_items;
    auto start_handle = startMissionAndSaveParams(response);

    _result_callback(GetParam().second);
    start_handle.wait();

    EXPECT_EQ(GetParam().first,
              rpc::MissionResult::Result_Name(response->mission_result().result()));
}

class MissionServiceImplIsFinishedTest : public MissionServiceImplTestBase {
protected:
    void checkReturnsCorrectFinishedStatus(const bool expected_finished_status);
};

TEST_F(MissionServiceImplIsFinishedTest, isMissionFinishedCallsGetter)
{
    EXPECT_CALL(_mission, mission_finished()).Times(1);
    dronecode_sdk::rpc::mission::IsMissionFinishedResponse response;

    _mission_service.IsMissionFinished(nullptr, nullptr, &response);
}

TEST_F(MissionServiceImplIsFinishedTest, isMissionFinishedgetsCorrectValue)
{
    checkReturnsCorrectFinishedStatus(false);
    checkReturnsCorrectFinishedStatus(true);
}

void MissionServiceImplIsFinishedTest::checkReturnsCorrectFinishedStatus(
    const bool expected_finished_status)
{
    ON_CALL(_mission, mission_finished()).WillByDefault(Return(expected_finished_status));
    dronecode_sdk::rpc::mission::IsMissionFinishedResponse response;

    _mission_service.IsMissionFinished(nullptr, nullptr, &response);

    EXPECT_EQ(expected_finished_status, response.is_finished());
}

TEST_F(MissionServiceImplIsFinishedTest, isMissionFinishedDoesNotCrashWithNullResponse)
{
    _mission_service.IsMissionFinished(nullptr, nullptr, nullptr);
}

class MissionServiceImplPauseTest : public MissionServiceImplTestBase {
protected:
    std::future<void> pauseMissionAndSaveParams(std::shared_ptr<PauseMissionResponse> response);
};

INSTANTIATE_TEST_CASE_P(MissionResultCorrespondences,
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

    EXPECT_EQ(GetParam().first,
              rpc::MissionResult::Result_Name(response->mission_result().result()));
}

class MissionServiceImplSetCurrentTest : public MissionServiceImplTestBase {};

INSTANTIATE_TEST_CASE_P(MissionResultCorrespondences,
                        MissionServiceImplSetCurrentTest,
                        ::testing::ValuesIn(generateInputPairs()));

ACTION_P2(SaveSetItemCallback, callback, callback_saved_promise)
{
    *callback = arg1;
    callback_saved_promise->set_value();
}

TEST_F(MissionServiceImplSetCurrentTest, setCurrentItemDoesNotCrashWithNullRequest)
{
    _mission_service.SetCurrentMissionItemIndex(nullptr, nullptr, nullptr);
}

TEST_F(MissionServiceImplSetCurrentTest, setCurrentItemSetsRightValue)
{
    const int expected_item_index = ARBITRARY_INDEX;
    EXPECT_CALL(_mission, set_current_mission_item_async(expected_item_index, _))
        .WillOnce(SaveSetItemCallback(&_result_callback, &_callback_saved_promise));
    dronecode_sdk::rpc::mission::SetCurrentMissionItemIndexRequest request;
    request.set_index(expected_item_index);

    auto set_current_item_handle = std::async(std::launch::async, [this, &request]() {
        _mission_service.SetCurrentMissionItemIndex(nullptr, &request, nullptr);
    });

    _callback_saved_future.wait();
    _result_callback(dronecode_sdk::Mission::Result::SUCCESS);
    set_current_item_handle.wait();
}

TEST_P(MissionServiceImplSetCurrentTest, setCurrentItemResultIsTranslatedCorrectly)
{
    dronecode_sdk::rpc::mission::SetCurrentMissionItemIndexResponse response;
    dronecode_sdk::rpc::mission::SetCurrentMissionItemIndexRequest request;
    request.set_index(ARBITRARY_INDEX);
    EXPECT_CALL(_mission, set_current_mission_item_async(_, _))
        .WillOnce(SaveSetItemCallback(&_result_callback, &_callback_saved_promise));

    auto set_current_item_handle = std::async(std::launch::async, [this, &request, &response]() {
        _mission_service.SetCurrentMissionItemIndex(nullptr, &request, &response);
    });

    _callback_saved_future.wait();

    _result_callback(GetParam().second);
    set_current_item_handle.wait();

    EXPECT_EQ(GetParam().first,
              rpc::MissionResult::Result_Name(response.mission_result().result()));
}

class MissionServiceImplGetCurrentTest : public MissionServiceImplTestBase {};

TEST_F(MissionServiceImplGetCurrentTest, getCurrentDoesNotCrashWithNullResponse)
{
    _mission_service.GetCurrentMissionItemIndex(nullptr, nullptr, nullptr);
}

TEST_F(MissionServiceImplGetCurrentTest, getCurrentReturnsCorrectIndex)
{
    const auto expected_index = ARBITRARY_INDEX;
    dronecode_sdk::rpc::mission::GetCurrentMissionItemIndexResponse response;
    ON_CALL(_mission, current_mission_item()).WillByDefault(Return(expected_index));

    _mission_service.GetCurrentMissionItemIndex(nullptr, nullptr, &response);
    EXPECT_EQ(expected_index, response.index());
}

class MissionServiceImplGetCountTest : public MissionServiceImplTestBase {};

TEST_F(MissionServiceImplGetCountTest, getCountDoesNotCrashWithNullResponse)
{
    _mission_service.GetMissionCount(nullptr, nullptr, nullptr);
}

TEST_F(MissionServiceImplGetCountTest, getCountReturnsCorrectIndex)
{
    const auto expected_index = ARBITRARY_INDEX;
    dronecode_sdk::rpc::mission::GetMissionCountResponse response;
    ON_CALL(_mission, total_mission_items()).WillByDefault(Return(expected_index));

    _mission_service.GetMissionCount(nullptr, nullptr, &response);
    EXPECT_EQ(expected_index, response.count());
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

    std::future<void>
    subscribeMissionProgressAsync(std::vector<std::pair<int, int>> &progress_events) const;

    std::unique_ptr<grpc::Server> _server;
    std::unique_ptr<MissionService::Stub> _stub;
};

TEST_F(MissionServiceImplProgressTest, registersToMissionProgress)
{
    dc::Mission::progress_callback_t progress_callback;
    EXPECT_CALL(_mission, subscribe_progress(_))
        .WillOnce(SaveResult(&progress_callback, &_callback_saved_promise));
    std::vector<std::pair<int, int>> progress_events;

    auto progress_events_future = subscribeMissionProgressAsync(progress_events);
    _callback_saved_future.wait();
    progress_callback(0, 1);
    progress_events_future.wait();
}

std::future<void> MissionServiceImplProgressTest::subscribeMissionProgressAsync(
    std::vector<std::pair<int, int>> &progress_events) const
{
    return std::async(std::launch::async, [&]() {
        grpc::ClientContext context;
        dronecode_sdk::rpc::mission::SubscribeMissionProgressRequest request;
        auto response_reader = _stub->SubscribeMissionProgress(&context, request);

        dronecode_sdk::rpc::mission::MissionProgressResponse response;
        while (response_reader->Read(&response)) {
            auto progress_event =
                std::make_pair(response.current_item_index(), response.mission_count());

            progress_events.push_back(progress_event);
        }

        response_reader->Finish();
    });
}

TEST_F(MissionServiceImplProgressTest, SendsMultipleMissionProgressEvents)
{
    dc::Mission::progress_callback_t progress_callback;
    EXPECT_CALL(_mission, subscribe_progress(_))
        .WillOnce(SaveResult(&progress_callback, &_callback_saved_promise));

    auto expected_mission_count = ARBITRARY_SMALL_INT;
    std::vector<std::pair<int, int>> expected_progress_events;
    for (int i = 0; i < expected_mission_count; i++) {
        expected_progress_events.push_back(std::make_pair(i, expected_mission_count));
    }
    std::vector<std::pair<int, int>> received_progress_events;

    auto progress_events_future = subscribeMissionProgressAsync(received_progress_events);
    _callback_saved_future.wait();

    for (const auto progress_event : expected_progress_events) {
        progress_callback(progress_event.first, progress_event.second);
    }
    progress_events_future.wait();

    ASSERT_EQ(expected_mission_count, received_progress_events.size());
    EXPECT_EQ(expected_progress_events, received_progress_events);
}

std::vector<InputPair> generateInputPairs()
{
    std::vector<InputPair> input_pairs;
    input_pairs.push_back(std::make_pair("UNKNOWN", dc::Mission::Result::UNKNOWN));
    input_pairs.push_back(std::make_pair("SUCCESS", dc::Mission::Result::SUCCESS));
    input_pairs.push_back(std::make_pair("ERROR", dc::Mission::Result::ERROR));
    input_pairs.push_back(
        std::make_pair("TOO_MANY_MISSION_ITEMS", dc::Mission::Result::TOO_MANY_MISSION_ITEMS));
    input_pairs.push_back(std::make_pair("BUSY", dc::Mission::Result::BUSY));
    input_pairs.push_back(std::make_pair("TIMEOUT", dc::Mission::Result::TIMEOUT));
    input_pairs.push_back(
        std::make_pair("INVALID_ARGUMENT", dc::Mission::Result::INVALID_ARGUMENT));
    input_pairs.push_back(std::make_pair("UNSUPPORTED", dc::Mission::Result::UNSUPPORTED));
    input_pairs.push_back(
        std::make_pair("NO_MISSION_AVAILABLE", dc::Mission::Result::NO_MISSION_AVAILABLE));
    input_pairs.push_back(
        std::make_pair("FAILED_TO_OPEN_QGC_PLAN", dc::Mission::Result::FAILED_TO_OPEN_QGC_PLAN));
    input_pairs.push_back(
        std::make_pair("FAILED_TO_PARSE_QGC_PLAN", dc::Mission::Result::FAILED_TO_PARSE_QGC_PLAN));
    input_pairs.push_back(
        std::make_pair("UNSUPPORTED_MISSION_CMD", dc::Mission::Result::UNSUPPORTED_MISSION_CMD));

    return input_pairs;
}

} // namespace
