#include "log.hpp"
#include "mavsdk.hpp"
#include "plugins/modes/modes.hpp"
#include "plugins/mavlink_direct_server/mavlink_direct_server.hpp"
#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace mavsdk;
using namespace std::chrono_literals;

namespace {

// Pretends to be an autopilot implementing the MAVLink standard modes protocol: it answers
// AVAILABLE_MODES requests, acks mode commands and reports the mode in CURRENT_MODE.
class FakeModesAutopilot {
public:
    struct Entry {
        uint8_t standard_mode;
        uint32_t custom_mode;
        uint32_t properties;
        std::string name;
    };

    explicit FakeModesAutopilot(std::shared_ptr<ServerComponent> server_component) :
        _direct(server_component)
    {
        for (const auto command :
             {MAV_CMD_REQUEST_MESSAGE, MAV_CMD_DO_SET_MODE, MAV_CMD_DO_SET_STANDARD_MODE}) {
            _handles.push_back(_direct.subscribe_command(
                command,
                [this](MavlinkDirectServer::MavlinkMessage message) { handle_command(message); }));
        }
    }

    ~FakeModesAutopilot()
    {
        for (auto& handle : _handles) {
            _direct.unsubscribe_command(handle);
        }
    }

    void set_modes(std::vector<Entry> modes)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _modes = std::move(modes);
        ++_seq;
    }

    void set_supports_available_modes(bool supported)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _supports_available_modes = supported;
    }

    void set_supports_set_standard_mode(bool supported)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _supports_set_standard_mode = supported;
    }

    void send_monitor()
    {
        uint8_t seq;
        {
            std::lock_guard<std::mutex> lock(_mutex);
            seq = _seq;
        }
        send("AVAILABLE_MODES_MONITOR", nlohmann::json{{"seq", seq}});
    }

    void send_current_mode(uint32_t custom_mode, uint8_t standard_mode)
    {
        send(
            "CURRENT_MODE",
            nlohmann::json{
                {"standard_mode", standard_mode},
                {"custom_mode", custom_mode},
                {"intended_custom_mode", custom_mode}});
    }

    // Waits for a command with the given id and returns its params.
    std::optional<nlohmann::json> wait_for_command(int command, std::chrono::milliseconds timeout)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        std::optional<nlohmann::json> result;
        _cv.wait_for(lock, timeout, [&]() {
            const auto it =
                std::find_if(_commands.begin(), _commands.end(), [&](const nlohmann::json& json) {
                    return json["command"].get<int>() == command;
                });
            if (it == _commands.end()) {
                return false;
            }
            result = *it;
            _commands.erase(it);
            return true;
        });
        return result;
    }

    unsigned num_available_modes_requests()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _num_available_modes_requests;
    }

private:
    void handle_command(const MavlinkDirectServer::MavlinkMessage& message)
    {
        auto json = nlohmann::json::parse(message.fields_json, nullptr, false);
        if (json.is_discarded()) {
            return;
        }

        const int command = json["command"].get<int>();
        // NaN params arrive as null.
        const auto param = [&](const char* name) {
            return json[name].is_number() ? json[name].get<float>() : 0.0f;
        };
        const auto param1 = param("param1");
        const auto param2 = param("param2");

        std::unique_lock<std::mutex> lock(_mutex);

        if (command == MAV_CMD_REQUEST_MESSAGE &&
            static_cast<int>(param1) == MAVLINK_MSG_ID_AVAILABLE_MODES) {
            ++_num_available_modes_requests;
            if (!_supports_available_modes) {
                lock.unlock();
                send_ack(message, command, MAV_RESULT_UNSUPPORTED);
                return;
            }
            const auto modes = _modes;
            const auto seq = _seq;
            lock.unlock();

            send_ack(message, command, MAV_RESULT_ACCEPTED);
            const auto index = static_cast<unsigned>(param2);
            for (unsigned i = 1; i <= modes.size(); ++i) {
                if (index == 0 || index == i) {
                    send_available_mode(modes, i, seq);
                }
            }
            return;
        }

        if (command == MAV_CMD_REQUEST_MESSAGE) {
            lock.unlock();
            send_ack(message, command, MAV_RESULT_UNSUPPORTED);
            return;
        }

        if (command == MAV_CMD_DO_SET_STANDARD_MODE && !_supports_set_standard_mode) {
            lock.unlock();
            send_ack(message, command, MAV_RESULT_UNSUPPORTED);
            return;
        }

        if (command == MAV_CMD_DO_SET_MODE || command == MAV_CMD_DO_SET_STANDARD_MODE) {
            _commands.push_back(json);
            _cv.notify_all();
            lock.unlock();
            send_ack(message, command, MAV_RESULT_ACCEPTED);
            return;
        }
    }

    void send_available_mode(const std::vector<Entry>& modes, unsigned index, uint8_t seq)
    {
        const auto& entry = modes.at(index - 1);
        send(
            "AVAILABLE_MODES",
            nlohmann::json{
                {"number_modes", modes.size()},
                {"mode_index", index},
                {"standard_mode", entry.standard_mode},
                {"custom_mode", entry.custom_mode},
                {"properties", entry.properties},
                {"mode_name", entry.name},
                {"seq", seq}});
    }

    void send_ack(const MavlinkDirectServer::MavlinkMessage& command, int command_id, int result)
    {
        send(
            "COMMAND_ACK",
            nlohmann::json{
                {"command", command_id},
                {"result", result},
                {"progress", 0},
                {"result_param2", 0},
                {"target_system", command.system_id},
                {"target_component", command.component_id}},
            command.system_id,
            command.component_id);
    }

    void send(
        const std::string& name,
        const nlohmann::json& fields,
        uint32_t target_system_id = 0,
        uint32_t target_component_id = 0)
    {
        MavlinkDirectServer::MavlinkMessage message;
        message.message_name = name;
        message.target_system_id = target_system_id;
        message.target_component_id = target_component_id;
        message.fields_json = fields.dump();
        _direct.send_message(message);
    }

    MavlinkDirectServer _direct;
    std::vector<MavlinkDirectServer::CommandHandle> _handles{};

    std::mutex _mutex{};
    std::condition_variable _cv{};
    std::vector<Entry> _modes{};
    uint8_t _seq{0};
    bool _supports_available_modes{true};
    bool _supports_set_standard_mode{true};
    unsigned _num_available_modes_requests{0};
    std::vector<nlohmann::json> _commands{};
};

// A PX4-like list: standard modes come without a name, and there are two non-standard
// modes named "Hold".
const std::vector<FakeModesAutopilot::Entry> px4_like_modes{
    {MAV_STANDARD_MODE_NON_STANDARD, 0x00010000, MAV_MODE_PROPERTY_ADVANCED, "Manual"},
    {MAV_STANDARD_MODE_POSITION_HOLD, 0x00030000, 0, ""},
    {MAV_STANDARD_MODE_NON_STANDARD, 0x03040000, MAV_MODE_PROPERTY_ADVANCED, "Hold"},
    {MAV_STANDARD_MODE_SAFE_RECOVERY, 0x05040000, MAV_MODE_PROPERTY_ADVANCED, ""},
    {MAV_STANDARD_MODE_NON_STANDARD,
     0x00060000,
     MAV_MODE_PROPERTY_ADVANCED | MAV_MODE_PROPERTY_NOT_USER_SELECTABLE,
     "Offboard"},
    {MAV_STANDARD_MODE_NON_STANDARD, 0x09040000, MAV_MODE_PROPERTY_AUTO_MODE, "Hold"},
};

std::optional<Modes::Mode> find_mode(const std::vector<Modes::Mode>& modes, const std::string& name)
{
    const auto it = std::find_if(
        modes.begin(), modes.end(), [&](const Modes::Mode& mode) { return mode.name == name; });
    if (it == modes.end()) {
        return std::nullopt;
    }
    return *it;
}

template<typename Predicate>
bool wait_until(Predicate predicate, std::chrono::milliseconds timeout)
{
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    while (std::chrono::steady_clock::now() < deadline) {
        if (predicate()) {
            return true;
        }
        std::this_thread::sleep_for(10ms);
    }
    return predicate();
}

} // namespace

TEST(SystemTest, ModesAvailableModes)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    FakeModesAutopilot autopilot{mavsdk_autopilot.server_component()};
    autopilot.set_modes(px4_like_modes);

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17080"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17080"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto modes = Modes{maybe_system.value()};

    ASSERT_TRUE(wait_until([&]() { return modes.available_modes().size() == 6; }, 5s));
    const auto available_modes = modes.available_modes();

    const auto manual = find_mode(available_modes, "Manual");
    ASSERT_TRUE(manual);
    EXPECT_EQ(manual->standard_mode, Modes::StandardMode::NonStandard);
    EXPECT_EQ(manual->custom_mode, 0x00010000u);
    EXPECT_TRUE(manual->is_user_selectable);
    EXPECT_TRUE(manual->is_advanced);
    EXPECT_FALSE(manual->is_auto_mode);

    // Standard modes get their standard name.
    const auto position = find_mode(available_modes, "Position");
    ASSERT_TRUE(position);
    EXPECT_EQ(position->standard_mode, Modes::StandardMode::PositionHold);
    EXPECT_EQ(position->custom_mode, 0x00030000u);
    EXPECT_FALSE(position->is_advanced);

    const auto safe_recovery = find_mode(available_modes, "Safe Recovery");
    ASSERT_TRUE(safe_recovery);
    EXPECT_EQ(safe_recovery->standard_mode, Modes::StandardMode::SafeRecovery);

    const auto offboard = find_mode(available_modes, "Offboard");
    ASSERT_TRUE(offboard);
    EXPECT_FALSE(offboard->is_user_selectable);

    // Duplicate names are made unique.
    const auto hold = find_mode(available_modes, "Hold");
    ASSERT_TRUE(hold);
    EXPECT_EQ(hold->custom_mode, 0x03040000u);
    const auto hold_1 = find_mode(available_modes, "Hold (1)");
    ASSERT_TRUE(hold_1);
    EXPECT_EQ(hold_1->custom_mode, 0x09040000u);
    EXPECT_TRUE(hold_1->is_auto_mode);
}

TEST(SystemTest, ModesSetModeAndCurrentMode)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    FakeModesAutopilot autopilot{mavsdk_autopilot.server_component()};
    autopilot.set_modes(px4_like_modes);

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17081"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17081"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto modes = Modes{maybe_system.value()};

    ASSERT_TRUE(wait_until([&]() { return modes.available_modes().size() == 6; }, 5s));

    // Names are matched exactly.
    EXPECT_EQ(modes.set_mode("hold"), Modes::Result::ModeNotFound);
    EXPECT_EQ(modes.set_mode("Does not exist"), Modes::Result::ModeNotFound);

    EXPECT_EQ(modes.set_mode("Hold (1)"), Modes::Result::Success);
    auto command = autopilot.wait_for_command(MAV_CMD_DO_SET_MODE, 1s);
    ASSERT_TRUE(command);
    EXPECT_EQ(
        static_cast<int>((*command)["param1"].get<float>()) & MAV_MODE_FLAG_CUSTOM_MODE_ENABLED,
        MAV_MODE_FLAG_CUSTOM_MODE_ENABLED);
    EXPECT_EQ(static_cast<uint32_t>((*command)["param2"].get<float>()), 0x09040000u);

    std::mutex mutex;
    std::vector<Modes::Mode> current_modes;
    auto handle = modes.subscribe_current_mode([&](Modes::Mode mode) {
        std::lock_guard<std::mutex> lock(mutex);
        current_modes.push_back(mode);
    });

    autopilot.send_current_mode(0x09040000, MAV_STANDARD_MODE_NON_STANDARD);
    ASSERT_TRUE(wait_until([&]() { return modes.current_mode().name == "Hold (1)"; }, 2s));

    autopilot.send_current_mode(0x05040000, MAV_STANDARD_MODE_SAFE_RECOVERY);
    ASSERT_TRUE(wait_until([&]() { return modes.current_mode().name == "Safe Recovery"; }, 2s));
    EXPECT_EQ(modes.current_mode().standard_mode, Modes::StandardMode::SafeRecovery);

    // Sending the same mode again is not a change.
    autopilot.send_current_mode(0x05040000, MAV_STANDARD_MODE_SAFE_RECOVERY);
    std::this_thread::sleep_for(200ms);

    modes.unsubscribe_current_mode(handle);
    {
        std::lock_guard<std::mutex> lock(mutex);
        ASSERT_GE(current_modes.size(), 2u);
        EXPECT_EQ(current_modes.back().name, "Safe Recovery");
        EXPECT_EQ(current_modes.at(current_modes.size() - 2).name, "Hold (1)");
    }
}

TEST(SystemTest, ModesSetStandardMode)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    FakeModesAutopilot autopilot{mavsdk_autopilot.server_component()};
    autopilot.set_modes(px4_like_modes);

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17082"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17082"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto modes = Modes{maybe_system.value()};

    ASSERT_TRUE(wait_until([&]() { return modes.available_modes().size() == 6; }, 5s));

    EXPECT_EQ(modes.set_standard_mode(Modes::StandardMode::NonStandard), Modes::Result::Unsupported);

    EXPECT_EQ(modes.set_standard_mode(Modes::StandardMode::SafeRecovery), Modes::Result::Success);
    auto command = autopilot.wait_for_command(MAV_CMD_DO_SET_STANDARD_MODE, 1s);
    ASSERT_TRUE(command);
    EXPECT_EQ(
        static_cast<int>((*command)["param1"].get<float>()), MAV_STANDARD_MODE_SAFE_RECOVERY);

    // Without MAV_CMD_DO_SET_STANDARD_MODE, a known standard mode is set as custom mode.
    autopilot.set_supports_set_standard_mode(false);
    EXPECT_EQ(modes.set_standard_mode(Modes::StandardMode::PositionHold), Modes::Result::Success);
    command = autopilot.wait_for_command(MAV_CMD_DO_SET_MODE, 1s);
    ASSERT_TRUE(command);
    EXPECT_EQ(static_cast<uint32_t>((*command)["param2"].get<float>()), 0x00030000u);

    // A standard mode the autopilot doesn't have.
    EXPECT_EQ(modes.set_standard_mode(Modes::StandardMode::Orbit), Modes::Result::Unsupported);
}

TEST(SystemTest, ModesMonitorTriggersUpdate)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    FakeModesAutopilot autopilot{mavsdk_autopilot.server_component()};
    autopilot.set_modes(px4_like_modes);

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17083"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17083"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto modes = Modes{maybe_system.value()};

    std::mutex mutex;
    std::vector<std::vector<Modes::Mode>> updates;
    auto handle = modes.subscribe_available_modes([&](std::vector<Modes::Mode> available_modes) {
        std::lock_guard<std::mutex> lock(mutex);
        updates.push_back(available_modes);
    });

    ASSERT_TRUE(wait_until([&]() { return modes.available_modes().size() == 6; }, 5s));

    // The same seq again doesn't trigger a download.
    const auto requests_before = autopilot.num_available_modes_requests();
    autopilot.send_monitor();
    std::this_thread::sleep_for(300ms);
    EXPECT_EQ(autopilot.num_available_modes_requests(), requests_before);

    // An external mode gets registered.
    auto new_modes = px4_like_modes;
    new_modes.push_back({MAV_STANDARD_MODE_NON_STANDARD, 0x000c0000, 0, "My Mode"});
    autopilot.set_modes(new_modes);
    autopilot.send_monitor();

    ASSERT_TRUE(wait_until([&]() { return modes.available_modes().size() == 7; }, 5s));
    EXPECT_TRUE(find_mode(modes.available_modes(), "My Mode"));
    EXPECT_EQ(modes.set_mode("My Mode"), Modes::Result::Success);

    modes.unsubscribe_available_modes(handle);
    {
        std::lock_guard<std::mutex> lock(mutex);
        ASSERT_GE(updates.size(), 2u);
        EXPECT_EQ(updates.back().size(), 7u);
    }
}

TEST(SystemTest, ModesFallback)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk::Configuration autopilot_configuration{ComponentType::Autopilot};
    autopilot_configuration.set_autopilot(Autopilot::Px4);
    Mavsdk mavsdk_autopilot{autopilot_configuration};

    FakeModesAutopilot autopilot{mavsdk_autopilot.server_component()};
    autopilot.set_supports_available_modes(false);
    autopilot.set_supports_set_standard_mode(false);

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17084"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17084"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto modes = Modes{maybe_system.value()};

    // The request is retried before giving up.
    ASSERT_TRUE(wait_until([&]() { return !modes.available_modes().empty(); }, 10s));
    EXPECT_GE(autopilot.num_available_modes_requests(), 1u);

    const auto available_modes = modes.available_modes();
    ASSERT_TRUE(find_mode(available_modes, "Hold"));
    ASSERT_TRUE(find_mode(available_modes, "Safe Recovery"));

    // PX4 takes the main and sub mode in separate params.
    EXPECT_EQ(modes.set_mode("Hold"), Modes::Result::Success);
    auto command = autopilot.wait_for_command(MAV_CMD_DO_SET_MODE, 1s);
    ASSERT_TRUE(command);
    EXPECT_EQ(static_cast<int>((*command)["param2"].get<float>()), 4); // AUTO
    EXPECT_EQ(static_cast<int>((*command)["param3"].get<float>()), 3); // LOITER

    // Standard modes are set as custom modes, without trying MAV_CMD_DO_SET_STANDARD_MODE.
    EXPECT_EQ(modes.set_standard_mode(Modes::StandardMode::SafeRecovery), Modes::Result::Success);
    command = autopilot.wait_for_command(MAV_CMD_DO_SET_MODE, 1s);
    ASSERT_TRUE(command);
    EXPECT_EQ(static_cast<int>((*command)["param2"].get<float>()), 4); // AUTO
    EXPECT_EQ(static_cast<int>((*command)["param3"].get<float>()), 5); // RTL
}
