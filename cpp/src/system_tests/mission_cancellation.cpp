#include "log.hpp"
#include "mavsdk.hpp"
#include "plugins/mission/mission.hpp"
#include "plugins/mission_raw_server/mission_raw_server.hpp"
#include <chrono>
#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>
#include <string>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace mavsdk;

static Mission::MissionItem add_waypoint(
    double latitude_deg,
    double longitude_deg,
    float relative_altitude_m,
    float speed_m_s,
    bool is_fly_through,
    float gimbal_pitch_deg,
    float gimbal_yaw_deg,
    bool take_photo);

static Mission::MissionPlan create_mission_plan()
{
    Mission::MissionPlan mission_plan{};
    for (unsigned i = 0; i < 20; ++i) {
        mission_plan.mission_items.push_back(
            add_waypoint(47.3981703270545, 8.54564902186397, 20.0, 3.0, true, -90.0, 0.0, false));
    }
    return mission_plan;
}

// Transfer timeout on the groundstation. The client keeps a stalled transfer alive for
// several retries of this before giving up, which is how long cancel() has to land.
static constexpr double stall_timeout_s = 5.0;

// The item at which the transfer is held up mid-flight.
static constexpr int stall_at_seq = 10;

// Holds a transfer up mid-flight by dropping one message type from stall_at_seq on.
// Waiting for the stall before cancelling means the transfer can't have finished yet,
// so cancel() always hits a live transfer. Once released, nothing is dropped any more,
// so whatever arrives after the cancel still reaches the finished transfer.
class TransferStall {
public:
    explicit TransferStall(std::string message_name) : _message_name(std::move(message_name)) {}

    bool should_keep(const Mavsdk::MavlinkMessage& message)
    {
        if (message.message_name != _message_name) {
            return true;
        }
        const auto fields = nlohmann::json::parse(message.fields_json, nullptr, false);
        if (fields.is_discarded() || !fields.contains("seq") ||
            fields["seq"].get<int>() < stall_at_seq) {
            return true;
        }
        std::lock_guard<std::mutex> lock(_mutex);
        if (_released) {
            return true;
        }
        _stalled = true;
        _cv.notify_all();
        return false;
    }

    bool wait_until_stalled()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        return _cv.wait_for(lock, std::chrono::seconds(10), [this]() { return _stalled; });
    }

    void release()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _released = true;
    }

private:
    const std::string _message_name;
    std::mutex _mutex;
    std::condition_variable _cv;
    bool _stalled{false};
    bool _released{false};
};

TEST(Mission, UploadCancellation)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    mavsdk_groundstation.set_timeout_s(stall_timeout_s);
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    auto mission_raw_server = MissionRawServer{mavsdk_autopilot.server_component()};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();
    ASSERT_TRUE(system->has_autopilot());

    auto mission = Mission{system};

    // The callbacks can outlive the locals of this test if an ASSERT returns early,
    // so they share the state.
    auto stall = std::make_shared<TransferStall>("MISSION_REQUEST_INT");
    auto in_handle = mavsdk_groundstation.subscribe_incoming_messages_json(
        [stall](Mavsdk::MavlinkMessage message) { return stall->should_keep(message); });

    auto result_prom = std::make_shared<std::promise<Mission::Result>>();
    std::future<Mission::Result> result_fut = result_prom->get_future();

    LogInfo("Starting mission upload...");
    mission.upload_mission_async(create_mission_plan(), [result_prom](Mission::Result result) {
        result_prom->set_value(result);
    });

    ASSERT_TRUE(stall->wait_until_stalled());
    LogInfo("Upload is stalled at item {}, cancelling...", stall_at_seq);
    mission.cancel_mission_upload();
    stall->release();

    ASSERT_EQ(result_fut.wait_for(std::chrono::seconds(30)), std::future_status::ready);
    EXPECT_EQ(result_fut.get(), Mission::Result::TransferCancelled);
    LogInfo("Mission upload cancelled successfully.");

    mavsdk_groundstation.unsubscribe_incoming_messages_json(in_handle);
}

TEST(Mission, DownloadCancellation)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    mavsdk_groundstation.set_timeout_s(stall_timeout_s);
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    auto mission_raw_server = MissionRawServer{mavsdk_autopilot.server_component()};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();
    ASSERT_TRUE(system->has_autopilot());

    auto mission = Mission{system};

    // Upload without interference first, so there is something to download.
    LogInfo("Uploading mission first...");
    ASSERT_EQ(mission.upload_mission(create_mission_plan()), Mission::Result::Success);
    LogInfo("Mission uploaded successfully.");

    auto stall = std::make_shared<TransferStall>("MISSION_ITEM_INT");
    auto in_handle = mavsdk_groundstation.subscribe_incoming_messages_json(
        [stall](Mavsdk::MavlinkMessage message) { return stall->should_keep(message); });

    auto result_prom = std::make_shared<std::promise<Mission::Result>>();
    std::future<Mission::Result> result_fut = result_prom->get_future();

    LogInfo("Starting mission download...");
    mission.download_mission_async(
        [result_prom](Mission::Result result, const Mission::MissionPlan& /*mission_plan*/) {
            result_prom->set_value(result);
        });

    ASSERT_TRUE(stall->wait_until_stalled());
    LogInfo("Download is stalled at item {}, cancelling...", stall_at_seq);
    mission.cancel_mission_download();
    stall->release();

    ASSERT_EQ(result_fut.wait_for(std::chrono::seconds(30)), std::future_status::ready);
    EXPECT_EQ(result_fut.get(), Mission::Result::TransferCancelled);
    LogInfo("Mission download cancelled successfully.");

    mavsdk_groundstation.unsubscribe_incoming_messages_json(in_handle);
}

Mission::MissionItem add_waypoint(
    double latitude_deg,
    double longitude_deg,
    float relative_altitude_m,
    float speed_m_s,
    bool is_fly_through,
    float gimbal_pitch_deg,
    float gimbal_yaw_deg,
    bool take_photo)
{
    Mission::MissionItem new_item{};
    new_item.latitude_deg = latitude_deg;
    new_item.longitude_deg = longitude_deg;
    new_item.relative_altitude_m = relative_altitude_m;
    new_item.speed_m_s = speed_m_s;
    new_item.is_fly_through = is_fly_through;

    new_item.gimbal_pitch_deg = gimbal_pitch_deg;
    new_item.gimbal_yaw_deg = gimbal_yaw_deg;
    if (take_photo) {
        new_item.camera_action = Mission::MissionItem::CameraAction::TakePhoto;
    }
    return new_item;
}
