#include <algorithm>
#include <chrono>
#include <future>
#include <vector>
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
static std::vector<uint8_t> mission_types{
    MAV_MISSION_TYPE_MISSION, MAV_MISSION_TYPE_FENCE, MAV_MISSION_TYPE_RALLY};

#define MISSION_TYPE_PARAMETERS \
    ::testing::Values(MAV_MISSION_TYPE_MISSION, MAV_MISSION_TYPE_FENCE, MAV_MISSION_TYPE_RALLY)

static constexpr double timeout_s = 0.5;

#define ONCE_ONLY \
    static bool called[3] = {false, false, false}; \
    EXPECT_FALSE(called[mission_type]); \
    called[mission_type] = true

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
        ON_CALL(mock_sender, compatibility_mode()).WillByDefault(Return(CompatibilityMode::Auto));
    }

    static ItemInt make_item(uint8_t type, uint16_t sequence)
    {
        ItemInt item;

        // Set the command based on the type to be somewhat realistic.
        switch (type) {
            case MAV_MISSION_TYPE_FENCE:
                item.command = MAV_CMD_NAV_FENCE_POLYGON_VERTEX_INCLUSION;
                break;
            case MAV_MISSION_TYPE_RALLY:
                item.command = MAV_CMD_NAV_RALLY_POINT;
                break;
            default:
                item.command = MAV_CMD_NAV_WAYPOINT;
                break;
        }

        item.seq = sequence;
        item.frame = MAV_FRAME_MISSION;

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

    static mavlink_message_t make_mission_count(unsigned count, uint8_t type)
    {
        mavlink_message_t message;
        mavlink_msg_mission_count_pack(
            own_address.system_id,
            own_address.component_id,
            &message,
            target_address.system_id,
            target_address.component_id,
            count,
            type,
            0);
        return message;
    }

    static bool is_correct_autopilot_mission_request_int(
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

    static bool is_correct_autopilot_mission_ack(
        uint8_t type, uint8_t result, uint8_t target_component, const mavlink_message_t& message)
    {
        if (message.msgid != MAVLINK_MSG_ID_MISSION_ACK) {
            return false;
        }

        mavlink_mission_ack_t ack;
        mavlink_msg_mission_ack_decode(&message, &ack);
        return (
            message.sysid == own_address.system_id && message.compid == own_address.component_id &&
            ack.target_system == target_address.system_id &&
            ack.target_component == target_component && ack.type == result &&
            ack.mission_type == type);
    }

    static bool is_the_same_mission_item_int(const ItemInt& item, const mavlink_message_t& message)
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

    static mavlink_message_t
    make_mission_item(const std::vector<ItemInt>& item_ints, std::size_t index)
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

    static bool
    is_correct_mission_send_count(uint8_t type, unsigned count, const mavlink_message_t& message)
    {
        if (message.msgid != MAVLINK_MSG_ID_MISSION_COUNT) {
            return false;
        }

        mavlink_mission_count_t mission_count;
        mavlink_msg_mission_count_decode(&message, &mission_count);

        return (
            message.msgid == MAVLINK_MSG_ID_MISSION_COUNT &&
            message.sysid == own_address.system_id && message.compid == own_address.component_id &&
            mission_count.target_system == target_address.system_id &&
            mission_count.target_component == target_address.component_id &&
            mission_count.count == count && mission_count.mission_type == type);
    }

    static mavlink_message_t make_mission_request_int(uint8_t type, int sequence)
    {
        mavlink_message_t message;
        mavlink_msg_mission_request_int_pack(
            own_address.system_id,
            own_address.component_id,
            &message,
            target_address.system_id,
            target_address.component_id,
            sequence,
            type);
        return message;
    }

    static mavlink_message_t make_mission_ack(uint8_t type, uint8_t result)
    {
        mavlink_message_t message;
        mavlink_msg_mission_ack_pack(
            own_address.system_id,
            own_address.component_id,
            &message,
            target_address.system_id,
            target_address.component_id,
            result,
            type,
            0);
        return message;
    }

    static mavlink_message_t make_mission_request(uint8_t type, int sequence)
    {
        mavlink_message_t message;
        mavlink_msg_mission_request_pack(
            target_address.system_id,
            target_address.component_id,
            &message,
            own_address.system_id,
            own_address.component_id,
            sequence,
            type);
        return message;
    }

    static bool
    is_correct_mission_ack(uint8_t type, uint8_t result, const mavlink_message_t& message)
    {
        if (message.msgid != MAVLINK_MSG_ID_MISSION_ACK) {
            return false;
        }

        mavlink_mission_ack_t ack;
        mavlink_msg_mission_ack_decode(&message, &ack);
        return (
            message.sysid == own_address.system_id && message.compid == own_address.component_id &&
            ack.target_system == target_address.system_id &&
            ack.target_component == target_address.component_id && ack.type == result &&
            ack.mission_type == type);
    }

    MockSender mock_sender;
    MavlinkMessageHandler message_handler;
    FakeTime time;
    TimeoutHandler timeout_handler;
    MavlinkMissionTransferServer mmt;
};

class MissionTypeParameterTest : public MavlinkMissionTransferServerTest,
                                 public ::testing::WithParamInterface<uint8_t> {
public:
    uint8_t mission_type{};

protected:
    void SetUp() override
    {
        MavlinkMissionTransferServerTest::SetUp();
        mission_type = GetParam();
    }
};

TEST_P(MissionTypeParameterTest, ReceiveIncomingMissionSendsMissionRequests)
{
    ON_CALL(mock_sender, queue_message(_)).WillByDefault(Return(true));

    std::vector<ItemInt> items;
    items.push_back(make_item(mission_type, 0));
    items.push_back(make_item(mission_type, 1));

    mmt.receive_incoming_items_async(
        mission_type,
        items.size(),
        target_address.system_id,
        target_address.component_id,
        [](Result result, uint8_t type, const std::vector<ItemInt>& output_items) {
            UNUSED(output_items);
            UNUSED(type);
            UNUSED(result);
            EXPECT_TRUE(false);
        });

    EXPECT_CALL(
        mock_sender,
        queue_message(Truly([&](std::function<mavlink_message_t(
                                    MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_correct_autopilot_mission_request_int(
                mission_type, 0, target_address.component_id, fun(own_address, channel));
        })));

    mmt.do_work();

    message_handler.process_message(make_mission_count(items.size(), mission_type));
}

TEST_P(MissionTypeParameterTest, ReceiveIncomingMissionResendsMissionRequestsAndTimesOutEventually)
{
    ON_CALL(mock_sender, queue_message(_)).WillByDefault(Return(true));

    std::vector<ItemInt> items;
    items.push_back(make_item(mission_type, 0));
    items.push_back(make_item(mission_type, 1));

    std::promise<void> prom;
    auto fut = prom.get_future();
    mmt.receive_incoming_items_async(
        mission_type,
        items.size(),
        target_address.system_id,
        target_address.component_id,
        [&prom](Result result, uint8_t type, const std::vector<ItemInt>& output_items) {
            UNUSED(output_items);
            UNUSED(type);
            EXPECT_EQ(result, Result::Timeout);
            prom.set_value();
        });

    EXPECT_CALL(
        mock_sender,
        queue_message(Truly([&](std::function<mavlink_message_t(
                                    MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_correct_autopilot_mission_request_int(
                mission_type, 0, target_address.component_id, fun(own_address, channel));
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

TEST_P(MissionTypeParameterTest, ReceiveIncomingMissionResendsRequestItemAgainForSecondItem)
{
    ON_CALL(mock_sender, queue_message(_)).WillByDefault(Return(true));

    std::vector<ItemInt> items;
    items.push_back(make_item(mission_type, 0));
    items.push_back(make_item(mission_type, 1));
    items.push_back(make_item(mission_type, 2));

    std::promise<void> prom;
    auto fut = prom.get_future();
    mmt.receive_incoming_items_async(
        mission_type,
        items.size(),
        target_address.system_id,
        target_address.component_id,
        [&prom](Result result, uint8_t type, const std::vector<ItemInt>& output_items) {
            UNUSED(output_items);
            UNUSED(type);
            EXPECT_EQ(result, Result::Timeout);
            prom.set_value();
        });

    // We almost exhaust the retries of the first one.
    EXPECT_CALL(
        mock_sender,
        queue_message(Truly([&](std::function<mavlink_message_t(
                                    MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_correct_autopilot_mission_request_int(
                mission_type, 0, target_address.component_id, fun(own_address, channel));
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
        queue_message(Truly([&](std::function<mavlink_message_t(
                                    MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_correct_autopilot_mission_request_int(
                mission_type, 1, target_address.component_id, fun(own_address, channel));
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

TEST_P(MissionTypeParameterTest, ReceiveIncomingMissionEmptyList)
{
    ON_CALL(mock_sender, queue_message(_)).WillByDefault(Return(true));

    std::promise<void> prom;
    auto fut = prom.get_future();
    mmt.receive_incoming_items_async(
        mission_type,
        0,
        target_address.system_id,
        target_address.component_id,
        [&prom](Result result, uint8_t type, const std::vector<ItemInt>& items) {
            UNUSED(items);
            UNUSED(type);
            EXPECT_EQ(result, Result::Success);
            prom.set_value();
        });

    EXPECT_CALL(
        mock_sender,
        queue_message(Truly([&](std::function<mavlink_message_t(
                                    MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_correct_autopilot_mission_ack(
                mission_type,
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

TEST_P(MissionTypeParameterTest, ReceiveIncomingMissionCanBeCancelled)
{
    ON_CALL(mock_sender, queue_message(_)).WillByDefault(Return(true));

    std::vector<ItemInt> items;
    items.push_back(make_item(mission_type, 0));
    items.push_back(make_item(mission_type, 1));
    items.push_back(make_item(mission_type, 2));

    std::promise<void> prom;
    auto fut = prom.get_future();
    auto transfer = mmt.receive_incoming_items_async(
        mission_type,
        items.size(),
        target_address.system_id,
        target_address.component_id,
        [&prom](Result result, uint8_t type, const std::vector<ItemInt>& output_items) {
            UNUSED(output_items);
            UNUSED(type);
            EXPECT_EQ(result, Result::Cancelled);
            prom.set_value();
        });
    mmt.do_work();

    message_handler.process_message(make_mission_item(items, 0));

    EXPECT_CALL(
        mock_sender,
        queue_message(Truly([&](std::function<mavlink_message_t(
                                    MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_correct_autopilot_mission_ack(
                mission_type,
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

TEST_P(MissionTypeParameterTest, SendOutgoingMissionEmptyMission)
{
    std::vector<ItemInt> items;

    std::promise<void> prom;
    auto fut = prom.get_future();

    ON_CALL(mock_sender, queue_message(_)).WillByDefault(Return(true));

    mmt.send_outgoing_items_async(
        mission_type,
        items,
        target_address.system_id,
        target_address.component_id,
        [&prom, this](Result result) {
            EXPECT_EQ(result, Result::Success);
            ONCE_ONLY;
            prom.set_value();
        });
    mmt.do_work();

    // empty mission should just send a zero count and be done
    message_handler.process_message(make_mission_ack(mission_type, MAV_MISSION_ACCEPTED));

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

TEST_P(MissionTypeParameterTest, SendOutgoingMissionDoesNotCrashIfCallbackIsNull)
{
    ON_CALL(mock_sender, queue_message(_)).WillByDefault(Return(false));

    // Catch the empty case
    std::vector<ItemInt> items;
    mmt.send_outgoing_items_async(
        mission_type, items, target_address.system_id, target_address.component_id, nullptr);
    mmt.do_work();

    items.push_back(make_item(mission_type, 0));
    items.push_back(make_item(mission_type, 1));

    mmt.send_outgoing_items_async(
        mission_type, items, target_address.system_id, target_address.component_id, nullptr);
    mmt.do_work();

    // Catch the WrongSequence case as well.
    items.push_back(make_item(mission_type, 3));
    mmt.send_outgoing_items_async(
        mission_type, items, target_address.system_id, target_address.component_id, nullptr);
    mmt.do_work();

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

TEST_P(MissionTypeParameterTest, SendOutgoingMissionReturnsConnectionErrorWhenSendMessageFails)
{
    ON_CALL(mock_sender, queue_message(_)).WillByDefault(Return(false));

    std::vector<ItemInt> items;
    items.push_back(make_item(mission_type, 0));
    items.push_back(make_item(mission_type, 1));

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.send_outgoing_items_async(
        mission_type,
        items,
        target_address.system_id,
        target_address.component_id,
        [&prom, this](Result result) {
            EXPECT_EQ(result, Result::ConnectionError);
            ONCE_ONLY;
            prom.set_value();
        });
    mmt.do_work();

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    // We want to be sure a timeout is not still triggered later.
    time.sleep_for(std::chrono::milliseconds(static_cast<int>(timeout_s * 1.1 * 1000.)));
    timeout_handler.run_once();

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

TEST_P(MissionTypeParameterTest, SendOutgoingMissionSendsCount)
{
    std::vector<ItemInt> items;
    items.push_back(make_item(mission_type, 0));
    items.push_back(make_item(mission_type, 1));

    ON_CALL(mock_sender, queue_message(_)).WillByDefault(Return(true));

    EXPECT_CALL(
        mock_sender,
        queue_message(Truly([&](std::function<mavlink_message_t(
                                    MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_correct_mission_send_count(
                mission_type, items.size(), fun(own_address, channel));
        })));

    mmt.send_outgoing_items_async(
        mission_type,
        items,
        target_address.system_id,
        target_address.component_id,
        [](Result result) {
            UNUSED(result);
            EXPECT_TRUE(false);
        });
    mmt.do_work();
}

TEST_P(MissionTypeParameterTest, SendOutgoingMissionResendsCount)
{
    std::vector<ItemInt> items;
    items.push_back(make_item(mission_type, 0));
    items.push_back(make_item(mission_type, 1));

    ON_CALL(mock_sender, queue_message(_)).WillByDefault(Return(true));

    EXPECT_CALL(
        mock_sender,
        queue_message(Truly([&](std::function<mavlink_message_t(
                                    MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_correct_mission_send_count(
                mission_type, items.size(), fun(own_address, channel));
        })))
        .Times(2);

    mmt.send_outgoing_items_async(
        mission_type,
        items,
        target_address.system_id,
        target_address.component_id,
        [](Result result) {
            UNUSED(result);
            EXPECT_TRUE(false);
        });
    mmt.do_work();

    time.sleep_for(std::chrono::milliseconds(static_cast<int>(timeout_s * 1.1 * 1000.)));
    timeout_handler.run_once();
}

TEST_P(MissionTypeParameterTest, SendOutgoingMissionTimeoutAfterSendCount)
{
    std::vector<ItemInt> items;
    items.push_back(make_item(mission_type, 0));
    items.push_back(make_item(mission_type, 1));

    ON_CALL(mock_sender, queue_message(_)).WillByDefault(Return(true));

    EXPECT_CALL(
        mock_sender,
        queue_message(Truly([&](std::function<mavlink_message_t(
                                    MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_correct_mission_send_count(
                mission_type, items.size(), fun(own_address, channel));
        })))
        .Times(MavlinkMissionTransferServer::retries);

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.send_outgoing_items_async(
        mission_type,
        items,
        target_address.system_id,
        target_address.component_id,
        [&prom, this](Result result) {
            EXPECT_EQ(result, Result::Timeout);
            ONCE_ONLY;
            prom.set_value();
        });
    mmt.do_work();

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(0)), std::future_status::timeout);

    // After the specified retries we should give up with a timeout.
    for (unsigned i = 0; i < MavlinkMissionTransferServer::retries; ++i) {
        time.sleep_for(std::chrono::milliseconds(static_cast<int>(timeout_s * 1.1 * 1000.)));
        timeout_handler.run_once();
    }

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);
}

TEST_P(MissionTypeParameterTest, SendOutgoingMissionSendsMissionItems)
{
    std::vector<ItemInt> items;
    items.push_back(make_item(mission_type, 0));
    items.push_back(make_item(mission_type, 1));

    ON_CALL(mock_sender, queue_message(_)).WillByDefault(Return(true));

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.send_outgoing_items_async(
        mission_type,
        items,
        target_address.system_id,
        target_address.component_id,
        [&prom, this](Result result) {
            EXPECT_EQ(result, Result::Success);
            ONCE_ONLY;
            prom.set_value();
        });
    mmt.do_work();

    EXPECT_CALL(
        mock_sender,
        queue_message(Truly([&items](std::function<mavlink_message_t(
                                         MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_the_same_mission_item_int(items[0], fun(own_address, channel));
        })));

    message_handler.process_message(make_mission_request_int(mission_type, 0));

    EXPECT_CALL(
        mock_sender,
        queue_message(Truly([&items](std::function<mavlink_message_t(
                                         MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_the_same_mission_item_int(items[1], fun(own_address, channel));
        })));

    message_handler.process_message(make_mission_request_int(mission_type, 1));

    message_handler.process_message(make_mission_ack(mission_type, MAV_MISSION_ACCEPTED));

    // We are finished and should have received the successful result.
    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    // We do not expect a timeout later though.
    time.sleep_for(std::chrono::milliseconds(static_cast<int>(timeout_s * 1.1 * 1000.)));
    timeout_handler.run_once();

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

TEST_P(MissionTypeParameterTest, SendOutgoingMissionResendsMissionItems)
{
    std::vector<ItemInt> items;
    items.push_back(make_item(mission_type, 0));
    items.push_back(make_item(mission_type, 1));

    ON_CALL(mock_sender, queue_message(_)).WillByDefault(Return(true));

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.send_outgoing_items_async(
        mission_type,
        items,
        target_address.system_id,
        target_address.component_id,
        [&prom, this](Result result) {
            EXPECT_EQ(result, Result::Success);
            ONCE_ONLY;
            prom.set_value();
        });
    mmt.do_work();

    EXPECT_CALL(
        mock_sender,
        queue_message(Truly([&items](std::function<mavlink_message_t(
                                         MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_the_same_mission_item_int(items[0], fun(own_address, channel));
        })));

    message_handler.process_message(make_mission_request_int(mission_type, 0));

    EXPECT_CALL(
        mock_sender,
        queue_message(Truly([&items](std::function<mavlink_message_t(
                                         MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_the_same_mission_item_int(items[0], fun(own_address, channel));
        })));

    // Request 0 again in case it had not arrived.
    message_handler.process_message(make_mission_request_int(mission_type, 0));

    EXPECT_CALL(
        mock_sender,
        queue_message(Truly([&items](std::function<mavlink_message_t(
                                         MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_the_same_mission_item_int(items[1], fun(own_address, channel));
        })));

    // Request 1 finally.
    message_handler.process_message(make_mission_request_int(mission_type, 1));

    message_handler.process_message(make_mission_ack(mission_type, MAV_MISSION_ACCEPTED));

    // We are finished and should have received the successful result.
    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

TEST_P(MissionTypeParameterTest, SendOutgoingMissionResendsMissionItemsButGivesUpAfterSomeRetries)
{
    std::vector<ItemInt> items;
    items.push_back(make_item(mission_type, 0));
    items.push_back(make_item(mission_type, 1));

    ON_CALL(mock_sender, queue_message(_)).WillByDefault(Return(true));

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.send_outgoing_items_async(
        mission_type,
        items,
        target_address.system_id,
        target_address.component_id,
        [&prom, this](Result result) {
            EXPECT_EQ(result, Result::Timeout);
            ONCE_ONLY;
            prom.set_value();
        });
    mmt.do_work();

    EXPECT_CALL(
        mock_sender,
        queue_message(Truly([&items](std::function<mavlink_message_t(
                                         MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_the_same_mission_item_int(items[0], fun(own_address, channel));
        })))
        .Times(MavlinkMissionTransferServer::retries);

    for (unsigned i = 0; i < MavlinkMissionTransferServer::retries; ++i) {
        message_handler.process_message(make_mission_request_int(mission_type, 0));
    }

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(0)), std::future_status::timeout);

    message_handler.process_message(make_mission_request_int(mission_type, 0));

    // We are finished and should have received the successful result.
    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

TEST_P(MissionTypeParameterTest, SendOutgoingMissionAckArrivesTooEarly)
{
    std::vector<ItemInt> items;
    items.push_back(make_item(mission_type, 0));
    items.push_back(make_item(mission_type, 1));

    ON_CALL(mock_sender, queue_message(_)).WillByDefault(Return(true));

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.send_outgoing_items_async(
        mission_type,
        items,
        target_address.system_id,
        target_address.component_id,
        [&prom, this](Result result) {
            EXPECT_EQ(result, Result::ProtocolError);
            ONCE_ONLY;
            prom.set_value();
        });
    mmt.do_work();

    EXPECT_CALL(
        mock_sender,
        queue_message(Truly([&items](std::function<mavlink_message_t(
                                         MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_the_same_mission_item_int(items[0], fun(own_address, channel));
        })));

    message_handler.process_message(make_mission_request_int(mission_type, 0));

    // Don't request item 1 but already send ack.
    message_handler.process_message(make_mission_ack(mission_type, MAV_MISSION_ACCEPTED));

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

class MavlinkMissionTransferServerNackTests
    : public MavlinkMissionTransferServerTest,
      public ::testing::WithParamInterface<std::tuple<uint8_t, uint8_t, Result>> {};

TEST_P(MavlinkMissionTransferServerNackTests, SendOutgoingMissionNackAreHandled)
{
    uint8_t mission_type = std::get<0>(GetParam());
    uint8_t mavlink_nack = std::get<1>(GetParam());
    Result mavsdk_nack = std::get<2>(GetParam());

    std::vector<ItemInt> items;
    items.push_back(make_item(mission_type, 0));
    items.push_back(make_item(mission_type, 1));

    ON_CALL(mock_sender, queue_message(_)).WillByDefault(Return(true));

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.send_outgoing_items_async(
        mission_type,
        items,
        target_address.system_id,
        target_address.component_id,
        [&prom, &mavsdk_nack](Result result) {
            EXPECT_EQ(result, mavsdk_nack);
            prom.set_value();
        });
    mmt.do_work();

    EXPECT_CALL(
        mock_sender,
        queue_message(Truly([&items](std::function<mavlink_message_t(
                                         MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_the_same_mission_item_int(items[0], fun(own_address, channel));
        })));

    message_handler.process_message(make_mission_request_int(mission_type, 0));

    // Send nack now.
    message_handler.process_message(make_mission_ack(mission_type, mavlink_nack));

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

INSTANTIATE_TEST_SUITE_P(
    MavlinkMissionTransferServerTests,
    MavlinkMissionTransferServerNackTests,
    ::testing::Values(
        std::make_tuple(MAV_MISSION_TYPE_MISSION, MAV_MISSION_ERROR, Result::ProtocolError),
        std::make_tuple(
            MAV_MISSION_TYPE_MISSION, MAV_MISSION_UNSUPPORTED_FRAME, Result::UnsupportedFrame),
        std::make_tuple(MAV_MISSION_TYPE_MISSION, MAV_MISSION_UNSUPPORTED, Result::Unsupported),
        std::make_tuple(
            MAV_MISSION_TYPE_MISSION, MAV_MISSION_NO_SPACE, Result::TooManyMissionItems),
        std::make_tuple(MAV_MISSION_TYPE_MISSION, MAV_MISSION_INVALID, Result::InvalidParam),
        std::make_tuple(MAV_MISSION_TYPE_FENCE, MAV_MISSION_INVALID_PARAM1, Result::InvalidParam),
        std::make_tuple(MAV_MISSION_TYPE_FENCE, MAV_MISSION_INVALID_PARAM2, Result::InvalidParam),
        std::make_tuple(MAV_MISSION_TYPE_FENCE, MAV_MISSION_INVALID_PARAM3, Result::InvalidParam),
        std::make_tuple(MAV_MISSION_TYPE_FENCE, MAV_MISSION_INVALID_PARAM4, Result::InvalidParam),
        std::make_tuple(MAV_MISSION_TYPE_FENCE, MAV_MISSION_INVALID_PARAM5_X, Result::InvalidParam),
        std::make_tuple(MAV_MISSION_TYPE_RALLY, MAV_MISSION_INVALID_PARAM6_Y, Result::InvalidParam),
        std::make_tuple(MAV_MISSION_TYPE_RALLY, MAV_MISSION_INVALID_PARAM7, Result::InvalidParam),
        std::make_tuple(
            MAV_MISSION_TYPE_RALLY, MAV_MISSION_INVALID_SEQUENCE, Result::InvalidSequence),
        std::make_tuple(MAV_MISSION_TYPE_RALLY, MAV_MISSION_DENIED, Result::Denied),
        std::make_tuple(
            MAV_MISSION_TYPE_RALLY, MAV_MISSION_OPERATION_CANCELLED, Result::Cancelled)));

TEST_P(MissionTypeParameterTest, SendOutgoingMissionTimeoutNotTriggeredDuringTransfer)
{
    std::vector<ItemInt> items;
    items.push_back(make_item(mission_type, 0));
    items.push_back(make_item(mission_type, 1));
    items.push_back(make_item(mission_type, 2));

    ON_CALL(mock_sender, queue_message(_)).WillByDefault(Return(true));

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.send_outgoing_items_async(
        mission_type,
        items,
        target_address.system_id,
        target_address.component_id,
        [&prom, this](Result result) {
            EXPECT_EQ(result, Result::Success);
            ONCE_ONLY;
            prom.set_value();
        });
    mmt.do_work();

    EXPECT_CALL(
        mock_sender,
        queue_message(Truly([&items](std::function<mavlink_message_t(
                                         MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_the_same_mission_item_int(items[0], fun(own_address, channel));
        })));

    message_handler.process_message(make_mission_request_int(mission_type, 0));

    // We almost use up the max timeout in each cycle.
    time.sleep_for(std::chrono::milliseconds(static_cast<int>(timeout_s * 0.8 * 1000.)));
    timeout_handler.run_once();

    EXPECT_CALL(
        mock_sender,
        queue_message(Truly([&items](std::function<mavlink_message_t(
                                         MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_the_same_mission_item_int(items[1], fun(own_address, channel));
        })));

    message_handler.process_message(make_mission_request_int(mission_type, 1));

    time.sleep_for(std::chrono::milliseconds(static_cast<int>(timeout_s * 0.8 * 1000.)));
    timeout_handler.run_once();

    EXPECT_CALL(
        mock_sender,
        queue_message(Truly([&items](std::function<mavlink_message_t(
                                         MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_the_same_mission_item_int(items[2], fun(own_address, channel));
        })));

    message_handler.process_message(make_mission_request_int(mission_type, 2));

    time.sleep_for(std::chrono::milliseconds(static_cast<int>(timeout_s * 0.8 * 1000.)));
    timeout_handler.run_once();

    message_handler.process_message(make_mission_ack(mission_type, MAV_MISSION_ACCEPTED));

    // We are finished and should have received the successful result.
    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

TEST_P(MissionTypeParameterTest, SendOutgoingMissionTimeoutAfterSendMissionItem)
{
    std::vector<ItemInt> items;
    items.push_back(make_item(mission_type, 0));
    items.push_back(make_item(mission_type, 1));

    ON_CALL(mock_sender, queue_message(_)).WillByDefault(Return(true));

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.send_outgoing_items_async(
        mission_type,
        items,
        target_address.system_id,
        target_address.component_id,
        [&prom, this](Result result) {
            EXPECT_EQ(result, Result::Timeout);
            ONCE_ONLY;
            prom.set_value();
        });
    mmt.do_work();

    EXPECT_CALL(
        mock_sender,
        queue_message(Truly([&items](std::function<mavlink_message_t(
                                         MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_the_same_mission_item_int(items[0], fun(own_address, channel));
        })));

    message_handler.process_message(make_mission_request_int(mission_type, 0));

    // Make sure single timeout does not trigger it yet.
    time.sleep_for(std::chrono::milliseconds(static_cast<int>(timeout_s * 1000. + 250)));
    timeout_handler.run_once();

    EXPECT_EQ(fut.wait_for(std::chrono::milliseconds(50)), std::future_status::timeout);

    // But multiple do.
    for (unsigned i = 0; i < (MavlinkMissionTransferServer::retries - 1); ++i) {
        time.sleep_for(std::chrono::milliseconds(static_cast<int>(timeout_s * 1000.)));
        timeout_handler.run_once();
    }

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    // Ignore later (wrong) ack.
    message_handler.process_message(make_mission_ack(mission_type, MAV_MISSION_ACCEPTED));

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

TEST_P(MissionTypeParameterTest, SendOutgoingMissionDoesNotCrashOnRandomMessages)
{
    message_handler.process_message(make_mission_request_int(mission_type, 0));

    message_handler.process_message(make_mission_ack(mission_type, MAV_MISSION_ACCEPTED));

    time.sleep_for(std::chrono::milliseconds(static_cast<int>(timeout_s * 1.1 * 1000.)));
    timeout_handler.run_once();

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

TEST_P(MissionTypeParameterTest, SendOutgoingMissionCanBeCancelled)
{
    std::vector<ItemInt> items;
    items.push_back(make_item(mission_type, 0));
    items.push_back(make_item(mission_type, 1));

    ON_CALL(mock_sender, queue_message(_)).WillByDefault(Return(true));

    std::promise<void> prom;
    auto fut = prom.get_future();

    auto transfer = mmt.send_outgoing_items_async(
        mission_type,
        items,
        target_address.system_id,
        target_address.component_id,
        [&prom, this](Result result) {
            EXPECT_EQ(result, Result::Cancelled);
            ONCE_ONLY;
            prom.set_value();
        });
    mmt.do_work();

    message_handler.process_message(make_mission_request_int(mission_type, 0));

    EXPECT_CALL(
        mock_sender,
        queue_message(Truly([&](std::function<mavlink_message_t(
                                    MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_correct_mission_ack(
                mission_type, MAV_MISSION_OPERATION_CANCELLED, fun(own_address, channel));
        })));

    auto ptr = transfer.lock();
    EXPECT_TRUE(ptr);
    if (ptr) {
        ptr->cancel();
    }

    // We are finished and should have received the successful result.
    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    // We do not expect a timeout later though.
    time.sleep_for(std::chrono::milliseconds(static_cast<int>(timeout_s * 1.1 * 1000.)));
    timeout_handler.run_once();

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

TEST_P(MissionTypeParameterTest, SendOutgoingMissionNacksNonIntCase)
{
    std::vector<ItemInt> items;
    items.push_back(make_item(mission_type, 0));

    ON_CALL(mock_sender, queue_message(_)).WillByDefault(Return(true));

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.send_outgoing_items_async(
        mission_type,
        items,
        target_address.system_id,
        target_address.component_id,
        [&prom, this](Result result) {
            EXPECT_EQ(result, Result::Success);
            ONCE_ONLY;
            prom.set_value();
        });
    mmt.do_work();

    EXPECT_CALL(
        mock_sender,
        queue_message(Truly([&](std::function<mavlink_message_t(
                                    MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_correct_mission_ack(
                mission_type, MAV_MISSION_UNSUPPORTED, fun(own_address, channel));
        })))
        .Times(1);

    // First the non-int wrong case comes in.
    message_handler.process_message(make_mission_request(mission_type, 0));

    EXPECT_CALL(
        mock_sender,
        queue_message(Truly([&items](std::function<mavlink_message_t(
                                         MavlinkAddress mavlink_address, uint8_t channel)> fun) {
            return is_the_same_mission_item_int(items[0], fun(own_address, channel));
        })));

    message_handler.process_message(make_mission_request_int(mission_type, 0));
    message_handler.process_message(make_mission_ack(mission_type, MAV_MISSION_ACCEPTED));

    // We are finished and should have received the successful result.
    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    // We do not expect a timeout later though.
    time.sleep_for(std::chrono::milliseconds(static_cast<int>(timeout_s * 1.1 * 1000.)));
    timeout_handler.run_once();

    mmt.do_work();
    EXPECT_TRUE(mmt.is_idle());
}

INSTANTIATE_TEST_SUITE_P(
    MavlinkMissionTransferServer, MissionTypeParameterTest, MISSION_TYPE_PARAMETERS);
