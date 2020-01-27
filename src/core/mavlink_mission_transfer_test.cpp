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
using MockSender = NiceMock<mavsdk::testing::MockSender>;

using Result = MAVLinkMissionTransfer::Result;

#define ONCE_ONLY \
    static bool called = false; \
    EXPECT_FALSE(called); \
    called = true;

MAVLinkMissionTransfer::ItemInt make_item()
{
    MAVLinkMissionTransfer::ItemInt item;
    return item;
}

TEST(MAVLinkMissionTransfer, UploadMissionDoesComplainAboutNoItems)
{
    MockSender mock_sender;
    FakeReceiver fake_receiver;
    FakeTimeouter fake_timeouter;

    MAVLinkMissionTransfer mmt(mock_sender, fake_receiver, fake_timeouter);

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

TEST(MAVLinkMissionTransfer, UploadMissionDoesNotCrashIfCallbackIsNull)
{
    MockSender mock_sender;
    FakeReceiver fake_receiver;
    FakeTimeouter fake_timeouter;

    MAVLinkMissionTransfer mmt(mock_sender, fake_receiver, fake_timeouter);

    ON_CALL(mock_sender, send_message(_))
        .WillByDefault(Return(false));

    std::vector<MAVLinkMissionTransfer::ItemInt> items;
    mmt.upload_items_async(items, nullptr);

    items.push_back(make_item());
    items.push_back(make_item());

    mmt.upload_items_async(items, nullptr);
}

TEST(MAVLinkMissionTransfer, UploadMissionReturnsConnectionErrorWhenSendMessageFails)
{
    MockSender mock_sender;
    FakeReceiver fake_receiver;
    FakeTimeouter fake_timeouter;

    MAVLinkMissionTransfer mmt(mock_sender, fake_receiver, fake_timeouter);

    ON_CALL(mock_sender, send_message(_))
        .WillByDefault(Return(false));

    std::vector<MAVLinkMissionTransfer::ItemInt> items;
    items.push_back(make_item());
    items.push_back(make_item());

    std::promise<void> prom;
    auto fut = prom.get_future();

    mmt.upload_items_async(items, [&prom](Result result) {
        EXPECT_EQ(result, Result::ConnectionError);
        ONCE_ONLY;
        prom.set_value();
    });

    EXPECT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);
}

TEST(MAVLinkMissionTransfer, UploadMissionSendsCount)
{
    FakeSender fake_sender;
    FakeReceiver fake_receiver;
    FakeTimeouter fake_timeouter;

    MAVLinkMissionTransfer mmt(fake_sender, fake_receiver, fake_timeouter);

    std::vector<MAVLinkMissionTransfer::ItemInt> items;
    items.push_back(make_item());
    items.push_back(make_item());
    mmt.upload_items_async(items, [](Result result) {
        UNUSED(result);
        EXPECT_TRUE(false);
    });
    EXPECT_EQ(fake_sender.last_message().msgid, MAVLINK_MSG_ID_MISSION_COUNT);
}


#if 0
TEST(MAVLinkMissionTransfer, UploadMissionTimeoutAfterSendCount)
{
    FakeSender fake_sender;
    FakeReceiver fake_receiver;
    FakeTimeouter fake_timeouter;

    MAVLinkMissionTransfer mmt(fake_sender, fake_receiver, fake_timeouter);

    std::vector<MAVLinkMissionTransfer::ItemInt> items;
    items.push_back(make_item());
    items.push_back(make_item());
    mmt.upload_items_async(items, [](Result result) {
        EXPECT_EQ(result, Result::Timeout);
        ONCE_ONLY;
    });

    fake_timeouter.set_expired();
}

#endif
