#include "mavsdk.hpp"
#include "log.hpp"
#include "fs_helpers.hpp"
#include "plugins/action/action.hpp"
#include "plugins/action_server/action_server.hpp"
#include "plugins/camera/camera.hpp"
#include "plugins/camera_server/camera_server.hpp"
#include "plugins/ftp/ftp.hpp"
#include "plugins/ftp_server/ftp_server.hpp"
#include "plugins/mavlink_direct/mavlink_direct.hpp"
#include "plugins/mavlink_direct_server/mavlink_direct_server.hpp"
#include "plugins/mission/mission.hpp"
#include "plugins/mission_raw_server/mission_raw_server.hpp"
#include "plugins/param/param.hpp"
#include "plugins/param_server/param_server.hpp"
#include <algorithm>
#include <chrono>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace mavsdk;

// 32 bit system IDs, as merged in ArduPilot/pymavlink#1229. A system ID above
// 255 no longer fits the MAVLink 2 header's single sysid byte, so the sender
// sets MAVLINK_IFLAG_SYSID32 and the header grows by 3 bytes. A target above
// 255 likewise moves into a 4 byte extended header behind
// MAVLINK_IFLAG_TARGET32, and the payload's 8 bit target_system then holds
// the sentinel 255, which does not read as a broadcast.
//
// Most handlers used to check the decoded payload target against their own
// system ID, which only holds the sentinel for a wide target, so the tests
// below go through each kind of targeted exchange.

// 0x0A000001 is 10.0.0.1, which is the point of the feature: an IPv4 address
// used directly as a system ID.
static constexpr uint32_t autopilot_sysid = 0x0A000001;
static constexpr uint32_t groundstation_sysid = 0x0A000002;

static constexpr uint8_t target_system_sentinel = 255;

TEST(Sysid32, Discovery)
{
    Mavsdk mavsdk_groundstation{
        Mavsdk::Configuration{groundstation_sysid, MAV_COMP_ID_MISSIONPLANNER, false}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{autopilot_sysid, MAV_COMP_ID_AUTOPILOT1, true}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17010"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17010"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);

    // The full 32 bit value has to survive discovery. Truncating would report
    // system 1 here, which is a different (and very common) system.
    EXPECT_EQ(maybe_system.value()->get_system_id(), autopilot_sysid);
}

TEST(Sysid32, CommandRoundtrip)
{
    Mavsdk mavsdk_groundstation{
        Mavsdk::Configuration{groundstation_sysid, MAV_COMP_ID_MISSIONPLANNER, false}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{autopilot_sysid, MAV_COMP_ID_AUTOPILOT1, true}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17011"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17011"), ConnectionResult::Success);

    auto action_server = ActionServer{mavsdk_autopilot.server_component()};
    action_server.set_armable(true, true);
    action_server.set_disarmable(true, true);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto action = Action{maybe_system.value()};

    // A command is targeted, so this only works if the target system ID makes
    // it into the extended header and the ack finds its way back. Both sides
    // route on the 32 bit value.
    EXPECT_EQ(action.arm(), Action::Result::Success);
    EXPECT_EQ(action.disarm(), Action::Result::Success);
}

// An 8 bit ground station, e.g. QGroundControl, talking to an autopilot with a
// wide system ID: commands go out with a small target and come back with an
// extended one.
TEST(Sysid32, CommandRoundtripEightBitGroundStation)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{autopilot_sysid, MAV_COMP_ID_AUTOPILOT1, true}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17012"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17012"), ConnectionResult::Success);

    auto action_server = ActionServer{mavsdk_autopilot.server_component()};
    action_server.set_armable(true, true);
    action_server.set_disarmable(true, true);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    EXPECT_EQ(maybe_system.value()->get_system_id(), autopilot_sysid);
    auto action = Action{maybe_system.value()};

    EXPECT_EQ(action.arm(), Action::Result::Success);
    EXPECT_EQ(action.disarm(), Action::Result::Success);
}

// The other way round: a ground station with a wide system ID and an 8 bit
// autopilot, so the commands carry an extended target and the acks don't.
TEST(Sysid32, CommandRoundtripEightBitAutopilot)
{
    Mavsdk mavsdk_groundstation{
        Mavsdk::Configuration{groundstation_sysid, MAV_COMP_ID_MISSIONPLANNER, false}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17013"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17013"), ConnectionResult::Success);

    auto action_server = ActionServer{mavsdk_autopilot.server_component()};
    action_server.set_armable(true, true);
    action_server.set_disarmable(true, true);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto action = Action{maybe_system.value()};

    EXPECT_EQ(action.arm(), Action::Result::Success);
    EXPECT_EQ(action.disarm(), Action::Result::Success);
}

TEST(Sysid32, Params)
{
    Mavsdk mavsdk_groundstation{
        Mavsdk::Configuration{groundstation_sysid, MAV_COMP_ID_MISSIONPLANNER, false}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{autopilot_sysid, MAV_COMP_ID_AUTOPILOT1, true}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17014"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17014"), ConnectionResult::Success);

    auto param_server = ParamServer{mavsdk_autopilot.server_component()};
    ASSERT_EQ(param_server.provide_param_int("TEST_INT", 42), ParamServer::Result::Success);
    ASSERT_EQ(param_server.provide_param_float("TEST_FLOAT", 1.5f), ParamServer::Result::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto param = Param{maybe_system.value()};

    // The param server only answers requests addressed to its own system ID.
    auto int_result = param.get_param_int("TEST_INT");
    EXPECT_EQ(int_result.first, Param::Result::Success);
    EXPECT_EQ(int_result.second, 42);

    EXPECT_EQ(param.set_param_float("TEST_FLOAT", 2.5f), Param::Result::Success);
    auto server_float_result = param_server.retrieve_param_float("TEST_FLOAT");
    EXPECT_EQ(server_float_result.first, ParamServer::Result::Success);
    EXPECT_FLOAT_EQ(server_float_result.second, 2.5f);

    auto all_params = param.get_all_params();
    EXPECT_EQ(all_params.int_params.size(), 1);
    EXPECT_EQ(all_params.float_params.size(), 1);
}

TEST(Sysid32, MissionUploadDownload)
{
    Mavsdk mavsdk_groundstation{
        Mavsdk::Configuration{groundstation_sysid, MAV_COMP_ID_MISSIONPLANNER, false}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{autopilot_sysid, MAV_COMP_ID_AUTOPILOT1, true}};

    // Registered before connecting, so that MISSION_INT is in the capabilities
    // of the first AUTOPILOT_VERSION.
    auto mission_raw_server = MissionRawServer{mavsdk_autopilot.server_component()};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17015"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17015"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto mission = Mission{maybe_system.value()};

    Mission::MissionPlan mission_plan;
    for (unsigned i = 0; i < 5; ++i) {
        Mission::MissionItem item{};
        item.latitude_deg = 47.398170327054473 + (i * 1e-6);
        item.longitude_deg = 8.5456490218639658 + (i * 1e-6);
        item.relative_altitude_m = 10.0f + (i * 0.2f);
        item.speed_m_s = 5.0f;
        item.acceptance_radius_m = 2.0f;
        mission_plan.mission_items.push_back(item);
    }

    // Every message of the mission protocol is targeted, in both directions.
    ASSERT_EQ(mission.upload_mission(mission_plan), Mission::Result::Success);

    auto result = mission.download_mission();
    ASSERT_EQ(result.first, Mission::Result::Success);
    EXPECT_EQ(result.second, mission_plan);
}

TEST(Sysid32, Ftp)
{
    Mavsdk mavsdk_groundstation{
        Mavsdk::Configuration{groundstation_sysid, MAV_COMP_ID_MISSIONPLANNER, false}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{autopilot_sysid, MAV_COMP_ID_AUTOPILOT1, true}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17016"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17016"), ConnectionResult::Success);

    const auto root_dir = test_data_dir() / "sysid32";
    ASSERT_TRUE(reset_directories(root_dir));

    auto ftp_server = FtpServer{mavsdk_autopilot.server_component()};
    ftp_server.set_root_dir(root_dir.string());

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto ftp = Ftp{maybe_system.value()};

    // Both the FTP client and server drop messages that are not addressed to
    // their own system ID.
    ASSERT_EQ(ftp.create_directory("folder"), Ftp::Result::Success);
    EXPECT_TRUE(file_exists(root_dir / "folder"));

    auto list_result = ftp.list_directory(".");
    ASSERT_EQ(list_result.first, Ftp::Result::Success);
    const auto& entries = list_result.second.entries;
    EXPECT_TRUE(std::any_of(entries.begin(), entries.end(), [](const auto& entry) {
        return entry.name == "folder" &&
               entry.entry_type == Ftp::FilesystemEntry::EntryType::Directory;
    }));
}

TEST(Sysid32, CameraTakePhoto)
{
    Mavsdk mavsdk_groundstation{
        Mavsdk::Configuration{groundstation_sysid, MAV_COMP_ID_MISSIONPLANNER, false}};
    Mavsdk mavsdk_camera{Mavsdk::Configuration{autopilot_sysid, MAV_COMP_ID_CAMERA, true}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17017"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_camera.add_any_connection("udpout://127.0.0.1:17017"), ConnectionResult::Success);

    auto camera_server = std::make_shared<CameraServer>(mavsdk_camera.server_component());
    std::weak_ptr<CameraServer> camera_server_weak = camera_server;

    CameraServer::Information information{};
    information.vendor_name = "CoolCameras";
    information.model_name = "Frozen Super";
    information.firmware_version = "4.0.0";
    information.definition_file_version = 1;
    information.definition_file_uri = "";
    camera_server->set_information(information);

    // The camera server acks its commands separately from the command
    // handler, which is the path that has to carry the full origin.
    camera_server->subscribe_take_photo([camera_server_weak](int32_t index) {
        auto server = camera_server_weak.lock();
        if (!server) {
            return;
        }
        CameraServer::CaptureInfo info;
        info.index = index;
        info.is_success = true;
        server->respond_take_photo(CameraServer::CameraFeedback::Ok, info);
    });

    auto prom = std::make_shared<std::promise<std::shared_ptr<System>>>();
    auto fut = prom->get_future();
    auto flag = std::make_shared<std::once_flag>();

    auto handle =
        mavsdk_groundstation.subscribe_on_new_system([prom, flag, &mavsdk_groundstation]() {
            const auto system = mavsdk_groundstation.systems().back();
            if (system->is_connected() && system->has_camera()) {
                std::call_once(*flag, [&]() { prom->set_value(system); });
            }
        });

    ASSERT_EQ(fut.wait_for(std::chrono::seconds(10)), std::future_status::ready);
    mavsdk_groundstation.unsubscribe_on_new_system(handle);
    auto system = fut.get();
    EXPECT_EQ(system->get_system_id(), autopilot_sysid);

    auto camera = Camera{system};

    // The camera information is requested and has to come back first.
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    while (camera.camera_list().cameras.empty()) {
        ASSERT_LT(std::chrono::steady_clock::now(), deadline);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    EXPECT_EQ(
        camera.take_photo(camera.camera_list().cameras[0].component_id), Camera::Result::Success);
}

TEST(Sysid32, MavlinkDirectRoundtrip)
{
    Mavsdk mavsdk_groundstation{
        Mavsdk::Configuration{groundstation_sysid, MAV_COMP_ID_MISSIONPLANNER, false}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{autopilot_sysid, MAV_COMP_ID_AUTOPILOT1, true}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17018"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17018"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);

    auto receiver = MavlinkDirect{maybe_system.value()};
    auto sender = MavlinkDirectServer{mavsdk_autopilot.server_component()};

    auto prom = std::make_shared<std::promise<MavlinkDirect::MavlinkMessage>>();
    auto fut = prom->get_future();
    auto flag = std::make_shared<std::once_flag>();

    auto handle = receiver.subscribe_message(
        "GLOBAL_POSITION_INT", [prom, flag](MavlinkDirect::MavlinkMessage message) {
            std::call_once(*flag, [&]() { prom->set_value(message); });
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    MavlinkDirectServer::MavlinkMessage message;
    message.message_name = "GLOBAL_POSITION_INT";
    message.fields_json = R"({"time_boot_ms":12345,"lat":473977418,"lon":-1223974560,"alt":100500,)"
                          R"("relative_alt":50250,"vx":100,"vy":-50,"vz":25,"hdg":18000})";

    ASSERT_EQ(sender.send_message(message), MavlinkDirectServer::Result::Success);

    ASSERT_EQ(fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);
    auto received = fut.get();

    EXPECT_EQ(received.system_id, autopilot_sysid);
    EXPECT_EQ(nlohmann::json::parse(received.fields_json)["lat"], 473977418);

    receiver.unsubscribe_message(handle);
}

TEST(Sysid32, TargetAboveEightBits)
{
    Mavsdk mavsdk_groundstation{
        Mavsdk::Configuration{groundstation_sysid, MAV_COMP_ID_MISSIONPLANNER, false}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{autopilot_sysid, MAV_COMP_ID_AUTOPILOT1, true}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17019"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17019"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);

    auto receiver = MavlinkDirectServer{mavsdk_autopilot.server_component()};
    auto sender = MavlinkDirect{maybe_system.value()};

    auto prom = std::make_shared<std::promise<MavlinkDirectServer::MavlinkMessage>>();
    auto fut = prom->get_future();
    auto flag = std::make_shared<std::once_flag>();

    auto raw_prom = std::make_shared<std::promise<std::vector<uint8_t>>>();
    auto raw_fut = raw_prom->get_future();
    auto raw_flag = std::make_shared<std::once_flag>();

    auto raw_handle = mavsdk_autopilot.subscribe_incoming_messages_json(
        [raw_prom, raw_flag](Mavsdk::MavlinkMessage message) {
            if (message.message_name == "COMMAND_LONG") {
                std::call_once(*raw_flag, [&]() { raw_prom->set_value(message.raw_bytes); });
            }
            return true;
        });

    // An unknown command, so the command receiver leaves it alone.
    auto handle = receiver.subscribe_message(
        "COMMAND_LONG", [prom, flag](MavlinkDirectServer::MavlinkMessage message) {
            std::call_once(*flag, [&]() { prom->set_value(message); });
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // The payload's target_system field is only 8 bits wide, so this target
    // has to travel in the extended header instead. Were it truncated it would
    // arrive as 1, and were it zeroed it would look like a broadcast.
    MavlinkDirect::MavlinkMessage message;
    message.message_name = "COMMAND_LONG";
    message.target_system_id = autopilot_sysid;
    message.target_component_id = MAV_COMP_ID_AUTOPILOT1;
    message.fields_json =
        R"({"command":31000,"confirmation":0,"param1":1.0,"param2":0.0,"param3":0.0,)"
        R"("param4":0.0,"param5":0.0,"param6":0.0,"param7":0.0})";

    ASSERT_EQ(sender.send_message(message), MavlinkDirect::Result::Success);

    ASSERT_EQ(fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);
    auto received = fut.get();

    EXPECT_EQ(received.system_id, groundstation_sysid);
    EXPECT_EQ(received.target_system_id, autopilot_sysid);
    EXPECT_EQ(received.target_component_id, MAV_COMP_ID_AUTOPILOT1);

    // On the wire, the payload field holds the sentinel rather than a
    // truncated ID or a broadcast.
    const auto fields = nlohmann::json::parse(received.fields_json);
    EXPECT_EQ(fields["target_system"], target_system_sentinel);
    EXPECT_EQ(fields["target_component"], MAV_COMP_ID_AUTOPILOT1);

    // The header: SYSID32 | TARGET32, the 4 byte source system ID at offset 5,
    // and the 4 byte target system ID right after the 3 byte message ID. The
    // target component is not part of the header.
    ASSERT_EQ(raw_fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);
    const auto raw_bytes = raw_fut.get();
    ASSERT_GE(raw_bytes.size(), 17u);
    EXPECT_EQ(raw_bytes[0], 0xFD);
    EXPECT_EQ(raw_bytes[2], 0x02 | 0x04);
    const auto read_u32 = [&raw_bytes](size_t offset) {
        return static_cast<uint32_t>(raw_bytes[offset]) |
               (static_cast<uint32_t>(raw_bytes[offset + 1]) << 8) |
               (static_cast<uint32_t>(raw_bytes[offset + 2]) << 16) |
               (static_cast<uint32_t>(raw_bytes[offset + 3]) << 24);
    };
    EXPECT_EQ(read_u32(5), groundstation_sysid);
    EXPECT_EQ(raw_bytes[9], MAV_COMP_ID_MISSIONPLANNER);
    EXPECT_EQ(read_u32(13), autopilot_sysid);

    mavsdk_autopilot.unsubscribe_incoming_messages_json(raw_handle);
    receiver.unsubscribe_message(handle);
}

// A MAVSDK instance in the middle that has an 8 bit system ID itself has to
// route a wide target by the extended header, not by the sentinel.
TEST(Sysid32, Forwarding)
{
    Mavsdk mavsdk_groundstation{
        Mavsdk::Configuration{groundstation_sysid, MAV_COMP_ID_MISSIONPLANNER, false}};
    Mavsdk mavsdk_forwarder{Mavsdk::Configuration{ComponentType::CompanionComputer}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{autopilot_sysid, MAV_COMP_ID_AUTOPILOT1, true}};

    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17020"), ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_forwarder.add_any_connection(
            "udpin://0.0.0.0:17020", ForwardingOption::ForwardingOn),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_forwarder.add_any_connection(
            "udpout://127.0.0.1:17021", ForwardingOption::ForwardingOn),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17021"),
        ConnectionResult::Success);

    auto action_server = ActionServer{mavsdk_autopilot.server_component()};
    action_server.set_armable(true, true);
    action_server.set_disarmable(true, true);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    EXPECT_EQ(maybe_system.value()->get_system_id(), autopilot_sysid);
    auto action = Action{maybe_system.value()};

    EXPECT_EQ(action.arm(), Action::Result::Success);
    EXPECT_EQ(action.disarm(), Action::Result::Success);
}

TEST(Sysid32, EightBitPeerStillWorks)
{
    // A system ID that fits in 8 bits must not set any of the new incompat
    // flags, otherwise every peer that predates this feature drops our frames.
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{42, MAV_COMP_ID_AUTOPILOT1, true}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17022"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17022"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    EXPECT_EQ(maybe_system.value()->get_system_id(), 42);

    auto prom = std::make_shared<std::promise<std::vector<uint8_t>>>();
    auto fut = prom->get_future();
    auto flag = std::make_shared<std::once_flag>();

    auto handle = mavsdk_groundstation.subscribe_incoming_messages_json(
        [prom, flag](Mavsdk::MavlinkMessage message) {
            if (message.message_name == "HEARTBEAT" && message.system_id == 42) {
                std::call_once(*flag, [&]() { prom->set_value(message.raw_bytes); });
            }
            return true;
        });

    ASSERT_EQ(fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);
    const auto raw_bytes = fut.get();

    // A plain MAVLink 2 frame: no incompat flags, and the sysid in its single
    // byte.
    ASSERT_GE(raw_bytes.size(), 10u);
    EXPECT_EQ(raw_bytes[0], 0xFD);
    EXPECT_EQ(raw_bytes[2], 0);
    EXPECT_EQ(raw_bytes[5], 42);

    mavsdk_groundstation.unsubscribe_incoming_messages_json(handle);
}
