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

const static MAVLinkMissionTransfer::Config config{.own_system_id = 42,
                                                   .own_component_id = 16,
                                                   .target_system_id = 99,
                                                   .target_component_id = 101};

#define ONCE_ONLY \
    static bool called = false; \
    EXPECT_FALSE(called); \
    called = true;

MAVLinkMissionTransfer::ItemInt make_item(uint8_t type, uint16_t sequence)
{
    MAVLinkMissionTransfer::ItemInt item{
        .seq = sequence,
        .frame = MAV_FRAME_MISSION,
        .command = MAV_CMD_NAV_WAYPOINT,
        .current = uint8_t(sequence == 0 ? 1 : 0),
        .autocontinue = 1,
        .param1 = 1.0f,
        .param2 = 2.0f,
        .param3 = 3.0f,
        .param4 = 4.0f,
        .x = 5,
        .y = 6,
        .z = 7.0f,
        .mission_type = type,
    };
    return item;
}

TEST(MAVLinkMissionTransfer, UploadMissionDoesComplainAboutNoItems)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

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

TEST(MAVLinkMissionTransfer, UploadMissionDoesComplainAboutWrongSequence)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    std::vector<MAVLinkMissionTransfer::ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 2));

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.upload_items_async(items, [&prom](Result result) {
        EXPECT_EQ(result, Result::WrongSequence);
        ONCE_ONLY;
        prom.set_value();
    });

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);
}

TEST(MAVLinkMissionTransfer, UploadMissionDoesComplainAboutInconsistentMissionTypes)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    std::vector<MAVLinkMissionTransfer::ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_FENCE, 1));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 2));

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.upload_items_async(items, [&prom](Result result) {
        EXPECT_EQ(result, Result::MissionTypeNotConsistent);
        ONCE_ONLY;
        prom.set_value();
    });

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);
}

TEST(MAVLinkMissionTransfer, UploadMissionDoesComplainAboutNoCurrent)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    std::vector<MAVLinkMissionTransfer::ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 1));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 2));
    // Remove the current flag again.
    items[0].current = 0;

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.upload_items_async(items, [&prom](Result result) {
        EXPECT_EQ(result, Result::CurrentInvalid);
        ONCE_ONLY;
        prom.set_value();
    });

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);
}

TEST(MAVLinkMissionTransfer, UploadMissionDoesComplainAboutTwoCurrents)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    std::vector<MAVLinkMissionTransfer::ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 1));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 2));
    // Add another current.
    items[1].current = 1;

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.upload_items_async(items, [&prom](Result result) {
        EXPECT_EQ(result, Result::CurrentInvalid);
        ONCE_ONLY;
        prom.set_value();
    });

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);
}

TEST(MAVLinkMissionTransfer, UploadMissionDoesNotCrashIfCallbackIsNull)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    ON_CALL(mock_sender, send_message(_)).WillByDefault(Return(false));

    // Catch the empty case
    std::vector<MAVLinkMissionTransfer::ItemInt> items;
    mmt.upload_items_async(items, nullptr);

    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 1));

    mmt.upload_items_async(items, nullptr);

    // Catch the WrongSequence case as well.
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 3));
    mmt.upload_items_async(items, nullptr);
}

TEST(MAVLinkMissionTransfer, UploadMissionReturnsConnectionErrorWhenSendMessageFails)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    ON_CALL(mock_sender, send_message(_)).WillByDefault(Return(false));

    std::vector<MAVLinkMissionTransfer::ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 1));

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.upload_items_async(items, [&prom](Result result) {
        EXPECT_EQ(result, Result::ConnectionError);
        ONCE_ONLY;
        prom.set_value();
    });

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    // We want to be sure a timeout is not still triggered later.
    time.sleep_for(std::chrono::milliseconds(
        static_cast<int>(MAVLinkMissionTransfer::timeout_s * 1.1) * 1000));
    timeout_handler.run_once();
}

TEST(MAVLinkMissionTransfer, UploadMissionSendsCount)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    std::vector<MAVLinkMissionTransfer::ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_FENCE, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_FENCE, 1));

    ON_CALL(mock_sender, send_message(_)).WillByDefault(Return(true));

    EXPECT_CALL(mock_sender, send_message(Truly([&items](const mavlink_message_t& message) {
                    mavlink_mission_count_t mission_count;
                    mavlink_msg_mission_count_decode(&message, &mission_count);

                    return (
                        message.msgid == MAVLINK_MSG_ID_MISSION_COUNT &&
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
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    std::vector<MAVLinkMissionTransfer::ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 1));

    ON_CALL(mock_sender, send_message(_)).WillByDefault(Return(true));

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.upload_items_async(items, [&prom](Result result) {
        EXPECT_EQ(result, Result::Timeout);
        ONCE_ONLY;
        prom.set_value();
    });

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(0)), std::future_status::timeout);

    time.sleep_for(std::chrono::milliseconds(
        static_cast<int>(MAVLinkMissionTransfer::timeout_s * 1.1) * 1000));
    timeout_handler.run_once();

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);
}

mavlink_message_t make_mission_item_request(int sequence)
{
    mavlink_message_t message;
    mavlink_msg_mission_request_int_pack(
        config.own_system_id,
        config.own_component_id,
        &message,
        config.target_system_id,
        config.target_component_id,
        sequence,
        MAV_MISSION_TYPE_MISSION);
    return message;
}

TEST(MAVLinkMissionTransfer, UploadMissionSendsMissionItems)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    std::vector<MAVLinkMissionTransfer::ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 1));

    ON_CALL(mock_sender, send_message(_)).WillByDefault(Return(true));

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.upload_items_async(items, [&prom](Result result) {
        EXPECT_EQ(result, Result::Success);
        ONCE_ONLY;
        prom.set_value();
    });

    EXPECT_CALL(mock_sender, send_message(Truly([&items](const mavlink_message_t& message) {
                    mavlink_mission_item_int_t mission_item_int;
                    mavlink_msg_mission_item_int_decode(&message, &mission_item_int);

                    return (
                        message.msgid == MAVLINK_MSG_ID_MISSION_ITEM_INT &&
                        message.sysid == config.own_system_id &&
                        message.compid == config.own_component_id &&
                        mission_item_int.target_system == config.target_system_id &&
                        mission_item_int.target_component == config.target_component_id &&
                        mission_item_int.seq == 0 && // style
                        mission_item_int.frame == items[0].frame &&
                        mission_item_int.command == items[0].command &&
                        mission_item_int.current == items[0].current &&
                        mission_item_int.autocontinue == items[0].autocontinue &&
                        mission_item_int.param1 == items[0].param1 &&
                        mission_item_int.param2 == items[0].param2 &&
                        mission_item_int.param3 == items[0].param3 &&
                        mission_item_int.param4 == items[0].param4 &&
                        mission_item_int.x == items[0].x && // style
                        mission_item_int.y == items[0].y && // style
                        mission_item_int.z == items[0].z &&
                        mission_item_int.mission_type == items[0].mission_type);
                })));

    message_handler.process_message(make_mission_item_request(0));

    // EXPECT_EQ(fut.wait_for(std::chrono::seconds(0)), std::future_status::timeout);

    // time.sleep_for(
    //    std::chrono::milliseconds(
    //        static_cast<int>(MAVLinkMissionTransfer::timeout_s * 1.1) * 1000));
    // timeout_handler.run_once();

    // EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);
}
