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
using ItemInt = MAVLinkMissionTransfer::ItemInt;

static MAVLinkMissionTransfer::Config config{42, 16, 99, 101};

#define ONCE_ONLY \
    static bool called = false; \
    EXPECT_FALSE(called); \
    called = true;

ItemInt make_item(uint8_t type, uint16_t sequence)
{
    ItemInt item;

    item.seq = sequence;
    item.frame = MAV_FRAME_MISSION;
    item.command = MAV_CMD_NAV_WAYPOINT;
    item.current = uint8_t(sequence == 0 ? 1 : 0);
    item.autocontinue = 1;
    item.param1 = 1.0f;
    item.param2 = 2.0f;
    item.param3 = 3.0f;
    item.param4 = 4.0f;
    item.x = 5;
    item.y = 6;
    item.z = 7.0f;
    item.mission_type = type;

    return item;
}

TEST(MAVLinkMissionTransfer, UploadMissionDoesComplainAboutNoItems)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    std::vector<ItemInt> items;

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.upload_items_async(MAV_MISSION_TYPE_MISSION, items, [&prom](Result result) {
        EXPECT_EQ(result, Result::NoMissionAvailable);
        ONCE_ONLY;
        prom.set_value();
    });
    mmt.do_work();

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

TEST(MAVLinkMissionTransfer, UploadMissionDoesComplainAboutWrongSequence)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    std::vector<ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 2));

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.upload_items_async(MAV_MISSION_TYPE_MISSION, items, [&prom](Result result) {
        EXPECT_EQ(result, Result::InvalidSequence);
        ONCE_ONLY;
        prom.set_value();
    });
    mmt.do_work();

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

TEST(MAVLinkMissionTransfer, UploadMissionDoesComplainAboutInconsistentMissionTypesInAPI)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    std::vector<ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_FENCE, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_FENCE, 1));
    items.push_back(make_item(MAV_MISSION_TYPE_FENCE, 2));

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.upload_items_async(MAV_MISSION_TYPE_MISSION, items, [&prom](Result result) {
        EXPECT_EQ(result, Result::MissionTypeNotConsistent);
        ONCE_ONLY;
        prom.set_value();
    });
    mmt.do_work();

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

TEST(MAVLinkMissionTransfer, UploadMissionDoesComplainAboutInconsistentMissionTypesInItems)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    std::vector<ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_FENCE, 1));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 2));

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.upload_items_async(MAV_MISSION_TYPE_MISSION, items, [&prom](Result result) {
        EXPECT_EQ(result, Result::MissionTypeNotConsistent);
        ONCE_ONLY;
        prom.set_value();
    });
    mmt.do_work();

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

TEST(MAVLinkMissionTransfer, UploadMissionDoesComplainAboutNoCurrent)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    std::vector<ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 1));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 2));
    // Remove the current flag again.
    items[0].current = 0;

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.upload_items_async(MAV_MISSION_TYPE_MISSION, items, [&prom](Result result) {
        EXPECT_EQ(result, Result::CurrentInvalid);
        ONCE_ONLY;
        prom.set_value();
    });
    mmt.do_work();

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

TEST(MAVLinkMissionTransfer, UploadMissionDoesComplainAboutTwoCurrents)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    std::vector<ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 1));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 2));
    // Add another current.
    items[1].current = 1;

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.upload_items_async(MAV_MISSION_TYPE_MISSION, items, [&prom](Result result) {
        EXPECT_EQ(result, Result::CurrentInvalid);
        ONCE_ONLY;
        prom.set_value();
    });
    mmt.do_work();

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
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
    std::vector<ItemInt> items;
    mmt.upload_items_async(MAV_MISSION_TYPE_MISSION, items, nullptr);
    mmt.do_work();

    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 1));

    mmt.upload_items_async(MAV_MISSION_TYPE_MISSION, items, nullptr);
    mmt.do_work();

    // Catch the WrongSequence case as well.
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 3));
    mmt.upload_items_async(MAV_MISSION_TYPE_MISSION, items, nullptr);
    mmt.do_work();

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

TEST(MAVLinkMissionTransfer, UploadMissionReturnsConnectionErrorWhenSendMessageFails)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    ON_CALL(mock_sender, send_message(_)).WillByDefault(Return(false));

    std::vector<ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 1));

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.upload_items_async(MAV_MISSION_TYPE_MISSION, items, [&prom](Result result) {
        EXPECT_EQ(result, Result::ConnectionError);
        ONCE_ONLY;
        prom.set_value();
    });
    mmt.do_work();

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    // We want to be sure a timeout is not still triggered later.
    time.sleep_for(std::chrono::milliseconds(
        static_cast<int>(MAVLinkMissionTransfer::timeout_s * 1.1 * 1000.)));
    timeout_handler.run_once();

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

bool is_correct_mission_send_count(uint8_t type, unsigned count, const mavlink_message_t& message)
{
    if (message.msgid != MAVLINK_MSG_ID_MISSION_COUNT) {
        return false;
    }

    mavlink_mission_count_t mission_count;
    mavlink_msg_mission_count_decode(&message, &mission_count);
    return (
        message.msgid == MAVLINK_MSG_ID_MISSION_COUNT && message.sysid == config.own_system_id &&
        message.compid == config.own_component_id &&
        mission_count.target_system == config.target_system_id &&
        mission_count.target_component == config.target_component_id &&
        mission_count.count == count && mission_count.mission_type == type);
}

TEST(MAVLinkMissionTransfer, UploadMissionSendsCount)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    std::vector<ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_FENCE, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_FENCE, 1));

    ON_CALL(mock_sender, send_message(_)).WillByDefault(Return(true));

    EXPECT_CALL(mock_sender, send_message(Truly([&items](const mavlink_message_t& message) {
                    return is_correct_mission_send_count(
                        MAV_MISSION_TYPE_FENCE, items.size(), message);
                })));

    mmt.upload_items_async(MAV_MISSION_TYPE_FENCE, items, [](Result result) {
        UNUSED(result);
        EXPECT_TRUE(false);
    });
    mmt.do_work();
}

TEST(MAVLinkMissionTransfer, UploadMissionResendsCount)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    std::vector<ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_FENCE, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_FENCE, 1));

    ON_CALL(mock_sender, send_message(_)).WillByDefault(Return(true));

    EXPECT_CALL(mock_sender, send_message(Truly([&items](const mavlink_message_t& message) {
                    return is_correct_mission_send_count(
                        MAV_MISSION_TYPE_FENCE, items.size(), message);
                })))
        .Times(2);

    mmt.upload_items_async(MAV_MISSION_TYPE_FENCE, items, [](Result result) {
        UNUSED(result);
        EXPECT_TRUE(false);
    });
    mmt.do_work();

    time.sleep_for(std::chrono::milliseconds(
        static_cast<int>(MAVLinkMissionTransfer::timeout_s * 1.1 * 1000.)));
    timeout_handler.run_once();
}

TEST(MAVLinkMissionTransfer, UploadMissionTimeoutAfterSendCount)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    std::vector<ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 1));

    ON_CALL(mock_sender, send_message(_)).WillByDefault(Return(true));

    EXPECT_CALL(mock_sender, send_message(Truly([&items](const mavlink_message_t& message) {
                    return is_correct_mission_send_count(
                        MAV_MISSION_TYPE_MISSION, items.size(), message);
                })))
        .Times(MAVLinkMissionTransfer::retries);

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.upload_items_async(MAV_MISSION_TYPE_MISSION, items, [&prom](Result result) {
        EXPECT_EQ(result, Result::Timeout);
        ONCE_ONLY;
        prom.set_value();
    });
    mmt.do_work();

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(0)), std::future_status::timeout);

    // After the specified retries we should give up with a timeout.
    for (unsigned i = 0; i < MAVLinkMissionTransfer::retries; ++i) {
        time.sleep_for(std::chrono::milliseconds(
            static_cast<int>(MAVLinkMissionTransfer::timeout_s * 1.1 * 1000.)));
        timeout_handler.run_once();
    }

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

mavlink_message_t make_mission_ack(uint8_t result)
{
    mavlink_message_t message;
    mavlink_msg_mission_ack_pack(
        config.own_system_id,
        config.own_component_id,
        &message,
        config.target_system_id,
        config.target_component_id,
        result,
        MAV_MISSION_TYPE_MISSION);
    return message;
}

bool is_the_same_mission_item_int(const ItemInt& item, const mavlink_message_t& message)
{
    if (message.msgid != MAVLINK_MSG_ID_MISSION_ITEM_INT) {
        return false;
    }
    mavlink_mission_item_int_t mission_item_int;
    mavlink_msg_mission_item_int_decode(&message, &mission_item_int);

    return (
        message.sysid == config.own_system_id && //
        message.compid == config.own_component_id && //
        mission_item_int.target_system == config.target_system_id && //
        mission_item_int.target_component == config.target_component_id && //
        mission_item_int.seq == item.seq && //
        mission_item_int.frame == item.frame && //
        mission_item_int.command == item.command && //
        mission_item_int.current == item.current && //
        mission_item_int.autocontinue == item.autocontinue && //
        mission_item_int.param1 == item.param1 && //
        mission_item_int.param2 == item.param2 && //
        mission_item_int.param3 == item.param3 && //
        mission_item_int.param4 == item.param4 && //
        mission_item_int.x == item.x && //
        mission_item_int.y == item.y && //
        mission_item_int.z == item.z && //
        mission_item_int.mission_type == item.mission_type);
}

TEST(MAVLinkMissionTransfer, UploadMissionSendsMissionItems)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    std::vector<ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 1));

    ON_CALL(mock_sender, send_message(_)).WillByDefault(Return(true));

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.upload_items_async(MAV_MISSION_TYPE_MISSION, items, [&prom](Result result) {
        EXPECT_EQ(result, Result::Success);
        ONCE_ONLY;
        prom.set_value();
    });
    mmt.do_work();

    EXPECT_CALL(mock_sender, send_message(Truly([&items](const mavlink_message_t& message) {
                    return is_the_same_mission_item_int(items[0], message);
                })));

    message_handler.process_message(make_mission_item_request(0));

    EXPECT_CALL(mock_sender, send_message(Truly([&items](const mavlink_message_t& message) {
                    return is_the_same_mission_item_int(items[1], message);
                })));

    message_handler.process_message(make_mission_item_request(1));

    message_handler.process_message(make_mission_ack(MAV_MISSION_ACCEPTED));

    // We are finished and should have received the successful result.
    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    // We do not expect a timeout later though.
    time.sleep_for(std::chrono::milliseconds(
        static_cast<int>(MAVLinkMissionTransfer::timeout_s * 1.1 * 1000.)));
    timeout_handler.run_once();

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

TEST(MAVLinkMissionTransfer, UploadMissionResendsMissionItems)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    std::vector<ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 1));

    ON_CALL(mock_sender, send_message(_)).WillByDefault(Return(true));

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.upload_items_async(MAV_MISSION_TYPE_MISSION, items, [&prom](Result result) {
        EXPECT_EQ(result, Result::Success);
        ONCE_ONLY;
        prom.set_value();
    });
    mmt.do_work();

    EXPECT_CALL(mock_sender, send_message(Truly([&items](const mavlink_message_t& message) {
                    return is_the_same_mission_item_int(items[0], message);
                })));

    message_handler.process_message(make_mission_item_request(0));

    EXPECT_CALL(mock_sender, send_message(Truly([&items](const mavlink_message_t& message) {
                    return is_the_same_mission_item_int(items[0], message);
                })));

    // Request 0 again in case it had not arrived.
    message_handler.process_message(make_mission_item_request(0));

    EXPECT_CALL(mock_sender, send_message(Truly([&items](const mavlink_message_t& message) {
                    return is_the_same_mission_item_int(items[1], message);
                })));

    // Request 1 finally.
    message_handler.process_message(make_mission_item_request(1));

    message_handler.process_message(make_mission_ack(MAV_MISSION_ACCEPTED));

    // We are finished and should have received the successful result.
    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

TEST(MAVLinkMissionTransfer, UploadMissionResendsMissionItemsButGivesUpAfterSomeRetries)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    std::vector<ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_FENCE, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_FENCE, 1));

    ON_CALL(mock_sender, send_message(_)).WillByDefault(Return(true));

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.upload_items_async(MAV_MISSION_TYPE_FENCE, items, [&prom](Result result) {
        EXPECT_EQ(result, Result::Timeout);
        ONCE_ONLY;
        prom.set_value();
    });
    mmt.do_work();

    EXPECT_CALL(mock_sender, send_message(Truly([&items](const mavlink_message_t& message) {
                    return is_the_same_mission_item_int(items[0], message);
                })))
        .Times(MAVLinkMissionTransfer::retries);

    for (unsigned i = 0; i < MAVLinkMissionTransfer::retries; ++i) {
        message_handler.process_message(make_mission_item_request(0));
    }

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(0)), std::future_status::timeout);

    message_handler.process_message(make_mission_item_request(0));

    // We are finished and should have received the successful result.
    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

TEST(MAVLinkMissionTransfer, UploadMissionAckArrivesTooEarly)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    std::vector<ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 1));

    ON_CALL(mock_sender, send_message(_)).WillByDefault(Return(true));

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.upload_items_async(MAV_MISSION_TYPE_MISSION, items, [&prom](Result result) {
        EXPECT_EQ(result, Result::ProtocolError);
        ONCE_ONLY;
        prom.set_value();
    });
    mmt.do_work();

    EXPECT_CALL(mock_sender, send_message(Truly([&items](const mavlink_message_t& message) {
                    return is_the_same_mission_item_int(items[0], message);
                })));

    message_handler.process_message(make_mission_item_request(0));

    // Don't request item 1 but already send ack.
    message_handler.process_message(make_mission_ack(MAV_MISSION_ACCEPTED));

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

TEST(MAVLinkMissionTransfer, UploadMissionNacksAreHandled)
{
    const std::vector<std::pair<uint8_t, Result>> nack_cases{
        {MAV_MISSION_ERROR, Result::ProtocolError},
        {MAV_MISSION_UNSUPPORTED_FRAME, Result::UnsupportedFrame},
        {MAV_MISSION_UNSUPPORTED, Result::Unsupported},
        {MAV_MISSION_NO_SPACE, Result::TooManyMissionItems},
        {MAV_MISSION_INVALID, Result::InvalidParam},
        {MAV_MISSION_INVALID_PARAM1, Result::InvalidParam},
        {MAV_MISSION_INVALID_PARAM2, Result::InvalidParam},
        {MAV_MISSION_INVALID_PARAM3, Result::InvalidParam},
        {MAV_MISSION_INVALID_PARAM4, Result::InvalidParam},
        {MAV_MISSION_INVALID_PARAM5_X, Result::InvalidParam},
        {MAV_MISSION_INVALID_PARAM6_Y, Result::InvalidParam},
        {MAV_MISSION_INVALID_PARAM7, Result::InvalidParam},
        {MAV_MISSION_INVALID_SEQUENCE, Result::InvalidSequence},
        {MAV_MISSION_DENIED, Result::Denied},
        {MAV_MISSION_OPERATION_CANCELLED, Result::Cancelled},
    };

    for (const auto& nack_case : nack_cases) {
        MockSender mock_sender;
        MAVLinkMessageHandler message_handler;
        FakeTime time;
        TimeoutHandler timeout_handler(time);

        MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

        std::vector<ItemInt> items;
        items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
        items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 1));

        ON_CALL(mock_sender, send_message(_)).WillByDefault(Return(true));

        std::promise<void> prom;
        auto fut = prom.get_future();

        mmt.upload_items_async(MAV_MISSION_TYPE_MISSION, items, [&prom, &nack_case](Result result) {
            EXPECT_EQ(result, nack_case.second);
            prom.set_value();
        });
        mmt.do_work();

        EXPECT_CALL(mock_sender, send_message(Truly([&items](const mavlink_message_t& message) {
                        return is_the_same_mission_item_int(items[0], message);
                    })));

        message_handler.process_message(make_mission_item_request(0));

        // Send nack now.
        message_handler.process_message(make_mission_ack(nack_case.first));

        EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

        mmt.do_work();
        EXPECT_TRUE(mmt.is_idle());
    }
}

TEST(MAVLinkMissionTransfer, UploadMissionTimeoutNotTriggeredDuringTransfer)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    std::vector<ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 1));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 2));

    ON_CALL(mock_sender, send_message(_)).WillByDefault(Return(true));

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.upload_items_async(MAV_MISSION_TYPE_MISSION, items, [&prom](Result result) {
        EXPECT_EQ(result, Result::Success);
        ONCE_ONLY;
        prom.set_value();
    });
    mmt.do_work();

    EXPECT_CALL(mock_sender, send_message(Truly([&items](const mavlink_message_t& message) {
                    return is_the_same_mission_item_int(items[0], message);
                })));

    message_handler.process_message(make_mission_item_request(0));

    // We almost use up the max timeout in each cycle.
    time.sleep_for(std::chrono::milliseconds(
        static_cast<int>(MAVLinkMissionTransfer::timeout_s * 0.8 * 1000.)));
    timeout_handler.run_once();

    EXPECT_CALL(mock_sender, send_message(Truly([&items](const mavlink_message_t& message) {
                    return is_the_same_mission_item_int(items[1], message);
                })));

    message_handler.process_message(make_mission_item_request(1));

    time.sleep_for(std::chrono::milliseconds(
        static_cast<int>(MAVLinkMissionTransfer::timeout_s * 0.8 * 1000.)));
    timeout_handler.run_once();

    EXPECT_CALL(mock_sender, send_message(Truly([&items](const mavlink_message_t& message) {
                    return is_the_same_mission_item_int(items[2], message);
                })));

    message_handler.process_message(make_mission_item_request(2));

    time.sleep_for(std::chrono::milliseconds(
        static_cast<int>(MAVLinkMissionTransfer::timeout_s * 0.8 * 1000.)));
    timeout_handler.run_once();

    message_handler.process_message(make_mission_ack(MAV_MISSION_ACCEPTED));

    // We are finished and should have received the successful result.
    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

TEST(MAVLinkMissionTransfer, UploadMissionTimeoutAfterSendMissionItem)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    std::vector<ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 1));

    ON_CALL(mock_sender, send_message(_)).WillByDefault(Return(true));

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.upload_items_async(MAV_MISSION_TYPE_MISSION, items, [&prom](Result result) {
        EXPECT_EQ(result, Result::Timeout);
        ONCE_ONLY;
        prom.set_value();
    });
    mmt.do_work();

    EXPECT_CALL(mock_sender, send_message(Truly([&items](const mavlink_message_t& message) {
                    return is_the_same_mission_item_int(items[0], message);
                })));

    message_handler.process_message(make_mission_item_request(0));

    time.sleep_for(std::chrono::milliseconds(
        static_cast<int>(MAVLinkMissionTransfer::timeout_s * 1.1 * 1000.)));
    timeout_handler.run_once();

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    // Ignore later (wrong) ack.
    message_handler.process_message(make_mission_ack(MAV_MISSION_ACCEPTED));

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

TEST(MAVLinkMissionTransfer, UploadMissionDoesNotCrashOnRandomMessages)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    message_handler.process_message(make_mission_item_request(0));

    message_handler.process_message(make_mission_ack(MAV_MISSION_ACCEPTED));

    time.sleep_for(std::chrono::milliseconds(
        static_cast<int>(MAVLinkMissionTransfer::timeout_s * 1.1 * 1000.)));
    timeout_handler.run_once();

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

TEST(MAVLinkMissionTransfer, DownloadMissionSendsRequestList)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    ON_CALL(mock_sender, send_message(_)).WillByDefault(Return(true));

    EXPECT_CALL(mock_sender, send_message(Truly([](const mavlink_message_t& message) {
                    mavlink_mission_request_list_t mission_request_list;
                    mavlink_msg_mission_request_list_decode(&message, &mission_request_list);

                    return (
                        message.msgid == MAVLINK_MSG_ID_MISSION_REQUEST_LIST &&
                        message.sysid == config.own_system_id &&
                        message.compid == config.own_component_id &&
                        mission_request_list.target_system == config.target_system_id &&
                        mission_request_list.target_component == config.target_component_id &&
                        mission_request_list.mission_type == MAV_MISSION_TYPE_MISSION);
                })));

    mmt.download_items_async(
        MAV_MISSION_TYPE_MISSION, [](Result result, std::vector<ItemInt> items) {
            UNUSED(result);
            UNUSED(items);
            EXPECT_TRUE(false);
        });
    mmt.do_work();
}

bool is_correct_mission_request_list(uint8_t type, const mavlink_message_t& message)
{
    if (message.msgid != MAVLINK_MSG_ID_MISSION_REQUEST_LIST) {
        return false;
    }
    mavlink_mission_request_list_t mission_request_list;
    mavlink_msg_mission_request_list_decode(&message, &mission_request_list);

    return (
        message.sysid == config.own_system_id && //
        message.compid == config.own_component_id && //
        mission_request_list.target_system == config.target_system_id && //
        mission_request_list.target_component == config.target_component_id && //
        mission_request_list.mission_type == type);
}

TEST(MAVLinkMissionTransfer, DownloadMissionResendsRequestList)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    ON_CALL(mock_sender, send_message(_)).WillByDefault(Return(true));

    EXPECT_CALL(mock_sender, send_message(Truly([](const mavlink_message_t& message) {
                    return is_correct_mission_request_list(MAV_MISSION_TYPE_MISSION, message);
                })))
        .Times(2);

    mmt.download_items_async(
        MAV_MISSION_TYPE_MISSION, [](Result result, std::vector<ItemInt> items) {
            UNUSED(result);
            UNUSED(items);
            EXPECT_TRUE(false);
        });
    mmt.do_work();

    time.sleep_for(std::chrono::milliseconds(
        static_cast<int>(MAVLinkMissionTransfer::timeout_s * 1.1 * 1000.)));
    timeout_handler.run_once();
}

TEST(MAVLinkMissionTransfer, DownloadMissionResendsRequestListButGivesUpAfterSomeRetries)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    ON_CALL(mock_sender, send_message(_)).WillByDefault(Return(true));

    EXPECT_CALL(mock_sender, send_message(Truly([](const mavlink_message_t& message) {
                    return is_correct_mission_request_list(MAV_MISSION_TYPE_MISSION, message);
                })))
        .Times(MAVLinkMissionTransfer::retries);

    std::promise<void> prom;
    auto fut = prom.get_future();
    mmt.download_items_async(
        MAV_MISSION_TYPE_MISSION, [&prom](Result result, std::vector<ItemInt> items) {
            UNUSED(items);
            EXPECT_EQ(result, Result::Timeout);
            ONCE_ONLY;
            prom.set_value();
        });
    mmt.do_work();

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(0)), std::future_status::timeout);

    for (unsigned i = 0; i < MAVLinkMissionTransfer::retries; ++i) {
        time.sleep_for(std::chrono::milliseconds(
            static_cast<int>(MAVLinkMissionTransfer::timeout_s * 1.1 * 1000.)));
        timeout_handler.run_once();
    }

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

bool is_correct_mission_request_int(
    uint8_t type, unsigned sequence, const mavlink_message_t& message)
{
    if (message.msgid != MAVLINK_MSG_ID_MISSION_REQUEST_INT) {
        return false;
    }

    mavlink_mission_request_int_t mission_request_int;
    mavlink_msg_mission_request_int_decode(&message, &mission_request_int);
    return (
        message.sysid == config.own_system_id && message.compid == config.own_component_id &&
        mission_request_int.target_system == config.target_system_id &&
        mission_request_int.target_component == config.target_component_id &&
        mission_request_int.seq == sequence && mission_request_int.mission_type == type);
}

mavlink_message_t make_mission_count(unsigned count)
{
    mavlink_message_t message;
    mavlink_msg_mission_count_pack(
        config.own_system_id,
        config.own_component_id,
        &message,
        config.target_system_id,
        config.target_component_id,
        count,
        MAV_MISSION_TYPE_MISSION);
    return message;
}

TEST(MAVLinkMissionTransfer, DownloadMissionSendsMissionRequests)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    ON_CALL(mock_sender, send_message(_)).WillByDefault(Return(true));

    mmt.download_items_async(
        MAV_MISSION_TYPE_MISSION, [](Result result, std::vector<ItemInt> items) {
            UNUSED(items);
            UNUSED(result);
            EXPECT_TRUE(false);
        });
    mmt.do_work();

    EXPECT_CALL(mock_sender, send_message(Truly([](const mavlink_message_t& message) {
                    return is_correct_mission_request_int(MAV_MISSION_TYPE_MISSION, 0, message);
                })));

    std::vector<ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 1));

    message_handler.process_message(make_mission_count(items.size()));
}

TEST(MAVLinkMissionTransfer, DownloadMissionResendsMissionRequestsAndTimesOutEventually)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    ON_CALL(mock_sender, send_message(_)).WillByDefault(Return(true));

    std::promise<void> prom;
    auto fut = prom.get_future();
    mmt.download_items_async(
        MAV_MISSION_TYPE_MISSION, [&prom](Result result, std::vector<ItemInt> items) {
            UNUSED(items);
            EXPECT_EQ(result, Result::Timeout);
            prom.set_value();
        });
    mmt.do_work();

    EXPECT_CALL(mock_sender, send_message(Truly([](const mavlink_message_t& message) {
                    return is_correct_mission_request_int(MAV_MISSION_TYPE_MISSION, 0, message);
                })))
        .Times(MAVLinkMissionTransfer::retries);

    std::vector<ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 1));

    message_handler.process_message(make_mission_count(items.size()));

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(0)), std::future_status::timeout);

    // After the specified retries we should give up with a timeout.
    for (unsigned i = 0; i < MAVLinkMissionTransfer::retries; ++i) {
        time.sleep_for(std::chrono::milliseconds(
            static_cast<int>(MAVLinkMissionTransfer::timeout_s * 1.1 * 1000.)));
        timeout_handler.run_once();
    }

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

bool is_correct_mission_ack(uint8_t type, uint8_t result, const mavlink_message_t& message)
{
    if (message.msgid != MAVLINK_MSG_ID_MISSION_ACK) {
        return false;
    }

    mavlink_mission_ack_t ack;
    mavlink_msg_mission_ack_decode(&message, &ack);
    return (
        message.sysid == config.own_system_id && message.compid == config.own_component_id &&
        ack.target_system == config.target_system_id &&
        ack.target_component == config.target_component_id && ack.type == result &&
        ack.mission_type == type);
}

mavlink_message_t make_mission_item(const std::vector<ItemInt> item_ints, std::size_t index)
{
    mavlink_message_t message;
    mavlink_msg_mission_item_int_pack(
        config.own_system_id,
        config.own_component_id,
        &message,
        config.target_system_id,
        config.target_component_id,
        index,
        item_ints[index].frame,
        item_ints[index].command,
        item_ints[index].current,
        item_ints[index].autocontinue,
        item_ints[index].param1,
        item_ints[index].param2,
        item_ints[index].param3,
        item_ints[index].param4,
        item_ints[index].x,
        item_ints[index].y,
        item_ints[index].z,
        item_ints[index].mission_type);
    return message;
}

TEST(MAVLinkMissionTransfer, DownloadMissionSendsAllMissionRequestsAndAck)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    ON_CALL(mock_sender, send_message(_)).WillByDefault(Return(true));

    std::promise<void> prom;
    auto fut = prom.get_future();
    mmt.download_items_async(
        MAV_MISSION_TYPE_MISSION, [&prom](Result result, std::vector<ItemInt> items) {
            UNUSED(items);
            EXPECT_EQ(result, Result::Success);
            prom.set_value();
        });
    mmt.do_work();

    std::vector<ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 1));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 2));

    EXPECT_CALL(mock_sender, send_message(Truly([](const mavlink_message_t& message) {
                    return is_correct_mission_request_int(MAV_MISSION_TYPE_MISSION, 0, message);
                })));

    message_handler.process_message(make_mission_count(items.size()));

    EXPECT_CALL(mock_sender, send_message(Truly([](const mavlink_message_t& message) {
                    return is_correct_mission_request_int(MAV_MISSION_TYPE_MISSION, 1, message);
                })));

    message_handler.process_message(make_mission_item(items, 0));

    EXPECT_CALL(mock_sender, send_message(Truly([](const mavlink_message_t& message) {
                    return is_correct_mission_request_int(MAV_MISSION_TYPE_MISSION, 2, message);
                })));

    message_handler.process_message(make_mission_item(items, 1));

    EXPECT_CALL(mock_sender, send_message(Truly([](const mavlink_message_t& message) {
                    return is_correct_mission_ack(
                        MAV_MISSION_TYPE_MISSION, MAV_MISSION_ACCEPTED, message);
                })));

    message_handler.process_message(make_mission_item(items, 2));

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

TEST(MAVLinkMissionTransfer, DownloadMissionResendsRequestItemAgainForSecondItem)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    ON_CALL(mock_sender, send_message(_)).WillByDefault(Return(true));

    std::promise<void> prom;
    auto fut = prom.get_future();
    mmt.download_items_async(
        MAV_MISSION_TYPE_MISSION, [&prom](Result result, std::vector<ItemInt> items) {
            UNUSED(items);
            EXPECT_EQ(result, Result::Timeout);
            prom.set_value();
        });
    mmt.do_work();

    std::vector<ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 1));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 2));

    // We almost exhaust the retries of the first one.
    EXPECT_CALL(mock_sender, send_message(Truly([](const mavlink_message_t& message) {
                    return is_correct_mission_request_int(MAV_MISSION_TYPE_MISSION, 0, message);
                })))
        .Times(MAVLinkMissionTransfer::retries - 1);

    message_handler.process_message(make_mission_count(items.size()));

    for (unsigned i = 0; i < MAVLinkMissionTransfer::retries - 2; ++i) {
        time.sleep_for(std::chrono::milliseconds(
            static_cast<int>(MAVLinkMissionTransfer::timeout_s * 1.1 * 1000.)));
        timeout_handler.run_once();
    }

    // This time we go over the retry limit.
    EXPECT_CALL(mock_sender, send_message(Truly([](const mavlink_message_t& message) {
                    return is_correct_mission_request_int(MAV_MISSION_TYPE_MISSION, 1, message);
                })))
        .Times(MAVLinkMissionTransfer::retries);

    message_handler.process_message(make_mission_item(items, 0));

    for (unsigned i = 0; i < MAVLinkMissionTransfer::retries; ++i) {
        time.sleep_for(std::chrono::milliseconds(
            static_cast<int>(MAVLinkMissionTransfer::timeout_s * 1.1 * 1000.)));
        timeout_handler.run_once();
    }

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

TEST(MAVLinkMissionTransfer, DownloadMissionEmptyList)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    ON_CALL(mock_sender, send_message(_)).WillByDefault(Return(true));

    std::promise<void> prom;
    auto fut = prom.get_future();
    mmt.download_items_async(
        MAV_MISSION_TYPE_MISSION, [&prom](Result result, std::vector<ItemInt> items) {
            UNUSED(items);
            EXPECT_EQ(result, Result::Success);
            prom.set_value();
        });
    mmt.do_work();

    EXPECT_CALL(mock_sender, send_message(Truly([](const mavlink_message_t& message) {
                    return is_correct_mission_ack(
                        MAV_MISSION_TYPE_MISSION, MAV_MISSION_ACCEPTED, message);
                })));

    message_handler.process_message(make_mission_count(0));

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    // We want to be sure a timeout is not still triggered later.
    time.sleep_for(std::chrono::milliseconds(
        static_cast<int>(MAVLinkMissionTransfer::timeout_s * 1.1 * 1000.)));
    timeout_handler.run_once();

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

TEST(MAVLinkMissionTransfer, DownloadMissionTimeoutNotTriggeredDuringTransfer)
{
    MockSender mock_sender;
    MAVLinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler(time);

    MAVLinkMissionTransfer mmt(config, mock_sender, message_handler, timeout_handler);

    ON_CALL(mock_sender, send_message(_)).WillByDefault(Return(true));

    std::promise<void> prom;
    auto fut = prom.get_future();
    mmt.download_items_async(
        MAV_MISSION_TYPE_MISSION, [&prom](Result result, std::vector<ItemInt> items) {
            UNUSED(items);
            EXPECT_EQ(result, Result::Success);
            prom.set_value();
        });
    mmt.do_work();

    std::vector<ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 1));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 2));

    // We almost use up the max timeout in each cycle.
    time.sleep_for(std::chrono::milliseconds(static_cast<int>(
        MAVLinkMissionTransfer::timeout_s * MAVLinkMissionTransfer::retries * 0.8 * 1000.)));
    timeout_handler.run_once();

    message_handler.process_message(make_mission_count(items.size()));

    time.sleep_for(std::chrono::milliseconds(static_cast<int>(
        MAVLinkMissionTransfer::timeout_s * MAVLinkMissionTransfer::retries * 0.8 * 1000.)));
    timeout_handler.run_once();

    message_handler.process_message(make_mission_item(items, 0));

    time.sleep_for(std::chrono::milliseconds(static_cast<int>(
        MAVLinkMissionTransfer::timeout_s * MAVLinkMissionTransfer::retries * 0.8 * 1000.)));
    timeout_handler.run_once();

    message_handler.process_message(make_mission_item(items, 1));

    time.sleep_for(std::chrono::milliseconds(static_cast<int>(
        MAVLinkMissionTransfer::timeout_s * MAVLinkMissionTransfer::retries * 0.8 * 1000.)));
    timeout_handler.run_once();

    message_handler.process_message(make_mission_item(items, 2));

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}
