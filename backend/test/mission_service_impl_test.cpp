#include <future>
#include <gmock/gmock.h>
#include <utility>
#include <vector>

#include "mission/mission_service_impl.h"
#include "mission/mission_item.h"
#include "mission/mocks/mission_mock.h"

namespace {

using testing::_;
using testing::NiceMock;
using testing::Return;

using MockMission = NiceMock<dronecore::testing::MockMission>;
using MissionServiceImpl = dronecore::backend::MissionServiceImpl<MockMission>;

using MissionResult = dronecore::rpc::mission::MissionResult;
using InputPair = std::pair<std::string, dronecore::Mission::Result>;

using UploadMissionRequest = dronecore::rpc::mission::UploadMissionRequest;
using UploadMissionResponse = dronecore::rpc::mission::UploadMissionResponse;

using MissionItemRef = std::shared_ptr<dronecore::MissionItem>;

using CameraAction = dronecore::MissionItem::CameraAction;
using RPCCameraAction = dronecore::rpc::mission::MissionItem::CameraAction;

std::vector<InputPair> generateInputPairs();

class MissionServiceImplUploadTest : public ::testing::TestWithParam<InputPair>
{
protected:
    MissionServiceImplUploadTest()
        : _mission_service(_mission)
    {
        _callback_saved_future = _callback_saved_promise.get_future();
    }

    /**
     * Uploads the mission and saves the result callback together with the actual list of items
     * that are sent to dronecore. The result callback is saved in _result_callback, and the
     * mission items are saved in _uploaded_mission.
     */
    std::future<void> uploadMissionAndSaveParams(std::shared_ptr<UploadMissionRequest> request,
                                                 std::shared_ptr<UploadMissionResponse> response);

    /* Generate an UploadMissionRequest from a list of mission items. */
    std::shared_ptr<UploadMissionRequest>
    generateUploadRequest(const std::vector<MissionItemRef> &mission_items) const;

    /* Translate a CameraAction into an RPCCameraAction */
    RPCCameraAction translateCameraAction(const CameraAction camera_action) const;

    /**
     * Upload a list of items through gRPC, catch the list that is actually sent by
     * the backend, and verify that it has been sent correctly over gRPC.
     */
    void checkItemsAreUploadedCorrectly(std::vector<MissionItemRef> &mission_items);

    /* Generate a list of one mission item. */
    std::vector<MissionItemRef> generateListOfOneItem();

    /* Generate a list of multiple mission items. */
    std::vector<MissionItemRef> generateListOfMultipleItems();

    /* The mocked mission module. */
    MockMission _mission;

    /* The mission service that is actually being tested here. */
    MissionServiceImpl _mission_service;

    /* Captures the actual mission sent to dronecore by the backend. */
    std::vector<MissionItemRef> _uploaded_mission;

    /* UploadMission returns its result through a callback, which is saved in _result_callback. */
    dronecore::testing::mission_result_callback_t _result_callback;

    /* The tests need to make sure that _result_callback has been set before calling it, hence the promise. */
    std::promise<void> _callback_saved_promise;

    /* The tests need to make sure that _result_callback has been set before calling it, hence the future. */
    std::future<void> _callback_saved_future;
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
    EXPECT_CALL(_mission, upload_mission_async(_, _))
    .WillOnce(SaveUploadParams(&_uploaded_mission, &_result_callback, &_callback_saved_promise));

    auto upload_handle = std::async(std::launch::async, [this]() {
        _mission_service.UploadMission(nullptr, nullptr, nullptr);
    });
    _callback_saved_future.wait();
    _result_callback(dronecore::Mission::Result::UNKNOWN);
}

TEST_P(MissionServiceImplUploadTest, uploadResultIsTranslatedCorrectly)
{
    auto response = std::make_shared<UploadMissionResponse>();
    std::vector<MissionItemRef> mission_items;
    auto request = generateUploadRequest(mission_items);
    auto upload_handle = uploadMissionAndSaveParams(request, response);

    _result_callback(GetParam().second);
    upload_handle.wait();

    EXPECT_EQ(GetParam().first, MissionResult::Result_Name(response->mission_result().result()));
}

std::future<void> MissionServiceImplUploadTest::uploadMissionAndSaveParams(
    std::shared_ptr<UploadMissionRequest> request,
    std::shared_ptr<UploadMissionResponse> response)
{
    EXPECT_CALL(_mission, upload_mission_async(_, _))
    .WillOnce(SaveUploadParams(&_uploaded_mission, &_result_callback, &_callback_saved_promise));

    auto upload_handle = std::async(std::launch::async, [this, request, response]() {
        _mission_service.UploadMission(nullptr, request.get(), response.get());
    });

    _callback_saved_future.wait();
    return upload_handle;
}

std::shared_ptr<UploadMissionRequest>
MissionServiceImplUploadTest::generateUploadRequest(const std::vector<MissionItemRef>
                                                    &mission_items) const
{
    auto request = std::make_shared<UploadMissionRequest>();
    auto rpc_mission = new dronecore::rpc::mission::Mission();

    for (const auto &mission_item : mission_items) {
        auto rpc_mission_item = rpc_mission->add_mission_item();
        rpc_mission_item->set_latitude_deg(mission_item->get_latitude_deg());
        rpc_mission_item->set_longitude_deg(mission_item->get_longitude_deg());
        rpc_mission_item->set_relative_altitude_m(mission_item->get_relative_altitude_m());
        rpc_mission_item->set_speed_m_s(mission_item->get_speed_m_s());
        rpc_mission_item->set_is_fly_through(mission_item->get_fly_through());
        rpc_mission_item->set_gimbal_pitch_deg(mission_item->get_gimbal_pitch_deg());
        rpc_mission_item->set_gimbal_yaw_deg(mission_item->get_gimbal_yaw_deg());
        rpc_mission_item->set_camera_action(translateCameraAction(mission_item->get_camera_action()));
    }

    request->set_allocated_mission(rpc_mission);

    return request;
}

RPCCameraAction MissionServiceImplUploadTest::translateCameraAction(const CameraAction
                                                                    camera_action) const
{
    return static_cast<RPCCameraAction>(camera_action);
}

TEST_F(MissionServiceImplUploadTest, uploadsEmptyMissionWhenNullRequest)
{
    auto upload_handle = uploadMissionAndSaveParams(nullptr, nullptr);

    _result_callback(dronecore::Mission::Result::UNKNOWN);
    upload_handle.wait();

    EXPECT_EQ(0, _uploaded_mission.size());
}

TEST_F(MissionServiceImplUploadTest, uploadsOneItemMission)
{
    auto mission_items = generateListOfOneItem();
    checkItemsAreUploadedCorrectly(mission_items);
}

std::vector<MissionItemRef> MissionServiceImplUploadTest::generateListOfOneItem()
{
    std::vector<MissionItemRef> mission_items;

    auto mission_item = std::make_shared<dronecore::MissionItem>();
    mission_item->set_position(41.848695, 75.132751);
    mission_item->set_relative_altitude(50.4);
    mission_item->set_speed(8.3);
    mission_item->set_fly_through(false);
    mission_item->set_gimbal_pitch_and_yaw(45.2, 90.3);
    mission_item->set_camera_action(CameraAction::NONE);

    mission_items.push_back(mission_item);
    return mission_items;
}

void MissionServiceImplUploadTest::checkItemsAreUploadedCorrectly(std::vector<MissionItemRef>
                                                                  &mission_items)
{
    auto request = generateUploadRequest(mission_items);

    auto upload_handle = uploadMissionAndSaveParams(request, nullptr);
    _result_callback(dronecore::Mission::Result::UNKNOWN);
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

std::vector<MissionItemRef> MissionServiceImplUploadTest::generateListOfMultipleItems()
{
    std::vector<MissionItemRef> mission_items;

    auto mission_item0 = std::make_shared<dronecore::MissionItem>();
    mission_item0->set_position(41.848695, 75.132751);
    mission_item0->set_relative_altitude(50.4);
    mission_item0->set_speed(8.3);
    mission_item0->set_fly_through(false);
    mission_item0->set_gimbal_pitch_and_yaw(45.2, 90.3);
    mission_item0->set_camera_action(CameraAction::NONE);

    auto mission_item1 = std::make_shared<dronecore::MissionItem>();
    mission_item1->set_position(46.522626, 6.635356);
    mission_item1->set_relative_altitude(76.2);
    mission_item1->set_speed(6);
    mission_item1->set_fly_through(true);
    mission_item1->set_gimbal_pitch_and_yaw(41.2, 70.3);
    mission_item1->set_camera_action(CameraAction::TAKE_PHOTO);

    auto mission_item2 = std::make_shared<dronecore::MissionItem>();
    mission_item2->set_position(-50.995944711358824, -72.99892046835936);
    mission_item2->set_relative_altitude(24);
    mission_item2->set_speed(4.2);
    mission_item2->set_fly_through(false);
    mission_item2->set_gimbal_pitch_and_yaw(55, 68.8);
    mission_item2->set_camera_action(CameraAction::START_PHOTO_INTERVAL);

    auto mission_item3 = std::make_shared<dronecore::MissionItem>();
    mission_item3->set_position(46.522652, 6.621356);
    mission_item3->set_relative_altitude(71.2);
    mission_item3->set_speed(7.1);
    mission_item3->set_fly_through(false);
    mission_item3->set_gimbal_pitch_and_yaw(11.2, 20.3);
    mission_item3->set_camera_action(CameraAction::STOP_PHOTO_INTERVAL);

    auto mission_item4 = std::make_shared<dronecore::MissionItem>();
    mission_item4->set_position(48.142652, 3.626236);
    mission_item4->set_relative_altitude(56.9);
    mission_item4->set_speed(5.4);
    mission_item4->set_fly_through(false);
    mission_item4->set_gimbal_pitch_and_yaw(14.6, 31.5);
    mission_item4->set_camera_action(CameraAction::START_VIDEO);

    auto mission_item5 = std::make_shared<dronecore::MissionItem>();
    mission_item5->set_position(11.142334, 4.622234);
    mission_item5->set_relative_altitude(65.3);
    mission_item5->set_speed(5.7);
    mission_item5->set_fly_through(true);
    mission_item5->set_gimbal_pitch_and_yaw(17.2, 90);
    mission_item5->set_camera_action(CameraAction::STOP_VIDEO);

    mission_items.push_back(mission_item0);
    mission_items.push_back(mission_item1);
    mission_items.push_back(mission_item2);
    mission_items.push_back(mission_item3);
    mission_items.push_back(mission_item4);
    mission_items.push_back(mission_item5);

    return mission_items;
}

std::vector<InputPair> generateInputPairs()
{
    std::vector<InputPair> input_pairs;
    input_pairs.push_back(std::make_pair("UNKNOWN", dronecore::Mission::Result::UNKNOWN));
    input_pairs.push_back(std::make_pair("SUCCESS", dronecore::Mission::Result::SUCCESS));
    input_pairs.push_back(std::make_pair("ERROR", dronecore::Mission::Result::ERROR));
    input_pairs.push_back(std::make_pair("TOO_MANY_MISSION_ITEMS",
                                         dronecore::Mission::Result::TOO_MANY_MISSION_ITEMS));
    input_pairs.push_back(std::make_pair("BUSY", dronecore::Mission::Result::BUSY));
    input_pairs.push_back(std::make_pair("TIMEOUT", dronecore::Mission::Result::TIMEOUT));
    input_pairs.push_back(std::make_pair("INVALID_ARGUMENT",
                                         dronecore::Mission::Result::INVALID_ARGUMENT));
    input_pairs.push_back(std::make_pair("UNSUPPORTED", dronecore::Mission::Result::UNSUPPORTED));
    input_pairs.push_back(std::make_pair("NO_MISSION_AVAILABLE",
                                         dronecore::Mission::Result::NO_MISSION_AVAILABLE));
    input_pairs.push_back(std::make_pair("FAILED_TO_OPEN_QGC_PLAN",
                                         dronecore::Mission::Result::FAILED_TO_OPEN_QGC_PLAN));
    input_pairs.push_back(std::make_pair("FAILED_TO_PARSE_QGC_PLAN",
                                         dronecore::Mission::Result::FAILED_TO_PARSE_QGC_PLAN));
    input_pairs.push_back(std::make_pair("UNSUPPORTED_MISSION_CMD",
                                         dronecore::Mission::Result::UNSUPPORTED_MISSION_CMD));

    return input_pairs;
}

} // namespace
