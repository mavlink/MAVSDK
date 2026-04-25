// Example: send one STATUSTEXT for every severity level to QGC via ServerUtility.
//
// Run against sim:
//   ./statustext_hello udpin://0.0.0.0:14540
//
// Run against real vehicle (companion computer on serial):
//   ./statustext_hello serial:///dev/ttyUSB0:57600

#include <chrono>
#include <iostream>
#include <string_view>
#include <thread>

#include <mavsdk/mavsdk.hpp>
#include <mavsdk/plugins/server_utility/server_utility.hpp>

using namespace mavsdk;
using std::this_thread::sleep_for;

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <connection_url>\n";
        return 1;
    }

    // Run as a companion-computer component so STATUSTEXT appears in the GCS
    // as a companion, not as the GCS talking to itself.
    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::CompanionComputer}};

    if (mavsdk.add_any_connection(argv[1]) != ConnectionResult::Success) {
        std::cerr << "Connection failed\n";
        return 1;
    }

    std::cout << "Waiting for vehicle..." << std::flush;
    while (mavsdk.systems().empty()) {
        sleep_for(std::chrono::milliseconds(100));
        std::cout << "." << std::flush;
    }
    std::cout << "\n";

    auto system = mavsdk.systems().front();
    ServerUtility server_utility{system};

    // Send one message per severity level so every MAV_SEVERITY value is exercised.
    const std::pair<ServerUtility::StatusTextType, std::string_view> levels[] = {
        {ServerUtility::StatusTextType::Debug,     "DEBUG: low-level diagnostic info"},
        {ServerUtility::StatusTextType::Info,      "INFO: normal operational message"},
        {ServerUtility::StatusTextType::Notice,    "NOTICE: unusual but not an error"},
        {ServerUtility::StatusTextType::Warning,   "WARNING: action may be required"},
        {ServerUtility::StatusTextType::Error,     "ERROR: recoverable error condition"},
        {ServerUtility::StatusTextType::Critical,  "CRITICAL: immediate action required"},
        {ServerUtility::StatusTextType::Alert,     "ALERT: system is unusable"},
        {ServerUtility::StatusTextType::Emergency, "EMERGENCY: total system failure"},
    };

    for (const auto& [type, msg] : levels) {
        const auto result = server_utility.send_status_text(type, std::string{msg});
        std::cout << to_string(type) << ": " << result << "\n";
        sleep_for(std::chrono::milliseconds(500));
    }

    return 0;
}
