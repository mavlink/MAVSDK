//
// Example to use MavlinkDirect to send a custom message (GAS_SENSOR) which
// is not part of MAVLink, by loading its XML definition at runtime
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

    // Define our own message for a gas sensor. It is not part of any MAVLink dialect, so
    // MAVSDK does not know about it until we load its definition.
    // Note that the receiving side needs to know the definition as well to make sense of it.
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

    // Load custom XML
    auto xml_result = mavlink_direct.load_custom_xml(custom_xml);
    if (xml_result != MavlinkDirect::Result::Success) {
        std::cerr << "Failed to load custom XML: " << xml_result << std::endl;
        return 1;
    }

    std::cout << "Custom XML loaded successfully" << std::endl;

    // Create GAS_SENSOR message
    MavlinkDirect::MavlinkMessage gas_sensor_message{};
    gas_sensor_message.message_name = "GAS_SENSOR";
    gas_sensor_message.system_id = config.get_system_id(); // Your component's system ID
    gas_sensor_message.component_id = config.get_component_id(); // Your component's component ID
    gas_sensor_message.target_system_id = 0; // Does not apply for this message
    gas_sensor_message.target_component_id = 0; // Does not apply for this message

    const auto start_time = std::chrono::steady_clock::now();

    for (int counter = 0; counter < 20; ++counter) {
        // Made-up values that change a bit over time
        const auto time_usec = std::chrono::duration_cast<std::chrono::microseconds>(
                                   std::chrono::steady_clock::now() - start_time)
                                   .count();
        const float co2 = 420.0f + counter * 5.0f;
        const float ch4 = 1.9f + counter * 0.1f;
        const int16_t temperature = 2150 + counter * 10; // 21.5 degC and rising

        // In a real application you would probably use a JSON library such as
        // nlohmann/json to assemble this.
        gas_sensor_message.fields_json = "{\"time_usec\": " + std::to_string(time_usec) +
                                         ", \"id\": 0" + ", \"co2\": " + std::to_string(co2) +
                                         ", \"ch4\": " + std::to_string(ch4) +
                                         ", \"temperature\": " + std::to_string(temperature) + "}";

        auto result = mavlink_direct.send_message(gas_sensor_message);
        if (result == MavlinkDirect::Result::Success) {
            std::cout << "GAS_SENSOR message " << (counter + 1) << "/20 sent: " << co2
                      << " ppm CO2, " << ch4 << " ppm CH4" << std::endl;
        } else {
            std::cerr << "GAS_SENSOR message could not be sent: " << result << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "Sent all 20 GAS_SENSOR messages. Exiting." << std::endl;

    return 0;
}
