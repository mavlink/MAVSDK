//
// Example to use MavlinkDirect to receive a known message (OBSTACLE_DISTANCE)
// and a custom message (GAS_SENSOR), e.g. to check whether an autopilot
// forwards them from another link.
//
// Use together with the mavlink_direct_sender and mavlink_direct_sender_custom
// examples.
//

#include <mavsdk/mavsdk.hpp>
#include <mavsdk/plugins/mavlink_direct/mavlink_direct.hpp>
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
              << "For example, to connect to the simulator use URL: udpin://0.0.0.0:14550\n";
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};

    auto connection_result = mavsdk.add_any_connection(argv[1]);
    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << std::endl;
        return 1;
    }

    std::cout << "Waiting for system to connect..." << std::endl;
    auto system = mavsdk.first_autopilot(3.0);
    if (!system) {
        std::cerr << "Timed out waiting for system" << std::endl;
        return 1;
    }

    auto mavlink_direct = MavlinkDirect{system.value()};

    // Same definition as in the mavlink_direct_sender_custom example. We need it
    // to parse the message, the forwarding autopilot in between does not.
    std::string custom_xml = R"(
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
</mavlink>)";

    auto xml_result = mavlink_direct.load_custom_xml(custom_xml);
    if (xml_result != MavlinkDirect::Result::Success) {
        std::cerr << "Failed to load custom XML: " << xml_result << std::endl;
        return 1;
    }

    auto print_message = [](const MavlinkDirect::MavlinkMessage& message) {
        std::cout << "** " << message.message_name << " from " << message.system_id << "/"
                  << message.component_id << " **\n"
                  << message.fields_json << std::endl;
    };

    auto obstacle_handle = mavlink_direct.subscribe_message("OBSTACLE_DISTANCE", print_message);
    auto gas_handle = mavlink_direct.subscribe_message("GAS_SENSOR", print_message);

    std::cout << "Listening for OBSTACLE_DISTANCE and GAS_SENSOR. Press Ctrl+C to exit..."
              << std::endl;
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    mavlink_direct.unsubscribe_message(gas_handle);
    mavlink_direct.unsubscribe_message(obstacle_handle);

    return 0;
}
