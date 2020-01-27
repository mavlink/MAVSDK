#include <chrono>
#include <future>
#include <gtest/gtest.h>

#include "global_include.h"
#include "mavlink_mission_transfer.h"
#include "mocks/sender_mock.h"

using namespace mavsdk;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Truly;
using MockSender = NiceMock<mavsdk::testing::MockSender>;

using Result = MAVLinkMissionTransfer::Result;

const static MAVLinkMissionTransfer::Config config {
    .own_system_id = 42,
    .own_component_id = 16,
    .target_system_id = 99,
    .target_component_id = 101 };


#define ONCE_ONLY \
    static bool called = false; \
    EXPECT_FALSE(called); \
    called = true;

MAVLinkMissionTransfer::ItemInt make_mission_item()
{
    MAVLinkMissionTransfer::ItemInt item;
    item.mission_type = MAV_MISSION_TYPE_MISSION;
    return item;
}

MAVLinkMissionTransfer::ItemInt make_geofence_item()
{
    MAVLinkMissionTransfer::ItemInt item;
    item.mission_type = MAV_MISSION_TYPE_FENCE;
    return item;
}

MAVLinkMissionTransfer::ItemInt make_rally_item()
{
    MAVLinkMissionTransfer::ItemInt item;
    item.mission_type = MAV_MISSION_TYPE_RALLY;
    return item;
}

TEST(MAVLinkMissionTransfer, UploadMissionDoesComplainAboutNoItems)
{
    MockSender mock_sender;
    FakeReceiver fake_receiver;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, fake_receiver, timeout_handler);

    std::vector<MAVLinkMissionTransfer::ItemInt> items;

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.upload_items_async(items, [&prom](Result result) {
        EXPECT_EQ(result, Result::NoMissionAvailable);
        ONCE_ONLY;
        prom.set_value();
    });

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);
}

TEST(MAVLinkMissionTransfer, UploadMissionDoesComplainAboutInconsistentMissionTypes)
{
    MockSender mock_sender;
    FakeReceiver fake_receiver;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, fake_receiver, timeout_handler);

    std::vector<MAVLinkMissionTransfer::ItemInt> items;
    items.push_back(make_mission_item());
    items.push_back(make_geofence_item());
    items.push_back(make_mission_item());

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.upload_items_async(items, [&prom](Result result) {
        EXPECT_EQ(result, Result::MissionTypeNotConsistent);
        ONCE_ONLY;
        prom.set_value();
    });

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);
}

TEST(MAVLinkMissionTransfer, UploadMissionDoesNotCrashIfCallbackIsNull)
{
    MockSender mock_sender;
    FakeReceiver fake_receiver;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, fake_receiver, timeout_handler);

    ON_CALL(mock_sender, send_message(_))
        .WillByDefault(Return(false));

    std::vector<MAVLinkMissionTransfer::ItemInt> items;
    mmt.upload_items_async(items, nullptr);

    items.push_back(make_mission_item());
    items.push_back(make_mission_item());

    mmt.upload_items_async(items, nullptr);
}

TEST(MAVLinkMissionTransfer, UploadMissionReturnsConnectionErrorWhenSendMessageFails)
{
    MockSender mock_sender;
    FakeReceiver fake_receiver;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, fake_receiver, timeout_handler);

    ON_CALL(mock_sender, send_message(_))
        .WillByDefault(Return(false));

    std::vector<MAVLinkMissionTransfer::ItemInt> items;
    items.push_back(make_mission_item());
    items.push_back(make_mission_item());

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.upload_items_async(items, [&prom](Result result) {
        EXPECT_EQ(result, Result::ConnectionError);
        ONCE_ONLY;
        prom.set_value();
    });

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    // We want to be sure a timeout is not still triggered later.
    time.sleep_for(
        std::chrono::milliseconds(
            static_cast<int>(MAVLinkMissionTransfer::timeout_s * 1.1) * 1000));
    timeout_handler.run_once();
}

TEST(MAVLinkMissionTransfer, UploadMissionSendsCount)
{
    MockSender mock_sender;
    FakeReceiver fake_receiver;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, fake_receiver, timeout_handler);

    std::vector<MAVLinkMissionTransfer::ItemInt> items;
    items.push_back(make_geofence_item());
    items.push_back(make_geofence_item());

    ON_CALL(mock_sender, send_message(_))
        .WillByDefault(Return(true));

    EXPECT_CALL(mock_sender, send_message(Truly(
                    [&items](const mavlink_message_t& message) {

        mavlink_mission_count_t mission_count;
        mavlink_msg_mission_count_decode(&message, &mission_count);

        return (message.msgid == MAVLINK_MSG_ID_MISSION_COUNT &&
                message.sysid == config.own_system_id &&
                message.compid == config.own_component_id &&
                mission_count.target_system == config.target_system_id &&
                mission_count.target_component == config.target_component_id &&
                mission_count.count == items.size() &&
                mission_count.mission_type == items[0].mission_type);
    })));

    mmt.upload_items_async(items, [](Result result) {
        UNUSED(result);
        EXPECT_TRUE(false);
    });
}

TEST(MAVLinkMissionTransfer, UploadMissionTimeoutAfterSendCount)
{
    MockSender mock_sender;
    FakeReceiver fake_receiver;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, fake_receiver, timeout_handler);

    std::vector<MAVLinkMissionTransfer::ItemInt> items;
    items.push_back(make_mission_item());
    items.push_back(make_mission_item());

    ON_CALL(mock_sender, send_message(_))
        .WillByDefault(Return(true));

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.upload_items_async(items, [&prom](Result result) {
        EXPECT_EQ(result, Result::Timeout);
        ONCE_ONLY;
        prom.set_value();
    });

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(0)), std::future_status::timeout);

    time.sleep_for(
        std::chrono::milliseconds(
            static_cast<int>(MAVLinkMissionTransfer::timeout_s * 1.1) * 1000));
    timeout_handler.run_once();

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);
}
