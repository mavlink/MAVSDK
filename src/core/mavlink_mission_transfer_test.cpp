#include <gtest/gtest.h>
#include "mavlink_mission_transfer.h"
#include "global_include.h"

using namespace mavsdk;

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

TEST(MAVLinkMissionTransfer, UploadMissionUnableToSend)
{
    FakeSender fake_sender;
    FakeReceiver fake_receiver;
    FakeTimeouter fake_timeouter;

    MAVLinkMissionTransfer mmt(fake_sender, fake_receiver, fake_timeouter);

    std::vector<MAVLinkMissionTransfer::ItemInt> items;
    items.push_back(make_item());
    items.push_back(make_item());

    fake_sender.able_to_send = false;

    mmt.upload_items_async(items, [](Result result) {
        EXPECT_EQ(result, Result::ConnectionError);
        ONCE_ONLY;
    });
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
