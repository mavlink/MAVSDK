#include <memory>
#include <string>
#include <gtest/gtest.h>
#include <asio/io_context.hpp>

#include "mavlink_message_handler.hpp"
#include "mavlink_parameter_client.hpp"
#include "mavlink_parameter_helper.hpp"
#include "mavsdk_time.hpp"
#include "timeout_handler.hpp"
#include "sender.hpp"
#include "mocks/sender_mock.hpp"

using namespace mavsdk;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnRef;
using MockSender = NiceMock<mavsdk::testing::MockSender>;

using Result = MavlinkParameterClient::Result;

static constexpr uint8_t target_system_id = 1;
static constexpr uint8_t autopilot_component_id = MAV_COMP_ID_AUTOPILOT1;
static constexpr uint8_t peripheral_component_id = 51;
static constexpr double timeout_s = 0.5;

namespace {

mavlink_message_t make_param_value_int(
    uint8_t system_id,
    uint8_t component_id,
    const std::string& name,
    int32_t value,
    uint16_t count = 1,
    uint16_t index = 0)
{
    ParamValue param_value;
    param_value.set(value);
    const auto param_id = param_id_to_message_buffer(name);

    mavlink_message_t message{};
    mavlink_msg_param_value_pack(
        system_id,
        component_id,
        &message,
        param_id.data(),
        param_value.get_4_float_bytes_bytewise(),
        param_value.get_mav_param_type(),
        count,
        index);
    return message;
}

mavlink_message_t make_param_ext_value_custom(
    uint8_t system_id,
    uint8_t component_id,
    const std::string& name,
    const std::string& value,
    uint16_t count = 1,
    uint16_t index = 0)
{
    ParamValue param_value;
    param_value.set(value);
    const auto param_id = param_id_to_message_buffer(name);
    const auto value_buf = param_value.get_128_bytes();

    mavlink_message_t message{};
    mavlink_msg_param_ext_value_pack(
        system_id,
        component_id,
        &message,
        param_id.data(),
        value_buf.data(),
        param_value.get_mav_param_ext_type(),
        count,
        index);
    return message;
}

mavlink_message_t make_param_ext_ack(
    uint8_t system_id,
    uint8_t component_id,
    const std::string& name,
    const std::string& value,
    uint8_t result)
{
    ParamValue param_value;
    param_value.set(value);
    const auto param_id = param_id_to_message_buffer(name);
    const auto value_buf = param_value.get_128_bytes();

    mavlink_message_t message{};
    mavlink_msg_param_ext_ack_pack(
        system_id,
        component_id,
        &message,
        param_id.data(),
        value_buf.data(),
        param_value.get_mav_param_ext_type(),
        result);
    return message;
}

} // namespace

class MavlinkParameterClientTest : public ::testing::Test {
protected:
    MavlinkParameterClientTest() : timeout_handler(time) {}

    void SetUp() override
    {
        ON_CALL(mock_sender, io_context()).WillByDefault(ReturnRef(io_context));
        ON_CALL(mock_sender, queue_message(_)).WillByDefault(Return(true));
    }

    void TearDown() override
    {
        io_context.restart();
        while (io_context.poll()) {
        }
    }

    std::unique_ptr<MavlinkParameterClient>
    make_client(uint8_t target_component_id, bool use_extended)
    {
        auto client = std::make_unique<MavlinkParameterClient>(
            mock_sender,
            message_handler,
            timeout_handler,
            []() { return timeout_s; },
            []() { return Autopilot::Px4; },
            target_system_id,
            target_component_id,
            use_extended);
        drain_io();
        return client;
    }

    void drain_io()
    {
        io_context.restart();
        while (io_context.poll()) {
        }
    }

    void deliver(const mavlink_message_t& message)
    {
        drain_io();
        message_handler.process_message(message);
        drain_io();
    }

    asio::io_context io_context;
    MockSender mock_sender;
    MavlinkMessageHandler message_handler{io_context};
    FakeTime time;
    TimeoutHandler timeout_handler;
};

TEST_F(MavlinkParameterClientTest, IgnoresParamValueFromOtherComponent)
{
    auto client = make_client(peripheral_component_id, false);

    bool called = false;
    Result got_result = Result::UnknownError;
    int32_t got_value = -1;
    client->get_param_int_async(
        "SHARED_INT",
        [&](Result result, int32_t value) {
            called = true;
            got_result = result;
            got_value = value;
        },
        this);
    drain_io();

    deliver(make_param_value_int(target_system_id, autopilot_component_id, "SHARED_INT", 10));
    EXPECT_FALSE(called);

    deliver(make_param_value_int(target_system_id, peripheral_component_id, "SHARED_INT", 51));
    ASSERT_TRUE(called);
    EXPECT_EQ(got_result, Result::Success);
    EXPECT_EQ(got_value, 51);
}

TEST_F(MavlinkParameterClientTest, IgnoresParamExtFromOtherComponent)
{
    auto client = make_client(peripheral_component_id, true);

    bool get_called = false;
    Result get_result = Result::UnknownError;
    std::string get_value;
    client->get_param_custom_async(
        "SHARED_STR",
        [&](Result result, const std::string& value) {
            get_called = true;
            get_result = result;
            get_value = value;
        },
        this);
    drain_io();

    deliver(make_param_ext_value_custom(
        target_system_id, autopilot_component_id, "SHARED_STR", "auto"));
    EXPECT_FALSE(get_called);

    deliver(make_param_ext_value_custom(
        target_system_id, peripheral_component_id, "SHARED_STR", "comp51"));
    ASSERT_TRUE(get_called);
    EXPECT_EQ(get_result, Result::Success);
    EXPECT_EQ(get_value, "comp51");

    bool set_called = false;
    Result set_result = Result::UnknownError;
    client->set_param_custom_async(
        "SHARED_STR",
        "updated",
        [&](Result result) {
            set_called = true;
            set_result = result;
        },
        this);
    drain_io();

    deliver(make_param_ext_ack(
        target_system_id, autopilot_component_id, "SHARED_STR", "updated", PARAM_ACK_FAILED));
    EXPECT_FALSE(set_called);

    deliver(make_param_ext_ack(
        target_system_id, peripheral_component_id, "SHARED_STR", "updated", PARAM_ACK_ACCEPTED));
    ASSERT_TRUE(set_called);
    EXPECT_EQ(set_result, Result::Success);
}

TEST_F(MavlinkParameterClientTest, AcceptsAllComponentsWhenTargetIsBroadcast)
{
    auto client = make_client(MAV_COMP_ID_ALL, false);

    bool called = false;
    Result got_result = Result::UnknownError;
    int32_t got_value = -1;
    client->get_param_int_async(
        "ANY_COMP",
        [&](Result result, int32_t value) {
            called = true;
            got_result = result;
            got_value = value;
        },
        this);
    drain_io();

    deliver(make_param_value_int(target_system_id, peripheral_component_id, "ANY_COMP", 99));
    ASSERT_TRUE(called);
    EXPECT_EQ(got_result, Result::Success);
    EXPECT_EQ(got_value, 99);
}
