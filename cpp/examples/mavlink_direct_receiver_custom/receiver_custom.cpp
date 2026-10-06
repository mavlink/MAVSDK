//
// Example to use MavlinkDirect to receive a custom message (GAS_SENSOR) which
// is not part of MAVLink, by loading its XML definition at runtime.
//
// Use together with the mavlink_direct_sender_custom example.
//
// MavlinkDirect represents the message fields as JSON, so we use the
// nlohmann/json library to read them.
//

#include <mavsdk/mavsdk.hpp>
#include <mavsdk/plugins/mavlink_direct/mavlink_direct.hpp>
#include <nlohmann/json.hpp>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>

using namespace mavsdk;
using json = nlohmann::json;

// Same definition as on the sending side. We need it to make sense of the message.
const std::string gas_sensor_xml = R"(
<mavlink>
    <messages>
        <message id="44000" name="GAS_SENSOR">
            <description>Gas concentration measured by a sensor.</description>
            <field type="uint64_t" name="time_usec" units="us">Timestamp (time since system boot).</field>
            <field type="uint8_t" name="id" instance="true">Sensor ID.</field>
            <field type="float" name="co2" units="ppm">CO2 concentration.</field>
            <field type="float" name="ch4" units="ppm">Methane concentration.</field>
            <field type="int16_t" name="temperature" units="cdegC">Sensor temperature.</field>
        </message>
    </messages>
</mavlink>
)";

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url>\n"
              << "Connection URL format should be :\n"
              << " For TCP server: tcpin://<our_ip>:<port>\n"
              << " For TCP client: tcpout://<remote_ip>:<port>\n"
              << " For UDP server: udpin://<our_ip>:<port>\n"
              << " For UDP client: udpout://<remote_ip>:<port>\n"
              << " For Serial : serial://</path/to/serial/dev>:<baudrate>]\n"
              << "For example, to connect to the simulator use URL: udpin://0.0.0.0:14550\n";
}

void on_gas_sensor(const MavlinkDirect::MavlinkMessage& message)
{
    const auto fields = json::parse(message.fields_json);
    // The temperature is in cdegC, as given by the message definition.
    std::cout << std::fixed << std::setprecision(1) << "GAS_SENSOR " << fields["id"].get<int>()
              << " received: " << fields["co2"].get<double>() << " ppm CO2, "
              << fields["ch4"].get<double>() << " ppm CH4, "
              << fields["temperature"].get<int>() / 100.0 << " degC" << std::endl;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    // Set up as ground station
    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};

    auto connection_result = mavsdk.add_any_connection(argv[1]);
    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << std::endl;
        return 1;
    }

    std::cout << "Waiting for an autopilot..." << std::endl;
    auto system = mavsdk.first_autopilot(10.0);
    if (!system) {
        std::cerr << "No autopilot found" << std::endl;
        return 1;
    }

    auto mavlink_direct = MavlinkDirect{system.value()};

    auto xml_result = mavlink_direct.load_custom_xml(gas_sensor_xml);
    if (xml_result != MavlinkDirect::Result::Success) {
        std::cerr << "Failed to load custom XML: " << xml_result << std::endl;
        return 1;
    }
    std::cout << "Custom XML loaded successfully" << std::endl;

    mavlink_direct.subscribe_message("GAS_SENSOR", on_gas_sensor);

    std::cout << "Waiting for GAS_SENSOR messages. Press Ctrl+C to exit..." << std::endl;
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
