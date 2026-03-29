//
// Example to use MavlinkDirect to send AIRSPEED message which needs to be
// loaded as custom because it is only in development.xml
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mavlink_direct/mavlink_direct.h>
#include <iostream>
#include <chrono>
#include <thread>

using namespace mavsdk;

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

    // Set up as companion computer
    auto config = Mavsdk::Configuration{ComponentType::CompanionComputer};

    // Create MAVSDK instance
    Mavsdk mavsdk{config};

    // Connect using the provided connection URL
    auto connection_result = mavsdk.add_any_connection(argv[1]);
    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << std::endl;
        return 1;
    }

    // Discover and connect to system
    auto system = mavsdk.first_autopilot(3.0);
    if (!system) {
        std::cerr << "No autopilot found" << std::endl;
        return 1;
    }

    // Create MavlinkDirect plugin instance
    auto mavlink_direct = MavlinkDirect{system.value()};

    // Define custom XML with AIRSPEED message (ID 295)
    // NOTE: We load this as custom XML because the AIRSPEED message is currently
    // only available in development.xml but not yet in common.xml, so not available
    // in MAVSDK by default yet.

    std::string custom_xml = R"(
<mavlink>
    <messages>
        <message id="295" name="AIRSPEED">
            <description>Airspeed sensor data</description>
            <field type="uint8_t" name="id">Sensor ID</field>
            <field type="float" name="airspeed">Calibrated airspeed in m/s</field>
            <field type="int16_t" name="temperature">Temperature in centidegrees</field>
            <field type="float" name="raw_press">Raw differential pressure</field>
            <field type="uint8_t" name="flags">Airspeed sensor flags</field>
        </message>
    </messages>
</mavlink>)";

    // Load custom XML
    auto xml_result = mavlink_direct.load_custom_xml(custom_xml);
    if (xml_result != MavlinkDirect::Result::Success) {
        std::cerr << "Failed to load custom XML: " << xml_result << std::endl;
        return 1;
    }

    std::cout << "Custom XML loaded successfully" << std::endl;

    // Create AIRSPEED message
    MavlinkDirect::MavlinkMessage airspeed_message{};
    airspeed_message.message_name = "AIRSPEED";
    airspeed_message.system_id = config.get_system_id(); // Your component's system ID
    airspeed_message.component_id = config.get_component_id(); // Your component's component ID
    airspeed_message.target_system_id = 0; // Does not apply for this message
    airspeed_message.target_component_id = 0; // Does not apply for this message

    int counter = 0;
    while (counter < 20) {
        // Hard-coded values with variation
        float airspeed = 25.0f + (counter * 0.5f); // Airspeed from 25.0 to 34.5 m/s
        int16_t temperature = 2000 + (counter * 10); // Temperature from 20.00°C to 21.90°C
        float raw_press = 100.0f + (counter * 2.0f); // Raw pressure from 100.0 to 138.0

        airspeed_message.fields_json = R"({
            "id": 1,
            "airspeed": )" + std::to_string(airspeed) +
                                       R"(,
            "temperature": )" + std::to_string(temperature) +
                                       R"(,
            "raw_press": )" + std::to_string(raw_press) +
                                       R"(,
            "flags": 3
        })";

        // Send the message
        auto result = mavlink_direct.send_message(airspeed_message);
        if (result == MavlinkDirect::Result::Success) {
            std::cout << "AIRSPEED message " << (counter + 1) << "/20 sent successfully - "
                      << "airspeed: " << airspeed << " m/s, "
                      << "temp: " << (temperature / 100.0f) << "°C" << std::endl;
        } else {
            std::cerr << "AIRSPEED message could not be sent: " << result << std::endl;
        }

        counter++;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    std::cout << "Sent all 20 AIRSPEED messages. Exiting." << std::endl;

    return 0;
}
