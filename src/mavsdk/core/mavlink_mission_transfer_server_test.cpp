#include <algorithm>
#include <chrono>
#include <future>
#include <gtest/gtest.h>

#include "mavlink_mission_transfer_server.h"
#include "mocks/sender_mock.h"
#include "unused.h"

using namespace mavsdk;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Truly;
using MockSender = NiceMock<mavsdk::testing::MockSender>;

using Result = MavlinkMissionTransferServer::Result;
using ItemInt = MavlinkMissionTransferServer::ItemInt;

static MavlinkAddress own_address{42, 16};
static uint8_t channel{0};
static MavlinkAddress target_address{99, MAV_COMP_ID_AUTOPILOT1};

static constexpr double timeout_s = 0.5;

#define ONCE_ONLY \
    static bool called = false; \
    EXPECT_FALSE(called); \
    called = true

class MavlinkMissionTransferServerTest : public ::testing::Test {
protected:
    MavlinkMissionTransferServerTest() :
        ::testing::Test(),
        timeout_handler(time),
        mmt(mock_sender, message_handler, timeout_handler, []() { return timeout_s; })
    {}

    void SetUp() override
    {
        ON_CALL(mock_sender, get_own_system_id()).WillByDefault(Return(own_address.system_id));
        ON_CALL(mock_sender, get_own_component_id())
            .WillByDefault(Return(own_address.component_id));
        ON_CALL(mock_sender, autopilot()).WillByDefault(Return(Autopilot::Px4));
    }

    MockSender mock_sender;
    MavlinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler;
    MavlinkMissionTransferServer mmt;
};

static ItemInt make_item(uint8_t type, uint16_t sequence)
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

static mavlink_message_t make_mission_count(unsigned count)
{
    mavlink_message_t message;
    mavlink_msg_mission_count_pack(
        own_address.system_id,
        own_address.component_id,
        &message,
        target_address.system_id,
        target_address.component_id,
        count,
        MAV_MISSION_TYPE_MISSION,
        0);
    return message;
}

bool is_correct_autopilot_mission_request_int(
    uint8_t type, unsigned sequence, uint8_t target_component, const mavlink_message_t& message)
{
    if (message.msgid != MAVLINK_MSG_ID_MISSION_REQUEST_INT) {
        return false;
    }

    mavlink_mission_request_int_t mission_request_int;
    mavlink_msg_mission_request_int_decode(&message, &mission_request_int);
    return (
        message.sysid == own_address.system_id && message.compid == own_address.component_id &&
        mission_request_int.target_system == target_address.system_id &&
        mission_request_int.target_component == target_component &&
        mission_request_int.seq == sequence && mission_request_int.mission_type == type);
}

bool is_correct_autopilot_mission_ack(
    uint8_t type, uint8_t result, uint8_t target_component, const mavlink_message_t& message)
{
    if (message.msgid != MAVLINK_MSG_ID_MISSION_ACK) {
        return false;
    }

    mavlink_mission_ack_t ack;
    mavlink_msg_mission_ack_decode(&message, &ack);
    return (
        message.sysid == own_address.system_id && message.compid == own_address.component_id &&
        ack.target_system == target_address.system_id && ack.target_component == target_component &&
        ack.type == result && ack.mission_type == type);
}

bool is_the_same_mission_item_int(const ItemInt& item, const mavlink_message_t& message)
{
    if (message.msgid != MAVLINK_MSG_ID_MISSION_ITEM_INT) {
        return false;
    }
    mavlink_mission_item_int_t mission_item_int;
    mavlink_msg_mission_item_int_decode(&message, &mission_item_int);

    return (
        message.sysid == own_address.system_id && //
        message.compid == own_address.component_id && //
        mission_item_int.target_system == target_address.system_id && //
        mission_item_int.target_component == target_address.component_id && //
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

TEST_F(MavlinkMissionTransferServerTest, ReceiveIncomingMissionSendsMissionRequests)
{
    ON_CALL(mock_sender, queue_message(_)).WillByDefault(Return(true));

    std::vector<ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 1));

    mmt.receive_incoming_items_async(
        MAV_MISSION_TYPE_MISSION,
        items.size(),
        target_address.system_id,
        target_address.component_id,
        [](Result result, const std::vector<ItemInt>& output_items) {
            UNUSED(output_items);
            UNUSED(result);
            EXPECT_TRUE(false);
        });

    EXPECT_CALL(
        mock_sender,
        queue_message(Truly([](std::function<mavlink_message_t(
                                   MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_correct_autopilot_mission_request_int(
                MAV_MISSION_TYPE_MISSION,
                0,
                target_address.component_id,
                fun(own_address, channel));
        })));

    mmt.do_work();

    message_handler.process_message(make_mission_count(items.size()));
}

TEST_F(
    MavlinkMissionTransferServerTest,
    ReceiveIncomingMissionResendsMissionRequestsAndTimesOutEventually)
{
    ON_CALL(mock_sender, queue_message(_)).WillByDefault(Return(true));

    std::vector<ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 1));

    std::promise<void> prom;
    auto fut = prom.get_future();
    mmt.receive_incoming_items_async(
        MAV_MISSION_TYPE_MISSION,
        items.size(),
        target_address.system_id,
        target_address.component_id,
        [&prom](Result result, const std::vector<ItemInt>& output_items) {
            UNUSED(output_items);
            EXPECT_EQ(result, Result::Timeout);
            prom.set_value();
        });

    EXPECT_CALL(
        mock_sender,
        queue_message(Truly([](std::function<mavlink_message_t(
                                   MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_correct_autopilot_mission_request_int(
                MAV_MISSION_TYPE_MISSION,
                0,
                target_address.component_id,
                fun(own_address, channel));
        })))
        .Times(MavlinkMissionTransferServer::retries);

    mmt.do_work();

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(0)), std::future_status::timeout);

    // After the specified retries we should give up with a timeout.
    for (unsigned i = 0; i < MavlinkMissionTransferServer::retries; ++i) {
        time.sleep_for(std::chrono::milliseconds(static_cast<int>(timeout_s * 1.1 * 1000.)));
        timeout_handler.run_once();
    }

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

mavlink_message_t make_mission_item(const std::vector<ItemInt>& item_ints, std::size_t index)
{
    mavlink_message_t message;
    mavlink_msg_mission_item_int_pack(
        own_address.system_id,
        own_address.component_id,
        &message,
        target_address.system_id,
        target_address.component_id,
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

TEST_F(MavlinkMissionTransferServerTest, ReceiveIncomingMissionResendsRequestItemAgainForSecondItem)
{
    ON_CALL(mock_sender, queue_message(_)).WillByDefault(Return(true));

    std::vector<ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 1));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 2));

    std::promise<void> prom;
    auto fut = prom.get_future();
    mmt.receive_incoming_items_async(
        MAV_MISSION_TYPE_MISSION,
        items.size(),
        target_address.system_id,
        target_address.component_id,
        [&prom](Result result, const std::vector<ItemInt>& output_items) {
            UNUSED(output_items);
            EXPECT_EQ(result, Result::Timeout);
            prom.set_value();
        });

    // We almost exhaust the retries of the first one.
    EXPECT_CALL(
        mock_sender,
        queue_message(Truly([](std::function<mavlink_message_t(
                                   MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_correct_autopilot_mission_request_int(
                MAV_MISSION_TYPE_MISSION,
                0,
                target_address.component_id,
                fun(own_address, channel));
        })))
        .Times(MavlinkMissionTransferServer::retries - 1);

    mmt.do_work();

    for (unsigned i = 0; i < MavlinkMissionTransferServer::retries - 2; ++i) {
        time.sleep_for(std::chrono::milliseconds(static_cast<int>(timeout_s * 1.1 * 1000.)));
        timeout_handler.run_once();
    }

    // This time we go over the retry limit.
    EXPECT_CALL(
        mock_sender,
        queue_message(Truly([](std::function<mavlink_message_t(
                                   MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_correct_autopilot_mission_request_int(
                MAV_MISSION_TYPE_MISSION,
                1,
                target_address.component_id,
                fun(own_address, channel));
        })))
        .Times(MavlinkMissionTransferServer::retries);

    message_handler.process_message(make_mission_item(items, 0));

    for (unsigned i = 0; i < MavlinkMissionTransferServer::retries; ++i) {
        time.sleep_for(std::chrono::milliseconds(static_cast<int>(timeout_s * 1.1 * 1000.)));
        timeout_handler.run_once();
    }

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

TEST_F(MavlinkMissionTransferServerTest, ReceiveIncomingMissionEmptyList)
{
    ON_CALL(mock_sender, queue_message(_)).WillByDefault(Return(true));

    std::promise<void> prom;
    auto fut = prom.get_future();
    mmt.receive_incoming_items_async(
        MAV_MISSION_TYPE_MISSION,
        0,
        target_address.system_id,
        target_address.component_id,
        [&prom](Result result, const std::vector<ItemInt>& items) {
            UNUSED(items);
            EXPECT_EQ(result, Result::Success);
            prom.set_value();
        });

    EXPECT_CALL(
        mock_sender,
        queue_message(Truly([](std::function<mavlink_message_t(
                                   MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_correct_autopilot_mission_ack(
                MAV_MISSION_TYPE_MISSION,
                MAV_MISSION_ACCEPTED,
                target_address.component_id,
                fun(own_address, channel));
        })));

    mmt.do_work();

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    // We want to be sure a timeout is not still triggered later.
    time.sleep_for(std::chrono::milliseconds(static_cast<int>(timeout_s * 1.1 * 1000.)));
    timeout_handler.run_once();

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

TEST_F(MavlinkMissionTransferServerTest, ReceiveIncomingMissionCanBeCancelled)
{
    ON_CALL(mock_sender, queue_message(_)).WillByDefault(Return(true));

    std::vector<ItemInt> items;
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 0));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 1));
    items.push_back(make_item(MAV_MISSION_TYPE_MISSION, 2));

    std::promise<void> prom;
    auto fut = prom.get_future();
    auto transfer = mmt.receive_incoming_items_async(
        MAV_MISSION_TYPE_MISSION,
        items.size(),
        target_address.system_id,
        target_address.component_id,
        [&prom](Result result, const std::vector<ItemInt>& output_items) {
            UNUSED(output_items);
            EXPECT_EQ(result, Result::Cancelled);
            prom.set_value();
        });
    mmt.do_work();

    message_handler.process_message(make_mission_item(items, 0));

    EXPECT_CALL(
        mock_sender,
        queue_message(Truly([](std::function<mavlink_message_t(
                                   MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_correct_autopilot_mission_ack(
                MAV_MISSION_TYPE_MISSION,
                MAV_MISSION_OPERATION_CANCELLED,
                target_address.component_id,
                fun(own_address, channel));
        })));

    auto ptr = transfer.lock();
    EXPECT_TRUE(ptr);
    if (ptr) {
        ptr->cancel();
    }

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}
