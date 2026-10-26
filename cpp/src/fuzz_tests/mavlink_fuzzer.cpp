#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <string>
#include <vector>

#include <mavsdk/mavsdk.hpp>
#include "mavlink_include.hpp"

// Build the list of valid dialect message IDs once, so the fuzzer only emits
// messages that map to a real message entry. The previous code derived the id
// from a single byte (fuzz_data[offset + 2]), limiting it to ids 0..255 and
// missing every higher id in the dialect (e.g. the 300/11000-range messages).
static const std::vector<uint32_t>& valid_msg_ids()
{
    static const std::vector<uint32_t> ids = [] {
        std::vector<uint32_t> v;
        for (uint32_t id = 0; id <= 300000; ++id) {
            if (mavlink_get_msg_entry(id) != nullptr) {
                v.push_back(id);
            }
        }
        return v;
    }();
    return ids;
}
#include "../mavsdk/core/log.hpp"

// Include all client plugins (not server plugins)
#include <plugins/action/action.hpp>
#include <plugins/calibration/calibration.hpp>
#include <plugins/camera/camera.hpp>
#include <plugins/component_metadata/component_metadata.hpp>
#include <plugins/events/events.hpp>
#include <plugins/failure/failure.hpp>
#include <plugins/follow_me/follow_me.hpp>
#include <plugins/ftp/ftp.hpp>
#include <plugins/geofence/geofence.hpp>
#include <plugins/gimbal/gimbal.hpp>
#include <plugins/gripper/gripper.hpp>
#include <plugins/info/info.hpp>
#include <plugins/log_files/log_files.hpp>
#include <plugins/log_streaming/log_streaming.hpp>
#include <plugins/manual_control/manual_control.hpp>
#include <plugins/mavlink_direct/mavlink_direct.hpp>
#include <plugins/mission/mission.hpp>
#include <plugins/mission_raw/mission_raw.hpp>
#include <plugins/mocap/mocap.hpp>
#include <plugins/offboard/offboard.hpp>
#include <plugins/param/param.hpp>
#include <plugins/rtk/rtk.hpp>
#include <plugins/shell/shell.hpp>
#include <plugins/telemetry/telemetry.hpp>
#include <plugins/transponder/transponder.hpp>
#include <plugins/tune/tune.hpp>
#include <plugins/winch/winch.hpp>
#include <plugins/mavlink_passthrough/mavlink_passthrough.hpp>

using namespace mavsdk;

// Watchdog to detect stalls/deadlocks - aborts if not reset within timeout
class Watchdog {
public:
    explicit Watchdog(std::chrono::milliseconds timeout) : timeout_(timeout), running_(true)
    {
        thread_ = std::thread([this]() { run(); });
    }

    ~Watchdog()
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            running_ = false;
        }
        cv_.notify_all();
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    // Call this before each operation to reset the watchdog
    void reset()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        last_reset_ = std::chrono::steady_clock::now();
    }

    // Non-copyable
    Watchdog(const Watchdog&) = delete;
    Watchdog& operator=(const Watchdog&) = delete;

private:
    void run()
    {
        while (true) {
            std::unique_lock<std::mutex> lock(mutex_);

            // Wait for timeout or stop signal
            cv_.wait_for(lock, timeout_ / 2);

            if (!running_) {
                return;
            }

            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_reset_);

            if (elapsed > timeout_) {
                // Stall detected - abort to generate core dump
                LogErr(
                    "WATCHDOG: Stall detected! No activity for {}ms. Aborting to generate core dump.",
                    elapsed.count());
                std::abort();
            }
        }
    }

    std::chrono::milliseconds timeout_;
    std::chrono::steady_clock::time_point last_reset_{std::chrono::steady_clock::now()};
    std::atomic<bool> running_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::thread thread_;
};

// Plugin instances per system to maximize message processing coverage
struct SystemPlugins {
    std::shared_ptr<System> system;
    std::unique_ptr<Action> action;
    std::unique_ptr<Calibration> calibration;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<ComponentMetadata> component_metadata;
    std::unique_ptr<Events> events;
    std::unique_ptr<Failure> failure;
    std::unique_ptr<FollowMe> follow_me;
    std::unique_ptr<Ftp> ftp;
    std::unique_ptr<Geofence> geofence;
    std::unique_ptr<Gimbal> gimbal;
    std::unique_ptr<Gripper> gripper;
    std::unique_ptr<Info> info;
    std::unique_ptr<LogFiles> log_files;
    std::unique_ptr<LogStreaming> log_streaming;
    std::unique_ptr<ManualControl> manual_control;
    std::unique_ptr<MavlinkDirect> mavlink_direct;
    std::unique_ptr<Mission> mission;
    std::unique_ptr<MissionRaw> mission_raw;
    std::unique_ptr<Mocap> mocap;
    std::unique_ptr<Offboard> offboard;
    std::unique_ptr<Param> param;
    std::unique_ptr<Rtk> rtk;
    std::unique_ptr<Shell> shell;
    std::unique_ptr<Telemetry> telemetry;
    std::unique_ptr<Transponder> transponder;
    std::unique_ptr<Tune> tune;
    std::unique_ptr<Winch> winch;
    std::unique_ptr<MavlinkPassthrough> mavlink_passthrough;
};

// System ID used only by the sentinel heartbeat (see wait_until_processed()). The fuzzed
// messages and the keep-alive heartbeat always come from system ID 1.
static constexpr uint8_t SENTINEL_SYSTEM_ID = 254;

// Main fuzzer class that manages MAVSDK connection and plugin instances
class MavsdkFuzzer {
public:
    MavsdkFuzzer() = default;
    ~MavsdkFuzzer() = default;

    // Non-copyable, non-movable
    MavsdkFuzzer(const MavsdkFuzzer&) = delete;
    MavsdkFuzzer& operator=(const MavsdkFuzzer&) = delete;
    MavsdkFuzzer(MavsdkFuzzer&&) = delete;
    MavsdkFuzzer& operator=(MavsdkFuzzer&&) = delete;

    bool initialize();
    void process_fuzz_input(const uint8_t* data, size_t size);

private:
    void send_heartbeat();
    void send_sentinel(uint32_t sentinel);
    bool subscribe_to_sentinel();
    void wait_until_processed();
    void create_plugins_for_system(std::shared_ptr<System> system);
    size_t process_fuzz_data_to_mavlink(
        const uint8_t* fuzz_data,
        size_t fuzz_size,
        uint8_t* output_buffer,
        size_t output_buffer_size);

    // Declared before mavsdk_ so they outlive it: the sentinel callback that updates them
    // can run until mavsdk_ is destroyed.
    std::mutex sentinel_mutex_;
    std::condition_variable sentinel_cv_;
    uint32_t sentinel_sent_{0};
    uint32_t sentinel_seen_{0};

    std::unique_ptr<Mavsdk> mavsdk_;
    std::unique_ptr<Watchdog> watchdog_;
    std::vector<std::unique_ptr<SystemPlugins>> system_plugins_;
    std::unique_ptr<MavlinkDirect> sentinel_mavlink_direct_;
    std::chrono::steady_clock::time_point last_heartbeat_{std::chrono::steady_clock::now()};
    bool initialized_{false};
};

void MavsdkFuzzer::send_heartbeat()
{
    mavlink_message_t heartbeat_msg;
    mavlink_msg_heartbeat_pack(
        1,
        1,
        &heartbeat_msg, // system_id=1, component_id=1
        MAV_TYPE_QUADROTOR, // type
        MAV_AUTOPILOT_PX4, // autopilot
        0, // base_mode
        0, // custom_mode
        MAV_STATE_STANDBY // system_status
    );

    uint8_t heartbeat_buffer[MAVLINK_MAX_PACKET_LEN];
    size_t heartbeat_size = mavlink_msg_to_send_buffer(heartbeat_buffer, &heartbeat_msg);

    mavsdk_->pass_received_raw_bytes(
        reinterpret_cast<const char*>(heartbeat_buffer), heartbeat_size);
}

void MavsdkFuzzer::send_sentinel(uint32_t sentinel)
{
    mavlink_message_t sentinel_msg;
    mavlink_msg_heartbeat_pack(
        SENTINEL_SYSTEM_ID,
        1,
        &sentinel_msg,
        MAV_TYPE_GCS,
        MAV_AUTOPILOT_INVALID,
        0,
        sentinel, // custom_mode carries the sequence number
        MAV_STATE_UNINIT);

    uint8_t sentinel_buffer[MAVLINK_MAX_PACKET_LEN];
    size_t sentinel_size = mavlink_msg_to_send_buffer(sentinel_buffer, &sentinel_msg);

    mavsdk_->pass_received_raw_bytes(reinterpret_cast<const char*>(sentinel_buffer), sentinel_size);
}

bool MavsdkFuzzer::subscribe_to_sentinel()
{
    // The first sentinel creates the sentinel system, then we listen to its heartbeats through
    // MavlinkDirect. Only the sentinel system sends from SENTINEL_SYSTEM_ID, so the fuzzed
    // messages (system ID 1) never reach this subscription.
    send_sentinel(0);

    std::shared_ptr<System> sentinel_system;
    for (int i = 0; i < 100 && !sentinel_system; ++i) {
        for (auto& system : mavsdk_->systems()) {
            if (system->get_system_id() == SENTINEL_SYSTEM_ID) {
                sentinel_system = system;
                break;
            }
        }
        if (!sentinel_system) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    if (!sentinel_system) {
        LogErr("Sentinel system did not appear");
        return false;
    }

    sentinel_mavlink_direct_ = std::make_unique<MavlinkDirect>(sentinel_system);
    sentinel_mavlink_direct_->subscribe_message(
        "HEARTBEAT", [this](const MavlinkDirect::MavlinkMessage& message) {
            static const std::string key = "\"custom_mode\":";
            const auto pos = message.fields_json.find(key);
            if (pos == std::string::npos) {
                return;
            }
            const auto sentinel = static_cast<uint32_t>(
                std::strtoul(message.fields_json.c_str() + pos + key.size(), nullptr, 10));
            {
                std::lock_guard<std::mutex> lock(sentinel_mutex_);
                sentinel_seen_ = sentinel;
            }
            sentinel_cv_.notify_all();
        });

    return true;
}

void MavsdkFuzzer::wait_until_processed()
{
    // pass_received_raw_bytes() only posts the bytes to the io_context, which parses them later.
    // Without waiting, the fuzzer injects far faster than MAVSDK can process, and the queue of
    // pending buffers grows until the fuzzer runs out of memory. The io_context handles the
    // buffers in order, so once the sentinel queued behind them has been seen, everything passed
    // in before it has been processed too. If processing stalls, the watchdog aborts while we
    // wait here.
    const uint32_t sentinel = ++sentinel_sent_;

    send_sentinel(sentinel);

    std::unique_lock<std::mutex> lock(sentinel_mutex_);
    sentinel_cv_.wait(lock, [this, sentinel]() { return sentinel_seen_ == sentinel; });
}

void MavsdkFuzzer::create_plugins_for_system(std::shared_ptr<System> system)
{
    // The sentinel system only needs the MavlinkDirect subscription set up in
    // subscribe_to_sentinel(), not a full plugin set.
    if (system->get_system_id() == SENTINEL_SYSTEM_ID) {
        return;
    }

    LogDebug("Creating plugins for system ID: {}", system->get_system_id());

    auto plugins = std::make_unique<SystemPlugins>();
    plugins->system = system;

    // Create all client plugin instances for this system
    // Each plugin registers message handlers, so instantiating them ensures
    // fuzzed messages get processed by all relevant code paths
    plugins->action = std::make_unique<Action>(system);
    plugins->calibration = std::make_unique<Calibration>(system);
    plugins->camera = std::make_unique<Camera>(system);
    plugins->component_metadata = std::make_unique<ComponentMetadata>(system);
    plugins->events = std::make_unique<Events>(system);
    plugins->failure = std::make_unique<Failure>(system);
    plugins->follow_me = std::make_unique<FollowMe>(system);
    plugins->ftp = std::make_unique<Ftp>(system);
    plugins->geofence = std::make_unique<Geofence>(system);
    plugins->gimbal = std::make_unique<Gimbal>(system);
    plugins->gripper = std::make_unique<Gripper>(system);
    plugins->info = std::make_unique<Info>(system);
    plugins->log_files = std::make_unique<LogFiles>(system);
    plugins->log_streaming = std::make_unique<LogStreaming>(system);
    plugins->manual_control = std::make_unique<ManualControl>(system);
    plugins->mavlink_direct = std::make_unique<MavlinkDirect>(system);
    plugins->mission = std::make_unique<Mission>(system);
    plugins->mission_raw = std::make_unique<MissionRaw>(system);
    plugins->mocap = std::make_unique<Mocap>(system);
    plugins->offboard = std::make_unique<Offboard>(system);
    plugins->param = std::make_unique<Param>(system);
    plugins->rtk = std::make_unique<Rtk>(system);
    plugins->shell = std::make_unique<Shell>(system);
    plugins->telemetry = std::make_unique<Telemetry>(system);
    plugins->transponder = std::make_unique<Transponder>(system);
    plugins->tune = std::make_unique<Tune>(system);
    plugins->winch = std::make_unique<Winch>(system);
    plugins->mavlink_passthrough = std::make_unique<MavlinkPassthrough>(system);

    system_plugins_.push_back(std::move(plugins));

    LogDebug(
        "Successfully created {} plugin sets, latest for system ID: {}",
        system_plugins_.size(),
        system->get_system_id());
}

bool MavsdkFuzzer::initialize()
{
    if (initialized_) {
        return true;
    }

    // Create MAVSDK instance
    mavsdk_ = std::make_unique<Mavsdk>(Mavsdk::Configuration{ComponentType::GroundStation});

    // Add raw connection for direct byte injection (no network overhead)
    ConnectionResult result = mavsdk_->add_any_connection("raw://");

    if (result != ConnectionResult::Success) {
        mavsdk_.reset();
        return false;
    }

    // Subscribe to outgoing bytes and discard them (required for raw connection to work)
    mavsdk_->subscribe_raw_bytes_to_be_sent([](const char*, size_t) {
        // Discard outgoing messages - we don't need them for fuzzing
    });

    // Subscribe to new systems and create plugins for each one dynamically
    mavsdk_->subscribe_on_new_system([this]() {
        LogDebug("New system callback triggered!");
        // A new system appeared, check for any systems we haven't seen yet
        auto all_systems = mavsdk_->systems();
        LogDebug("Found {} systems", all_systems.size());
        for (auto system : all_systems) {
            // Check if we already have plugins for this system
            bool found = false;
            for (const auto& existing_plugins : system_plugins_) {
                if (existing_plugins->system == system) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                LogDebug("System not found in plugin list, creating plugins...");
                create_plugins_for_system(system);
            } else {
                LogDebug("System already has plugins created");
            }
        }
    });

    // Also handle any systems that might already exist
    auto existing_systems = mavsdk_->systems();
    LogDebug("Checking for existing systems, found: {}", existing_systems.size());
    for (auto system : existing_systems) {
        LogDebug("Creating plugins for existing system ID: {}", system->get_system_id());
        create_plugins_for_system(system);
    }

    // Send initial heartbeat to trigger system discovery
    LogDebug("Sending initial heartbeat to trigger system discovery");
    send_heartbeat();

    // Give MAVSDK time to process the heartbeat and discover the system
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Check again for systems after the heartbeat
    auto post_heartbeat_systems = mavsdk_->systems();
    LogDebug("After heartbeat, checking for systems, found: {}", post_heartbeat_systems.size());
    for (auto system : post_heartbeat_systems) {
        // Check if we already have plugins for this system
        bool found = false;
        for (const auto& existing_plugins : system_plugins_) {
            if (existing_plugins->system == system) {
                found = true;
                break;
            }
        }
        if (!found) {
            LogDebug("Creating plugins for post-heartbeat system ID: {}", system->get_system_id());
            create_plugins_for_system(system);
        } else {
            LogDebug("Post-heartbeat system already has plugins");
        }
    }

    if (!subscribe_to_sentinel()) {
        system_plugins_.clear();
        mavsdk_.reset();
        return false;
    }

    // Start watchdog with 3 second timeout to detect stalls/deadlocks
    watchdog_ = std::make_unique<Watchdog>(std::chrono::milliseconds(3000));

    initialized_ = true;
    return true;
}

size_t MavsdkFuzzer::process_fuzz_data_to_mavlink(
    const uint8_t* fuzz_data, size_t fuzz_size, uint8_t* output_buffer, size_t output_buffer_size)
{
    size_t total_bytes_written = 0;
    size_t offset = 0;

    // Process fuzz data in chunks, creating MAVLink messages
    while (offset < fuzz_size &&
           (output_buffer_size - total_bytes_written) >= MAVLINK_MAX_PACKET_LEN) {
        // Determine how much data to use for this message
        size_t chunk_size =
            std::min(static_cast<size_t>(255), fuzz_size - offset); // MAVLink max payload is 255

        if (chunk_size < 3) {
            break; // Need minimum data for a meaningful message (msg_id + seq + some payload)
        }

        // Create a MAVLink message structure from the fuzz data
        mavlink_message_t msg;

        // Use fixed system ID and component ID for better plugin message routing
        uint8_t system_id = 1; // Fixed to ensure consistent message routing
        uint8_t component_id = 1; // Fixed to ensure consistent message routing
        // Select a valid dialect message id from two fuzz bytes (the single-byte
        // id used before could only reach ids 0..255).
        const std::vector<uint32_t>& vids = valid_msg_ids();
        const uint32_t sel = (static_cast<uint32_t>(fuzz_data[offset + 2]) << 8) ^
                             static_cast<uint32_t>(fuzz_data[offset + 1]);
        const uint32_t msg_id = vids.empty() ? 0u : vids[sel % vids.size()];

        // Fill the message with fuzz data
        msg.msgid = msg_id;
        msg.sysid = system_id;
        msg.compid = component_id;
        msg.seq = fuzz_data[offset + 1]; // Use second byte for sequence

        // Copy payload data (up to 255 bytes)
        size_t payload_size =
            std::min(chunk_size - 3, static_cast<size_t>(255)); // -3 for seq, msg_id, and one more
        msg.len = payload_size;

        if (payload_size > 0 && offset + 3 + payload_size <= fuzz_size) {
            memcpy(msg.payload64, fuzz_data + offset + 3, payload_size);
        } else {
            memset(msg.payload64, 0, sizeof(msg.payload64));
            msg.len = 0;
        }

        // Use the real crc_extra / min_length / max_length for this message id,
        // so the receiver's CRC check passes and the message actually reaches its
        // handler. Finalizing with crc_extra = 0 (as before) made every frame fail
        // the receiver's CRC and get dropped, so only the framing layer was ever
        // exercised, never the plugin message handlers.
        // msg.msgid always comes from valid_msg_ids(), i.e. ids for which
        // mavlink_get_msg_entry() is non-null, so the lookup is guaranteed to succeed.
        const mavlink_msg_entry_t* entry = mavlink_get_msg_entry(msg.msgid);
        const uint8_t max_len = entry->max_msg_len;
        if (msg.len > max_len) {
            msg.len = max_len;
        }
        msg.checksum = 0; // Will be calculated by mavlink_finalize_message

        // Finalize the message (calculates the correct checksum).
        mavlink_finalize_message(
            &msg, system_id, component_id, entry->min_msg_len, msg.len, entry->crc_extra);

        // Convert to send buffer
        size_t message_length =
            mavlink_msg_to_send_buffer(output_buffer + total_bytes_written, &msg);
        total_bytes_written += message_length;

        // Move to next chunk
        offset += chunk_size;
    }

    return total_bytes_written;
}

void MavsdkFuzzer::process_fuzz_input(const uint8_t* data, size_t size)
{
    // Need minimum data to create meaningful messages
    if (size < 3 || size > 1024) {
        return;
    }

    if (!initialized_ || !mavsdk_ || !watchdog_) {
        return;
    }

    // Reset watchdog at start of each input processing
    watchdog_->reset();

    // Send heartbeat every second to keep the system alive
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_heartbeat_).count() >=
        1000) {
        send_heartbeat();
        last_heartbeat_ = now;
    }

    // Process fuzz data into MAVLink messages with correct checksums
    uint8_t mavlink_buffer[2048]; // Allow for multiple messages
    size_t total_mavlink_size =
        process_fuzz_data_to_mavlink(data, size, mavlink_buffer, sizeof(mavlink_buffer));

    if (total_mavlink_size == 0) {
        return;
    }

    // Pass all generated MAVLink messages directly to MAVSDK via raw connection
    mavsdk_->pass_received_raw_bytes(
        reinterpret_cast<const char*>(mavlink_buffer), total_mavlink_size);

    // Don't move on to the next input before MAVSDK has processed this one.
    wait_until_processed();

    // Reset watchdog after successful processing
    watchdog_->reset();
}

// Global fuzzer instance
static std::unique_ptr<MavsdkFuzzer> g_fuzzer;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // Initialize on first run
    if (!g_fuzzer) {
        g_fuzzer = std::make_unique<MavsdkFuzzer>();
        if (!g_fuzzer->initialize()) {
            g_fuzzer.reset();
            return 0;
        }
    }

    // Process the fuzz input
    g_fuzzer->process_fuzz_input(data, size);
    return 0;
}

// Cleanup function (called by libfuzzer on exit)
extern "C" void LLVMFuzzerFinalize()
{
    // Clean up the fuzzer instance
    g_fuzzer.reset();
}