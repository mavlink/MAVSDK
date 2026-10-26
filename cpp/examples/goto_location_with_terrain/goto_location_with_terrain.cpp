//
// Variant of the goto_location example that bypasses Action::goto_location
// and instead sends DO_REPOSITION as a COMMAND_INT via MavlinkDirect, using
// MAV_FRAME_GLOBAL_TERRAIN_ALT_INT so the altitude is interpreted as AGL.
//

#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <thread>
#include <mavsdk/mavsdk.hpp>
#include <mavsdk/plugins/action/action.hpp>
#include <mavsdk/plugins/mavlink_direct/mavlink_direct.hpp>
#include <mavsdk/plugins/param/param.hpp>
#include <mavsdk/plugins/telemetry/telemetry.hpp>
#include <nlohmann/json.hpp>

using namespace mavsdk;
using json = nlohmann::json;
using std::chrono::seconds;
using std::this_thread::sleep_for;

static constexpr int cmd_do_reposition = 192;
static constexpr int frame_global_terrain_alt_int = 11;
static constexpr int reposition_flags_change_mode = 1;

// Send DO_REPOSITION as a COMMAND_INT in the terrain-altitude frame and wait for the ack.
// Returns true if PX4 acked with MAV_RESULT_ACCEPTED (0).
bool goto_terrain(
    MavlinkDirect& mavlink_direct,
    uint8_t target_system_id,
    double lat_deg,
    double lon_deg,
    float alt_agl_m)
{
    const int32_t lat_e7 = static_cast<int32_t>(std::round(lat_deg * 1e7));
    const int32_t lon_e7 = static_cast<int32_t>(std::round(lon_deg * 1e7));

    // null means NaN, which keeps the current yaw.
    const json fields = {
        {"target_system", target_system_id},
        {"target_component", 1},
        {"frame", frame_global_terrain_alt_int},
        {"command", cmd_do_reposition},
        {"current", 0},
        {"autocontinue", 0},
        {"param1", -1},
        {"param2", reposition_flags_change_mode},
        {"param3", 0},
        {"param4", nullptr},
        {"x", lat_e7},
        {"y", lon_e7},
        {"z", alt_agl_m}};

    MavlinkDirect::MavlinkMessage command_int{};
    command_int.message_name = "COMMAND_INT";
    command_int.fields_json = fields.dump();

    std::cout << "Sending DO_REPOSITION to " << lat_deg << ", " << lon_deg << " at " << alt_agl_m
              << " m AGL...\n";

    // This retransmits if needed and waits for the final COMMAND_ACK.
    const auto [send_result, ack] = mavlink_direct.send_command(command_int);
    if (send_result != MavlinkDirect::Result::Success) {
        std::cerr << "No COMMAND_ACK received for DO_REPOSITION: " << send_result << '\n';
        return false;
    }

    // Success only means we got an ack, the outcome is its MAV_RESULT.
    const auto ack_fields = json::parse(ack.fields_json, nullptr, false);
    if (!ack_fields.is_object() || !ack_fields.contains("result")) {
        std::cerr << "COMMAND_ACK missing result field: " << ack.fields_json << '\n';
        return false;
    }
    const int result_value = ack_fields["result"].get<int>();
    std::cout << "DO_REPOSITION result: " << result_value << '\n';
    return result_value == 0;
}

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url>\n"
              << "Connection URL format should be :\n"
              << " For TCP server: tcpin://<our_ip>:<port>\n"
              << " For TCP client: tcpout://<remote_ip>:<port>\n"
              << " For UDP server: udpin://<our_ip>:<port>\n"
              << " For UDP client: udpout://<remote_ip>:<port>\n"
              << " For Serial : serial://</path/to/serial/dev>:<baudrate>]\n"
              << "For example, to connect to the simulator use URL: udpin://0.0.0.0:14540\n";
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    ConnectionResult connection_result = mavsdk.add_any_connection(argv[1]);

    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << '\n';
        return 1;
    }

    auto system = mavsdk.first_autopilot(3.0);
    if (!system) {
        std::cerr << "Timed out waiting for system\n";
        return 1;
    }

    auto telemetry = Telemetry{system.value()};
    auto action = Action{system.value()};
    auto mavlink_direct = MavlinkDirect{system.value()};
    auto param = Param{system.value()};

    // Slow cruise speed.
    if (param.set_param_float("MPC_XY_CRUISE", 3.0f) != Param::Result::Success) {
        std::cerr << "Failed to set MPC_XY_CRUISE\n";
        return 1;
    }
    // And tighten the altitude P gain so terrain following is observable.
    if (param.set_param_float("MPC_Z_P", 2.0f) != Param::Result::Success) {
        std::cerr << "Failed to set MPC_Z_P\n";
        return 1;
    }

    // A previous flight can leave the vehicle in Land mode, in which PX4
    // refuses to arm and the vehicle never becomes armable. Switch to Hold first.
    std::cout << "Switching to Hold...\n";
    const Action::Result initial_hold_result = action.hold();
    if (initial_hold_result != Action::Result::Success) {
        std::cerr << "Switching to Hold failed: " << initial_hold_result << '\n';
        return 1;
    }

    while (!telemetry.health().is_armable) {
        std::cout << "Vehicle is getting ready to arm\n";
        sleep_for(seconds(1));
    }

    std::cout << "Arming...\n";
    const Action::Result arm_result = action.arm();
    if (arm_result != Action::Result::Success) {
        std::cerr << "Arming failed: " << arm_result << '\n';
        return 1;
    }

    // Take off to 5 m so we are clear of the ground before repositioning.
    const float takeoff_altitude_m = 5.0f;
    std::cout << "Setting takeoff altitude to " << takeoff_altitude_m << " m...\n";
    const Action::Result set_takeoff_result = action.set_takeoff_altitude(takeoff_altitude_m);
    if (set_takeoff_result != Action::Result::Success) {
        std::cerr << "Set takeoff altitude failed: " << set_takeoff_result << '\n';
        return 1;
    }

    std::cout << "Taking off...\n";
    const Action::Result takeoff_result = action.takeoff();
    if (takeoff_result != Action::Result::Success) {
        std::cerr << "Takeoff failed: " << takeoff_result << '\n';
        return 1;
    }

    while (telemetry.position().relative_altitude_m < takeoff_altitude_m - 1.0f) {
        std::cout << "Climbing: " << telemetry.position().relative_altitude_m << " m\n";
        sleep_for(seconds(1));
    }

    const uint8_t target_system_id = system.value()->get_system_id();
    const float target_alt_agl_m = 5.0f;

    // Go ~200 m east, then back ~200 m west.
    if (!goto_terrain(mavlink_direct, target_system_id, 47.397971, 8.547, target_alt_agl_m)) {
        return 1;
    }
    sleep_for(seconds(30));

    if (!goto_terrain(mavlink_direct, target_system_id, 47.397971, 8.546164, target_alt_agl_m)) {
        return 1;
    }
    sleep_for(seconds(30));

    std::cout << "Landing...\n";
    const Action::Result land_result = action.land();
    if (land_result != Action::Result::Success) {
        std::cerr << "Land failed: " << land_result << '\n';
        return 1;
    }

    while (telemetry.in_air()) {
        std::cout << "Vehicle is landing...\n";
        sleep_for(seconds(1));
    }
    std::cout << "Landed!\n";

    sleep_for(seconds(3));
    std::cout << "Finished...\n";

    return 0;
}
